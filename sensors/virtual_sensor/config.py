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

"""Configuration data management."""

import types
import sys
import json

filename = "virtual_sensor_config.json"
resources_key = 'resources'

def default_portname_prefix():
    if sys.platform.startswith("linux"): return "/dev/ttyUSB"
    elif sys.platform.startswith("win"): return "COM"
    else: return ""

default_slip_channel = 26
default_ip = "bbbb:0000:0000:0000:0123:4567:89ab:cdef"
default_host_port = 5683
default_client_mac = "0123456789abcdef"
default_client_name = "Virtual sensor"
default_sensor_definition_filename = "sensor_definition.json"
default_timeout = 500


class Config:
    """Holds the values which are stored in the config file.
    
    The class also provides means to synchronize these data with the UI."""

    def __init__(self, cli_data):
        """Initialize the data to default values, then try to read the
        config file."""

        self.widget_updates = []
        cli_sensor = cli_data.get('filename')
        cli_timeout = cli_data.get('timeout')
        self.dict = {
            'port_name': default_portname_prefix(),
            'slip_channel': default_slip_channel,
            'host_ip': default_ip,
            'host_port': default_host_port,
            'client_name': default_client_name,
            'client_mac': default_client_mac,
            'update_delay': 1.0,
            'sensor_definition_filename': default_sensor_definition_filename,
            'timeout': default_timeout,
            resources_key: {},
            }
        self.resource_widget_updates = []
        # Next we read the configuration file a first time. As
        # self.dict[resources_key] is empty, no resource-specific data will be
        # loaded.
        self.load()
        if cli_sensor is not None:
            self.dict['sensor_definition_filename'] = cli_sensor
        if cli_timeout is not None and not cli_timeout < 0:
            self.dict['timeout'] = cli_timeout


    def init_resources(self,resources):
        """Do the resource-specific part of initialization.

        This has to be done (and redone) after the resource definition file
        has been loaded."""
        self.resource_widget_updates = []
        for resource in resources.list():
            if not resource.value_generator.is_constant():
                field_dict = {}
                for field in resource.ui_fields:
                    field_dict[field.config_name] = field.default
                self.dict[resources_key][resource.names.config_name] = field_dict
        # Now we can initialize resource-specific data. We do not re-initialize
        # the previously initialized global data lest we overwrite changes that
        # have been done in the meantime.
        self.dict[resources_key] = merge_dicts(
            self.dict[resources_key], self.loaded_dict[resources_key])

    def value(self,key):
        """Access a global value."""
        return self.dict[key]

    def _set(self,key,value):
        self.dict[key] = value

    def resource_value(self,resource,key):
        """Access a value pertaining to a resource"""
        return self.dict[resources_key][resource][key]

    def _set_resource(self,resource,key,value):
        self.dict[resources_key][resource][key] = value

    def connect(self, key, widget_changed_connect, widget_change_slot):
        """Connect a global value to some widget.
        
        widget_changed_connect is the connect method of the widget's
        valueChanged signal: a function which takes a setter function
        as input and registers it with the widget, such that the setter
        function is called whenever the value in the widget changes.
        
        widget_change_slot is a function which takes a value and updates the
        widget to the new value. It will be called whenever the configuration
        data changes from other sources than the widget; e.g. from loading the
        config file. 
        It is also called right away for an initial synchronization.
        """
        widget_change_slot(self.value(key))
        widget_changed_connect(lambda value: self._set(key,value))
        self.widget_updates.append(
            lambda: widget_change_slot(self.value(key)))

    def connect_resource(self, resource, key,
            widget_changed_connect, widget_change_slot):
        """Connect a value pertaining to a resource to some widget.
        
        widget_changed_connect is the connect method of the widget's
        valueChanged signal: a function which takes a setter function
        as input and registers it with the widget, such that the setter
        function is called whenever the value in the widget changes.
        
        widget_change_slot is a function which takes a value and updates the
        widget to the new value. It will be called whenever the configuration
        data changes from other sources than the widget; e.g. from loading the
        config file. 
        It is also called right away for an initial synchronization.
        """
        widget_change_slot(self.resource_value(resource,key))
        widget_changed_connect(
            lambda value: self._set_resource(resource,key,value))
        self.resource_widget_updates.append(
            lambda: widget_change_slot(self.resource_value(resource,key)))

    def save(self):
        """Write the configuration file."""
        with open(filename,'w') as file:
            json.dump(self.dict, file, indent=2)

    def load(self):
        """Read the configuration file."""
        try:
            with open(filename,'r') as file:
                self.loaded_dict = json.load(file)
        except Exception:
            self.loaded_dict = { resources_key: {}}
        self.dict = merge_dicts(self.dict, self.loaded_dict)
        for widget_update in self.widget_updates:
            widget_update()
        for resource_widget_update in self.resource_widget_updates:
            resource_widget_update()


def merge_dicts(dict1,dict2):
    # The new dictionary has the same keys as dict1.
    # For keys also present in dict2, the value of dict2 is used, except in the
    # case of subdictionaries, where merging is done recursively.
    merged_dict = dict1.copy()
    for key in dict1.keys():
        if key in dict2:
            if type(dict1[key])==types.DictType:
                merged_dict[key] = merge_dicts(dict1[key],dict2[key])
            else:
                merged_dict[key] = dict2[key]
    return merged_dict

