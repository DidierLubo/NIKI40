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
 * Created by FZI Forschungszentrum Informatik on 03.02.2017.
 */

public class EisSensor extends EisDevice {

    private static String TAG = EisDevice.class.getName();


    public String NodeIDFlowX;
    public String NodeIDFlowY;

    public String getNodeIDFlowX() {
        return NodeIDFlowX;
    }

    public void setNodeIDFlowX(String nodeIDFlowX) {
        NodeIDFlowX = nodeIDFlowX;
    }

    public String getNodeIDFlowY() {
        return NodeIDFlowY;
    }

    public void setNodeIDFlowY(String nodeIDFlowY) {
        NodeIDFlowY = nodeIDFlowY;
    }

    public String nodeValueX;
    public String nodeValueY;

    public String getNodeValueX() {
        return nodeValueX;
    }

    public void setNodeValueX(String nodeValueX) {
        this.nodeValueX = nodeValueX;
    }

    public String getNodeValueY() {
        return nodeValueY;
    }

    public void setNodeValueY(String nodeValueY) {
        this.nodeValueY = nodeValueY;
    }

    // sensor data
    private volatile double airflow;
    private volatile double angle;


    public EisSensor(String address) {
        super(address);
        this.airflow = 0;
    }

    /**
     *
     * @return the measured temperature in celsius
     */

    public double getAirflow() {
        return airflow;
    }

    public void setAirflow(double x, double y) {
        x -= Constants.getCalibrationOffsetX(getAddress());
        y -= Constants.getCalibrationOffsetY(getAddress());
        this.airflow = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
        this.angle = Math.toDegrees(Math.atan2(y, -x));
    }

    public double getAngle() {
        return angle;
    }

}


