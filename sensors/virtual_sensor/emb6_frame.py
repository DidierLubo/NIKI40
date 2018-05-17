# encoding: utf-8

# Copyright 2017,2018 NIKI 4.0 project team
#
# NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
# Project partners are FZI Forschungszentrum Informatik am Karlsruher
# Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
# für angewandte Forschung e.V. (www.hahn-schickard.de) and
# Hochschule Offenburg (www.hs-offenburg.de).
# This file was developed by Mark Weyer at Hahn-Schickard. It is based on
# serial-emulator.py by Manuel Schappacher at Hochschule Offenburg.
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

"""Low-level emb::6 frame management."""

import sys
import binascii

import serial
import crcmod

import logging


class Emb6Error(Exception):
    """Virtual base class of all emb::6-related errors."""

class PortUnavailable(Emb6Error):
    """Indicate an error during opening the serial port."""

class IncompleteFrame(Emb6Error):
    """Indicate that a frame was not timely received."""

class InconsistentLength(Emb6Error):
    """Indicate that a frame's length and inverted length do not match."""

class CRCError(Emb6Error):
    """Indicate an invalid checksum."""


_crcfunc = crcmod.mkCrcFun(poly=0x13d65, initCrc=0, rev=False)

def calc_crc(bytes):
    crc = _crcfunc(bytes)
    crc = ~crc
    crc = bytearray([(crc >> i & 0xff) for i in (8,0)])
    return crc


class Framer:
    """Frame-level serial communication with an emb::6 device.
    
    """


    def __init__(self, portname, cli_dict):
        self.cli_dict = cli_dict

        try: 
            ser = serial.Serial(
                port = portname,
                baudrate = 115200,
                parity = serial.PARITY_NONE,
                stopbits = serial.STOPBITS_ONE,
                bytesize = serial.EIGHTBITS,
                xonxoff = False,
                rtscts = False,
                dsrdtr = False,
                timeout = 0.2
            )
            ser.close()
            ser.open()
        except Exception, e:
            print "error open serial port: " + str(e)
            raise PortUnavailable()

        if ser.isOpen():

            try:
                #flush input buffer, discarding all its contents
                ser.flushInput()
                #flush output buffer, aborting current output 
                #and discard all that is in buffer
                ser.flushOutput()
                self.port = ser

            except Exception, e1:
                print "error communicating...: " + str(e1)
                raise PortUnavailable()

        else:
            print "cannot open serial port "
            raise PortUnavailable()


    def stop(self):
        self.port.__exit__()
        del self.port


    def write(self,bytes):
        """Wrap a raw message into a frame and send it.
        
        """
        # Create SOF and length
        sof = bytearray([0xA5])
        length = bytearray([len(bytes) / 256, len(bytes) % 256])
        if self.cli_dict.get('inverted_length_field'):
            length += bytearray([x ^ 0xFF for x in length])

        # Create CRC from data
        crc = calc_crc(bytes)

        # Assemble complete frame
        frame = sof + length + bytes + crc
        if self.cli_dict.get('log_bytes'): logging.timestamp(self.cli_dict) + "  ->", binascii.hexlify(frame)
        if self.cli_dict.get('log_frames'):
            print logging.timestamp(self.cli_dict) + "  ->", binascii.hexlify(sof), \
                binascii.hexlify(length), \
                binascii.hexlify(bytes), \
                binascii.hexlify(crc)
        self.port.write(frame)

    def write_hex(self,message):
        """Wrap a hex message into a frame and send it.
        
        """
        self.write(binascii.unhexlify(message))

    def read_bytes(self,length):
        bytes = self.port.read(length)
        if self.cli_dict.get('log_bytes'): print logging.timestamp(self.cli_dict) + "<- ", binascii.hexlify(bytes)
        return bytes

    def read(self):
        """Read a frame and return the raw payload.
        
        """
        magic = bytearray(self.read_bytes(1))
        if len(magic) == 1:
            if magic[0] != 0xA5:
                # We are outside of any frame.
                # Whatever we read here is whatever.
                if self.cli_dict.get('log_garbage'):
                    sys.stderr.write(magic)
                    sys.stderr.flush()
                return None
            else:
                len_code = bytearray(self.read_bytes(2))
                if len(len_code) != 2:
                    raise IncompleteFrame()

		if self.cli_dict.get('inverted_length_field'):
                    leni_code = bytearray(self.read_bytes(2))
                    if len(leni_code) != 2:
                        raise IncompleteFrame()

		    len_code_inv = bytearray([x ^ 0xFF for x in len_code])
		    if leni_code != len_code_inv:
                        raise InconsistentLength()

                    len_code += leni_code

                length = len_code[0]*256+len_code[1]
                payload = self.read_bytes(length)
                if len(payload) != length:
                    raise IncompleteFrame()
                crc = bytearray(self.read_bytes(2))
                if len(crc) != 2:
                    raise IncompleteFrame()
                if crc != calc_crc(payload):
                    raise CRCError()
                if self.cli_dict.get('log_frames'):
                    print logging.timestamp(self.cli_dict) + " <- ", binascii.hexlify(magic), \
                      binascii.hexlify(len_code), binascii.hexlify(payload), \
                      binascii.hexlify(crc)
                return payload
        else: return None

    def read_hex(self):
        """Read a frame and return the payload in hex.
        
        """
        frame = self.read()
        if frame is None: return None
        else: return binascii.hexlify(frame)

