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

import com.prosysopc.ua.client.MonitoredDataItem;

import org.opcfoundation.ua.builtintypes.NodeId;

/**
 * Created by FZI Forschungszentrum Informatik on 04.04.2018.
 * The following class represents a sensor with his Node Ids,
 * which will be added to a subscription and assigned with OPC UA Client DataChangeListeners,
 * responsible for live data extraction from the server.
 */

public class OpcuaMonitoredItem extends MonitoredDataItem {


    public OpcuaMonitoredItem(NodeId nodeId) {
        super(nodeId);
    }

    String name;
    String type;

    public String getHelpValue() {
        return helpValue;
    }

    public void setHelpValue(String helpValue) {
        this.helpValue = helpValue;
    }

    String helpValue;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }
}
