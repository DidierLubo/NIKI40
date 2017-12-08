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

"""Abstract protocol management."""

succeed, wait, error = range(3)
"""Protocol return codes."""

class Internal(Exception): pass


class ProtocolEntry:
    """Abstract class for one step of a protocol."""

    def reset(self): pass

    def execute(self,engine):
        """Returns a protocol return code."""

    def position(self):
        """Return the (abstract) current position inside the protocol.
        
        The return type is not specified, but it must allow comparisons.
        Later positions must compare as larger.
        
        The use case is to determine progress."""


class Checkpoint(ProtocolEntry):
    """Calls an external action. Always succeeds."""

    def __init__(self,action):
        self.action = action

    def execute(self,engine):
        self.action()
        return succeed

    def position(self):
        return ()


class ReportError(ProtocolEntry):
    """Gives semantics to return value error.
    
    Encapsulates a ProtocolEntry.
    The error return value triggers an external reporting function."""

    def __init__(self,sub,report):
        self.sub = sub
        self.report = report

    def execute(self,engine):
        result = self.sub.execute(engine)
        if result == error:
            self.report()
        return result

    def reset(self):
        self.sub.reset()

    def position(self):
        return self.sub.position()


class Protocol(ProtocolEntry):
    """Sequentially operates on a list of ProtocolEntry."""

    def __init__(self,entries):
        self.entries = entries
        self.reset()
        self.index = -1

    def reset(self):
        for entry in self.entries:
            entry.reset()
        if len(self.entries)>0:
            self.index = 0
        else:
            self.index = -1

    def execute(self,engine):
        while 0 <= self.index < len(self.entries):
            result = self.entries[self.index].execute(engine)
            if result == succeed:
                self.index += 1
            else:
                return result
        return succeed

    def position(self):
        if 0 <= self.index < len(self.entries):
            return (self.index, self.entries[self.index].position())
        else:
            return (self.index, ())


class Timer:
    """Abstract class for timing as needed by TimeWaiting."""

    def start(self): pass

    def elapsed(self):
        """The return type may depend on the implementation."""


class TimeWaiting(ProtocolEntry):
    """Enhanced ProtocolEntry that determines the time spent waiting."""

    def __init__(self,sub,timer):
        self.sub = sub
        self.timer = timer

    def reset(self):
        self.sub.reset()
        self.timer.start()

    def execute(self,engine):
        return self.sub.execute(engine)

    def execute_time_waiting(self,engine):
        """Like execute, but also returns the time spent waiting.
        
        A pair is returned.
        The first component what execute would have returned.
        The second component is None, if progress has been made.
        Otherwise it is the total time spent waiting since the last progress."""

        position_before = self.sub.position()
        result = self.sub.execute(engine)
        position_after = self.sub.position()
        if position_before < position_after:
            self.timer.start()
            return (result, None)
        else:
            return (result, self.timer.elapsed())

    def position(self):
        return self.sub.position()


class OnTimeoutRetry(ProtocolEntry):
    """Will reset the sub ProtocolEntry when waiting too long."""

    def __init__(self,sub,timer,timeout):
        """timer must be a Timer. The return type of its elapsed method must
        be the same as the type of timeout. This type must allow comparisons."""
        self.sub = TimeWaiting(sub,timer)
        self.timeout = timeout

    def reset(self):
        self.sub.reset()
        self.best_position = self.sub.position()

    def execute(self,engine):
        result,waiting = self.sub.execute_time_waiting(engine)
        position = self.sub.position
        if position > self.best_position:
            self.best_position = position
        if waiting is not None and waiting > self.timeout:
            self.sub.reset()
        return result

    def position(self):
        return self.best_position

