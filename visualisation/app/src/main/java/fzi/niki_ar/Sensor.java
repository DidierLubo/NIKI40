/*
        Copyright 2017 NIKI 4.0 project team

        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by FZI Forschungszentrum Informatik.
        This file was also developed by Nathalie Hipp, Hahn-Schickard.

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

/**
 * Sensor class used for representation of real sensor.
 */

public class Sensor {

    String Name;
    Integer Identifier;
    String NamespaceIndex;
    String Value;
    String Description;

    Double VectorX;
    Double VectorY;

    String SensorValue;




    //sensor Data
    public double airflow;
    public double angle;
    private volatile double temperature;

    public String getName() {
        return Name;
    }

    public void setName(String name) {
        Name = name;
    }

    public Integer getIdentifier() {
        return Identifier;
    }

    public void setIdentifier(Integer identifier) {
        Identifier = identifier;
    }

    public String getNamespaceIndex() {
        return NamespaceIndex;
    }

    public void setNamespaceIndex(String namespaceIndex) {
        NamespaceIndex = namespaceIndex;
    }

    public String getValue() {
        return Value;
    }

    public void setValue(String value) {
        Value = value;
    }

    public String getDescription() {
        return Description;
    }

    public void setDescription(String description) {
        Description = description;
    }


    public String getSensorValue() {
        return SensorValue;
    }

    public void setSensorValue(String sensorValue) {
        SensorValue = sensorValue;
    }


    public Double getVectorX() {
        return VectorX;
    }

    public void setVectorX(Double vectorX) {
        VectorX = vectorX;
    }

    public Double getVectorY() {
        return VectorY;
    }

    public void setVectorY(Double vectorY) {
        VectorY = vectorY;
    }

    public double getAirflow() {
        return airflow;
    }


    /*
     * Method used to calculate the airflow from the X flow and Y flow
     */

    public void setAirflow(double x, double y) {
        this.airflow = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
        this.angle = Math.toDegrees(Math.atan2(y, -x));
    }

    public double getAngle() {
        return angle;
    }





}
