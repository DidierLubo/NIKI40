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

"""Pretty-printing of emb::6 frames."""

import binascii
import collections

class PrintingError(Exception):
    """Virtual base class of all printing-related errors."""

class TooLittlePayload(PrintingError):
    """Indicate a frame which is too short for the required arguments."""

class TooMuchPayload(PrintingError):
    """Indicate a frame which contains excess data."""

class Internal(PrintingError): pass
class NonLastFlexibleSize(Internal): pass


ArgumentPrinter = \
    collections.namedtuple('ArgumentPrinter',['len','sub_printer'])
# len is an int. Value zero denotes a flexible-size argument which takes the
# rest of the payload. Obviously, such a one must be the last argument.

CasePrinter = collections.namedtuple('CasePrinter',['name','arg_printers'])

def print_with_args(case_printer,left,payload,right):
    # left and right are parentheses (or something similar) to be put around
    # the argument list.
    result = case_printer.name
    for i in range(len(case_printer.arg_printers)):
        if i==0:
            result = result + left
        last = i == len(case_printer.arg_printers) - 1
        arg_len = case_printer.arg_printers[i].len
        if arg_len > len(payload):
            raise TooLittlePayload()
        if arg_len == 0:
            if not last:
                raise NonLastFlexibleSize()
            arg_len = len(payload)
        result = result + \
            case_printer.arg_printers[i].sub_printer(payload[:arg_len])
        payload = payload[arg_len:]
        if last:
            result = result + right
        else:
            result = result + ','
    if len(payload)>0:
        raise TooMuchPayload()
    return result

def print_cases(cases_printers,left,payload,right):
    # cases_printers is a dict mapping bytes to CasePrinter.
    if len(payload)==0:
        raise TooLittlePayload()
    if payload[0] in cases_printers:
        return print_with_args(
            cases_printers[payload[0]], left, payload[1:], right)
    else:
        return "(unknown: " + binascii.hexlify(payload) + ")"

def enum_printer(enum):
    # enum is a dict mapping bytes to strings.
    printer_dict = {
        key : CasePrinter(enum[key],[])
        for key in enum}
    return ArgumentPrinter(1,
        lambda payload: print_cases(printer_dict,'',payload,''))


response_printer = enum_printer({
    0x00: "OK",
    0x01: "ERROR",
    0x02: "ERROR_CMD",
    0x03: "ERROR_PARAM",
    0x30: "STATUS_STOPPED",
    0x31: "STATUS_STARTED",
    0x32: "STATUS_NETWORK",
    })

lwm2m_response_printer = enum_printer({
    0x00: "OK",
    0x01: "ERROR",
    0x02: "ERROR_CMD",
    0x03: "ERROR_PARAM",
    0x30: "STATUS_STOPPED",
    0x31: "STATUS_STARTED",
    0x32: "STATUS_REGISTERED",
    })

lwm2m_resource_type_printer = enum_printer({
    0x00: "BOOL",
    0x01: "INT",
    0x02: "FLOAT",
    0x03: "STRING",
    0x04: "METHOD",
})


hex_printer = ArgumentPrinter(0, lambda payload: binascii.hexlify(payload))
int8_printer = ArgumentPrinter(1, lambda payload: str(payload[0]))
int16_printer = ArgumentPrinter(2,
    lambda payload: str(payload[0]*256+payload[1]))

def colonize(block_length,payload):
    short = binascii.hexlify(payload)
    value = short[0:block_length]
    for i in range(1,len(short)//block_length):
        value += ":" + short[i*block_length : (i+1)*block_length]
    return value

ip_printer = ArgumentPrinter(16, lambda payload: colonize(4,payload))
mac_printer = ArgumentPrinter(8, lambda payload: colonize(2,payload))
string_printer = ArgumentPrinter(0, lambda payload: payload[:])

conf_par_printers = {
    0x00: CasePrinter("MACADDR", [mac_printer]),
    0x01: CasePrinter("PANID", [int16_printer]),
    0x03: CasePrinter("CHANNEL", [int8_printer]),
    }

lwm2m_conf_par_printers = {
    0x00: CasePrinter("BS_SRV_IP", [ip_printer]),
    0x01: CasePrinter("BS_SRV_PORT", [int16_printer]),
    0x02: CasePrinter("SRV_IP", [ip_printer]),
    0x03: CasePrinter("SRV_PORT", [int16_printer]),
    0x04: CasePrinter("CLI_NAME", [string_printer]),
}


def conf_printer(par_printers, with_value):
    if with_value:
        len = 0
        printers = par_printers
    else:
        len = 1
        printers = {
                key: CasePrinter(par_printers[key].name, [])
            for key in par_printers}
    return ArgumentPrinter(len,
        lambda payload: print_cases(printers, ' = ', payload, ''))


def lwm2m(payload):
    return print_cases(
        {
            0x00: CasePrinter("RET", [lwm2m_response_printer]),
            0x20: CasePrinter("CFG_SET",
                [conf_printer(lwm2m_conf_par_printers,True)]),
            0x21: CasePrinter("CFG_GET",
                [conf_printer(lwm2m_conf_par_printers,False)]),
            0x22: CasePrinter("CFG_RET",
                [conf_printer(lwm2m_conf_par_printers,True)]),
            0x31: CasePrinter("START", []),
            0x3f: CasePrinter("RESET", []),
            0x41: CasePrinter("STATUS_RET", [lwm2m_response_printer]),
            0x60: CasePrinter("OBJ_CREATE", [int16_printer, int8_printer]),
            0x62: CasePrinter("OBJ_RET",
                [int16_printer, int8_printer, lwm2m_response_printer]),
            0x70: CasePrinter("RES_CREATE",
                [int16_printer, int8_printer, int16_printer,
                    lwm2m_resource_type_printer,
                    int8_printer, int8_printer]),
            0x72: CasePrinter("RES_RET",
                [int16_printer, int8_printer, int16_printer,
                    lwm2m_response_printer]),
            0x82: CasePrinter("RES_WR_REQ",
                [int16_printer, int8_printer, int16_printer, hex_printer]),
            },
        '(', payload, ')')

def message(payload_hex):
    """interpret an emb6 message and return its description"""
    if len(payload_hex) == 0:
        raise TooLittlePayload
    payload = bytearray(binascii.unhexlify(payload_hex))
    if payload[0] == 0xe1:
        return "LWM2M_" + lwm2m(payload[1:])
    else:
        return print_cases(
            {
                0x00: CasePrinter("RET", [response_printer]),
                0x10: CasePrinter("PING", []),
                0x20: CasePrinter("CFG_SET",
                    [conf_printer(conf_par_printers,True)]),
                0x21: CasePrinter("CFG_GET",
                    [conf_printer(conf_par_printers,False)]),
                0x22: CasePrinter("CFG_RET",
                    [conf_printer(conf_par_printers,True)]),
                0x30: CasePrinter("DEVICE_STOP", []),
                0x31: CasePrinter("DEVICE_START", []),
                0x41: CasePrinter("STATUS_RET", [response_printer]),
                },
            '(', payload, ')')

