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
 * Created by FZI Forschungszentrum Informatik on 23.04.2018.
 * Class representing senosor type EMeter
 */

public class EMeter {

    public static String Name;
    public static String Voltage;
    public static String Current;
    public static String Power;
    public static Float emeterPower=0.0f;
    public static Float tempPrinter=0.0f;

    public static Float getTempPrinter() {
        return tempPrinter;
    }

    public static void setTempPrinter(Float tempPrinter) {
        EMeter.tempPrinter = tempPrinter;
    }

    public static Float getEmeterPower() {
        return emeterPower;
    }

    public static void setEmeterPower(Float emeterPower) {
        EMeter.emeterPower = emeterPower;
    }

    public static String getPower() {
        return Power;
    }

    public static void setPower(String power) {
        Power = power;
    }

    public static String getName() {
        return Name;
    }

    public static void setName(String name) {
        Name = name;
    }

    public static String getVoltage() {
        return Voltage;
    }

    public static void setVoltage(String voltage) {
        Voltage = voltage;
    }

    public static String getCurrent() {
        return Current;
    }

    public static void setCurrent(String current) {
        Current = current;
    }
}
