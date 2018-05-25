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
import android.os.Bundle;
import android.os.IBinder;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import de.fzi.niki40.eisarvisualisation.helper.OPCDataHelp;
import de.fzi.niki40.eisarvisualisation.helper.OpcuaItemManager;
import de.fzi.niki40.eisarvisualisation.helper.OpcuaMonitoredItem;

/**
 * Created by FZI Forschungszentrum Informatik on 06.03.2018.
 * The following activity represents the History Navigation view in the NIKI 4.0 app
 * It contains the list of sensors, fields for starting and end time point, as well cluster seek bar
 */

public class HistoryNavigationActivty extends Activity {

    SeekBar seekBarClusters;
    GetHistoryDataService service;
    boolean isBound = false;
    Button startArForHistoryData;
    Button seeHistoryData;
    TextView historyDataView;
    String visualize="Visualize";
    EditText startTime;
    EditText endTime;
    ListView listViewHistory;
    TextView numberOfClusters;
    public static final String BROADCAST_ACTION = "history data";
    MyBroadCastReceiver myBroadCastReceiver;
    private HistoryDataManager historyDataManager = HistoryDataManager.getInstance();
    List<HistoryDataObject> historyData = historyDataManager.getHistoryData();
    private OpcuaItemManager opcuaItemManager = OpcuaItemManager.getInstance();
    List<OpcuaMonitoredItem> opcDataItems = opcuaItemManager.getMonitoredSensors();
    List<String> optionalmonitoredItem = new ArrayList<String>();


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.historical_data_navigation_page);
        startArForHistoryData = (Button) findViewById(R.id.button);
        seekBarClusters = (SeekBar) findViewById(R.id.seekBarClusters);
        seeHistoryData = (Button) findViewById(R.id.seeHistoryData);
        historyDataView = (TextView) findViewById(R.id.historyData);
        numberOfClusters = (TextView) findViewById(R.id.numberOfClusters);
        startTime = (EditText) findViewById(R.id.startTime);
        endTime = (EditText) findViewById(R.id.endTime);
        listViewHistory =(ListView) findViewById(R.id.sensorListView);
        myBroadCastReceiver = new MyBroadCastReceiver();
        registerMyReceiver();
        for (int i=0; i<opcDataItems.size();i++){
            int charindex = opcDataItems.get(i).getName().lastIndexOf(".");
            String name = opcDataItems.get(i).getName().substring(0,charindex);
            addToList(name);
        }
        ListAdapter historyDataAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, optionalmonitoredItem);
        listViewHistory.setAdapter(historyDataAdapter);
        listViewHistory.setOnItemClickListener(new AdapterView.OnItemClickListener(){
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int i, long l) {
                String sensorName = String.valueOf(parent.getItemAtPosition(i));
                OPCDataHelp.setSensorName(sensorName);
                connect();
            }
        });
        seekBarClusters.setProgress(20);
        OPCDataHelp.setClusters(4);
        numberOfClusters.setText("Number of clusters: " + Integer.toString(OPCDataHelp.getClusters()));
        seekBarClusters.setOnSeekBarChangeListener(
                new SeekBar.OnSeekBarChangeListener() {
                    int progressValue;
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        progressValue = progress;
                        OPCDataHelp.setClusters(progress/4);
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {

                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {
                    numberOfClusters.setText("Number of clusters: " + Integer.toString(OPCDataHelp.getClusters()));
                    }
                }
        );

    }

    /*
     *   Method starting the service @GetHistoryDataService,
     *   with the given start and end time point,as well as number of clusters,
     *   in which the data devided and agregated
     */
    public void connect(){
        OPCDataHelp.setStartTime(startTime.getText().toString());
        OPCDataHelp.setEndTime(endTime.getText().toString());
        Intent intent = new Intent(HistoryNavigationActivty.this, GetHistoryDataService.class);
        bindService(intent, sc, Context.BIND_AUTO_CREATE);
    }

 /*
  *   Method starting the service @HistoryVisualizationArActivity,
  *   with the given start and end time point,as well as number of clusters,
  *   in which the data divided and aggregated
  */
    public void changeToARActivity(View view) {
        Intent startAR = new Intent(HistoryNavigationActivty.this, HistoryVisualizationArActivity.class);
        startActivity(startAR);
    }


 /*
  *  Method adding the sensors to the ListView
  */
    public void addToList(String s){
        if(!optionalmonitoredItem.contains(s)){
            optionalmonitoredItem.add(s);
        }
    }

    private ServiceConnection sc = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            GetHistoryDataService.MyLocalBinder binder = (GetHistoryDataService.MyLocalBinder) iBinder;
            service = binder.getService();
            service.GetHistoryDatafromNode();
            isBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            sc = null;
            isBound = false;

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
         /*
          *  Method making the color of the button @startArForHistoryData
          *  responsible for starting the AR activity green.
          */
        @Override
        public void onReceive(Context context, Intent intent) {
            try {
                if (seeHistoryData != null) {
                    startArForHistoryData.setText(visualize);
                    startArForHistoryData.setBackgroundColor(getResources().getColor(R.color.FZIgreen));
                }
            } catch (Exception ex) {
            }
        }
    }

}
