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
import android.widget.Toast;

import com.prosysopc.ua.UserIdentity;
import com.prosysopc.ua.client.UaClient;

import org.opcfoundation.ua.builtintypes.NodeId;
import org.opcfoundation.ua.core.ReferenceDescription;
import org.opcfoundation.ua.transport.security.SecurityMode;

import java.util.List;

import de.fzi.niki40.eisarvisualisation.helper.MonDataIt;
import de.fzi.niki40.eisarvisualisation.helper.MonitoredDataItemManager;

/**
 * Created by FZI Forschungszentrum Informatik on 03.04.2018.
 * Creates a connection to the OPC Server,
 * searches for the entered Sensor names as child nodes of the Node "Objects"
 */

public class ScanServerService extends Service {


    private final IBinder ScanBinder = new MyLocalBinder();
    private MonitoredDataItemManager monitoringManager = MonitoredDataItemManager.getInstance();
    List<MonDataIt> monitoredItems = monitoringManager.getMonitoredSensors();
    static UaClient scanner;

    public void ScanServerService(){

    }


    /*Connecting with the OPC UA Server via background service,
    scanning the node tree for nodes with "Browse Name" equal to
    the sensor names saved in the shared preferences of the application,
    if the names entered by the user and the "Browse Names" in the node tree match
    the corresponding node ids are saved in the shared preferences linked with the
    sensor names*/

    public void ScanNodes(){
        Toast.makeText(this, "Scanning Server", Toast.LENGTH_LONG).show();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    String url = sharedPref.getString("url", ""); // uses the IP address of the OPC UA server saved in the shared preferences under "url"
                    UserIdentity mUser = new UserIdentity();
                    scanner = new UaClient(url);
                    scanner.setTimeout(1000);
                    scanner.setSecurityMode(SecurityMode.NONE);
                    scanner.setUserIdentity(mUser); //verification of user identities
                    scanner.connect();
                    List<ReferenceDescription> references = scanner.getAddressSpace().browse(new NodeId(0, 85));
                    for (int start = 0; start < monitoredItems.size(); start++) {
                        String name = monitoredItems.get(start).getName();
                        for (int j = 0; j < references.size(); j++) {
                            if (references.get(j).getBrowseName().toString().equals(name) &&
                                references.get(j).getBrowseName().toString().contains("EMETER")) {
                                String idEmeter = references.get(j).getNodeId().toString();
                                monitoringManager.getMonitoredDataItem(name).setDescription(idEmeter.substring(idEmeter.lastIndexOf("=") + 1));
                                List<ReferenceDescription> referencesEmeter = scanner.getAddressSpace().browse(new NodeId(0,
                                        Integer.parseInt(monitoringManager.getMonitoredDataItem(name).getDescription())));
                                for (int m = 0; m < referencesEmeter.size(); m++) {
                                    String childOfEmeter = referencesEmeter.get(m).getBrowseName().toString();
                                    if (childOfEmeter.equals("EMeter-Values")) {
                                        String EMeterValues = referencesEmeter.get(m).getNodeId().toString();
                                        List<ReferenceDescription> allValues = scanner.getAddressSpace().browse(new NodeId(0,
                                                Integer.parseInt(EMeterValues.substring(EMeterValues.lastIndexOf("=") + 1))));
                                        for (int k = 0; k < allValues.size(); k++) {
                                            String valueName = allValues.get(k).getBrowseName().toString();
                                            if (valueName.equals("Voltage-P0")) {
                                                String targetId = allValues.get(k).getNodeId().toString();
                                                editor.putString(monitoringManager.getMonitoredDataItem("EMETER_CLIENT2").getName() +
                                                        ".voltage", targetId.substring(targetId.lastIndexOf("=") + 1));
                                                editor.apply();
                                            }
                                            if (valueName.equals("Current-P0")) {
                                                String targetId = allValues.get(k).getNodeId().toString();
                                                editor.putString(monitoringManager.getMonitoredDataItem("EMETER_CLIENT2").getName() +
                                                        ".current", targetId.substring(targetId.lastIndexOf("=") + 1));
                                                editor.apply();
                                            }
                                            if (valueName.equals("Power-P0")) {
                                                String targetId = allValues.get(k).getNodeId().toString();
                                                editor.putString(monitoringManager.getMonitoredDataItem("EMETER_CLIENT2").getName() +
                                                        ".power", targetId.substring(targetId.lastIndexOf("=") + 1));
                                                editor.apply();
                                            }
                                        }
                                    }
                                }
                            }
                            if (references.get(j).getBrowseName().toString().equals(name) &&
                                    references.get(j).getBrowseName().toString().equals("Anycubic 3D Printer") ) {
                                String idTemp = references.get(j).getNodeId().toString();
                                monitoringManager.getMonitoredDataItem(name).setDescription(idTemp.substring(idTemp.lastIndexOf("=") + 1));
                                List<ReferenceDescription> referencesTemp = scanner.getAddressSpace().browse(new NodeId(0,
                                        Integer.parseInt(monitoringManager.getMonitoredDataItem(name).getDescription())));
                                for (int m = 0; m < referencesTemp.size(); m++) {
                                    String childOfTemp = referencesTemp.get(m).getBrowseName().toString();
                                    if (childOfTemp.equals("Temperature")) {
                                        String TempValues = referencesTemp.get(m).getNodeId().toString();
                                        List<ReferenceDescription> allValues = scanner.getAddressSpace().browse(new NodeId(0,
                                                Integer.parseInt(TempValues.substring(TempValues.lastIndexOf("=") + 1))));
                                        for (int k = 0; k < allValues.size(); k++) {
                                            String valueName = allValues.get(k).getBrowseName().toString();
                                            if (valueName.equals("Sensor Value")) {
                                                String targetId = allValues.get(k).getNodeId().toString();
                                                editor.putString(monitoringManager.getMonitoredDataItem("Anycubic 3D Printer").getName() +
                                                        ".temp", targetId.substring(targetId.lastIndexOf("=") + 1));
                                                editor.apply();
                                            }
                                        }
                                    }
                                }
                            }
                                if (references.get(j).getBrowseName().toString().equals(name) &&
                                        !references.get(j).getBrowseName().toString().equals("EMETER_CLIENT2") &&
                                        !references.get(j).getBrowseName().toString().equals("Anycubic 3D Printer")) {
                                    String string1 = references.get(j).getNodeId().toString();
                                    monitoringManager.getMonitoredDataItem(name).setDescription(string1.substring(string1.lastIndexOf("=") + 1));
                                    monitoringManager.getMonitoredDataItem(name).setIdentifierFlowX(string1.substring(string1.lastIndexOf("=") + 1));
                                    List<ReferenceDescription> references2ndlvl = scanner.getAddressSpace().browse(new NodeId(0,
                                            Integer.parseInt(monitoringManager.getMonitoredDataItem(name).getDescription())));
                                    for (int m = 0; m < references2ndlvl.size(); m++) {
                                        String childOfSensorObjectNode = references2ndlvl.get(m).getBrowseName().toString();
                                        if (childOfSensorObjectNode.equals("EIS-Values")){
                                            String nodeId = references2ndlvl.get(m).getNodeId().toString();
                                            List<ReferenceDescription> eisValues = scanner.getAddressSpace().browse(new NodeId(0,
                                                    Integer.parseInt(nodeId.substring(nodeId.lastIndexOf("=") + 1))));
                                            for (int names = 0; names<eisValues.size(); names++){
                                                String nodeName = eisValues.get(names).getBrowseName().toString();
                                                if (nodeName.equals("FlowX Value")){
                                                    String flowXid = eisValues.get(names).getNodeId().toString();
                                                    monitoringManager.getMonitoredDataItem(name).setIdentifierFlowX(flowXid.substring(flowXid.lastIndexOf("=") + 1));
                                                }
                                                if (nodeName.equals("FlowY Value")){
                                                    String flowXid = eisValues.get(names).getNodeId().toString();
                                                    monitoringManager.getMonitoredDataItem(name).setIdentifierFlowY(flowXid.substring(flowXid.lastIndexOf("=") + 1));
                                                }
                                            }
                                        }
                                    editor.putString(monitoringManager.getMonitoredDataItem(name).getName() +
                                            ".flowX", monitoringManager.getMonitoredDataItem(name).getIdentifierFlowX());
                                    editor.putString(monitoringManager.getMonitoredDataItem(name).getName() +
                                            ".flowY", monitoringManager.getMonitoredDataItem(name).getIdentifierFlowY());
                                    editor.apply();
                                    sendMyBroadCast(); //sending broadcasts to ScanServerNodeIDs, when server scanning is complete
                                }
                            }
                        }
                    }

                }
                catch (Exception e){
                }
            }
        }).start();
    }

    private void sendMyBroadCast()
    {
        try
        {
            Intent broadCastIntent = new Intent();
            broadCastIntent.setAction(ScanServerNodeIDs.BROADCAST_ACTION);
            sendBroadcast(broadCastIntent);

        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }

    @Override
    public IBinder onBind(Intent intent){
        return ScanBinder;
    }

    public class MyLocalBinder extends Binder {
        ScanServerService getService(){
            return ScanServerService.this;
        }
    }
}