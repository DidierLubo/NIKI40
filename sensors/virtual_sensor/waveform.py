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

"""Waveforms of periodic signals."""

import math
import random

import enum

def sine(x):
    return (math.sin(2*math.pi*x) + 1) / 2

def sawtooth(x):
    return x

def triangle(x):
    if x<0.5:
        return 2*x
    else:
        return 2*(1-x)

def rect(x):
    if x<0.5:
        return 1
    else:
        return 0

def flat(x):
    return 0

def noise(x):
    return random.random()

float_list = [
    ("Sine", sine),
    ("Rectangle", rect),
    ("Triangle", triangle),
    ("Sawtooth", sawtooth),
    ("Flat", flat),
    ("White noise", noise),
    ]
"""List of waveforms with names."""

float = enum.Enum(float_list)


bool_rect = lambda x: x>=0.5
bool_constant = lambda c: lambda x: c
bool_noise = lambda x: random.random()>=0.5

bool_list = [
    ("True", bool_constant(True)),
    ("False", bool_constant(False)),
    ("Alternating", bool_rect),
    ("White noise", bool_noise),
]

bool = enum.Enum(bool_list)

