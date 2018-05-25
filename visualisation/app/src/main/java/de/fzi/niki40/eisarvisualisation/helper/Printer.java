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

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattCharacteristic;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import de.fzi.niki40.eisarvisualisation.bluetooth_connection.BluetoothLeService;

/**
 * Created by FZI Forschungszentrum Informatik on 08.03.2017.
 * Class representing the 3D Printer
 */

public class Printer extends EisDevice {

    private static String TAG = Printer.class.getName();

    private double temperature;
    private BluetoothGattCharacteristic tx;


    public Printer(BluetoothDevice device) {
        super(device);
    }

    public double getTemperature() {
        return temperature;
    }

    public void setTemperature(double temperature) {
        this.temperature = temperature;
    }

    public void sendDataToBLE(BluetoothLeService bleService, double w1, double w2, double w3) {
        Map<String, Double> jsonBuilder = new HashMap<>(3, 1.0f);
        jsonBuilder.put("w1", w1);
        jsonBuilder.put("w2", w2);
        jsonBuilder.put("w3", w3);
        JSONObject json = new JSONObject(jsonBuilder);


        bleService.enqueueToTransmit(gatt,
                gatt.getService(UUID.fromString("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")).getCharacteristic(UUID.fromString("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")),
                json.toString().getBytes(Charset.forName("UTF-8")));
    }


    public boolean jsonContainsPrinterData(JSONObject json) {
        return json.has(Constants.PRINTER_CURRENT_HOTEND_TEMPERATURE);
    }

    /**
     * Decode the byte stream and save the given temperature
     * @return true if the data has been decoded successfully, false otherwise
     */
    public boolean processJSON(JSONObject json) {
        double temp = Double.MIN_VALUE;
        try {
            temp = json.getDouble(Constants.PRINTER_CURRENT_HOTEND_TEMPERATURE);
            this.setTemperature(temp);

            Log.i(TAG, "Received temperature data: " + temp);
        }
        catch (JSONException e){
            Log.i(TAG, e.toString());
        }
        return temp > Double.MIN_VALUE;
    }
}
