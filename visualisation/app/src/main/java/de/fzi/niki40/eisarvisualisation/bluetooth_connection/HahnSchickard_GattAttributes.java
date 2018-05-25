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

import java.util.HashMap;

/**
 * GattAttributes used for sending and receiving data
 *
 * @author Nathalie Hipp, Hahn-Schickard
 * @modified by FZI Forschungszentrum Informatik on 03.02.2017.
 * @version 1.1
 */

public class HahnSchickard_GattAttributes {
    private static HashMap<String, String> attributes = new HashMap();
    public static String HAHN_SCHICKARD_SERVICE = "0000aaaa-0000-1000-8000-00805f9b34fb";
    public static String HAHN_SCHICKARD_CHARACTERISTIC = "0000fff0-0000-1000-8000-00805f9b34fb";
    public static String HAHN_SCHICKARD_DESCRIPTOR_CUD = "00002901-0000-1000-8000-00805f9b34fb";
    public static String HAHN_SCHICKARD_DESCRIPTOR_CCCD = "00002902-0000-1000-8000-00805f9b34fb";

    static {
        attributes.put(HAHN_SCHICKARD_SERVICE, "ESIMA Service");
        attributes.put(HAHN_SCHICKARD_CHARACTERISTIC, "ESIMA Data");
        attributes.put(HAHN_SCHICKARD_DESCRIPTOR_CUD, "Char. User. Descr.");
        attributes.put(HAHN_SCHICKARD_DESCRIPTOR_CCCD, "CCCD");
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}
