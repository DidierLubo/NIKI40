Binaries
==

This folder includes the binaries required for the NIKI4.0 kit mainly with regard to the communication.

Communication Module Firmware
--
The Firmware of the CC13xx communication controller must be equipped with the according firmware to allow the communication within the wireless sensor network. The binaries can be found here for different channels. The modules can be programmed using the [Texas Instrumens SmartRF Programmer]( http://www.ti.com/tool/FLASH-PROGRAMMER).

Fully Automated Gateway Installation
--
The gateway and the required services can be installed automatically. The installer included and runs all procedures to setup the gateway. Therefore, the archive needs to be extracted first before the installer included can be started. An internet connection is required so that additional packages and software can be downloaded. You can use the installer by running the following commands:

```
mkdir inst
tar -pxvzf niki-gateway-installer_<version>.tar.gz -C inst/
cd inst
sudo ./niki-gateway-installer.sh
```

**Please consider the following notes when using the automated
installer:**

-   An internet connection is required so that additional packages and software can be downloaded.
-   The installer walks you through the process After providing mySQL password, the installer can be left to run on
    its own
-   The whole procedure takes up to 60mins.
-   A reboot is required at the end of the installation.
-   **DO NOT DISSCONNECT FROM THE BEAGLEBONE DURING THE
    INSTALATION!**