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

package de.fzi.niki40.eisarvisualisation;

/**
 * Created by Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * Class responsible for temporary saving the history data recieved from OPC UA Server
 */

public class HistoryDataObject {
    public String Name;
    public String flowX;
    public String flowY;
    public Double prex;
    public Double prey;
    public String power;
    public double airflow;
    public double angle;

    public String getPower() {
        return power;
    }

    public void setPower(String power) {
        this.power = power;
    }

    public String getName() {
        return Name;
    }

    public void setName(String name) {
        Name = name;
    }

    public String getFlowX() {
        return flowX;
    }

    public void setFlowX(String flowX) {
        this.flowX = flowX;
    }

    public String getFlowY() {
        return flowY;
    }

    public void setFlowY(String flowY) {
        this.flowY = flowY;
    }

    public Double getPrex() {
        return prex;
    }

    public void setPrex(Double prex) {
        this.prex = prex;
    }

    public Double getPrey() {
        return prey;
    }

    public void setPrey(Double prey) {
        this.prey = prey;
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
