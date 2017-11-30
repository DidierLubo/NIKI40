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

package fzi.niki_ar.model.objects;

import org.artoolkit.ar.base.ARToolKit;
import org.artoolkit.ar.base.NativeInterface;
import org.artoolkit.ar.base.rendering.ARRenderer;

import java.util.List;

import javax.microedition.khronos.opengles.GL10;

import fzi.niki_ar.Sensor;
import fzi.niki_ar.SensorsManager;

/**
 * This is the AR Renderer class containing the visualisation logic.
 */

public class MultiRenderer extends ARRenderer {


    private SensorsManager sensorsManager = SensorsManager.getInstance();
    List<Sensor> sensors = sensorsManager.getSensors();

    private int[] markerID = {-1, -1, -1, -1};



    private Arrow arw = new Arrow(30.0f, 0.0f, 0.0f, 30.0f);
    private Prism prism = new Prism(20.0f, 0.0f, 0.0f, 40.0f);


    /**
     * Markers can be configured here.
     */
    @Override
    public boolean configureARScene() {

        // we are detecting 2D-matrix codes
        NativeInterface.arwSetPatternDetectionMode(NativeInterface.AR_MATRIX_CODE_DETECTION);
        // numeration of hamming63 range(0,7)
        NativeInterface.arwSetMatrixCodeType(NativeInterface.AR_MATRIX_CODE_3x3_HAMMING63);

        markerID[0] = ARToolKit.getInstance().addMarker("single_barcode;6;80");
        markerID[1] = ARToolKit.getInstance().addMarker("single_barcode;4;80");
        markerID[2] = ARToolKit.getInstance().addMarker("single_barcode;0;80");
        markerID[3] = ARToolKit.getInstance().addMarker("single_barcode;1;80");

        return true;



    }

    @Override
    public void draw(GL10 gl) {

        gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);

        // Apply the ARToolKit projection matrix
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glLoadMatrixf(ARToolKit.getInstance().getProjectionMatrix(), 0);

        gl.glEnable(GL10.GL_CULL_FACE);
        gl.glShadeModel(GL10.GL_SMOOTH);
        gl.glEnable(GL10.GL_DEPTH_TEST);
        gl.glFrontFace(GL10.GL_CW);

        // If the marker is visible, apply its transformation, and draw a cube
        if (ARToolKit.getInstance().queryMarkerVisible(markerID[1])) {


            gl.glMatrixMode(GL10.GL_MODELVIEW);
            gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(markerID[1]), 0);

            if (Math.abs(sensorsManager.getSensor("VirtualAirSensor").getAirflow()) > 0.99) { // only show the arrow, when there is airflow
                // rotate into given direction


                gl.glFrontFace(GL10.GL_CW);
                gl.glPushMatrix();

                gl.glTranslatef(0.0f, 0.0f, 40.0f);
                gl.glRotatef(90F, 0.0f, 0.0f, 1.0f); //rotate the Matrix by 90° around z-axis
                gl.glRotatef((float) sensorsManager.getSensor("VirtualAirSensor").getAngle(), 0.0f, 0.0f, -1.0f); //rotate the Matrix by angle calculated through Airflow
                gl.glScalef((float) (1 + Math.log10(Math.abs(sensorsManager.getSensor("VirtualAirSensor").getAirflow()))), (float) (1 + Math.log10(Math.abs(sensorsManager.getSensor("VirtualAirSensor").getAirflow()))), (float) (1 + Math.log10(Math.abs(sensorsManager.getSensor("VirtualAirSensor").getAirflow())))); // Scale the Matrix in the direction of the Tip //TODO: better algorithm for scaling


                arw.draw(gl);
                gl.glPopMatrix();

            } else {

                gl.glFrontFace(GL10.GL_CCW);
                //draw a prism when there is no airflow
                //needs to be GL_CCW (counterclockwise), not sure why, yet
                gl.glPushMatrix();

                gl.glTranslatef(0.0f, 0.0f, 40.0f);

                //gl.glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
                //gl.glTranslatef(0.0f, 0.0f, -zoffset);
                prism.draw(gl);
                gl.glPopMatrix();

            }
        }
            if (ARToolKit.getInstance().queryMarkerVisible(markerID[2])) {


                gl.glMatrixMode(GL10.GL_MODELVIEW);
                gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(markerID[2]), 0);

                if (Math.abs(sensorsManager.getSensor("NikiAirSensor").getAirflow()) > 0.99) { // only show the arrow, when there is airflow
                    // rotate into given direction


                    gl.glFrontFace(GL10.GL_CW);
                    gl.glPushMatrix();

                    gl.glTranslatef(0.0f, 0.0f, 40.0f);
                    gl.glRotatef(90F, 0.0f, 0.0f, 1.0f); //rotate the Matrix by 90° around z-axis
                    gl.glRotatef((float) sensorsManager.getSensor("NikiAirSensor").getAngle(), 0.0f, 0.0f, -1.0f); //rotate the Matrix by angle calculated through Airflow
                    gl.glScalef((float) (1 + Math.log10(Math.abs(sensorsManager.getSensor("NikiAirSensor").getAirflow()))), (float) (1 + Math.log10(Math.abs(sensorsManager.getSensor("NikiAirSensor").getAirflow()))), (float) (1 + Math.log10(Math.abs(sensorsManager.getSensor("NikiAirSensor").getAirflow())))); // Scale the Matrix in the direction of the Tip //TODO: better algorithm for scaling


                    arw.draw(gl);
                    gl.glPopMatrix();

                } else {

                    gl.glFrontFace(GL10.GL_CCW);
                    //draw a prism when there is no airflow
                    //needs to be GL_CCW (counterclockwise), not sure why, yet
                    gl.glPushMatrix();

                    gl.glTranslatef(0.0f, 0.0f, 40.0f);

                    //gl.glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
                    //gl.glTranslatef(0.0f, 0.0f, -zoffset);
                    prism.draw(gl);
                    gl.glPopMatrix();

                }






        }

    }
}
