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

"""Defines the protocols used by the virtual sensor."""

import struct
import binascii

import config
import protocol
import emb6
import resource as res


class UnsupportedResourceType(Exception): pass


timeout = 3000

expect_ok = emb6.expect_response(
    "0000",
    lambda message: message in ["0001", "0002"])

expect_lwm2m_ok = emb6.expect_response(
    "e10000",
    lambda message: message in ["0001", "0002", "e10001", "e10002"])


def parse_adress(blocks,block_length,s):
    result = '0' * (block_length * blocks)
    block = blocks-1
    source_index = len(s)-1
    dest_index = len(result)-1
    while source_index>=0:
        c = s[source_index]
        if c == ':':
            dest_index = block*block_length-1
            block -= 1
        else:
            result = result[:dest_index] + c + result[dest_index+1:]
            dest_index -= 1
        source_index -= 1
    return result.lower()

def parse_ip(s): return parse_adress(8,4,s)
def parse_mac(s): return parse_adress(8,2,s)


def ping(timer):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("10"),
            expect_ok,
            ]),
        timer, timeout)

def stop(timer):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("30"),
            expect_ok,
            emb6.expect_response("4130",),
            emb6.expect_response("e14130"),
            ]),
        timer, timeout)

def lwm2m_reset(timer):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("e13f"),
            expect_lwm2m_ok,
            ]),
        timer, timeout)

def set_conf_par(timer, conf_par, value):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("20" + conf_par + value),
            expect_ok,
            emb6.command("21" + conf_par),
            emb6.expect_response("22" + conf_par + value,
                lambda m: m[:2]=="22"),
            ]),
        timer, timeout)

def configure_part_1(gen_timer,config):
    return protocol.Protocol([
        set_conf_par(gen_timer(), "00",
            parse_mac(config.value('client_mac'))),
        set_conf_par(gen_timer(), "03",
            binascii.hexlify(bytearray([config.value('slip_channel')]))),
        ])

def set_lwm2m_conf_par(timer, conf_par, value):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("e120" + conf_par + value),
            expect_lwm2m_ok,
            emb6.command("e121" + conf_par),
            emb6.expect_response("e122" + conf_par + value,
                lambda m: m[:2]=="e122"),
            ]),
        timer, timeout)

def create_object(timer, obj, inst):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("e160"
                + binascii.hexlify(bytearray(
                    [obj//256, obj%256, inst]))),
            emb6.expect_response("e162"
                + binascii.hexlify(bytearray(
                    [obj//256, obj%256, inst]))
                + "00"),
            ]),
        timer, timeout)

def create_resource(timer, obj, inst, res, type_, size, mod):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("e170"
                + binascii.hexlify(bytearray(
                    [obj//256, obj%256, inst, res//256, res%256,
                        type_, size, mod]))),
            emb6.expect_response("e172"
                + binascii.hexlify(bytearray(
                    [obj//256, obj%256, inst, res//256, res%256, 0]))),
            ]),
        timer, timeout)

def configure_part_2(gen_timer,config,resources):
    port = config.value('host_port')
    objects = {resource.obj_id for resource in resources.list()}
    return protocol.Protocol([
        set_lwm2m_conf_par(gen_timer(), "02",
            parse_ip(config.value('host_ip'))),
        set_lwm2m_conf_par(gen_timer(), "03",
            binascii.hexlify(bytearray([port//256,port%256]))),
        set_lwm2m_conf_par(gen_timer(), "04",
            binascii.hexlify(config.value('client_name'))),
        ]
        + [create_object(gen_timer(), obj, 0) for obj in objects]
        + [create_resource(gen_timer(), resource.obj_id, 0, resource.res_id,
                resource.type, resource.size, 0)
            for resource in resources.list()])

def set_string_resource(timer, obj, inst, res, value):
    return protocol.OnTimeoutRetry(
        protocol.Protocol([
            emb6.command("e182"
                + binascii.hexlify(bytearray(
                  [obj//256, obj%256, inst, res//256, res%256]))
                + binascii.hexlify(value)),
            expect_lwm2m_ok,
            ]),
        timer, timeout)

def set_int_resource(timer, obj, inst, res, value):
    if value<0:
        value += 4294967296
    return set_string_resource(timer, obj, inst, res, bytearray(
        [value/16777216, (value/65536)%256, (value/256)%256, value%256]))

def set_bool_resource(timer, obj, inst, res, value):
    if value:
        int = 1
    else:
        int = 0
    return set_int_resource(timer, obj, inst, res, int)

def set_float_resource(timer, obj, inst, res, value):
    return set_string_resource(timer, obj, inst, res, struct.pack(">f",value))

def set_resource(gen_timer, resource, inst, value):
    if resource.type == res.resource_type_BOOL:
        handler = set_bool_resource
    elif resource.type == res.resource_type_INT:
        handler = set_int_resource
    elif resource.type == res.resource_type_FLOAT:
        handler = set_float_resource
    elif resource.type == res.resource_type_STRING:
        handler = set_string_resource
    else:
        raise UnsupportedResourceType
    return (handler(gen_timer(), resource.obj_id, inst, resource.res_id, value))

def init_resources(gen_timer,resources):
    return protocol.Protocol([
            set_resource(
                gen_timer, resource, 0, resource.value_generator.value())
        for resource in resources.list()])

def find_device(gen_timer,error):
    """Ensures that the device is listening to serial communication."""
    return protocol.ReportError(
        ping(gen_timer()),
        lambda: error("Ping failed."))

def config_wireless(gen_timer,error,config,resources):
    """Configures everything except maybe LWM2M stuff."""
    return protocol.ReportError(
        protocol.Protocol([
            stop(gen_timer()),
            configure_part_1(gen_timer,config),
            ]),
        lambda:
            error("Wireless configuration failed."))

def start_part_1a(gen_timer,error):
    """Starts non-LWM2M"""
    return protocol.ReportError(
        protocol.OnTimeoutRetry(
            protocol.Protocol([
                emb6.command("31"),
                emb6.expect_response("4131"),
                ]),
            gen_timer(), timeout),
        lambda:
            error("Device start failed."))

def start_part_1b(error):
    """Confirms that non-LWM2M startup succeeded."""
    return protocol.ReportError(
        emb6.expect_response("4132"),
        lambda:
            error("Device start failed."))

def config_lwm2m(gen_timer,error,config,resources):
    """Configures LWM2M stuff."""
    return protocol.ReportError(
        protocol.Protocol([
            lwm2m_reset(gen_timer()),
            configure_part_2(gen_timer,config,resources),
            init_resources(gen_timer,resources),
            ]),
        lambda:
            error("LWM2M configuration failed."))

def start_part_2a(gen_timer,error):
    """Start LWM2M."""
    return protocol.ReportError(
        protocol.OnTimeoutRetry(
            protocol.Protocol([
                emb6.command("e131"),
                emb6.expect_response("e14131"),
                ]),
            gen_timer(), timeout),
        lambda:
            error("LWM2M start failed."))

def start_part_2b(gen_timer,resources,error):
    """Confirms that LWM2M startup succeeded."""
    return protocol.ReportError(
        protocol.Protocol([
            emb6.expect_response("e14132"),
            ]),
        lambda:
            error("LWM2M start failed."))

def measurement(gen_timer,resource,report):
    """Sends a virtual measurement."""
    value = resource.value_generator.value()
    return protocol.Protocol([
        protocol.Checkpoint(
            lambda: report(
                "Sending: "
                + resource.ui_name
                + " = "
                + str(value))),
        set_resource(gen_timer, resource, 0, value),
        ])

