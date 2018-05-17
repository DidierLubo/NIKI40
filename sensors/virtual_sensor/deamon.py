# encoding: utf-8

# Copyright 2017,2018 NIKI 4.0 project team
#
# NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
# Project partners are FZI Forschungszentrum Informatik am Karlsruher
# Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
# für angewandte Forschung e.V. (www.hahn-schickard.de) and
# Hochschule Offenburg (www.hs-offenburg.de).
# This file was developed by Mark Weyer and Sebastian King at Hahn-Schickard.
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

    def __init__(self, window, config, progress_widget, state_widget,
            resources, cli_data):
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
        self.cli_data = cli_data

        self._init_measurements()

        self.start_protocol = (lambda:
            protocol.ReportError(
                protocol.Protocol([
                    protocols.find_device(core.QTime,self.window.error,config),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.device_found)),
                    protocols.config_wireless(
                        core.QTime,self.window.error,config,resources),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.emb6_configured)),
                    protocols.start_part_1a(
                        core.QTime,self.window.error,config),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.emb6_started)),
                    protocols.start_part_1b(self.window.error),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.emb6_connected)),
                    protocols.config_lwm2m(
                        core.QTime,self.window.error,config,resources),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.lwm2m_configured)),
                    protocols.start_part_2a(
                        core.QTime,self.window.error,config),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.lwm2m_started)),
                    protocols.start_part_2b(
                        core.QTime,self.window.error,config,resources),
                    protocol.Checkpoint(
                        lambda: self.set_state(self.lwm2m_connected)),
                    ]),
                lambda: self._stop(None)))

        self.stop_protocol = (lambda: protocols.send_stop_command)


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

        if not self.cli_data['instance_write']:
            self.measurement_protocols = [(resource,
                    (lambda resource: lambda: protocols.measurement(
                        core.QTime, self.config, resource,
                        self.state_widget.setText))
                    (resource))
                    # The first lambda is needed to force local binding of resource.
                    # Counterintuitively, this is not what the for construct
                    # already does.
                    # The second lambda is just deferred computation (which causes
                    # the need for local binding).
                for resource in self.resources.list()
                    if not resource.value_generator.is_constant()]

        if self.cli_data['instance_write']:
            by_object = self.resources.non_constant_by_object()
            self.measurement_protocols = [(by_object[obj],
                    (lambda obj: lambda: protocols.object_measurement(
                        core.QTime, self.config, obj, by_object[obj],
                        self.state_widget.setText))
                    (obj))
                    # The first lambda is needed to force local binding of obj.
                    # Counterintuitively, this is not what the for construct
                    # already does.
                    # The second lambda is just deferred computation.
                for obj in by_object]


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
            self.framer = emb6_frame.Framer(
                self.config.value('port_name'),self.cli_data)
            self.device = emb6.Emb6(self.framer,self.cli_data)
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
            self.stop_protocol().execute(self.device)
            self.framer.stop()
            self.framer = None
        self.window.lock_unlock(False)
        self.set_state(self.stopped)
        if message is not None: self.window.error(message)
        self.window.stopped()


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

        def next_protocol_no_iw():
            counter = 0
            while counter < len(self.measurement_protocols) and not self.config.resource_value(self.measurement_protocols[counter][0].names.config_name, 'check_status'):
                    counter += 1
            if counter < len(self.measurement_protocols):
                self.protocol = protocol.TimeWaiting(
                    self.measurement_protocols[counter][1](),
                    self.wait_timer)
                self.protocol.reset()
                self.in_delay = True
                self.delay_timer.start()
                self.measurement_protocols = \
                    self.measurement_protocols[1 + counter:] \
                    + self.measurement_protocols[:1 + counter]
            else:
                self.protocol = None
                self.window.error(
                    'nothing sent, because no resource is selected')

        def next_protocol_iw():
            counter = 0
            while counter < len(self.measurement_protocols) \
                  and not any(self.config.resource_value(res.names.config_name, 'check_status')
                    for res in self.measurement_protocols[counter][0]):
                counter += 1

            if counter < len(self.measurement_protocols):
                self.protocol = protocol.TimeWaiting(
                    self.measurement_protocols[counter][1](),
                    self.wait_timer)
                self.protocol.reset()
                self.in_delay = True
                self.delay_timer.start()
                self.measurement_protocols = \
                    self.measurement_protocols[1 + counter:] \
                    + self.measurement_protocols[:1 + counter]
            else:
                self.protocol = None
                self.window.error(
                    'nothing sent, because no resource is selected')


        if self.state == self.stopped:
            return
        if self.protocol is None:
            if not self.cli_data['instance_write']:
                next_protocol_no_iw()
            else:
                next_protocol_iw()
        if self.in_delay:
            if self.delay_timer.elapsed() / 1000.0 < \
              self.config.value('update_delay'):
                return
            else: self.in_delay = False
        if self.protocol is not None:
            result,waiting = self.protocol.execute_time_waiting(self.device)
            if result != protocol.error and waiting is None:
                self.window.error("")
            if waiting is not None:
                secs = waiting // 1000
                if secs>0:
                    self.window.error(
                        "Waiting for message since " + str(secs) + "s.")
            if result == protocol.succeed:
                if not self.cli_data['instance_write']:
                    next_protocol_no_iw()
                else:
                    next_protocol_iw()
