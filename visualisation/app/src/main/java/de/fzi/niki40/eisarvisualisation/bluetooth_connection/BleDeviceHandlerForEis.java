/*
        Copyright 2016-2018 NIKI 4.0 project team

        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by FZI Forschungszentrum Informatik.

        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.
*/

package de.fzi.niki40.eisarvisualisation.bluetooth_connection;


import java.util.ArrayList;
import java.util.List;

import de.fzi.niki40.eisarvisualisation.helper.Constants;
import de.fzi.niki40.eisarvisualisation.helper.EisSensor;
import de.fzi.niki40.eisarvisualisation.helper.Printer;

/**
 * Created by FZI Forschungszentrum Informatik on 03.02.2017.
 * Singleton Class to handle the EisSensor connections
 */

public class BleDeviceHandlerForEis {

    // one static instance of the class
    private static BleDeviceHandlerForEis instance;

    // printer object which  passes the temperature value
    private Printer printer;

    //List of Sensors
    private List<EisSensor> sensors = new ArrayList<>();

    private BleDeviceHandlerForEis() {
        //Hidden constructor
        sensors.add(new EisSensor(Constants.Sensor1Address));
        sensors.add(new EisSensor(Constants.Sensor2Address));
        sensors.add(new EisSensor(Constants.Sensor3Address));
    }

    /**
     * Initializes an instance of the singleton class if it not exists
     */
    public synchronized static void initInstance() {
        if (instance == null) {
            instance = new BleDeviceHandlerForEis();
        }
    }

    /**
     *
     * @return the global instance of the sensor handler
     */
    public static BleDeviceHandlerForEis getInstance() {
        initInstance();
        return instance;
    }

    public List<EisSensor> getSensors() {
        return sensors;
    }


    /**
     * Returns the sensor that fits to the given address
     * @param address The address of the sensor
     * @return Returns the fitting sensor if it is in the list of eisSensors, otherwise null
     * is returned
     */
    public synchronized EisSensor getSensor(String address) {
        for (EisSensor eisSensor : sensors) {
            if (eisSensor.getAddress().equals(address)) {
                return eisSensor;
            }
        }
        return null;
    }

    /**
     * initializes the printer object
     * @param printer
     */
    public void addPrinter(Printer printer) {
        this.printer = printer;
    }

    /**
     *
     * @return the printer object. This can be null if it was not initialized before
     */
    public Printer getPrinter() {
        return printer;
    }



    /**
     * Calls disconnect() to all eisSensors in the list
     */
    public void disconnectPrinter() {
        //disconnect the printer
        if (printer != null) {
            printer.disconnect();
        }
    }

    /**
     * Checks if at least one expected bluetooth le device was added to the list
     * @return true if there is at least one device available, false otherwise
     */
    public boolean isPrinterConnected() {
        if(printer != null) {
            return true;
        } else {
            return false;
        }
    }

    public boolean checkPrinterAddress(String address) {
        if (printer != null) {
            if (printer.getAddress().equals(address)) {
                return true;
            }
        }
        return false;
    }
}
