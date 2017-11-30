/*
        Copyright 2017 NIKI 4.0 project team

        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by Nathalie Hipp, Hahn-Schickard

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



package fzi.niki_ar;

import java.util.ArrayList;
import java.util.List;

/**
 * Singleton class managing the Sensors
 */

public class SensorsManager  {

    // one static instance of the class
    private static SensorsManager instance;

    //List to store all desired sensors
    private static List<Sensor> sensors = new ArrayList<>();


    private SensorsManager() {
        //Hidden constructor
    }


    /**
     * Initializes an instance of the singleton class if it not exists
     */
    public synchronized static void initInstance() {
        if (instance == null) {
            instance = new SensorsManager();
        }
    }

    /**
     *
     * @return the global instance of the sensor handler
     */
    public static SensorsManager getInstance() {
        initInstance();
        return instance;
    }


    public List<Sensor> getSensors() {
        return sensors;
    }

    /**
     * Adds a sensor to the sensor list
     * @param sensor The sensor
     */
    public void addSensor(Sensor sensor) {
        sensors.add(sensor);
    }


    /**
     * Returns the sensor that has to the given name
     * @param  name The name of the sensor
     * @return Returns the fitting sensor if it is in the list of sensors, otherwise null
     * is returned
     */
    public synchronized Sensor getSensor(String name) {
        for (Sensor sensor : sensors) {
            if (sensor.getName().equals(name)) {
                return sensor;
            }
        }
        return null;
    }


}

