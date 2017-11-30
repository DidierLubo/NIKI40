/*
        Copyright 2017 NIKI 4.0 project team

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


package fzi.niki_ar;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.widget.Toast;

import com.prosysopc.ua.UserIdentity;
import com.prosysopc.ua.client.MonitoredDataItem;
import com.prosysopc.ua.client.MonitoredDataItemListener;
import com.prosysopc.ua.client.Subscription;
import com.prosysopc.ua.client.UaClient;

import org.opcfoundation.ua.builtintypes.DataValue;
import org.opcfoundation.ua.builtintypes.NodeId;
import org.opcfoundation.ua.transport.security.SecurityMode;

import java.util.List;

    /* Class responsible for  the connection to the OPC Server,
    implemented as background service. */

public class OPCUaConnection extends Service {

    Subscription subscription;
    private final IBinder NIKIBinder = new MyLocalBinder();
    private SensorsManager sensorsManager = SensorsManager.getInstance();

    List<fzi.niki_ar.Sensor> sensors = sensorsManager.getSensors();




    public OPCUaConnection(){

    }



     /* Initializes the connection to the OPC Server,
     assigns the nodes and adds the sensors in the List of sensors */

    public void ConnectOPC ()  {




        Toast.makeText(this, "ConnectOPC() started", Toast.LENGTH_LONG).show();
        new Thread(new Runnable() {
            @Override
            public void run() {

                UaClient client = null;  //encapsulates the connection to the OPC UA server and handles the various details in the actual OPC UA communications
                subscription = new Subscription();
                String result = null;
                sensorsManager.addSensor(new fzi.niki_ar.Sensor());
                sensorsManager.addSensor(new fzi.niki_ar.Sensor());

                sensors.get(0).setName("VirtualAirSensor"); // add new Sensor to the list with the Name "VirtualAIrSensor"
                sensors.get(1).setName("NikiAirSensor");    // add new Sensor to the list with the Name "NikiAirSensor"

                try {
                    client = new UaClient("opc.tcp://192.168.1.135:8888"); // IP Address of the OPC UA Server has to be entered here!
                    client.setTimeout(60000);
                    client.setSecurityMode(SecurityMode.NONE);
                    client.setUserIdentity(new UserIdentity()); //verification of user identities

                    client.connect();



                    NodeId VectorX = new NodeId(0 , 60108); // creation of OPC UA Node with Name VectorX
                    NodeId VectorY = new NodeId(0 , 60114); // creation of OPC UA Node with Name VectorX

                   /* In order to monitor changes in the server, we define subscriptions.
                      These include a number of monitored items, which we listen to.
                      To monitor data changes, we use MonitoredDataItem. */

                    MonitoredDataItem dataItem = new MonitoredDataItem(VectorX);
                    MonitoredDataItem dataItem2 = new MonitoredDataItem(VectorY);

                    Subscription subscription = new Subscription();
                    subscription.addItem(dataItem);
                    subscription.addItem(dataItem2);

                    client.addSubscription(subscription);

                    /* In the client, we can then just listen to the change notifications,
                       which the server is instructed to send when the item values change
                       according to the subscription properties. */

                    MonitoredDataItemListener dataChangeListener = new MonitoredDataItemListener() {
                        @Override
                        public void onDataChange(MonitoredDataItem sender, DataValue prevValue,
                                                 DataValue value) {

                            if(value!=null){

                                sensorsManager.getSensor("VirtualAirSensor").setSensorValue(value.getValue().getValue().toString());
                                sensorsManager.getSensor("VirtualAirSensor").setVectorX(Double.parseDouble(sensorsManager.getSensor("VirtualAirSensor").getSensorValue()));

                                sensorsManager.getSensor("VirtualAirSensor").setAirflow(sensorsManager.getSensor("VirtualAirSensor").getVectorX(), sensorsManager.getSensor("VirtualAirSensor").getVectorY());

                            }
                        }
                    };

                     /* In the client, we can then just listen to the change notifications,
                        which the server is instructed to send when the item values change
                        according to the subscription properties: */

                    MonitoredDataItemListener dataChangeListener2 = new MonitoredDataItemListener() {
                        @Override
                        public void onDataChange(MonitoredDataItem sender, DataValue prevValue,
                                                 DataValue value) {

                            if(value!=null){

                                sensorsManager.getSensor("VirtualAirSensor").setSensorValue(value.getValue().getValue().toString());
                                sensorsManager.getSensor("VirtualAirSensor").setVectorY(Double.parseDouble(sensorsManager.getSensor("VirtualAirSensor").getSensorValue()));

                                sensorsManager.getSensor("VirtualAirSensor").setAirflow(sensorsManager.getSensor("VirtualAirSensor").getVectorX(), sensorsManager.getSensor("VirtualAirSensor").getVectorY());


                            }
                        }
                    };






                    dataItem.setDataChangeListener(dataChangeListener);
                    dataItem2.setDataChangeListener(dataChangeListener2);


                } catch (Exception e) {
                    result = e.toString();
                }

            }
        }).start();


    }




    @Override
    public IBinder onBind(Intent intent){
        return NIKIBinder;
    }


    public class MyLocalBinder extends Binder {
        OPCUaConnection getService(){
            return OPCUaConnection.this;
        }
    }


}
