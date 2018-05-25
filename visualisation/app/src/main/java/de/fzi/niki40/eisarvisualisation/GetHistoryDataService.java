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
import android.util.Log;
import android.widget.Toast;

import com.prosysopc.ua.UserIdentity;
import com.prosysopc.ua.client.UaClient;

import org.opcfoundation.ua.builtintypes.DataValue;
import org.opcfoundation.ua.builtintypes.DateTime;
import org.opcfoundation.ua.builtintypes.NodeId;
import org.opcfoundation.ua.builtintypes.QualifiedName;
import org.opcfoundation.ua.builtintypes.UnsignedInteger;
import org.opcfoundation.ua.core.HistoryData;
import org.opcfoundation.ua.core.HistoryReadDetails;
import org.opcfoundation.ua.core.HistoryReadResult;
import org.opcfoundation.ua.core.HistoryReadValueId;
import org.opcfoundation.ua.core.ReadRawModifiedDetails;
import org.opcfoundation.ua.core.TimestampsToReturn;
import org.opcfoundation.ua.transport.security.SecurityMode;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import de.fzi.niki40.eisarvisualisation.helper.OPCDataHelp;

/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * Service for extracting history data from the OPC UA Server
 */

public class GetHistoryDataService extends Service {

    private final IBinder scanHistoryBinder = new MyLocalBinder();
    private static int nodeIDHistory;

    public static int getNodeIDHistory() {
        return nodeIDHistory;
    }

    public static void setNodeIDHistory(int nodeIDHistory) {
        GetHistoryDataService.nodeIDHistory = nodeIDHistory;
    }

    private HistoryDataManager historyDataManager = HistoryDataManager.getInstance();
    List<HistoryDataObject> historyData = historyDataManager.getHistoryData();
    static UaClient scanner;

    public void GetHistoryDataService(){

    }

    public void GetHistoryDatafromNode(){
        Toast.makeText(this, "Extracting History Data", Toast.LENGTH_LONG).show();

     /* Initializes the connection to the OPC Server,
        checks which sensor is selected and extracts the history data for the specified time slot*/
        new Thread(new Runnable() {
            @Override
            public void run() {
                historyData.clear();
                try {
                    SharedPreferences sharedPref = getSharedPreferences("serverURL", Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    String url = sharedPref.getString("url", ""); // uses the IP address of the OPC UA server saved in the shared preferences under "url"

                    /* Implementation of extraction of history data for EMeter und Temp*/
                    if (OPCDataHelp.getSensorName().contains("EMETER_CLIENT") || OPCDataHelp.getSensorName().contains("Anycubic")){
                        if (OPCDataHelp.getSensorName().contains("EMETER_CLIENT")){
                            String nodePower = (sharedPref.getString(OPCDataHelp.getSensorName()+".current", ""));
                            setNodeIDHistory(Integer.parseInt(nodePower));;
                        } else {
                            String nodePower = (sharedPref.getString(OPCDataHelp.getSensorName()+".temp", ""));
                            setNodeIDHistory(Integer.parseInt(nodePower));;
                        }

                        scanner = new UaClient(url);
                        scanner.setTimeout(10000);
                        scanner.setSecurityMode(SecurityMode.NONE);
                        scanner.setUserIdentity(new UserIdentity()); //verification of user identities
                        scanner.connect();
                        NodeId nodePowerID = new NodeId(0, getNodeIDHistory()); // NodeId of the selected Sensor
                        String startTime = OPCDataHelp.getStartTime();
                        String endTime = OPCDataHelp.getEndTime();
                        SimpleDateFormat startTimeasDateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
                        Date startTimeasDate = startTimeasDateFormat.parse(startTime);
                        Date endTimeasDate = startTimeasDateFormat.parse(endTime);
                        long start = startTimeasDate.getTime();    //time given in miliseconds as long variable 1522752600000L
                        long end = endTimeasDate.getTime();      //time given in miliseconds as long variable  1522752700000L

                        /* History Data extraction for @nodePowerID
                         * standard procedures in OPC UA
                         */
                        HistoryReadDetails details = new ReadRawModifiedDetails(false,
                                DateTime.fromMillis(start), DateTime.fromMillis(end),
                                UnsignedInteger.MAX_VALUE, true);
                        HistoryReadValueId nodesToRead = new HistoryReadValueId(
                                nodePowerID, null,
                                QualifiedName.DEFAULT_BINARY_ENCODING, null);
                        HistoryReadResult[] result = scanner.historyRead(details, TimestampsToReturn.Both, true, nodesToRead);
                        HistoryData receivedHistoryData = result[0].getHistoryData().decode(scanner.getEncoderContext());
                        DataValue[] valuesPower = receivedHistoryData.getDataValues();

                       // Saving data in list historyData
                        for (int iEmeter = 0; iEmeter <valuesPower.length; iEmeter++){
                            historyDataManager.addHistoryDataObject(new HistoryDataObject());
                            historyData.get(historyData.size() - 1).setPower(valuesPower[iEmeter].getValue().getValue().toString());
                            String powerValue = historyData.get(historyData.size() - 1).getPower();
                            historyData.get(historyData.size() - 1).setPower(powerValue.substring(powerValue.indexOf("=") + 1, powerValue.indexOf(",")));
                            Log.d("ConDebug: ",  historyData.get(historyData.size() - 1).getPower());
                        }

                        // defining a cluster length dependent on the number of clusters,
                        // selected from User in Activity HisotryNavigationActivity and the size
                        // of received history data
                        int lenghtOfCluster =  historyData.size() / OPCDataHelp.getClusters();
                        float arrayMedianValues[] = new float[OPCDataHelp.getClusters()]; // creating array corresponding to the number of clusters
                        float sumOfValues=0;
                        float curruntValue;

                        //handling the data corresponding to the art of sensor - EMeter or Temp
                        //creating of an array with average data, used for AR visualization
                        if(OPCDataHelp.getSensorName().contains("EMETER")) {
                            for (int j = 0; j < OPCDataHelp.getClusters(); j++) {
                                for (int i = 0; i < lenghtOfCluster; i++) {
                                    curruntValue = 450.0f * Float.parseFloat(historyData.get(i + j * lenghtOfCluster).getPower());
                                    sumOfValues = sumOfValues + curruntValue;
                                }
                                arrayMedianValues[j] = (sumOfValues / lenghtOfCluster) ;
                                sumOfValues = 0;
                            }
                        } else {
                            for (int j = 0; j < OPCDataHelp.getClusters(); j++) {
                                for (int i = 0; i < lenghtOfCluster; i++) {
                                    curruntValue = Float.parseFloat(historyData.get(i + j * lenghtOfCluster).getPower());
                                    sumOfValues = sumOfValues + curruntValue;
                                }
                                arrayMedianValues[j] = (sumOfValues / lenghtOfCluster) ;
                                sumOfValues = 0;
                            }
                        }
                        HistoryVisualizationArActivity.setEmeterArray(arrayMedianValues); // saving the average data
                        sendMyBroadCast(); // Sending broadcast to the MainUIThread to create notification for the end user that extraction is complete
                    } else {

                    /*    Implementation of history data extraction for EIS Sensors
                        @nodex - NodeId of the selected Sensor for flow X
                        @nodey - NodeId of the selected Sensor for flow Y*/
                        String nodex = (sharedPref.getString(OPCDataHelp.getSensorName() + ".flowX", ""));
                        String nodey = (sharedPref.getString(OPCDataHelp.getSensorName() + ".flowY", ""));



                        // IP Address of the OPC UA Server has to be entered here!
                        scanner = new UaClient(url);
                        scanner.setTimeout(10000);
                        scanner.setSecurityMode(SecurityMode.NONE);
                        scanner.setUserIdentity(new UserIdentity()); //verification of user identities
                        scanner.connect();
                        NodeId mynode = new NodeId(0, Integer.parseInt(nodex));
                        NodeId mynode2 = new NodeId(0, Integer.parseInt(nodey));
                        String startTime = OPCDataHelp.getStartTime();
                        String endTime = OPCDataHelp.getEndTime();
                        SimpleDateFormat startTimeasDateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
                        Date startTimeasDate = startTimeasDateFormat.parse(startTime);
                        Date endTimeasDate = startTimeasDateFormat.parse(endTime);
                        long start = startTimeasDate.getTime();    //time given in miliseconds as long variable
                        long end = endTimeasDate.getTime();      //time given in miliseconds as long variable

                        /* History Data extraction for nodePowerID
                         * standard procedures in OPC UA
                         * for flow X and flow Y
                         */
                        HistoryReadDetails details = new ReadRawModifiedDetails(false,
                                DateTime.fromMillis(start), DateTime.fromMillis(end),
                                UnsignedInteger.MAX_VALUE, true);
                        HistoryReadValueId nodesToRead = new HistoryReadValueId(
                                mynode, null,
                                QualifiedName.DEFAULT_BINARY_ENCODING, null);
                        HistoryReadResult[] result = scanner.historyRead(details, TimestampsToReturn.Both, true, nodesToRead);
                        HistoryData dataFlowX = result[0].getHistoryData().decode(scanner.getEncoderContext());
                        DataValue[] values = dataFlowX.getDataValues();
                        HistoryReadValueId nodesToReadY = new HistoryReadValueId(
                                mynode2, null,
                                QualifiedName.DEFAULT_BINARY_ENCODING, null);
                        HistoryReadResult[] resultY = scanner.historyRead(details, TimestampsToReturn.Both, true, nodesToReadY);
                        HistoryData dataFlowY = resultY[0].getHistoryData().decode(scanner.getEncoderContext());
                        DataValue[] valuesY = dataFlowY.getDataValues();

                        /* Checking if one of the arrays contains more data
                         * if the extracted data for flow X and flow Y has difference in size of the array
                         * the larger array will be cut, so that both of them have the same size
                         * All data is saved in the objects @HistoryDataObject and added to the list @historyData
                         */
                        if (values.length > valuesY.length) {
                            for (int ia = 0; ia < valuesY.length; ia++) {
                                historyDataManager.addHistoryDataObject(new HistoryDataObject());
                                historyData.get(historyData.size() - 1).setFlowX(values[ia].getValue().getValue().toString());
                                historyData.get(historyData.size() - 1).setFlowY(valuesY[ia].getValue().getValue().toString());
                                String x = historyData.get(historyData.size() - 1).getFlowX();
                                String y = historyData.get(historyData.size() - 1).getFlowY();
                                historyData.get(historyData.size() - 1).setFlowX(x.substring(x.indexOf("=") + 1, x.indexOf(",")));
                                historyData.get(historyData.size() - 1).setFlowY(y.substring(y.indexOf("=") + 1, y.indexOf(",")));
                                historyData.get(historyData.size() - 1).setPrex(Double.parseDouble(historyData.get(historyData.size() - 1).getFlowX()));
                                historyData.get(historyData.size() - 1).setPrey(Double.parseDouble(historyData.get(historyData.size() - 1).getFlowY()));
                                Double parameterX = historyData.get(historyData.size() - 1).getPrex();
                                Double parameterY = historyData.get(historyData.size() - 1).getPrey();
                                historyData.get(historyData.size() - 1).setAirflow(parameterX, parameterY);
                            }
                        } else {
                            for (int ic = 0; ic < values.length; ic++) {
                                historyDataManager.addHistoryDataObject(new HistoryDataObject());
                                historyData.get(historyData.size() - 1).setFlowX(values[ic].getValue().getValue().toString());
                                historyData.get(historyData.size() - 1).setFlowY(valuesY[ic].getValue().getValue().toString());
                                String x = historyData.get(historyData.size() - 1).getFlowX();
                                String y = historyData.get(historyData.size() - 1).getFlowY();
                                historyData.get(historyData.size() - 1).setFlowX(x.substring(x.indexOf("=") + 1, x.indexOf(",")));
                                historyData.get(historyData.size() - 1).setFlowY(y.substring(y.indexOf("=") + 1, y.indexOf(",")));
                                historyData.get(historyData.size() - 1).setPrex(Double.parseDouble(historyData.get(historyData.size() - 1).getFlowX()));
                                historyData.get(historyData.size() - 1).setPrey(Double.parseDouble(historyData.get(historyData.size() - 1).getFlowY()));
                                Double parameterX = historyData.get(historyData.size() - 1).getPrex();
                                Double parameterY = historyData.get(historyData.size() - 1).getPrey();
                                historyData.get(historyData.size() - 1).setAirflow(parameterX, parameterY);
                            }
                        }

                        /* Data from the list @historyData is added in an float array @values.
                         * The data is used in class HistoryVisualizationArActivity for drawing
                         * the arrows, representing the average history windflow
                         */
                        float[] value = new float[historyData.size() * 2];
                        for (int i = 0; i < 2 * historyData.size(); i = i + 2) {
                            value[i] = Float.parseFloat(historyData.get(i / 2).flowX);
                        }
                        for (int i = 1; i < 2 * historyData.size(); i = i + 2) {
                            value[i] = Float.parseFloat(historyData.get((i - 1) / 2).flowY);
                        }
                        // setting the static array to store the history values
                        HistoryVisualizationArActivity.setArray(value);
                        // Sending broadcast to the MainUIThread to create notification for the end user that extraction is complete
                        sendMyBroadCast();
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
            broadCastIntent.setAction(HistoryNavigationActivty.BROADCAST_ACTION);
            sendBroadcast(broadCastIntent);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }

    @Override
    public IBinder onBind(Intent intent){
        return scanHistoryBinder;
    }

    public class MyLocalBinder extends Binder {
        GetHistoryDataService getService(){
            return GetHistoryDataService.this;
        }
    }
}

