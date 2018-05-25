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

import android.app.Activity;
import android.app.ListActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanSettings;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import de.fzi.niki40.eisarvisualisation.BluetoothDeviceListAdapter;
import de.fzi.niki40.eisarvisualisation.EisArActivity;
import de.fzi.niki40.eisarvisualisation.OPCUA_Connector;
import de.fzi.niki40.eisarvisualisation.R;
import de.fzi.niki40.eisarvisualisation.error.ErrorActivity;
import de.fzi.niki40.eisarvisualisation.helper.Constants;
import de.fzi.niki40.eisarvisualisation.helper.EisSensor;
import de.fzi.niki40.eisarvisualisation.helper.Printer;


/**
 * Created by FZI Forschungszentrum Informatik on 03.02.2017.
 * Launcher Activity of the app which scans for available bluetooth devices and lets the user
 * choose 'ESIMA-EIS'
 */
public class DeviceScanActivity extends ListActivity {


    OPCUA_Connector myService;
    boolean isBound = false;




    private BluetoothDeviceListAdapter bleDeviceListAdapter;
    private Button connectButton;
    private Button connectButtonWithOutPrinter;
    /**
     * BluetoothAdapter object for every bluetooth activity within the application
     */
    private BluetoothAdapter mBluetoothAdapter;
    /**
     * BluetoothLeScanner to handle bluetooth low energy scans
     */
    private BluetoothLeScanner mBluetoothLeScanner;

    /**
     * holds and handles all desired sensors
     */
    private BleDeviceHandlerForEis bleDeviceHandlerForEis = BleDeviceHandlerForEis.getInstance();
    List<EisSensor> eisSensors = bleDeviceHandlerForEis.getSensors();
    /**
     * Scan Settings and filter to filter the scan result list
     */
    private ScanSettings mSettings;
    private String[] mFilterList = {
            "00:A0:50:05:1A:10",
    };
    private List<ScanFilter> mFilters = new ArrayList<ScanFilter>();

    private boolean isScanning;
    private Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_scan);
        getActionBar().setTitle(R.string.title_scan_activity);
        mHandler = new Handler();

        //start service (OPC UA Connection)
       // Intent service = new Intent(this, OPCUA_Connector.class);
       // bindService(service, myConnection, Context.BIND_AUTO_CREATE);







        // determine whether BLE is supported on the device or not. If not show an error screen
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            showErrorActivity(getResources().getString(R.string.error_ble_not_supported));
        } else {
            // Set the scan mode and the filter list to scan and filter available bluetooth devices
            mSettings = new ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_BALANCED).build();
            for (int i = 0; i < mFilterList.length; i++) {
                ScanFilter filter = new ScanFilter.Builder().setDeviceAddress(mFilterList[i]).build();
                mFilters.add(filter);
            }
            // Initializes list view adapter.
            bleDeviceListAdapter = new BluetoothDeviceListAdapter(getLayoutInflater());
            setListAdapter(bleDeviceListAdapter);
            // Initializes the Bluetooth adapter
            final BluetoothManager bluetoothManager =
                    (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            mBluetoothAdapter = bluetoothManager.getAdapter();
            if (mBluetoothAdapter == null) {
                showErrorActivity(getResources().getString(R.string.error_bluetooth_not_supported));
            }
            //initialize button
            connectButton = (Button)findViewById(R.id.connect_button);
            //initialize button
            connectButtonWithOutPrinter = (Button)findViewById(R.id.connectWithoutPrinter);
            //connectButton.setEnabled(false);
            connectButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    //if all sensors are in range start EisArActivity
                    if (bleDeviceHandlerForEis.isPrinterConnected()) {
                        startConnection();
                    } else {
                        Toast.makeText(getApplicationContext(), getString(R.string.error_few_devices),
                                Toast.LENGTH_SHORT).show();

                    }

                }
            });
            connectButtonWithOutPrinter.setOnClickListener(new View.OnClickListener() {
                //start EisArActivity without connection to Printer
                @Override
                public void onClick(View view) {
                    startConnection();

                }
            });



        }
    }

    /*
    onButtonClick() {
        mBluetoothLeScanner.flushPendingScanResults(mScanCallback);
    }
    * */

    @Override
    protected void onResume() {
        super.onResume();
        // Ensures Bluetooth is enabled on the device.  If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            showEnableBluetoothRequest();
        } else if (mBluetoothLeScanner == null){
            mBluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
            scanLeDevice();
        } else {
            scanLeDevice();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Handles the visibility of stop/scan buttons and refresh icon
        getMenuInflater().inflate(R.menu.main, menu);
        if (!isScanning) {
            menu.findItem(R.id.menu_stop).setVisible(false);
            menu.findItem(R.id.menu_scan).setVisible(true);
            menu.findItem(R.id.menu_refresh).setActionView(null);
        } else {
            menu.findItem(R.id.menu_stop).setVisible(true);
            menu.findItem(R.id.menu_scan).setVisible(false);
            menu.findItem(R.id.menu_refresh).setActionView(
                    R.layout.actionbar_indeterminate_progress);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_scan:
                scanLeDevice();
                break;
            case R.id.menu_stop:
                isScanning = false;
                //Remove the stop runner because scanning is stopped manually
                mHandler.removeCallbacks(bleScanStopRunner);
                mBluetoothLeScanner.stopScan(mScanCallback);
                invalidateOptionsMenu();
                break;
        }
        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == Constants.REQUEST_ENABLE_BT) {
            if (resultCode == Activity.RESULT_OK) {
                if (mBluetoothLeScanner == null) {
                    mBluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
                }
                scanLeDevice();
            } else {
                showErrorActivity(getResources().getString(R.string.error_bluetooth_not_enabled));
            }
        }
    }

    private void startConnection() {
      /*if (isScanning) {
            mBluetoothLeScanner.stopScan(mScanCallback);
            mHandler.removeCallbacks(bleScanStopRunner);
            isScanning = false;
        }*/
        Intent intent = new Intent(this, EisArActivity.class);
        startActivity(intent);
    }

    /**
     * Shows an enable bluetooth request to the user
     */
    private void showEnableBluetoothRequest() {
        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(enableBtIntent, Constants.REQUEST_ENABLE_BT);
    }

    /**
     * Starts a new activity which shows an error message to the user and exit this activity
     * @param msg The error message that is shown to the user
     */
    private void showErrorActivity(String msg) {
        Intent intent = new Intent(this, ErrorActivity.class);
        intent.putExtra(ErrorActivity.ERROR_MSG_TAG, msg);
        startActivity(intent);
        finish();
        return;
    }

    /**
     * Scans for available bluetooth devices and uses mScanCallback to fill the list
     */
    private void scanLeDevice() {
        if (mBluetoothAdapter.isEnabled()) {
            bleDeviceListAdapter.clear();
            // Stops scanning after a pre-defined scan period.
            mHandler.postDelayed(bleScanStopRunner, Constants.SCAN_PERIOD);

            isScanning = true;
            mBluetoothLeScanner.startScan(Arrays.asList(
                    new ScanFilter.Builder().setDeviceAddress(Constants.PrinterAddress).build()),
                    new ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_LOW_POWER).build(),
                    mScanCallback);
            //mBluetoothLeScanner.startScan(mScanCallback);
        } else {
            isScanning = false;
            showEnableBluetoothRequest();
        }
        invalidateOptionsMenu();
    }

    /**
     * Callback to add the available bluetooth devices to the list after the scan finished
     */
    private ScanCallback mScanCallback =  new ScanCallback(){
        @Override
        public void onScanResult(int callbackType, final android.bluetooth.le.ScanResult result) {

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    bleDeviceListAdapter.addDevice(result.getDevice());
                    bleDeviceListAdapter.notifyDataSetChanged();
                }
            });

            //Test
            Log.i("SENSOR_INFO", "Address: " + result.getDevice().getAddress());
            Log.i("SENSOR_INFO", "Name: " + result.getDevice().getName());
            Log.i("SENSOR_INFO", "Bluetooth Class: " + result.getDevice().getBluetoothClass());
            if (isDeviceDesiredPrinter(result.getDevice())) {
                bleDeviceHandlerForEis.addPrinter(new Printer(result.getDevice()));
            }
        }
        @Override
        public void onScanFailed(int errorCode) {
            bleDeviceListAdapter.clear();
            bleDeviceHandlerForEis.disconnectPrinter();
            //connectButton.setEnabled(false);
        }

    };

    private boolean isDeviceDesiredPrinter(BluetoothDevice device) {
        String name = device.getAddress();
        if (name != null && name.equals(Constants.PrinterAddress)) {
            return true;
        }
        return false;
    }

    /**
     * Runnable object to stop the scan process
     */
    private Runnable bleScanStopRunner = new Runnable() {
        @Override
        public void run() {
            isScanning = false;
            mBluetoothLeScanner.stopScan(mScanCallback);
            invalidateOptionsMenu();
        }
    };


    private ServiceConnection myConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {



            OPCUA_Connector.MyLocalBinder binder = (OPCUA_Connector.MyLocalBinder) service;
            myService = binder.getService();
            myService.ConnectOPC();
            isBound = true;

        }


        @Override
        public void onServiceDisconnected(ComponentName name) {
            isBound = false;

        }
    };

}
