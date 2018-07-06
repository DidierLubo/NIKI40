[![N|Solid](https://www.hahn-schickard.de/typo3conf/ext/hsg_hsg_de/Resources/Public/Images/logo.png)](https://www.hahn-schickard.de/) 

# Profibus Sniffer physical test case descriptions 

## 1. Software loopback test 
PRU UART firmware is compiled with *enableLoopback()* set to directlly copy the RX content to TX buffer. This tests if the PRU UART has been configured properly for baisic comunication. 

Test succeeds if RX and TX containt matches up. 

Result: **Succees**

The results of the test is shown in image **SoftwareLoopbackTest.png**

## 2. Hardware loopback test 
PRU UART firmware is compiled without *enableLoopback()*, but with *handleLoopback()* function being called in the main firmware loop, which writes a string to TX and echos out the RX buffer content. This tests if the PRU UART0 pins have been multiplex correctlly. 

Test succeeds if transmitted string has been echoed out. 

Result: **Success** 

The results of the test is shown in image **HardwareLoopbackTest.png**

## 3. Serial communication test with echo back 
PRU firmware is not changed at all, the pins of UART1 are connected to UART to USB converter and a serial COM port is opened with following configuration options: 

| Baudrate         |  9600   | 
| ------------------ | :------: |
| Parity               | EVEN   | 
| Stop bit count  |    1       |
| Data size         |    8       |
| Flow control     | NONE  |

A string is typed in the serial COM port. This test if PRU UART firmware has been configure with correct line configuration options. *handleLoopback()* function trannsmits a string to the serial COM port and echoes the content of RX buffer to the terminal.

Test succeeds if serial COM port receives strings of data and if the PRU echoes the correct string out. 

Result: **Success**

The results of the test is shown in image **SerialCableTestWithEchoBack.png**

## 4. Serial communication test 
PRU firmware is changes, so the *handleLoopback()* function is disabled, the test follows the procedure described in test 3. This tests if PRU firmware is able to read the data from UART without sending anything to it. The firmware echoes the typed string from the serial COM port to the terminal.

Test succeeds if the correct string has been echoed out to the terminal. 

Result: **Success**

The results of the test is shown in image **SerialCableTest.png**

## 5. UART communication test with Profibus emulator

>TBD

## 6. RS485 communication test with Profibus emulator

>TBD

## 7. RS485 communication test with Profibus network 

>TBD