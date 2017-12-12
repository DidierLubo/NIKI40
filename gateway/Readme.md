# Gateway
The Gateway is a central part of NIKI 4.0 which consists of several components.

## Border-Router
The communication between the wireless sensors and the gateway uses 6LoWPAN as the basic protocol. Therefore, to allow communication between a host network and the sensor network an according translation is required. This is realized using a so called _Border-Router_ application. Niki 4.0 uses _Cetic 6LBR_ from https://github.com/cetic/6lbr/wiki.

## OPC UA Server
The interface to the visualization and also to other user interfaces is provided by an OPC UA server. The OPC UA Server provides the sensor information using the OPC UA protocol in a way that ny OPC UA client can access the data. To realize this architecture and to grant access to the wirelsess sensor nodes data different components are required.
### ASNeG OPC UA Server
The core of the OPC UA server is the server from ASNeG. It is used with slight modifications only, mainly regarding the build system. The current implementation is located at https://github.com/hso-esk/asneg. It also includes all the required submodules.
### Wakaama based LWM2M Server
The wireless sensor devices communicate using the LWM2M protocol over 6LoWPAN. Therefore an according LWM2M server is required at the gateway side. This is realized using the Wakaama LWM2M server. An according fork of the implementation and the underlying Wakaama server can be found here https://github.com/hso-esk/opcua-lwm2m-server and here https://github.com/hso-esk/opcua-lwm2m-server respectively.

### Generic Sensor Interface
The generic sensor interface provides an easy way to access any kind of sensor devices from the OPC UA server. Therefore, each type of sensor implements its specific methods that can be accessed by the OPC UA server using a common interface. The implementation of the sensor interface is located here https://github.com/hso-esk/opcua-sensor-interface