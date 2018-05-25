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

import android.util.Pair;

import org.artoolkit.ar.base.ARToolKit;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by FZI Forschungszentrum Informatik on 17.02.2017.
 */

public class MarkerMapping {

    private final List<Pair<Integer, String>> markerSensorPairs;

    public MarkerMapping(){
        Pair<Integer, String> marker_sensor1 = new Pair<>(ARToolKit.getInstance().addMarker("single_barcode;1;80"), Constants.Sensor1Address);
        Pair<Integer, String> marker_sensor2 = new Pair<>(ARToolKit.getInstance().addMarker("single_barcode;3;80"), Constants.Sensor2Address);
        Pair<Integer, String> marker_sensor3 = new Pair<>(ARToolKit.getInstance().addMarker("single_barcode;6;80"), Constants.Sensor3Address);
        Pair<Integer, String> marker_printer = new Pair<>(ARToolKit.getInstance().addMarker("single_barcode;4;80"), Constants.PrinterAddress);

        markerSensorPairs = new ArrayList<>();
        markerSensorPairs.add(marker_sensor1);
        markerSensorPairs.add(marker_sensor2);
        markerSensorPairs.add(marker_sensor3);
        markerSensorPairs.add(marker_printer);
    }

    /**
     *
     * @param sensorAddr
     * @return
     */
    public int getMarker(String sensorAddr) {
        for (Pair<Integer, String> pair : markerSensorPairs) {
            if (pair.second.equals(sensorAddr)) {
               return pair.first;
            }
        }
        return -1;
    }
}
