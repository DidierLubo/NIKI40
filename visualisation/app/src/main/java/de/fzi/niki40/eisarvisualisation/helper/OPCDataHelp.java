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

/**
 * Created by FZI Forschungszentrum Informatik on 14.02.2018.
 * Class containing static variables used in historical data visualization
 */

public class OPCDataHelp {

    public static boolean dataArrive;
    public static String statusData="Not Ready";



    public static String sensorName; // static variable containing the sensor name regarding history data
    public static String startTime; // static variable containing the starting point regarding history data
    public static String endTime;   // static variable containing the end point regarding history data
    public static int clusters;     // static variable containing the number of clusters regarding history data

    public static int getClusters() {
        return clusters;
    }

    public static void setClusters(int clusters) {
        OPCDataHelp.clusters = clusters;
    }

    public static String getSensorName() {
        return sensorName;
    }

    public static void setSensorName(String sensorName) {
        OPCDataHelp.sensorName = sensorName;
    }

    public static String getStartTime() {
        return startTime;
    }

    public static void setStartTime(String startTime) {
        OPCDataHelp.startTime = startTime;
    }

    public static String getEndTime() {
        return endTime;
    }

    public static void setEndTime(String endTime) {
        OPCDataHelp.endTime = endTime;
    }

    String Information;

    public String getInformation() {
        return Information;
    }

    public void setInformation(String information) {
        Information = information;
    }


    public static boolean isDataArrive() {
        return dataArrive;
    }

    public static void setDataArrive(boolean dataArrive) {
        OPCDataHelp.dataArrive = dataArrive;
    }

    public static String getStatusData() {
        return statusData;
    }

    public static void setStatusData(String statusData) {
        OPCDataHelp.statusData = statusData;
    }

}
