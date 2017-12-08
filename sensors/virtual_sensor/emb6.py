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

"""Semantic sugar for emb::6 frame handling.

This module can be used both for scripts and for protocols."""

import protocol
import emb6_frame
import emb6_printing as printing

class UnexpectedResponse(emb6_frame.Emb6Error):
    """Indicate a response that does not follow protocol."""
    def __init__(self,command,actual,expected):
        self.command = command
        self.actual_response = actual
        self.expected_response = expected
    def __str__(self):
        if self.command is None:
            command_text = "(follow-up)"
        else:
            command_text = printing.message(self.command)
        return command_text + " -> " \
          + printing.message(self.actual_response) + " / " \
          + printing.message(self.expected_response)


class Emb6:
    """GAS-level serial communication with an emb::6 device."""

    def __init__(self,framer):
        self.framer = framer
        self.log = False
        self.log_hex = False

    def read_frame(self):
        """Return the next frame (in hex)."""
        frame = self.framer.read_hex()
        while frame==None:
            frame = self.framer.read_hex()
        return frame

    def wait_for_frame(self,expected_frame):
        """Assert that the next frame is a specific one (in hex)."""
        frame = self.read_frame()
        if frame is not None and self.log_hex:
            print "<- ", frame
        if frame is not None and self.log:
            print "<- ", printing.message(frame)
        if frame != expected_frame:
            raise UnexpectedResponse(None, frame, expected_frame)

    def expect_message(self,message,fail_pattern):
        frame = self.framer.read_hex()
        if frame is not None and self.log_hex:
            print "<- ", frame
        if frame is not None and self.log:
            print "<- ", printing.message(frame)
        if frame is None:
            return protocol.wait
        elif frame==message:
            return protocol.succeed
        elif fail_pattern(frame):
            return protocol.error
        else:
            return protocol.wait

    def hex_command(self,command, expected_response = None):
        """Send a command (given in hex).
        
        A specific response may be expected (also given in hex).
        """
        self.framer.write_hex(command)
        if self.log_hex:
            print " ->", command
        if self.log:
            print " ->", printing.message(command)
        if expected_response is not None:
            response = self.read_frame()
            if self.log_hex:
                print "<- ", response
            if self.log:
                print "<- ", printing.message(response)
            if response != expected_response:
                raise UnexpectedResponse(command, response, expected_response)
        return protocol.succeed

    def hex_query(self,command):
        """Send a query (given in hex). Return the response (also in hex)."""
        self.framer.write_hex(command)
        if self.log_hex:
            print " ->", command
        if self.log:
            print " ->", printing.message(command)
        return self.read_frame()

    def ping(self):
        """Execute a ping handshake."""
        self.hex_command("10","0000")

    def start(self):
        """Start communication."""
        self.hex_command("31","0000")
        self.wait_for_frame("4131")
        self.wait_for_frame("e14131")
#        self.hex_command("e131","0000")
        self.wait_for_frame("4132")
        self.wait_for_frame("e14132")

    def stop(self):
        """Stop communication."""
        self.hex_command("30","0000")
        self.wait_for_frame("4130")
        self.wait_for_frame("e14130")


class command(protocol.ProtocolEntry):
    """A protocol.Protocol for sending a specific message.
    
    Uses emb6 as engine."""

    def __init__(self,message):
        self.message = message

    def execute(self,emb6):
        return emb6.hex_command(self.message)


class expect_response(protocol.ProtocolEntry):
    """A protocol.Protocol for receiving a specific message.

    Uses emb6 as engine.
    
    Received messages other than the expected one are ignored,
    unless they match fail_pattern in which case they are treated as errors."""

    def __init__(self, message, fail_pattern = lambda message: False):
        self.message = message
        self.fail_pattern = fail_pattern

    def execute(self,emb6):
        return emb6.expect_message(self.message,self.fail_pattern)

