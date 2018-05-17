# encoding: utf-8

# Copyright 2018 NIKI 4.0 project team
#
# NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
# Project partners are FZI Forschungszentrum Informatik am Karlsruher
# Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
# für angewandte Forschung e.V. (www.hahn-schickard.de) and
# Hochschule Offenburg (www.hs-offenburg.de).
# This file was developed by Sebastian King at Hahn-Schickard.
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

import sys


bool_commands = [
    ('log', True, 'pretty-printed logging of serial frame contents'),
    ('log_hex', False, 'hex logging of serial frame contents'),
    ('log_bytes', False, 'hex logging of prospective serial frame parts'),
    ('log_frames', False, 'hex logging of serial frames'),
    ('log_garbage', False, 'logging of serial data outside of frames'),
    ('log_time', False, 'timestamp inclusion in logs'),
    ('inverted_length_field', True,
      'use of inverted_length_field in frames (disable for legacy firmware)'),
    ('instance_write', False, 'usage of LWM2M_INST_WR_REQ'),
]

class UnknownOptionError(Exception):
    def __init__(self, arg):
        self.arg = arg
    def __str__(self):
        return repr(self.arg)

class Inconsistency(Exception):
    def __init__(self, arg):
        self.arg = arg
    def __str__(self):
        return repr(self.arg)


class commandline:
    def __init__(self):
        self.cli_dict = {k:v for k,v,_ in bool_commands}
        self.cli_dict.update({'filename':None, 'timeout':None})
        self.maxlen = self.max_length()

        args = sys.argv
        i = 1

        while i < len(args):
            for key,_,_ in bool_commands:
                if '-' + key == args[i]:
                    self.cli_dict[key] = True
                    break
                elif '-no_' + key == args[i]:
                    self.cli_dict[key] = False
                    break
            else:
                if args[i][0] != '-' and args[i][-5:] == '.json':
                    if self.cli_dict['filename'] is not None:
                        raise Inconsistency(args[i])
                    self.cli_dict['filename'] = args[i]
                elif '-timeout' == args[i]:
                    if self.cli_dict['timeout'] is not None:
                        raise Inconsistency(args[i][1:][:7])
                    i += 1
                    try:
                        self.cli_dict['timeout'] = int(args[i])
                    except ValueError:
                        print 'A nonnegative integer needs to be entered as timeout.'
                        raise SystemExit
                elif '-help' == args[i]:
                    self.help()
                else:
                    raise UnknownOptionError(args[i])
            i += 1

        for key,_,_ in bool_commands:
            if '-' + key in args and '-no_' + key in args:
                raise Inconsistency(key)



    def help(self):
        print 'Options and arguments:\n'
        for key,_,help in bool_commands:
            print '-' + key + (self.maxlen - len(key) + 1) * " " \
                + 'Enables ' + help
            print '-no_' + key + (self.maxlen - len(key) - 2) * ' ' \
                + 'Disables ' + help
        print 'filename.json' + (self.maxlen - 11) * ' ' + \
          'Selects the sensor definition file'
        print '-timeout n' + (self.maxlen - 8) * ' ' + 'Sets the timeout in ms'
        print '-help' + (self.maxlen - 3) * ' ' + 'Shows this help'
        exit(0)

    def max_length(self):
        lenlist = [(len(k) + 4) for k,_,_ in bool_commands]
        lenlist.append(13)
        lenlist.append(12)
        return max(lenlist)
