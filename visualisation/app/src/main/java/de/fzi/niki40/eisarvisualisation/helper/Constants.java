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

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * Created by FZI Forschungszentrum Informatik on 03.02.2017.
 * Class with constants
 */

public class Constants {

    //Scale factor for the arrow that shows the airflow (set between 1 and 100)
    public static float ARROW_SCALE = 10;
    //Flag, true if the data should be written on the screen, false otherwise
    public static final boolean DISPLAY_DATA = true;
    //Flag, true if the control bars should be displayed, false otherwise
    public static final boolean DISPLAY_CONTROL_BARS = false;
    //Flag, true if the send button should be available, false otherwise
    public static final boolean DISPLAY_SEND_BUTTON = false;

    public static final int SENSOR_COUNT = 3;

    public static final long SCAN_PERIOD = 10000;
    public static final int REQUEST_ENABLE_BT = 1;
    public static final String Sensor1Address = "00:A0:50:05:1F:17"; //1
    public static final String Sensor2Address = "00:A0:50:17:23:0B"; //2
    public static final String Sensor3Address = "00:A0:50:11:09:0B"; //3
    //public static final String PrinterAddress = "E5:84:86:02:08:10";
    public static final String PrinterAddress = "D6:DC:BD:40:A4:13";
    //public static final String ESIMA_EIS = "ESIMA-EIS";

    public final static String ACTION_GATT_CONNECTED = "de.fzi.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED = "de.fzi.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED = "de.fzi.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE = "de.fzi.bluetooth.le.ACTION_DATA_AVAILABLE";
    public final static String EXTRA_SENSOR_ADDRESS = "sensor.address";
    public final static String EXTRA_DATA = "sensor.data";

    public final static String JSON_SENSOR_ID = "sensorID";
    public final static String JSON_SENSOR_FLOW_X = "flowX";
    public final static String JSON_SENSOR_FLOW_Y = "flowY";

    public final static String PRINTER_CURRENT_HOTEND_TEMPERATURE = "h1";
    public final static String PRINTER_HOTEND_TARGET_TEMPERATURE = "th";


    //public final static UUID UUID_ESIMA = UUID.fromString(HahnSchickard_GattAttributes.HAHN_SCHICKARD_CHARACTERISTIC);
    public final static UUID UUID_PRINTER_CHAR = UUID.fromString("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
    public final static UUID UUID_PRINTER_DESC = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    private static List<Pair<String, Double>> offsetsXAxis;
    private static List<Pair<String, Double>> offsetsYAxis;
    private static boolean initialized = false;

    /**
     * Returns the X-Axis offset of the sensors air value which is used for calibration
     * @param sensorAddr The adress of the sensor
     * @return The offset value
     */
    public static double getCalibrationOffsetX(String sensorAddr) {
        if (!initialized) {
            initializeOffsets();
        }
        for (Pair<String, Double> pair : offsetsXAxis) {
            if (pair.first.equals(sensorAddr)) {
                return pair.second;
            }
        }
        return 0.0;
    }

    private static void initializeOffsets() {
        Pair<String, Double> sensor1_offset_x = new Pair<>(Sensor1Address, 0.0);
        Pair<String, Double> sensor1_offset_y = new Pair<>(Sensor1Address, +0.0);
         Pair<String, Double> sensor2_offset_x = new Pair<>(Sensor2Address, 0.0);
        Pair<String, Double> sensor2_offset_y = new Pair<>(Sensor2Address, +0.0);
        Pair<String, Double> sensor3_offset_x = new Pair<>(Sensor3Address, 0.0);
        Pair<String, Double> sensor3_offset_y = new Pair<>(Sensor3Address, +0.0);
        offsetsXAxis = new ArrayList<>();
        offsetsYAxis = new ArrayList<>();
        offsetsXAxis.add(sensor1_offset_x);
        offsetsXAxis.add(sensor2_offset_x);
        offsetsXAxis.add(sensor3_offset_x);
        offsetsYAxis.add(sensor1_offset_y);
        offsetsYAxis.add(sensor2_offset_y);
        offsetsYAxis.add(sensor3_offset_y);
        initialized = true;
    }

    /**
     * Returns the Y-Axis offset of the sensors air value which is used for calibration
     * @param sensorAddr The adress of the sensor
     * @return The offset value
     */
    public static double getCalibrationOffsetY(String sensorAddr) {
        if (!initialized) {
            initializeOffsets();
        }
        for (Pair<String, Double> pair : offsetsYAxis) {
            if (pair.first.equals(sensorAddr)) {
                return pair.second;
            }
        }
        return 0.0;
    }
}
