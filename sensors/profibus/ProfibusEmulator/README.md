[![N|Solid](https://www.hahn-schickard.de/typo3conf/ext/hsg_hsg_de/Resources/Public/Images/logo.png)](https://www.hahn-schickard.de/)

# PROFIBUS datagaram emulator

## Table of Contents
 - [Introduction](#introduction)
 - [Required dependencies ](#required-dependencies ) 
 - [Hardware](#hardware)

## Introduction 

This python script is used to test the PROFIBUS parser project. It simulates a data stream from the PROFIBUS network over __RS485__ interface. 

This parser can send a stream of real captured datapackets or a mock data written by the user himeslf. An example of such datá sample is given in __test.txt__ file.

## Required dependencies 

- __pyserial__ 

## Hardware

- RS485 to USB cable