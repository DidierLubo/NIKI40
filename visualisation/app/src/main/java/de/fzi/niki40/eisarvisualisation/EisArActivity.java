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

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import org.artoolkit.ar.base.ARActivity;
import org.artoolkit.ar.base.rendering.ARRenderer;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

import de.fzi.niki40.eisarvisualisation.bluetooth_connection.BleDeviceHandlerForEis;
import de.fzi.niki40.eisarvisualisation.bluetooth_connection.BluetoothLeService;
import de.fzi.niki40.eisarvisualisation.helper.Constants;
import de.fzi.niki40.eisarvisualisation.helper.EMeter;
import de.fzi.niki40.eisarvisualisation.helper.EisSensor;
import de.fzi.niki40.eisarvisualisation.helper.Printer;

/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * This class is responsible for displaying the measured live data.
 */
public class EisArActivity extends ARActivity {

    public static Context context;
    private final static String TAG = EisArActivity.class.getSimpleName();
    // BLE Service for the bluetooth connection
    private BluetoothLeService mBluetoothLeService;
    private BleDeviceHandlerForEis bleDeviceHandlerForEis = BleDeviceHandlerForEis.getInstance();
    private Button goToHistoryView;
    public static final String BROADCAST_ACTION = "live data";
    MyBroadCastReceiver myBroadCastReceiver;
    //Camera Frame for ARTool
    private FrameLayout cameraFrame;
    //Layout to display the measured data
    private RelativeLayout dataLayout;
    private TextView sensor1dataView;
    private TextView sensor2dataView;
    private TextView sensor3dataView;
    private TextView[] sensorDataViews = {sensor1dataView, sensor2dataView, sensor3dataView};
    private TextView printerDataView;
    //TEST
    private RelativeLayout seekBarLayout;
    private SeekBar tmpBar;
    private SeekBar windBarX;
    private SeekBar windBarY;
    private SeekBar windScaleBar;
    private Button sendButton;

    private NumberFormat formater = new DecimalFormat("#0.00");
    // temporary string to manage the airflow data
    private String incommingData = "";

    private StringBuilder currentReceiveBuffer = new StringBuilder();
    private Queue<String> receivedData = new LinkedList<>();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_eis_ar);
        context = getApplicationContext();
        // initialize UI elements
        seekBarLayout = (RelativeLayout) findViewById(R.id.seekBarLayout);
        tmpBar = (SeekBar) findViewById(R.id.seekBar);
        windBarX = (SeekBar) findViewById(R.id.seekBar1);
        windBarY = (SeekBar) findViewById(R.id.seekBar3);
        windScaleBar = (SeekBar) findViewById(R.id.seekBar2);
        initializeControlBars();
        sendButton = (Button) findViewById(R.id.sendButton);
        addSendButtonListener();
        cameraFrame = (FrameLayout) findViewById(R.id.cameraFrame);
        sensor1dataView = (TextView) findViewById(R.id.s1dataTextView);
        sensor2dataView = (TextView) findViewById(R.id.s2dataTextView);
        sensor3dataView = (TextView) findViewById(R.id.s3dataTextView);
        printerDataView = (TextView) findViewById(R.id.pdataTextView);
        dataLayout = (RelativeLayout) findViewById(R.id.dataLayout);
        // bind this client to the BluetoothLeService
        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
        checkIfTextIsEnabled();
        myBroadCastReceiver = new MyBroadCastReceiver();
        registerMyReceiver();




        goToHistoryView = (Button) findViewById(R.id.buttonHistoryData);

        goToHistoryView.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intentHistory = new Intent(EisArActivity.this, HistoryNavigationActivty.class);
                startActivity(intentHistory);
            }
            });
    }

    private void initializeControlBars() {
        if (Constants.DISPLAY_CONTROL_BARS) {
            windBarX.setMax(40);
            windBarY.setMax(40);
            windBarX.setProgress(20);
            windBarY.setProgress(20);
            windScaleBar.setMax(100);
            tmpBar.setProgress(195);
            tmpBar.setMax(300);
            tmpBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                    bleDeviceHandlerForEis.getPrinter().setTemperature(i);
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {

                }
            });

            windScaleBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                    Constants.ARROW_SCALE = i;
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {

                }
            });
        } else {
            seekBarLayout.setVisibility(View.INVISIBLE);
        }
    }

    private void addSendButtonListener() {
        if (Constants.DISPLAY_SEND_BUTTON) {
            sendButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    Printer printer = bleDeviceHandlerForEis.getPrinter();
                    if (printer != null) {
                        printer.sendDataToBLE(mBluetoothLeService, 1.2f, 1.33f, 1.4f);
                        Toast.makeText(getApplicationContext(), "Send data to printer",
                                Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(getApplicationContext(), "The printer is not connected",
                                Toast.LENGTH_SHORT).show();
                    }
                }
            });
        } else {
            sendButton.setVisibility(View.INVISIBLE);
        }
    }

    private void checkIfTextIsEnabled() {
        if (!Constants.DISPLAY_DATA) {
            dataLayout.setVisibility(View.GONE);
        }
    }

    @Override
    protected ARRenderer supplyRenderer() {
        return new EisArRenderer();
    }

    @Override
    protected FrameLayout supplyFrameLayout() {
        return cameraFrame;
    }

    @Override
    public void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        // Todo: check if this works
        if (mBluetoothLeService != null) {
            mBluetoothLeService.initiateConnection();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }

    /**
     * ServiceConnection callback gets notified when ble service is connected or disconnected
     */
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            mBluetoothLeService.initiateConnection();
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    /**
     * Handles the events fired by the BluetoothLeService.
     * ACTION_GATT_CONNECTED: connected to a GATT server.
     * ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
     * ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
     * ACTION_DATA_AVAILABLE: received data from the device. This can be a result of read or
     * notification operations.
     */
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            final String address = intent.getStringExtra(Constants.EXTRA_SENSOR_ADDRESS);

            boolean dataChanged = false;

            if (bleDeviceHandlerForEis.checkPrinterAddress(address)) {
                Printer printer = bleDeviceHandlerForEis.getPrinter();
                if (Constants.ACTION_GATT_DISCONNECTED.equals(action)) {
                    gattDisconnected();
                } else if (Constants.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                    List<BluetoothGattService> gattServices = printer.getGattServices();
                    for (BluetoothGattService s : gattServices) {
                        List<BluetoothGattCharacteristic> gattCharacteristic = s.getCharacteristics();
                        for (BluetoothGattCharacteristic c : gattCharacteristic) {
                            if (c.getUuid().equals(Constants.UUID_PRINTER_CHAR)) {
                                BluetoothGattCharacteristic characteristic = c;
                                int charaProp = characteristic.getProperties();
                                if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                                    mBluetoothLeService.readCharacteristic(printer, characteristic);
                                }
                                if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                                    mBluetoothLeService.setCharacteristicNotification(printer, characteristic, true);
                                }
                            }
                        }
                    }
                } else if (Constants.ACTION_DATA_AVAILABLE.equals(action)) {
                    // read the data
                    byte[] data = intent.getByteArrayExtra(Constants.EXTRA_DATA);
                    if (data != null && data.length > 0) {
                        splitAndBufferRawData(data);
                        while (!receivedData.isEmpty()) {
                            String dataLine = receivedData.poll();
                            JSONObject json = tryParseJSON(dataLine);
                            if (json != null) {
                                if (jsonContainsSensorData(json)) {
                                    dataChanged |= processJSON(json);
                                }
                                if (printer.jsonContainsPrinterData(json)){
                                    dataChanged |= printer.processJSON(json);
                                }
                            }
                        }
                    }
                }
            }

        }
    };

    /**
     * Should be called when a GattService disconnects.
     * disconnects all sensors and returns to the DeviceScanActivity
     */
    private void gattDisconnected() {
        Toast.makeText(getApplicationContext(), getString(R.string.error_disconnected),
                Toast.LENGTH_SHORT).show();
        mBluetoothLeService.disconnect();
        finish();
    }

    /**
     * Creates an IntentFilter for a Gatt update intent. This is a structured description of Intent
     * values to be matched. This IntentFilter contains
     * ACTION_GATT_CONNECTED, ACTION_GATT_DISCONNECTED, ACTION_GATT_SERVICES_DISCOVERED and
     * ACTION_DATA_AVAILABLE
     * @return the created IntentFilter which matches against the stated actions
     */
    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Constants.ACTION_GATT_CONNECTED);
        intentFilter.addAction(Constants.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(Constants.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(Constants.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    /**
     * Decodes a byte array containing the data into an EISStruct object and transfers this object
     * to the visible tab
     */
    private void displayData() {

        if (Constants.DISPLAY_DATA) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    List<EisSensor> eisSensors = bleDeviceHandlerForEis.getSensors();
                    Printer printer = bleDeviceHandlerForEis.getPrinter();

                    if (printer != null) {
                        printerDataView.setText(formater.format(printer.getTemperature()) + "°C");
                        printer.sendDataToBLE(mBluetoothLeService,
                                eisSensors.get(0).getAirflow(),
                                eisSensors.get(1).getAirflow(),
                                eisSensors.get(2).getAirflow());
                    }
                }
            });
        }
    }

    /**
     * Checks if the data contains airflow data
     * @param data
     * @return the sensor id if data contains the airflow for the dedicated sensor, -1 otherwise
     */
    private void splitAndBufferRawData(byte[] data) {

        String text = new String(data);
        for (char c : text.toCharArray()) {
            if (c == '{') {
                receivedData.add(currentReceiveBuffer.toString());
                currentReceiveBuffer = new StringBuilder();
                currentReceiveBuffer.append(c);
            } else {
                currentReceiveBuffer.append(c);
                if (c == '}') {
                    receivedData.add(currentReceiveBuffer.toString());
                    currentReceiveBuffer = new StringBuilder();
                }
            }
        }
    }

    private JSONObject tryParseJSON(String dataLine) {
        JSONObject reader = null;
        try {
             reader = new JSONObject(dataLine);
        } catch (JSONException e) {
            Log.i(TAG, e.toString());
        }
        return reader;
    }

    private boolean jsonContainsSensorData(JSONObject json) {
        return json.has(Constants.JSON_SENSOR_ID)
                && json.has(Constants.JSON_SENSOR_FLOW_X)
                && json.has(Constants.JSON_SENSOR_FLOW_Y);
    }

    private boolean processJSON(JSONObject json) {
        int sensorID = -1;
        double flowX = Double.MIN_VALUE;
        double flowY = Double.MIN_VALUE;

        try {
            sensorID = json.getInt(Constants.JSON_SENSOR_ID);
            flowX = json.getDouble("flowX");
            flowY = json.getDouble("flowY");
        } catch (JSONException ex) {
            Log.w(TAG, ex.toString());
        }
        if (flowX != Double.MIN_VALUE && flowY != Double.MIN_VALUE && sensorID != -1) {
            String sensorAddress;
            switch (sensorID) {
                case 1:
                    sensorAddress = Constants.Sensor1Address;
                    break;
                case 2:
                    sensorAddress = Constants.Sensor2Address;
                    break;
                case 3:
                    sensorAddress = Constants.Sensor3Address;
                    break;
                default:
                    Log.i(TAG, "Incorrect sensorID recieved");
                    return false;
            }
            Log.i(TAG, "Correct data for sensor(" + sensorAddress + ") received: x = " + flowX + "y = " + flowY);
            bleDeviceHandlerForEis.getSensor(sensorAddress).setAirflow(flowX, flowY);
            return true;
        }

        return false;
    }

    private void registerMyReceiver() {
        try
        {
            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(BROADCAST_ACTION);
            registerReceiver(myBroadCastReceiver, intentFilter);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }

    /*BroadcastReceiver responsible for displaying data from EMeter and Printer (Temp)
    Sending windflow Data back to Printer*/

    class MyBroadCastReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            try {

                if (sensor1dataView != null && sensor2dataView != null) {
                    float current = EMeter.getEmeterPower() * 40; // scalling the Data
                    sensor1dataView.setText(Float.toString(current) + " mA");
                    sensor2dataView.setText(Float.toString(EMeter.getTempPrinter())+ " °C");
                    displayData();
                }
            } catch (Exception ex) {
            }
        }

    }
}
