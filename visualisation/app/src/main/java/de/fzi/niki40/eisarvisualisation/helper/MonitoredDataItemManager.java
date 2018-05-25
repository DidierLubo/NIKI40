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

package de.fzi.niki40.eisarvisualisation.helper;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by FZI Forschungszentrum Informatik on 03.04.2018.
 * Singelton class class responsible for managing a list of @MonDataIt objects
 */

public class MonitoredDataItemManager {

    // one static instance of the class
    private static MonitoredDataItemManager instance;

    //List to store all desired sensors
    private static List<MonDataIt> MonitoredDataItems = new ArrayList<>();


    private MonitoredDataItemManager() {
        //Hidden constructor
    }


    /**
     * Initializes an instance of the singleton class if it not exists
     */
    public synchronized static void initInstance() {
        if (instance == null) {
            instance = new MonitoredDataItemManager();
        }
    }

    /**
     *
     * @return the global instance of the sensor handler
     */
    public static MonitoredDataItemManager getInstance() {
        initInstance();
        return instance;
    }


    public List<MonDataIt> getMonitoredSensors() {
        return MonitoredDataItems;
    }

    /**
     * Adds a sensor to the sensor list
     * @param monitoredDataItem
     */
    public void addMonitoredDataItem(MonDataIt monitoredDataItem) {
        MonitoredDataItems.add(monitoredDataItem);
    }



    /**
     * Returns the sensor that has to the given name
     * @param  name The name of the sensor
     * @return Returns the fitting sensor if it is in the list of sensors, otherwise null
     * is returned
     */
    public synchronized MonDataIt getMonitoredDataItem(String name) {
        for (MonDataIt monitoredDataItem : MonitoredDataItems) {
            if (monitoredDataItem.getName().equals(name)) {
                return monitoredDataItem;
            }
        }
        return null;
    }
}
