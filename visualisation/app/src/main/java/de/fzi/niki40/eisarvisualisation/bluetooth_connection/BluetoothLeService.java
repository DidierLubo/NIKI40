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

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

import de.fzi.niki40.eisarvisualisation.helper.Constants;
import de.fzi.niki40.eisarvisualisation.helper.EisDevice;
import de.fzi.niki40.eisarvisualisation.helper.Printer;

/**
 * Service for managing connection and data communication with a GATT server hosted on a
 * given Bluetooth LE device.

 * @author Nathalie Hipp, Hahn-Schickard
 * @modified by FZI Forschungszentrum Informatik on 03.02.2017.
 * @version 1.1
 */
public class BluetoothLeService extends Service {
    private final static String TAG = BluetoothLeService.class.getSimpleName();

    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private BleDeviceHandlerForEis bleDeviceHandlerForEis = BleDeviceHandlerForEis.getInstance();

    private final IBinder mBinder = new LocalBinder();

    private static class TransmissionTuple {
        public TransmissionTuple(BluetoothGatt gatt, BluetoothGattCharacteristic bgChar, byte[] data) {
            this.bgChar = bgChar;
            this.data = data;
            this.gatt = gatt;
        }
        public final BluetoothGatt gatt;
        public final BluetoothGattCharacteristic bgChar;
        public final byte[] data;
    }

    private final Queue<TransmissionTuple> transmissionBuffer = new LinkedList<>();
    protected Semaphore transmissionSemaphore = new Semaphore(1);

    /**
     * Creates an intent containing the action and sensor address and sends it to the
     * broadcast receiver
     * @param action description of the action
     * @param sensorAddr address of the current sensor
     */
    private void broadcastUpdate(final String action, String sensorAddr) {
        final Intent intent = new Intent(action);
        intent.putExtra(Constants.EXTRA_SENSOR_ADDRESS, sensorAddr);
        sendBroadcast(intent);
    }
    /**
     * Creates an intent containing the action, data and sensor address and sends it to the
     * broadcast receiver
     * @param action description of the action
     * @param sensorAddr address of the current sensor
     */
    private void broadcastUpdate(final String action, String sensorAddr, BluetoothGattCharacteristic characteristic) {
        final Intent intent = new Intent(action);
        byte[] data = characteristic.getValue();
        intent.putExtra(Constants.EXTRA_DATA, data);
        intent.putExtra(Constants.EXTRA_SENSOR_ADDRESS, sensorAddr);
        sendBroadcast(intent);
    }


    public class LocalBinder extends Binder {
        public BluetoothLeService getService() {
            return BluetoothLeService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        // After using a given device, you should make sure that BluetoothGatt.close() is called
        // such that resources are cleaned up properly.  In this particular example, close() is
        // invoked when the UI is disconnected from the Service.
        disconnect();
        return super.onUnbind(intent);
    }

    /**
     * Initializes a reference to the local Bluetooth adapter.
     *
     * @return Return true if the initialization is successful.
     */
    public boolean initialize() {
        if (mBluetoothManager == null) {
            mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (mBluetoothManager == null) {
                Log.e(TAG, "Unable to initialize BluetoothManager.");
                return false;
            }
        }
        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Log.e(TAG, "Unable to obtain a BluetoothAdapter.");
            return false;
        }

        return true;
    }

    /**
     * Connects to all sensors stored in BleDeviceHandlerForEis.
     *
     * @return Return true if the connection is initiated successfully. The connection result
     *         is reported asynchronously through the
     *         {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     *         callback.
     */
    public boolean initiateConnection() {
        if (mBluetoothAdapter == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return false;
        }
        Printer printer = bleDeviceHandlerForEis.getPrinter();
        if (printer != null) {
            EisDeviceGattCallback callback = new EisDeviceGattCallback(printer.getAddress());
            printer.connect(getApplicationContext(), callback);
        } else {
            Log.w(TAG, "No printer found");
            return false;
        }
        return true;
    }

    /**
     * Disconnects all existing connections. The disconnection result
     * is reported asynchronously through the
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     * callback.
     */
    public void disconnect() {
        if (mBluetoothAdapter == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        bleDeviceHandlerForEis.disconnectPrinter();
    }

    /**
     * Request a read on a given {@code BluetoothGattCharacteristic}. The read result is reported
     * asynchronously through the {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    public void readCharacteristic(EisDevice eisDevice, BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        } else if (eisDevice == null) {
            Log.w(TAG, "eisSensor object was null");
        }
        eisDevice.getGatt().readCharacteristic(characteristic);
    }

    /**
     * Enables or disables notification on a given characteristic.
     *
     * @param characteristic Characteristic to act on.
     * @param enabled If true, enable notification.  False otherwise.
     */
    public void setCharacteristicNotification(EisDevice eisDevice, BluetoothGattCharacteristic characteristic,
                                              boolean enabled) {
        if (mBluetoothAdapter == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        BluetoothGatt tmpGatt = eisDevice.getGatt();
        if (tmpGatt == null) {
            Log.w(TAG, "GATT null for eisSensor: " + eisDevice.getDevice().getAddress());
            return;
        }
        tmpGatt.setCharacteristicNotification(characteristic, enabled);

        if (Constants.UUID_PRINTER_CHAR.equals(characteristic.getUuid())) {
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(Constants.UUID_PRINTER_DESC);
            if (descriptor != null) {
                descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                tmpGatt.writeDescriptor(descriptor);
            }
        }

    }

    public void enqueueToTransmit(BluetoothGatt gatt, BluetoothGattCharacteristic viaCharacteristic, byte[] data) {
        try {
            transmissionSemaphore.acquire();
            for (int i = 0; i < data.length; i += 20) {
                this.transmissionBuffer.add(new TransmissionTuple(gatt, viaCharacteristic, Arrays.copyOfRange(data, i, i+20)));
            }
            if (!transmissionBuffer.isEmpty()) {
                TransmissionTuple tup = transmissionBuffer.poll();
                tup.bgChar.setValue(tup.data);
                tup.gatt.writeCharacteristic(tup.bgChar);
            }
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

    }

    /**
     * Implements callback methods for GATT events that the app cares about.
     * connection state change, services discovered, characteristic read and changed
     */
    private class EisDeviceGattCallback extends BluetoothGattCallback {
        private final String macAddress;

        public EisDeviceGattCallback(String macAddress) {
            this.macAddress = macAddress;
        }

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            String intentAction;
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                intentAction = Constants.ACTION_GATT_CONNECTED;
                broadcastUpdate(intentAction, macAddress);
                Log.i(TAG, "Connected to GATT server on: " + macAddress);
                // Attempts to discover services after successful connection.
                Log.i(TAG, "Attempting to start service discovery on sensor:" + macAddress);
                if (bleDeviceHandlerForEis.checkPrinterAddress(macAddress)) {
                    bleDeviceHandlerForEis.getPrinter().getGatt().discoverServices();
                }

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                intentAction = Constants.ACTION_GATT_DISCONNECTED;
                Log.i(TAG, "Disconnected from GATT server: " + macAddress);
                broadcastUpdate(intentAction, macAddress);
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(Constants.ACTION_GATT_SERVICES_DISCOVERED, macAddress);
            }
            Log.i(TAG, "onServicesDiscovered received: " + status);
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(Constants.ACTION_DATA_AVAILABLE, macAddress, characteristic);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            broadcastUpdate(Constants.ACTION_DATA_AVAILABLE, macAddress, characteristic);
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.i(TAG,"Characteristic write successful: UUID = " + characteristic.getUuid().toString());
            }
            if (!transmissionBuffer.isEmpty()) {
                TransmissionTuple tup = transmissionBuffer.poll();
                tup.bgChar.setValue(tup.data);
                tup.gatt.writeCharacteristic(tup.bgChar);
            } else {
                BluetoothLeService.this.transmissionSemaphore.release();
            }
        }
    }

}
