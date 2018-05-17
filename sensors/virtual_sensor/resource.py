# encoding: utf-8

# Copyright 2017,2018 NIKI 4.0 project team
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

"""Data structures for representing Resources and their parsing."""

import collections
import math
import json
import types

import waveform
import enum


class ValueGeneratorNotInitialized(Exception): pass
class SensorDefinitionReadError(Exception): pass
class SensorDefinitionFileError(SensorDefinitionReadError): pass
class SensorDefinitionParseError(SensorDefinitionReadError): pass
class SensorDefinitionTypeError(SensorDefinitionReadError): pass
class SensorDefinitionInternalError(SensorDefinitionReadError): pass


ui_type_float, ui_type_enum, ui_type_string, ui_type_bool = range(4)
"""Designate to the UI the type of UI-controlled fields."""

UiField = collections.namedtuple('UiField',
    ['ui_name', 'config_name', 'type', 'default', 'extra_data'])
"""Holds all the UI needs to known about a UI-controlled field."""

FloatExtraData = collections.namedtuple('FloatExtraData',
    ['range_min','range_max'])
"""Holds the extra data for UI-controlled fields of type float."""

EnumExtraData = collections.namedtuple('EnumExtraData',
    ['enum'])
"""Holds the extra data for UI-controlled enum fields."""

StringExtraData = collections.namedtuple('StringExtraData',
    ['max_length'])
"""Holds the extra data for UI-controlled string fields."""


class ValueGenerator:
    """Abstract base class of value generators for resources."""

    def initialize(self, timer, config): pass
    def is_constant(self): pass
    def value(self): pass

class ConstantValueGenerator(ValueGenerator):
    """The value is constant."""

    def __init__(self, the_value):
        self.c = the_value

    def initialize(self, timer, config): pass
    def is_constant(self): return True

    def value(self):
        return self.c

class EnumValueGenerator(ValueGenerator):

    def __init__(self, config_resource_name, config_field_name, enum):
        self.resource = config_resource_name
        self.field = config_field_name
        self.enum = enum

    def initialize(self, timer, config):
        self.config = config

    def is_constant(self): return False

    def value(self):
        name = self.config.resource_value(self.resource, self.field)
        return self.enum.value_map[name]

class WaveValueGenerator(EnumValueGenerator):
    """Based on waveforms."""

    def __init__(self, config_resource_name, waveforms):
        self.resource = config_resource_name
        self.waveform = \
          EnumValueGenerator(config_resource_name, 'waveform', waveforms)
        self.initialized = False

    def initialize(self, timer, config):
        self.waveform.initialize(timer, config)
        self.config = config
        self.time = timer
        self.time.start()
        self.last_f = config.resource_value(self.resource,'frequency') / 1000.0
        self.dt = 0.0
        self.initialized = True

    def is_constant(self): return False

    def value(self):
        if not self.initialized:
            raise ValueGeneratorNotInitialized()

        t = self.time.elapsed() / 1000.0
        x = ((t + self.dt)*self.last_f) % 1.0
        f = self.config.resource_value(self.resource,'frequency') / 1000.0
        if f != self.last_f:
            # rebase the time so assure continuity
            self.dt = x/f - t
        self.last_f = f
        return self.waveform.value()(x)


class WaveFloatValueGenerator(WaveValueGenerator):

    def __init__(self, config_resource_name):
        WaveValueGenerator.__init__(self, config_resource_name, waveform.float)

    def value(self):
        y0 = self.config.resource_value(self.resource,'wave_min')
        y1 = self.config.resource_value(self.resource,'wave_max')
        return y0 + (y1-y0) * WaveValueGenerator.value(self)

class WaveIntValueGenerator(WaveFloatValueGenerator):
    """Like WaveFloatValueGenerator, but rounds to ints."""

    def __init__(self, config_resource_name):
        WaveFloatValueGenerator.__init__(self, config_resource_name)

    def value(self):
        return int(math.floor(WaveFloatValueGenerator.value(self)+0.5))

class FreeformStringValueGenerator(ValueGenerator):
    """The value is the config value."""
    
    def __init__(self, config_resource_name):
        self.resource = config_resource_name
    
    def initialize(self, timer, config):
        self.config = config

    def is_constant(self): return False
    
    def value(self):
        return self.config.resource_value(self.resource, 'value')


resource_type_BOOL = 0
resource_type_INT = 1
resource_type_FLOAT = 2
resource_type_STRING = 3
resource_type_METHOD = 4

Names = collections.namedtuple('Names', ['ui_name', 'config_name'])

noname = Names('','')

Resource = collections.namedtuple('Resource',
    ['obj_id', 'res_id', 'type', 'size',
        'names', 'ui_fields', 'value_generator'])
"""Defines a single resource."""


def constant_float_resource(obj, res, value):
    return Resource(obj, res, resource_type_FLOAT, 4, noname, [],
        ConstantValueGenerator(value))

def constant_string_resource(obj, res, value):
    return Resource(obj, res, resource_type_STRING, len(value)+1, noname, [],
        ConstantValueGenerator(value))

def ui_bool_resource(obj, res, names):
    return Resource(obj, res, resource_type_BOOL, 4, names,
        [
            UiField('Update', 'check_status', ui_type_bool, True, []),
            UiField('Frequency / mHz', 'frequency', ui_type_float, 25,
                FloatExtraData(1, 10000)),
            UiField('Waveform', 'waveform', ui_type_enum, "Alternating",
                EnumExtraData(waveform.bool)),
            ],
        WaveValueGenerator(names.config_name, waveform.bool))

def ui_int_resource(obj, res, names,
        range_min, range_max, default_min, default_max):
    return Resource(obj, res, resource_type_INT, 4, names,
        [
            UiField('Update', 'check_status', ui_type_bool, True, []),
            UiField('min', 'wave_min', ui_type_float, default_min,
                FloatExtraData(range_min, range_max)),
            UiField('max', 'wave_max', ui_type_float, default_max,
                FloatExtraData(range_min, range_max)),
            UiField('Frequency / mHz', 'frequency', ui_type_float, 25,
                FloatExtraData(1, 10000)),
            UiField('Waveform', 'waveform', ui_type_enum, "Sine",
                EnumExtraData(waveform.float)),
            ],
        WaveIntValueGenerator(names.config_name))

def ui_float_resource(obj, res, names,
        range_min, range_max, default_min, default_max):
    return Resource(obj, res, resource_type_FLOAT, 4, names,
        [

            UiField('Update', 'check_status', ui_type_bool, True, []),
            UiField('min', 'wave_min', ui_type_float, default_min,
                FloatExtraData(range_min, range_max)),
            UiField('max', 'wave_max', ui_type_float, default_max,
                FloatExtraData(range_min, range_max)),
            UiField('Frequency / mHz', 'frequency', ui_type_float, 25,
                FloatExtraData(1, 10000)),
            UiField('Waveform', 'waveform', ui_type_enum, "Sine",
                EnumExtraData(waveform.float)),
            ],
        WaveFloatValueGenerator(names.config_name))


def get_entry(dict,field,types):
    if field in dict:
        value = dict[field]
        if type(value) in types:
            return value
        else:
            raise SensorDefinitionTypeError()
    else:
        raise SensorDefinitionParseError()

def get_names(dict):
    if 'name' in dict:
        name = get_entry(dict,'name',string_types)
        return Names(name,name)
    else:
        return Names(
            get_entry(dict,'UI name',string_types),
            get_entry(dict,'config name',string_types))

int_types = [types.IntType]
float_types = [types.IntType, types.FloatType]
string_types = [types.UnicodeType]


def ui_enum_resource(obj, res, names, values):
    def parse_pair(pair):
        if type(pair)==types.DictType:
            return (
                get_entry(pair,'name',string_types),
                get_entry(pair,'value',int_types))
        else:
            raise SensorDefinitionTypeError()

    the_enum = enum.Enum([parse_pair(pair) for pair in values])
    return Resource(obj, res, resource_type_INT, 4, names,
        [
            UiField('Update', 'check_status', ui_type_bool, True, []),
            UiField('Value', 'value', ui_type_enum, the_enum.list[0][0],
                EnumExtraData(the_enum))],
        EnumValueGenerator(names.config_name, 'value', the_enum))


def ui_string_resource(obj, res, names, max_length, default):
    return Resource(
        obj, res, resource_type_STRING, max_length+1, names,
        [
            UiField('Update', 'check_status', ui_type_bool, True, []),
            UiField('Value', 'value', ui_type_string, default,
                StringExtraData(max_length)),
            ],
        FreeformStringValueGenerator(names.config_name))


class Resources:
    """Manages the available resources.
    
    Fields:
    resources: list of all resources
    non_constant: dict mapping object ids to lists of non-constant resources
    """

    def __init__(self):
        self.resources = []
        self.non_constant = {}

    def load(self,filename):
        self.resources = []
        try:
            with open(filename,'r') as file:
                sensor_def = json.load(file)
        except IOError:
            raise SensorDefinitionFileError()
        except ValueError:
            raise SensorDefinitionParseError()
        if type(sensor_def)==types.ListType:
            for resource_def in sensor_def:
                if type(resource_def)==types.DictType:

                    def get_field(field, types):
                        return get_entry(resource_def, field, types)

                    obj_id = get_field("obj", int_types)
                    res_id = get_field("res", int_types)

                    type_ = get_field("type", string_types)
                    value = get_field("value", string_types)
                    if type_=='bool' and value=='wave':
                        resource = ui_bool_resource(obj_id, res_id,
                            get_names(resource_def))
                    elif type_=='int' and value=='wave':
                        resource = ui_int_resource(obj_id, res_id,
                            get_names(resource_def),
                            get_field("range min", float_types),
                            get_field("range max", float_types),
                            get_field("default min", float_types),
                            get_field("default max", float_types))
                    elif type_=='int' and value=='enum':
                        resource = ui_enum_resource(obj_id, res_id,
                            get_names(resource_def),
                            get_field("values", [types.ListType]))
                    elif type_=='float' and value=='wave':
                        resource = ui_float_resource(obj_id, res_id,
                            get_names(resource_def),
                            get_field("range min", float_types),
                            get_field("range max", float_types),
                            get_field("default min", float_types),
                            get_field("default max", float_types))
                    elif type_=='float' and value=='constant':
                        resource = constant_float_resource(obj_id, res_id,
                            get_field("constant", float_types))
                    elif type_=='string' and value=='freeform':
                        resource = ui_string_resource(obj_id, res_id,
                            get_names(resource_def),
                            get_field("max length", int_types),
                            get_field("default", string_types))
                    elif type_=='string' and value=='constant':
                        resource = constant_string_resource(obj_id, res_id,
                            get_field("constant", string_types))
                    else:
                        raise SensorDefinitionParseError()

                    self.resources.append(resource)
                else:
                    raise SensorDefinitionTypeError()
        else:
            raise SensorDefinitionTypeError()

        # Now, group the non-constant resources by object id
        self.non_constant = {}
        for r in self.resources:
            if not r.value_generator.is_constant():
                if r.obj_id in self.non_constant:
                    self.non_constant[r.obj_id].append(r)
                else:
                    self.non_constant[r.obj_id] = [r]

    def list(self):
        return self.resources

    def non_constant_by_object(self):
        return self.non_constant

