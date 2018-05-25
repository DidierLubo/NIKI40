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

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.TextView;

import org.artoolkit.ar.base.ARActivity;
import org.artoolkit.ar.base.rendering.ARRenderer;

import de.fzi.niki40.eisarvisualisation.bluetooth_connection.DeviceScanActivity;
import de.fzi.niki40.eisarvisualisation.helper.OPCDataHelp;

/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * This class is responsible for displaying the measured history data.
 */

public class HistoryVisualizationArActivity extends ARActivity {
    private static float[] array;
    private static float[] arrayEmeter;
    TextView textViewColor1;
    TextView textViewColor2;
    TextView textViewColor3;

    @Override
    public void onBackPressed() {
        Intent intent = new Intent(HistoryVisualizationArActivity.this, HistoryNavigationActivty.class);
        startActivity(intent);
        finish();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        textViewColor1 = (TextView) findViewById(R.id.textView5);
        textViewColor2 = (TextView) findViewById(R.id.textView6);
        textViewColor3 = (TextView) findViewById(R.id.textView11);
        super.onCreate(savedInstanceState);
        Integer clusters = OPCDataHelp.getClusters();
        setContentView(R.layout.history_data_activiry_layout);
        // this is the call that initiates the complete visualisation
        if (!OPCDataHelp.getSensorName().contains("EMETER_CLIENT") && !OPCDataHelp.getSensorName().contains("Anycubic")) {
            HistoryVisualizationArRenderer.setScene(array, clusters, 40.0f, 0.0f, 0.0f, 50.0f);
        }
    }

    @Override
    protected FrameLayout supplyFrameLayout() {
        return (FrameLayout) this.findViewById(R.id.historyDataAR);
    }

    //The following method returns different Renderer for the AR depending on the chosen sensor type
    protected ARRenderer supplyRenderer() {

        if(OPCDataHelp.getSensorName().contains("EMETER_CLIENT") || OPCDataHelp.getSensorName().contains("Anycubic")){
            return new EMeterHistoryRenderer();
        }
        else {
            return new HistoryVisualizationArRenderer();
        }
    }

    public static void setArray(float[] a) {
        array = a;
    }

    public static void setEmeterArray(float[] a){
        arrayEmeter = a;
    }

    public static float[] getArrayEmeter(){
        return arrayEmeter;
    }

    public void goLiveAr(View v) {
        Intent intent = new Intent(HistoryVisualizationArActivity.this, DeviceScanActivity.class);
        startActivity(intent);
    }
 }

