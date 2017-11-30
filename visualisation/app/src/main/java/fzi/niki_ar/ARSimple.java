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

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.Toast;

import org.artoolkit.ar.base.ARActivity;
import org.artoolkit.ar.base.rendering.ARRenderer;

import java.util.List;

import fzi.niki_ar.Helpers.OnSwipeTouchListener;
import fzi.niki_ar.model.objects.MultiRenderer;

/**
 * This is the main activity in the AR app
 */

public class ARSimple extends ARActivity {


    public static Context context;
    OPCUaConnection myService;
    boolean isBound = false;
    private SensorsManager sensorsManager = SensorsManager.getInstance();
    List<Sensor> sensors = sensorsManager.getSensors();
    View mToast;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        // New Intent to bind with the OPC Connection service
        Intent intent = new Intent(this, OPCUaConnection.class);
        bindService(intent, myConnection, Context.BIND_AUTO_CREATE);


        mToast = (FrameLayout) findViewById(R.id.mainLayout);




        // listen for swipe events
        mToast.setOnTouchListener(new OnSwipeTouchListener(context){
            @Override
            public void onSwipeLeft() {
                Toast.makeText(ARSimple.this,"SWIPPED Left", Toast.LENGTH_LONG).show();
            }

            @Override
            public void onSwipeRight() {

                Toast.makeText(ARSimple.this, "SWIPPED Right", Toast.LENGTH_LONG).show();
            }

            @Override
            public void onTouch() {
                Intent intentData = new Intent(ARSimple.this, DataActivity.class);
                startActivity(intentData);

            }

            @Override
            public void onLongTouch() {

            }
        });




    }

    /**
     * Provide our own Renderer.
     */
    @Override
    protected ARRenderer supplyRenderer() {
        return new MultiRenderer();
    }

    /**
     * Use the FrameLayout in this Activity's UI.
     */
    @Override
    protected FrameLayout supplyFrameLayout() {
        return (FrameLayout) this.findViewById(R.id.mainLayout);
    }

    /**
     * ServiceConnection callback gets notified when opc service is connected or disconnected
     */
    private ServiceConnection myConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            OPCUaConnection.MyLocalBinder binder = (OPCUaConnection.MyLocalBinder) service;
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
