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

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.IBinder;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.opcfoundation.ua.builtintypes.NodeId;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import de.fzi.niki40.eisarvisualisation.bluetooth_connection.BleDeviceHandlerForEis;
import de.fzi.niki40.eisarvisualisation.bluetooth_connection.DeviceScanActivity;
import de.fzi.niki40.eisarvisualisation.helper.EisSensor;
import de.fzi.niki40.eisarvisualisation.helper.MonDataIt;
import de.fzi.niki40.eisarvisualisation.helper.MonitoredDataItemManager;
import de.fzi.niki40.eisarvisualisation.helper.OpcuaItemManager;
import de.fzi.niki40.eisarvisualisation.helper.OpcuaMonitoredItem;

/**
 * Created by FZI Forschungszentrum Informatik on 05.03.2018.
 * The following activity represents the Expert view in the NIKI 4.0 app
 * It contains the operation menu, list of sensors, as well as the IP Address field
 */

public class ScanServerNodeIDs extends Activity {

    private BleDeviceHandlerForEis bleDeviceHandlerForEis = BleDeviceHandlerForEis.getInstance();
    List<EisSensor> eisSensors = bleDeviceHandlerForEis.getSensors();
    private MonitoredDataItemManager monitoringManager = MonitoredDataItemManager.getInstance();
    List<MonDataIt> monitoredItems = monitoringManager.getMonitoredSensors();
    private OpcuaItemManager opcuaItemManager = OpcuaItemManager.getInstance();
    List<OpcuaMonitoredItem> opcDataItems = opcuaItemManager.getMonitoredSensors();
    Button saveUrl;
    Button addSensor;
    Button delSensor;
    EditText serverURL;
    EditText sensorNameTextField;
    ListView listViewOperations;
    ListView listViewSensors;
    ListView listViewSensorsNotFound;
    TextView listSensors;
    TextView textViewAllSensors;
    public static final String BROADCAST_ACTION = "scan";
    MyBroadCastReceiver myBroadCastReceiver;
    List<String> listofFoundSensors = new ArrayList<String>();
    List<String> listofNotFoundSensors = new ArrayList<String>();
    ScanServerService myScanConnection;
    OPCUA_Connector opcConnect;
    boolean isBound = false;
    boolean isBoundOpcConnector = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        overridePendingTransition(R.anim.abc_fade_in, R.anim.abc_fade_out);
        setContentView(R.layout.activity_start_window);
        String[] operations = {"Scan Server", "Subscribe Sensor" ,"Live View", "History View"};
        ListAdapter operationsAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, operations);
        final ListAdapter sensorlistAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, listofFoundSensors); // listAdapter, representing the sensors found in the server
        final ListAdapter notfoundsensorlistAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, listofNotFoundSensors); // listAdapter, representing the non found sensors
        sensorNameTextField = (EditText) findViewById(R.id.sensorName1);
        serverURL = (EditText) findViewById(R.id.editTextServerIP);
        listSensors = (TextView) findViewById(R.id.textViewInformation);
        listViewOperations = (ListView) findViewById(R.id.listViewOperations);
        listViewSensorsNotFound = (ListView)  findViewById(R.id.listViewnotfound);
        listViewSensors = (ListView) findViewById(R.id.listViewSensors);
        saveUrl = (Button) findViewById(R.id.saveURL);
        addSensor = (Button) findViewById(R.id.addSensor) ;
        delSensor = (Button) findViewById(R.id.deleteSensor);
        myBroadCastReceiver = new MyBroadCastReceiver();
        registerMyReceiver();
        textViewAllSensors = (TextView) findViewById(R.id.textViewAllSensors);
        SharedPreferences preferencesCheck = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
        String url = preferencesCheck.getString("url", "");
        serverURL.setText(url);

        // if a url is given, execute following methods on start
        if(!url.isEmpty()) {
           extractDate();
           seesensors();
        }

        saveUrl.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
                SharedPreferences.Editor editor = sharedPref.edit();
                editor.putString("url", serverURL.getText().toString());
                editor.apply();

                Toast.makeText(ScanServerNodeIDs.this, "URL Saved", Toast.LENGTH_LONG).show();
            }
        });



        listViewOperations.setAdapter(operationsAdapter);
        listViewOperations.setOnItemClickListener(
                new AdapterView.OnItemClickListener() {
                    @Override
                    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                                if (i==0){
                                    scanserver();
                                }
                                 if (i==1){
                                    seesensors();
                                    extractDate();
                                 }
                                 if (i==2){
                                    searchprinter();

                                 }
                                 if (i==3){
                                     gotohistoryview();
                                 }
                    }
                }
        );

        listViewSensors.setAdapter(sensorlistAdapter);
        listViewSensors.setOnItemClickListener(new AdapterView.OnItemClickListener(){
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int i, long l) {
                String sensorname = String.valueOf(parent.getItemAtPosition(i));
                String flowX = sensorname + (".flowX");
                String flowY = sensorname + (".flowY");
                String voltage = sensorname + (".voltage");
                String power = sensorname + (".power");
                String current = sensorname + (".current");
                String temp = sensorname + (".temp");
                if (sensorname.equals("EMETER_CLIENT2")) {
                    sensorNameTextField.setText(sensorname);
                    listSensors.setText("ID-Voltage >" + sensorname + "< "+ opcuaItemManager.getOpcuaMonitoredItem(voltage).getType() + "\n" +
                            "ID-Power >" + sensorname + "< "+ opcuaItemManager.getOpcuaMonitoredItem(power).getType() + "\n" +
                            "ID-Current >" + sensorname + "< "+ opcuaItemManager.getOpcuaMonitoredItem(current).getType());
                } if (sensorname.equals("Anycubic 3D Printer")) {
                    sensorNameTextField.setText(sensorname);
                    listSensors.setText("ID-Temp >" + sensorname + "< "+ opcuaItemManager.getOpcuaMonitoredItem(temp).getType() + "\n");
                }
                if  (!sensorname.equals("EMETER_CLIENT2") && !sensorname.equals("Anycubic 3D Printer")) {
                    listSensors.setText("ID-FlowX >" + sensorname + "< " + opcuaItemManager.getOpcuaMonitoredItem(flowX).getType() + "\n" +
                            "ID-FlowY >" + sensorname + "< " + opcuaItemManager.getOpcuaMonitoredItem(flowY).getType());
                    sensorNameTextField.setText(sensorname);
                }
            }
        });

        listViewSensorsNotFound.setAdapter(notfoundsensorlistAdapter);
        listViewSensorsNotFound.setOnItemClickListener(new AdapterView.OnItemClickListener(){
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int i, long l) {
                String sensorname = String.valueOf(parent.getItemAtPosition(i));
                sensorNameTextField.setText(sensorname);
            }
        });

        addSensor.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if (sensorNameTextField.getText().toString().isEmpty()) {
                    Toast.makeText(ScanServerNodeIDs.this, "Please Enter Sensor's Name", Toast.LENGTH_SHORT).show();
                } else {
                    SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    if (monitoringManager.getMonitoredDataItem(sensorNameTextField.getText().toString()) == null) {
                        if (sensorNameTextField.getText().toString().equals("EMETER_CLIENT2")) {
                            monitoringManager.addMonitoredDataItem(new MonDataIt());
                            monitoredItems.get(monitoredItems.size() - 1).setName(sensorNameTextField.getText().toString());
                            editor.putString((monitoredItems.get(monitoredItems.size() - 1).getName()) + ".voltage", "");
                            editor.putString((monitoredItems.get(monitoredItems.size() - 1).getName()) + ".current", "");
                            editor.putString((monitoredItems.get(monitoredItems.size() - 1).getName()) + ".power", "");
                            editor.apply();
                        }
                        if (sensorNameTextField.getText().toString().equals("Anycubic 3D Printer")){
                            monitoringManager.addMonitoredDataItem(new MonDataIt());
                            monitoredItems.get(monitoredItems.size() - 1).setName(sensorNameTextField.getText().toString());
                            editor.putString((monitoredItems.get(monitoredItems.size() - 1).getName()) + ".temp", "");
                            editor.apply();
                        }
                        if (!sensorNameTextField.getText().toString().equals("Anycubic 3D Printer") && !sensorNameTextField.getText().toString().equals("EMETER_CLIENT2")) {
                            monitoringManager.addMonitoredDataItem(new MonDataIt());
                            monitoredItems.get(monitoredItems.size() - 1).setName(sensorNameTextField.getText().toString());
                            editor.putString((monitoredItems.get(monitoredItems.size() - 1).getName()) + ".flowX", "");
                            editor.putString((monitoredItems.get(monitoredItems.size() - 1).getName()) + ".flowY", "");
                            editor.apply();
                        }
                    } else {
                        Toast.makeText(ScanServerNodeIDs.this, "Sensor exist", Toast.LENGTH_SHORT).show();
                    }
                    if (!listofFoundSensors.contains(sensorNameTextField.getText().toString()) && !listofNotFoundSensors.contains(sensorNameTextField.getText().toString())) {
                        listofNotFoundSensors.add(sensorNameTextField.getText().toString());
                    }
                }
                startActivity(new Intent(ScanServerNodeIDs.this, ScanServerNodeIDs.class));
            }
        });

        delSensor.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
                SharedPreferences.Editor editor = sharedPref.edit();
                String todelete = sensorNameTextField.getText().toString();
                if (listofFoundSensors.contains(todelete)) {
                    int indexOfElementToDelete = listofFoundSensors.indexOf(todelete);
                    listofFoundSensors.remove(listofFoundSensors.get(indexOfElementToDelete));
                    String deleteflowx = todelete + (".flowX");
                    String deleteflowy = todelete + (".flowY");
                    opcDataItems.remove(opcuaItemManager.getOpcuaMonitoredItem(deleteflowx));
                    opcDataItems.remove(opcuaItemManager.getOpcuaMonitoredItem(deleteflowy));
                }
                if (listofNotFoundSensors.contains(todelete)) {
                   int indexOfElementToDelete = listofNotFoundSensors.indexOf(todelete);
                   listofNotFoundSensors.remove(listofNotFoundSensors.get(indexOfElementToDelete));
                }
                if (monitoringManager.getMonitoredDataItem(sensorNameTextField.getText().toString()) != null) {
                    monitoredItems.remove(monitoringManager.getMonitoredDataItem(sensorNameTextField.getText().toString()));
                }
                editor.remove((sensorNameTextField.getText().toString()) + ".flowX");
                editor.remove((sensorNameTextField.getText().toString()) + ".flowY");
                editor.apply();
                seesensors();
                startActivity(new Intent(ScanServerNodeIDs.this, ScanServerNodeIDs.class));
            }
        });
    }

    public void seesensors() {
        SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
        for (int i=0; i<opcDataItems.size();i++){
            int charindex = opcDataItems.get(i).getName().lastIndexOf(".");
            String name = opcDataItems.get(i).getName().substring(0,charindex);
            if(!listofFoundSensors.contains(name)){
                listofFoundSensors.add(name);
            }
        }
        for(int start = 0; start<listofFoundSensors.size();start++){
            String name = listofFoundSensors.get(start);
            if(listofNotFoundSensors.contains(name)){
                int i = listofFoundSensors.indexOf(name);
                listofFoundSensors.remove(i);
            }
        }
    }


   /*
    *   Method starting the history view Activity
    */
    public void gotohistoryview(){
        Intent intentHistory = new Intent(ScanServerNodeIDs.this, HistoryNavigationActivty.class);
        startActivity(intentHistory);
    }

    /*
     *   Method starting the scanning server background service responsible for checking,
     *   if a entered sensor is also represented in the OPC UA Server by a node
     */
    public void scanserver(){
        Intent intent2 = new Intent(ScanServerNodeIDs.this, ScanServerService.class);
        bindService(intent2, myConnectionScan, Context.BIND_AUTO_CREATE);
    }

    /*
     *   Method starting the DeviceScanAvtivity,
     *   where the user can select, if a connection to the 3D printer is required or not,
     *   also starts a background service, responsible for receiving the live data from the
     *   OPC UA server
     */
    public void searchprinter(){
        Intent intentData = new Intent(ScanServerNodeIDs.this, DeviceScanActivity.class);
        startActivity(intentData);


        Intent intent2 = new Intent(ScanServerNodeIDs.this, OPCUA_Connector.class);
        bindService(intent2, myConnectionOPC, Context.BIND_AUTO_CREATE);
    }

    /*
     *   By initialization cleares the lists of @opcDataItems
     *   Extracts the found sensors from the internal data base (shared preferences)
     *   and adds them to the list of @opcDataItems.
     *   @opcDataItems list contains the node ids of sensors,
     *   which are past to the OPCUA_Connector activity
     */
    public void extractDate(){
        opcDataItems.clear();
        SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
        String sharedPreferences = sharedPref.getAll().toString();
        String listOfSensorsIntermediate = sharedPreferences.substring(1, sharedPreferences.length() - 1);
        List<String> listOfSensors = Arrays.asList(listOfSensorsIntermediate.split(","));
        for (int j = 0; j < listOfSensors.size(); j++) {
            if (!listOfSensors.get(j).contains("url")) {
                if (listOfSensors.get(j).substring(0, 1).equals(" ")) {
                    int index = listOfSensors.get(j).indexOf("=");
                    String Name = listOfSensors.get(j).substring(1, index);
                    String IDs = listOfSensors.get(j).substring(index + 1, listOfSensors.get(j).length());
                    if (IDs.length() > 1) {
                        NodeId node = new NodeId(0, Integer.parseInt(IDs));
                        opcuaItemManager.addMonitoredDataItem(new OpcuaMonitoredItem(node));
                        opcDataItems.get(opcDataItems.size() - 1).setName(Name);
                        opcDataItems.get(opcDataItems.size() - 1).setType(IDs);
                    } else {
                        deleteNonExistingSensors(Name);
                    }
                } else {
                    int index = listOfSensors.get(j).indexOf("=");
                    String Name = listOfSensors.get(j).substring(0, index);
                    String IDs = listOfSensors.get(j).substring(index + 1, listOfSensors.get(j).length());
                    if (IDs.length() > 1) {
                        NodeId node = new NodeId(0, Integer.parseInt(IDs));
                        opcuaItemManager.addMonitoredDataItem(new OpcuaMonitoredItem(node));
                        opcDataItems.get(opcDataItems.size() - 1).setName(Name);
                        opcDataItems.get(opcDataItems.size() - 1).setType(IDs);
                    } else {
                        deleteNonExistingSensors(Name);
                    }
                }
            }
        }
    }

    /*
     *   Method to delete non found sensors from the list @listofNotFoundSensors
     */
    public void deleteNonExistingSensors(String s){
        String SensorName = s.substring(0, s.length()-6);
        if(!listofNotFoundSensors.contains(SensorName)){
            listofNotFoundSensors.add(SensorName);
        }
    }



    @Override
    protected void onStop() {
        super.onStop();
        // Unbind from the service
        if (isBound) {
            unbindService(myConnectionScan);
            isBound = false;
        }
    }

    /**
     * ServiceConnection callback gets notified when opc service is connected or disconnected
     */
    private ServiceConnection myConnectionScan = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            ScanServerService.MyLocalBinder binder = (ScanServerService.MyLocalBinder) service;
            myScanConnection = binder.getService();
            myScanConnection.ScanNodes();
            isBound = true;

        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            myScanConnection = null;
            isBound = false;

        }
    };



    private ServiceConnection myConnectionOPC = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            OPCUA_Connector.MyLocalBinder binderopc = (OPCUA_Connector.MyLocalBinder) service;
            opcConnect = binderopc.getService();
            opcConnect.ConnectOPC();
            isBoundOpcConnector = true;

        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            myConnectionOPC = null;
            isBoundOpcConnector = false;

        }
    };




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




    class MyBroadCastReceiver extends BroadcastReceiver {


        // When service @ScanServerService has scanned the node tree of OPC UA Server,
        // and a broadcast is send to the current activity, the view is refreshed, by restarting the
        // activity due to bug in ListView
        @Override
        public void onReceive(Context context, Intent intent) {

            try {
                startActivity(new Intent(ScanServerNodeIDs.this, ScanServerNodeIDs.class));
                }
             catch (Exception ex) {
            }
        }

    }



}
