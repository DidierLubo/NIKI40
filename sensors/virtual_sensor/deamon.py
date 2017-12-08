# encoding: utf-8

# Copyright 2017 NIKI 4.0 project team
#
# NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
# Project partners are FZI Forschungszentrum Informatik am Karlsruher
# Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
# für angewandte Forschung e.V. (www.hahn-schickard.de) and
# Hochschule Offenburg (www.hs-offenburg.de).
# This file was developed by Mark Weyer at Hahn-Schickard.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""High-level protocol management."""

import threading

import PySide.QtCore as core

import protocol
import emb6_frame
import emb6
import protocols
import waveform

class Internal(Exception): pass

class Deamon:
    """Runs the protocols for startup and for setting resources."""

    stopped, started, port_found, device_found, emb6_configured, emb6_started, \
      emb6_connected, lwm2m_configured, lwm2m_started, lwm2m_connected \
      = range(10)

    def __init__(self,
            window, config, progress_widget, state_widget, resources):
        """Define protocols and resource value data sources."""

        # self.measurement_protocols will hold a (cyclic) queue of resource
        # value data sources.

        self.window = window
        self.config = config
        self.resources = resources
        self.progress_widget = progress_widget
        progress_widget.setMaximum(self.lwm2m_connected)
        self.state_widget = state_widget
        self.wait_timer = core.QTime()
        self.delay_timer = core.QTime()
        self.set_state(self.stopped)
        self.in_delay = True
        self.framer = None
        self.lock = threading.Lock()

        self._init_measurements()

        self.start_protocol = (lambda:
            protocol.ReportError(
                protocol.Protocol([
                    protocols.find_device(core.QTime,self.window.error),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.device_found)),
                    protocols.config_wireless(
                        core.QTime,self.window.error,config,resources),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.emb6_configured)),
                    protocols.start_part_1a(core.QTime,self.window.error),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.emb6_started)),
                    protocols.start_part_1b(self.window.error),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.emb6_connected)),
                    protocols.config_lwm2m(
                        core.QTime,self.window.error,config,resources),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.lwm2m_configured)),
                    protocols.start_part_2a(core.QTime,self.window.error),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.lwm2m_started)),
                    protocols.start_part_2b(
                        core.QTime,resources,self.window.error),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.lwm2m_connected)),
                    ]),
                lambda: self._stop(None)))


    def _wrap_lock(self,method):
        # Makes a paremeterless method atomic using self.lock
        # We need that for all methods that are called by the UI.
        self.lock.acquire()
        try:
            method()
        except:
            self.lock.release()
            raise
        self.lock.release()


    def init_measurements(self):
        """Do or redo the sensor definition-dependend part of initialization."""

        self._wrap_lock(self._init_measurements)
    def _init_measurements(self):

        for resource in self.resources.list():
            resource.value_generator.initialize(core.QTime(), self.config)

        self.measurement_protocols = [
                (lambda resource: lambda: protocols.measurement(
                    core.QTime, resource, self.state_widget.setText))
                (resource)
                # The first lambda is needed to force local binding of resource.
                # Counterintuitively, this is not what the for construct
                # already does.
                # The second lambda is just deferred computation.
            for resource in self.resources.list()
                if not resource.value_generator.is_constant()]


    def set_state(self,new_state):
        self.state = new_state
        if self.state == self.stopped: description = "not running"
        elif self.state == self.started: description = "started"
        elif self.state == self.port_found:
            description = "serial port initialized"
        elif self.state == self.device_found:
            description = "connected to device"
        elif self.state == self.emb6_configured:
            description = "wireless connection configured"
        elif self.state == self.emb6_started:
            description = "wireless connection attempt started"
        elif self.state == self.emb6_connected:
            description = "wireless network access achieved"
        elif self.state == self.lwm2m_configured:
            description = "LWM2M configured"
        elif self.state == self.lwm2m_started:
            description = "LWM2M connection attempt started"
        elif self.state == self.lwm2m_connected:
            description = "registered at LWM2M server"
        else: raise Internal()
        self.progress_widget.setValue(new_state)
        self.state_widget.setText("State: " + description)


    def start(self):
        """Start or restart."""

        self._wrap_lock(self._start)
    def _start(self):
        self.window.lock_unlock(True)
        self.set_state(self.started)
        self.window.error("")
        try:
            if self.framer is not None:
                self.framer.stop()
            self.framer = emb6_frame.Framer(self.config.value('port_name'))
            self.device = emb6.Emb6(self.framer)
            self.device.log = True
            self.set_state(self.port_found)
        except emb6_frame.PortUnavailable:
            self._stop("Error initializing serial port. " \
              "Check serial port name and start again.")
            return
        self.protocol = protocol.TimeWaiting(
            self.start_protocol(),
            self.wait_timer)
        self.protocol.reset()
        self.in_delay = False
        self._tick()


    def stop(self):
        """Stop operating."""

        self._wrap_lock(self._stop)
    def _stop(self,message=""):
        if self.framer is not None:
            self.framer.stop()
            self.framer = None
        self.window.lock_unlock(False)
        self.set_state(self.stopped)
        if message is not None: self.window.error(message)


    def tick(self):
        """Proceed with the current protocol (if any).

        If the protocol is held up waiting, control is returned to the caller.
        In case of repeated waiting for more than 1s, a status error message is
        transmitted.

        If the protocol finishes, the next one is scheduled with the delay
        given by the update_delay config value.
        """

        self._wrap_lock(self._tick)
    def _tick(self):
        if self.state == self.stopped:
            return
        if self.in_delay:
            if self.delay_timer.elapsed() / 1000.0 < \
              self.config.value('update_delay'):
                return
            else: self.in_delay = False
        result,waiting = self.protocol.execute_time_waiting(self.device)
        if result != protocol.error and waiting is None:
            self.window.error("")
        if waiting is not None:
            secs = waiting // 1000
            if secs>0:
                self.window.error(
                    "Waiting for message since " + str(secs) + "s.")
        if result == protocol.succeed:
            self.protocol = protocol.TimeWaiting(
                self.measurement_protocols[0](),
                self.wait_timer)
            self.protocol.reset()
            self.in_delay = True
            self.delay_timer.start()
            self.measurement_protocols = \
              self.measurement_protocols[1:] \
              + self.measurement_protocols[:1]

