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
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.util.Log;

import java.util.List;

/**
 * Created by FZI Forschungszentrum Informatik on 10.03.2017.
 */

public class EisDevice {
    private static String TAG = EisDevice.class.getName();

    // MAC Adress of the device
    private final String address;

    private BluetoothDevice device;
    BluetoothGatt gatt;

    public EisDevice(BluetoothDevice device) {
        if (device != null) {
            this.device = device;
            this.address = device.getAddress();
        } else {
            this.address = null;
        }
    }

    public EisDevice(String address) {
        this.address = address;
    }

    /**
     * Compares the MAC-Address of this sensor with the other one
     * @param o
     * @return true if MAC-Address are equal, false otherwise
     */
    @Override
    public boolean equals(Object o) {
        if (o == null) {
            return false;
        } else if (o.getClass().equals(this.getClass())) {
            EisDevice tmp = (EisDevice) o;
            if (tmp.getAddress().equals(address)) {
                return true;
            }
        }
        return false;
    }

    public BluetoothDevice getDevice() {
        return device;
    }

    public BluetoothGatt getGatt() {
        return gatt;
    }

    public String getAddress() {
        return address;
    }

    public List<BluetoothGattService> getGattServices() {
        if (gatt != null) {
            return gatt.getServices();
        }
        return null;
    }

    public void closeGatt() {
        if (gatt != null) {
            gatt.close();
            gatt = null;
        }
    }

    /**
     * Connect to the GATT server hosted on the sensor
     * @param context The application context
     * @return Return true if the connection is initiated successfully. The connection result
     *         is reported asynchronously through the
     *         {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     *         callback
     */
    public boolean connect(Context context, BluetoothGattCallback callback) {
        // Previously connected device.  Try to reconnect.
        if (gatt != null) {
            Log.d(TAG, "Trying to connect with existing bluetoothGATT to: " + address);
            if (gatt.connect()) {
                return true;
            } else {
                Log.d(TAG, "Could not connect to existing bluetoothGATT: " + address);
                return false;
            }
        }

        // We want to directly connect to the device, so we are setting the autoConnect
        // parameter to false.
        gatt = device.connectGatt(context, false, callback);
        Log.d(TAG, "Trying to create a new connection to: " + device.getAddress());
        return true;
    }

    /**
     * Disconnect from the GATT server hosted on the sensor
     * @return true if GATT server could be disonnected, false otherwise
     */
    public boolean disconnect() {
        if (gatt != null) {
            gatt.disconnect();
            return true;
        } else {
            return false;
        }
    }
}
