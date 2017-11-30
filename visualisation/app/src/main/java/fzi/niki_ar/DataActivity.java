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

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.util.List;

/**
 * Created by goranov on 26.10.2017.
 * Activity used to visualize sensors and their values
 */

public class DataActivity extends Activity {


    Button getData;
    Button returnAR;
    TextView sensor1;
    TextView sensor2;
    TextView sensorResult1;
    TextView sensorResult2;


    TextView textView;
    Button button;


    private SensorsManager sensorsManager = SensorsManager.getInstance();
    List<Sensor> sensors = sensorsManager.getSensors();




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.serverdata);



        getData = (Button) findViewById(R.id.getData);
        returnAR = (Button) findViewById(R.id.returnAR);
        sensor1 = (TextView) findViewById(R.id.sensorName1);
        sensor2 = (TextView)  findViewById(R.id.sensorName2);
        sensorResult1 = (TextView) findViewById(R.id.sensorValue1);
        sensorResult2 = (TextView) findViewById(R.id.sensorValue2);



        returnAR.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                onBackPressed();
            }
        });


        getData.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                sensorResult1.setText(sensorsManager.getSensor("VirtualAirSensor").getVectorX().toString());
                sensorResult2.setText(sensorsManager.getSensor("VirtualAirSensor").getVectorY().toString());
            }
        });



    }


    // Go Back Method

    @Override
    public void onBackPressed() {

        super.onBackPressed();
    }
}
