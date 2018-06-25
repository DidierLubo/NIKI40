[![N|Solid](https://www.hahn-schickard.de/typo3conf/ext/hsg_hsg_de/Resources/Public/Images/logo.png)](https://www.hahn-schickard.de/)

# Profibus Sniffer for the NIKI 4.0

This repository holds the PRU firmware and required libraries to compile the firmware, the host application and Unity test setup for the Profibus Sniffer project.

## Directory structure

All Paths in this document are relative from the project root directory *./pru_rs485* until stated otherwise. Directory structure is as follows: 

<div style="padding-left:0em;">

pru_rs485 <- root directory

</div>

<div style="padding-left:1em;">

├── firmware <- The PRU firmware code.

</div>

<div style="padding-left:1em;">

│   └── gen <- Compiled firmware files

</div>

<div style="padding-left:1em;">

├── host <- Parser host application

</div>

<div style="padding-left:1em;">

 │   ├── Dissector <- Contains the packet type disssector implementations
</div>

<div style="padding-left:1em;">

 │   ├── PacketTypes <- Contains packet type definitions and there implementations

</div>

<div style="padding-left:1em;">

 │   ├── PatternMatchers <- Contains the Search algorithm implementations 

</div>

<div style="padding-left:1em;">

│   └── bin <- Contains the compilation files

</div>

<div style="padding-left:1em;">

├── include <- Library files for the PRU firmware compliation

</div>

<div style="padding-left:1em;">

│   └── am335x <- Beaglebone black PRU resource definitions

</div>

<div style="padding-left:1em;">

├── lib <- Compiled PRU to ARM library

</div>

<div style="padding-left:1em;">

    ├── ProfibusEmulator <- Contains the Emulation tool for the profibus network

</div>

<div style="padding-left:1em;">

└── tests <- Unit tests, test case definitions and there resoults. 

<div style="padding-left:1em;">

    ├── TestCases <- Contains the resaults of physical test cases

</div>

<div style="padding-left:1em;">

   └──  UnitTests <- Contains the Unit tests for the host application

</div>

</div>

</div>

> **Note:** The PRU and ARM communicate using the TI rpmsg driver. A detailed documentation is located in *pru_rs485/firmware* directory **README.md** file

>

> **Note:** The parser uses [LCM](http://lcm-proj.github.io/) to send out successfully read Profibus messages to an LWM2M client. The client is found in the 'lwmtwom' repository. 

## Documentation 

Each part of the Profibus Sniffer project has it's own documentation in the **README.md** files
