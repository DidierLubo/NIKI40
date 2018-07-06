#        Copyright 2016-2018 NIKI 4.0 project team
#        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
#        Project partners are FZI Forschungszentrum Informatik am Karlsruher
#        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
#        für angewandte Forschung e.V. (www.hahn-schickard.de) and
#        Hochschule Offenburg (www.hs-offenburg.de).
#        This file was developed by Hahn-Schickard.
#        Licensed under the Apache License, Version 2.0 (the "License");
#        you may not use this file except in compliance with the License.
#        You may obtain a copy of the License at
#        http://www.apache.org/licenses/LICENSE-2.0
#        Unless required by applicable law or agreed to in writing, software
#        distributed under the License is distributed on an "AS IS" BASIS,
#        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#        See the License for the specific language governing permissions and
#        limitations under the License.

import argparse, serial

parser = argparse.ArgumentParser(description='Profibus network emulator')
parser.add_argument("-b", "--baudrate", help="serial port baudrate", default=9600)
parser.add_argument("-p", "--port", help="serial port, for windows COM* and for Linux /dev/ttyUSB*",required=True)

args = parser.parse_args()

try : uartPort=serial.Serial(
    port=args.port,
    baudrate=args.baudrate,
    parity=serial.PARITY_EVEN,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)
except serial.SerialException :
    exit("Error occurred while opening serial port")

#dataFile=open("capturefzi.txt", "r")
while uartPort.is_open :
    dataFile=open("test.txt", "r")
    for line in dataFile :
        print str(line)
        uartPort.write(line)
        #uartPort.write("\n")
        if not line:
            dataFile.seek(0)
            break
