[![N|Solid](https://www.hahn-schickard.de/typo3conf/ext/hsg_hsg_de/Resources/Public/Images/logo.png)](https://www.hahn-schickard.de/)

# PRU firmware for the Profibus Sniffer
## Table of Contents
 - [Introduction](#introduction)
 - [PRU-ICCS Integration](#pru-iccs-Integration) 
 - [Interrupt handling](#interrupt-handling)
 - [Communication between cores](#communication-between-cores)
 - [Pin Multiplexing](#pin-multiplexing)
 - [Introduction to pin set up](#introduction_to_pin_set_up)
 - [Device tree compilation](#device_tree_compilation)
 - [Using the firmware](#using-the-firmware)
 - [Setting up the development environment ](#setting-up-the-development-environment)
 - [Setting up the Beaglebone for PRU](#setting-up-the-beaglebone-for-pru)

## Introduction
Programmable Real-Time Unit and Industrial Communication Subsystem (PRU-ICSS) is a fast (200-MHz, 32-bit), highly deterministic processor with single-cycle I/O access to the pins, shared memory space and integrated peripherals of the AM335x processing unit. PRU-ICSS is designed for fast real-time responses, specialized data handling operations, custom peripheral interfaces, and task offloading from the main processor cores of the system-on-chip (SoC) device. 

## PRU-ICCS Integration 
Figure 2 shows, what integrated peripherals the PRU-ICCS cores have. Please note that UART0 of the PRU1 core is different than the usual UARTs of the AM335x proccessor and has different registers. This UART is compliant to TL16C550 standard, more details about this peripheral can be found in [AM335xand AMIC110 Sitara™ Processors Technical Reference Manual](http://www.ti.com/lit/ug/spruh73p/spruh73p.pdf) page 241, chapter 4.4.4. To access the UART0 peripheral, UART1 pins P9.24 and P9.26 must be multiplex for Mode 5.
 
[![N|Solid](https://elinux.org/images/b/b1/PRUSS.png)](http://www.ti.com/lit/ug/spruh73p/spruh73p.pdf)

## Interrupt handling
The nature of PRU core does not allow for typical interrupt implemenation, because for a system to be higly deterministic, every state at every single point in time must be know, thus the inettrupt handlers are not used, instead, interrupts are implemented as system events, that must be polled and handled by the core at some prediterminted time. PRU-ICSS cores can cappture 64 system events over 10 channels. A detailed description of interrupt handling can be found in [AM335xand AMIC110 Sitara™ Processors Technical Reference Manual](http://www.ti.com/lit/ug/spruh73p/spruh73p.pdf) page 225, chapter 4.4.2 Interrupt Controller (INTC). 

## Communication between cores
Beaglebone Black boards come with two PRU cores that can be configured for use. Communication between the PRU and ARM processors is handled by R30 and R31 registors from the PRU side and by the __/dev/rpmsg_pru30__ for PRU0 and __/dev/rpmsg_pru31__ for PRU1 via  __rpmsg__ driver, which uses *virtio* (Virtual I/O), *vdev* (Virtual I/O device) and *vring* (Virtual I/O device ring buffer) constructs. It is based on Interrupt handling technique, which we discussed in a previos chapter. 

To facilitate duplex communication between the SoC and PRU two Vrings are used: *Vring1* (from SoC to PRU) and *Vring0* (from PRU to SoC). To notify the recepeint if there is a message in there respective Vring a Mailbox system is used. Just as with Vrings there are two Mailboxes which are used to check if a system kick event has occoured.

>More detailed description of the rpmsg communication can be found in [TI wiki page](http://processors.wiki.ti.com/index.php/PRU-ICSS_Remoteproc_and_RPMsg "PRU-ICSS Remoteproc and RPMsg")

Typical communication proccess can be described like this: 

1. Allocation of the data buffer in the Vring
2. Writing a system kick event into the senders Mailbox
3. Polling the senders Mailbox for the kicked Vring 
4. Reading the data from Vring 
5. Emptieng the used buffer 
6. Writing a system kick into the recipients Mailbox

>A quick setup guide for the rpmsg driver can be found at the [TI wiki page](http://processors.wiki.ti.com/index.php/RPMsg_Quick_Start_Guide "RPMsg Quick Start Guide"). 

## Pin Multiplexing
Pin configuration is done via the Device Tree (DT) and Device Tree Overlay files (DTO). A device tree source file (.dts) is used to configure the pin registers and then compiled into a device tree blob overlay file (.dtbo), which is placed in __/lib/firmware__ directory. To activate the desired DTOs __uEnv.txt__ file must be edited and the Kernel updated with the changes. For the purposes of this project a .dts file has been provided with the correct configuration settings. 

### Introduction to pin set up

Beaglebone black header map is given bellow, as a reference for the project setup. 

![Beaglebone black header](https://vadl.github.io/images/bbb/bbb_headers.png)

The header offsets and multipelxing options for the beaglebone black P8 and P9 header is shown bellow as a reference for the multiplexing options.

![Beaglebone black P8 header multiplexing](https://vadl.github.io/images/bbb/P8Header.png)

![Beaglebone black P9 header multiplexing](https://vadl.github.io/images/bbb/P9Header.png)

The provided device tree source file contains the following pin multiplexing configuration: 

| Pin      | Multiplexing mode 	|
| ------- | :----------------------:  | 
| P9.24  | Mode 5 Tx              	|
| P9.26  | Mode 5 Rx		|

These multiplexing options translate into the following register values: 

| Bit offset 	   	     | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| -------------------------- | - | - | - | - | - | - | - | - |
| Rx pin register value | x | 0 | 1 | 0 | 0 | 1 | 0 | 1 | 
| Tx pin register value | x | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 

These values can be read with: 

```
sudo cat /sys/kernel/debug/pinctrl/44e10800.pinmux/pins | less
```

*UART1 Rx* and *Tx* pins numbers are **Pin96** and **Pin97** respectivelly. To find the Pin numbers and there labels can be found with: 

```
less /sys/kernel/debug/pinctrl/44e10800.pinmux/pinmux-pins
```

Expected value for the **Pin96** is *0x35*, and *0x15* for **Pin97**. 

> More information about pinmultipelxing and device tree overlays can be found at [Derek Molloy's blog page](http://derekmolloy.ie/gpios-on-the-beaglebone-black-using-device-tree-overlays/) and [valvers webpage](http://www.valvers.com/embedded-linux/beaglebone-black/step04-gpio/).

### Device tree compilation

The .dts file can be compiled on the Beaglebone itselt with command:
 
```
$ dtc -O dtb -o PRU-UART-00A0.dtbo -b 0 -@ PRU-UART-00A0.dts
```

The compilation procedure might prop up some warnings like these: 

```
PRU-UART-00A0.dtbo: Warning (unit_address_vs_reg): Node /fragment@0 has a unit name, but no reg property
PRU-UART-00A0.dtbo: Warning (unit_address_vs_reg): Node /fragment@1 has a unit name, but no reg property
```

These warnings are caused by a known bug in DT generator and thus, can be ignored.

After the compilation has been completed, the generated **.dtbo** file can be placed to the proper location with command: 

```
$ sudo mv PRU-UART-00A0.dtbo /lib/firmware
```

Then the /boot/uEnv.txt file must be modified at `###Custom Cape` line with: 

```
dtb_overlay=/lib/firmware/PRU-UART-00A0.dtbo
``` 

And then the kernel can be updated: 

```
$ update-initramfs -uk `uname -r`
```

## Using the firmware

To start the PRU, we must first inform the kernel which firmware image must be loaded (this only needs to be done once). This can be done with command: 

```
$ echo 'am335x-pru1-fw' > /sys/class/remoteproc/remoteproc2/firmware
```

After the kernel knows which firmware to load, the PRU can be started with command: 

```
$ echo 'start' > /sys/class/remoteproc/remoteproc2/state
```

To check if the firmware was succefully loaded and the communication channel opened, run: 

```
$ dmesg
```

or with: 

```
dtc -I fs /proc/device-tree | less
```

### PRU commands
The PRU firmware can proccess these commands:

| Command       | Description   | 
| ------------- |:-----------------------------------------------------------------------| 
| start         | Starts the data sniffing, after reboot firmware waits for this command |
| stop          | Stops the uart until start command has been received                   |
| off           | Shuts down the uart and stops the PRU                                  |
| baud=****     | Changes the baudrate of the UART, by default, it is set to 9600        |

PRU firmware supports all PROFIBUS baudrates declared in this table: 

| Baudrate |
| :------: | 
| 9600     | 
| 19200    | 
| 45450    | 
| 93750    | 
| 187500   | 
| 500000   | 
| 1500000  |
| 3000000  |
| 6000000  |
| 12000000 |

To start the UART parsing via the PRU, send the start comand. For example, such command would be sent via the terminal like so: 

```
echo "start" > /dev/rpmsg_pru31
```

To change the baudrate of the UART parser, a similar command can be sent via the terminal: 

```
echo "baudrate=19200" > / dev/rpmsg_pru31
```

To temporary stop the parsing a stop command can be sent with: 

```
echo "stop" > /dev/rpmsg_pru31 
```

To completly stop the UART parser and shutdown the PRU an OFF command must be sent: 

```
echo "off" > /dev/rpmsg_pru31
```

## Setting up the development environment 

Download the code generation tools from Teaxs instruments:

```
$ wget -c http://software-dl.ti.com/codegen/esd/cgtpublicsw/PRU/2.1.4/ticgtpru2.1.4linuxinstallerx86.bin
```

Install the generation tools, in this case we install them in /opt/ti/pru
 
```
$ sudo mkdir -p /opt/ti/pru sudo chmod a+x ticgtpru2.1.4linuxinstallerx86.bin sudo ./ticgtpru2.1.4linuxinstallerx86.bin 
```

Check if the toolset has been installed: 

```
$ ls -l /opt/ti/pru/bin.cmd
```

Compile source code:

```
$ export PRU_CGT=/opt/ti/pru
$ cd ./pru_rs485/firmware/
$ make clean
$ make
```

The compiled output file can be found in ./gen directory as firmware.out, this file must be placed on the Beaglebone black and linked to the am335x-pru1-fw. To create a symbolic link to the firmware, run: 

```
$ ln -f -s firmware.out am335x-pru01-fw
```

To effitiently edit the source code, we use Code Composer Studio from Texas Instrument, it can be download from [TI wiki page](http://processors.wiki.ti.com/index.php/Download_CCS) or with wget fro linux: 

```
$ wget -c software-dl.ti.com/ccs/esd/CCSv8/CCS_8_0_0/exports/CCS8.0.0.00016_linux-x64.tar.gz
```

To install it on windows, just run the installer, on linux the installer must be unpacked, granted execution perimtions and then executed: 

```
$ mkdir ~/CCS
$ tar -pxvf CCS8.0.0.00016_linux-x64.tar.gz -C ~/CCS
$ sudo chmod +x ~/CSS/CCS8.0.0.00016_linux-x64/ccs_setup_linux_64_*.bin
$ ~/CSS/CCS8.0.0.00016_linux-x64/ccs_setup_linux_64_*.bin
```

## Setting up the Beaglebone for PRU 
__TBD__ 

>*Note - debian 9 linux versions from the [Beaglebone website](beaglebone.org) have the PRU rpmsg enabled by default.
