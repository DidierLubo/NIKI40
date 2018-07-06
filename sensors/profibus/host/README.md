[![N|Solid](https://www.hahn-schickard.de/typo3conf/ext/hsg_hsg_de/Resources/Public/Images/logo.png)](https://www.hahn-schickard.de/)

# PROFIBUS data-telegram parser 

## Table of Contents
 - [Introduction](#introduction)
 - [Supported datatelegrams](#supported-datatelegrams) 
 - [Program functionality](#program-functionality)
 - [Lightweight Communications and Marshalling](#lightweight-communications-and-marshalling)

## Introduction 

The programs consists of 3 threads, first one is responsible for reading the data from a device stream, the second one is responsible for dissecting the data packet from a given data buffer and the third one sends the dissected packets over LCM

## Supported datatelegrams 

Currentlly the project supports __SD2__ and __SD3__ PROFIBUS packet types. An interface __iPacketType__ can be used to implement the other packet types. Please take note that different search algorithms can be used while dissecting different packet types. A search algorithm can be implemented from __iPatternMatcher__ interface.

## Program functionality 

This program takes in these arguments from the terminal: 

| Command | Description       | Argument 			  | 
| :-----------: | ------------------- | :---------------------: 	|
| -s               | start command | 1  **OR**  0             	  |
| -d               | device port       | __/dev/rpmsg_pru31__ |
| -b		    | baudrate	        | 9600				  |

> Not implemented yet

## Lightweight Communications and Marshalling 

> TBD

