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

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Binder;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.widget.Toast;

import com.prosysopc.ua.UserIdentity;
import com.prosysopc.ua.client.MonitoredDataItem;
import com.prosysopc.ua.client.MonitoredDataItemListener;
import com.prosysopc.ua.client.Subscription;
import com.prosysopc.ua.client.UaClient;

import org.opcfoundation.ua.builtintypes.DataValue;
import org.opcfoundation.ua.transport.security.SecurityMode;

import java.util.List;

import de.fzi.niki40.eisarvisualisation.bluetooth_connection.BleDeviceHandlerForEis;
import de.fzi.niki40.eisarvisualisation.helper.EMeter;
import de.fzi.niki40.eisarvisualisation.helper.EisSensor;
import de.fzi.niki40.eisarvisualisation.helper.MonDataIt;
import de.fzi.niki40.eisarvisualisation.helper.MonitoredDataItemManager;
import de.fzi.niki40.eisarvisualisation.helper.OpcuaItemManager;
import de.fzi.niki40.eisarvisualisation.helper.OpcuaMonitoredItem;

/**
 * Created by FZI Forschungszentrum Informatik on 11.12.2017.
 * Responsible for connecting with the OPC UA server
 * The background service ensures that the client recieves live data from the server,
 * once the connection is established and the service is started
 */
public class OPCUA_Connector extends Service {

    private BleDeviceHandlerForEis bleDeviceHandlerForEis = BleDeviceHandlerForEis.getInstance();
    List<EisSensor> eisSensors = bleDeviceHandlerForEis.getSensors();
    private final IBinder NIKIBinder = new MyLocalBinder();
    private MonitoredDataItemManager monitoringManager = MonitoredDataItemManager.getInstance();
    List<MonDataIt> monitoredItems = monitoringManager.getMonitoredSensors();
    private OpcuaItemManager opcuaItemManager = OpcuaItemManager.getInstance();
    List<OpcuaMonitoredItem> opcDataItems = opcuaItemManager.getMonitoredSensors();
    static UaClient client;

     /* Initializes the connection to the OPC Server,
     assigns the nodes with DataChangeListeners and adds the nodes of sensors in a subscription */
    public void ConnectOPC ()  {
        Toast.makeText(this, "Connecting with Server", Toast.LENGTH_LONG).show();
        new Thread(new Runnable() {
            @Override
            public void run() {
                String result = null;
                try {
                    SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    String url = sharedPref.getString("url", ""); // uses the IP address of the OPC UA server saved in the shared preferences under "url"
                    client = new UaClient(url.toString());
                    client.setTimeout(60000);
                    client.setSecurityMode(SecurityMode.NONE);
                    client.setUserIdentity(new UserIdentity()); //verification of user identities
                    client.connect();

                    Subscription subscription = new Subscription();
                    for (int i=0; i<opcDataItems.size(); i++){
                        subscription.addItem(opcDataItems.get(i));
                    }
                    if (opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#1.flowX")!=null &&
                            opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#1.flowY") !=null){
                        opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#1.flowX").setDataChangeListener(ListenerX1);
                        opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#1.flowY").setDataChangeListener(ListenerY1);
                    }
                    if (opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#2.flowX")!=null &&
                            opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#2.flowY") !=null){
                        opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#2.flowX").setDataChangeListener(ListenerX2);
                        opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#2.flowY").setDataChangeListener(ListenerY2);
                    }
                    if (opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#3.flowX")!=null &&
                            opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#3.flowY") !=null){
                        opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#3.flowX").setDataChangeListener(ListenerX3);
                        opcuaItemManager.getOpcuaMonitoredItem("NIKI-EIS-SENSOR#3.flowY").setDataChangeListener(ListenerY3);
                    }
                    if (opcuaItemManager.getOpcuaMonitoredItem("EMETER_CLIENT2.power")!=null){
                        opcuaItemManager.getOpcuaMonitoredItem("EMETER_CLIENT2.current").setDataChangeListener(ListenerEMeterPower);
                    }
                    if (opcuaItemManager.getOpcuaMonitoredItem("Anycubic 3D Printer.temp")!=null){
                        opcuaItemManager.getOpcuaMonitoredItem("Anycubic 3D Printer.temp").setDataChangeListener(ListenerTemp);
                    }
                    client.addSubscription(subscription);
                } catch (Exception e) {
                    result = e.toString();
                }
            }
        }).start();
    }


    /*Subscription  Data Change Listener to node in OPC UA, extracting a value as string
    and transforming it in double,
    while calculating the Airflow, when new values are recieved form the OPC UA Server.
    One Listener per Node in OPC UA*/
    MonitoredDataItemListener ListenerX1 = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                eisSensors.get(0).setNodeValueX(s);
                Double x = Double.parseDouble(eisSensors.get(0).getNodeValueX());
                Double y = Double.parseDouble(eisSensors.get(0).getNodeValueY());
                double offset = 1.0;
                y = y / offset;
                eisSensors.get(0).setAirflow(x, y);
            }
        }
    };

    MonitoredDataItemListener ListenerY1 = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                eisSensors.get(0).setNodeValueY(s);
                Double x = Double.parseDouble(eisSensors.get(0).getNodeValueX());
                Double y = Double.parseDouble(eisSensors.get(0).getNodeValueY());
                double offset = 1.0;
                y = y / offset;
                eisSensors.get(0).setAirflow(x, y);
            }
        }
    };
    MonitoredDataItemListener ListenerX2 = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                eisSensors.get(1).setNodeValueX(s);
                Double x = Double.parseDouble(eisSensors.get(1).getNodeValueX());
                Double y = Double.parseDouble(eisSensors.get(1).getNodeValueY());
                double offset = 1.0;
                y = y / offset;
                eisSensors.get(1).setAirflow(x, y);
            }
        }
    };
    MonitoredDataItemListener ListenerY2 = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                eisSensors.get(1).setNodeValueY(s);
                Double x = Double.parseDouble(eisSensors.get(1).getNodeValueX());
                Double y = Double.parseDouble(eisSensors.get(1).getNodeValueY());
                double offset = 1.0;
                y = y / offset;
                eisSensors.get(1).setAirflow(x, y);
            }
        }
    };
    MonitoredDataItemListener ListenerX3 = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                eisSensors.get(2).setNodeValueX(s);
                Double x = Double.parseDouble(eisSensors.get(2).getNodeValueX());
                Double y = Double.parseDouble(eisSensors.get(2).getNodeValueY());
                double offset = 1.0;
                y = y / offset;
                eisSensors.get(2).setAirflow(x, y);
            }


        }
    };
    MonitoredDataItemListener ListenerY3 = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                eisSensors.get(2).setNodeValueY(s);
                Double x = Double.parseDouble(eisSensors.get(2).getNodeValueX());
                Double y = Double.parseDouble(eisSensors.get(2).getNodeValueY());
                double offset = 1.0;
                y = y / offset;
                eisSensors.get(2).setAirflow(x, y);
            }


        }
    };
    MonitoredDataItemListener ListenerEMeterPower = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                Float valuePower = Float.parseFloat(s);
                valuePower = valuePower * 25.0f; // scalling factor
                EMeter.setEmeterPower(valuePower);
                sendMyBroadCast();
            }
        }
    };
    MonitoredDataItemListener ListenerTemp = new MonitoredDataItemListener() {
        @Override
        public void onDataChange(MonitoredDataItem sender, DataValue prevValue, DataValue value) {
            if(value!=null){
                String s = value.getValue().getValue().toString();
                Float tempWert = Float.parseFloat(s);
                EMeter.setTempPrinter(tempWert);
                sendMyBroadCast();
            }
        }
    };

    /**
     *  Sending broadcasts to EisArActivity, when the new data is received
     */

    private void sendMyBroadCast()
    {
        try
        {
            Intent broadCastIntent = new Intent();
            broadCastIntent.setAction(EisArActivity.BROADCAST_ACTION);
            sendBroadcast(broadCastIntent);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }

    public OPCUA_Connector()  {

    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return NIKIBinder;
    }

    public class MyLocalBinder extends Binder {
        public OPCUA_Connector getService(){
            return OPCUA_Connector.this;
        }
    }

}
