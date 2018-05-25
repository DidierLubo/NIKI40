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

import org.artoolkit.ar.base.ARToolKit;
import org.artoolkit.ar.base.NativeInterface;
import org.artoolkit.ar.base.rendering.ARRenderer;

import javax.microedition.khronos.opengles.GL10;

import de.fzi.niki40.eisarvisualisation.bluetooth_connection.BleDeviceHandlerForEis;
import de.fzi.niki40.eisarvisualisation.helper.Constants;
import de.fzi.niki40.eisarvisualisation.helper.EMeter;
import de.fzi.niki40.eisarvisualisation.helper.EisSensor;
import de.fzi.niki40.eisarvisualisation.helper.MarkerMapping;
import de.fzi.niki40.eisarvisualisation.helper.Printer;
import de.fzi.niki40.eisarvisualisation.model.objects.Arrow;
import de.fzi.niki40.eisarvisualisation.model.objects.EMeterVisItem;
import de.fzi.niki40.eisarvisualisation.model.objects.Mark;
import de.fzi.niki40.eisarvisualisation.model.objects.Prism;
import de.fzi.niki40.eisarvisualisation.model.objects.TempGreen;
import de.fzi.niki40.eisarvisualisation.model.objects.TempRed;
import de.fzi.niki40.eisarvisualisation.model.objects.TempYellow;
import de.fzi.niki40.eisarvisualisation.model.objects.YellowPrism;


/**
 * Created by FZI Forschungszentrum Informatik on 06.03.2018.
 * Renderer responsible for the visualization of the live data received from the Server
 * Renderer is used in class EisArActivity
 */

public class EisArRenderer extends ARRenderer {

    private static final float TARGET_PRINTER = 215.0f;
    private static final float EPSILON_GREEN = 1.0f;
    private static final float EPSILON_YELLOW = 10.0f;
    private TempRed redCuboid = new TempRed(10.0f, 0.0f, 0.0f, 40.f);
    private TempYellow yellowCuboid = new TempYellow(10.0f, 0.0f, 0.0f, 40.f);
    private TempGreen greenCuboid = new TempGreen(10.0f, 0.0f, 0.0f, 40.f);
    private Mark mark = new Mark(10.0f,0.0f,0.0f,20.0f);
    private Arrow arw = new Arrow(30.0f, 0.0f, 0.0f, 30.0f);
    private Prism prism = new Prism(20.0f, 0.0f, 0.0f, 40.0f);
    private YellowPrism yellowPrism = new YellowPrism(20.0f, 0.0f, 0.0f, 40.0f);
    private EMeterVisItem eMeter = new EMeterVisItem(10.0f, 0.0f, 0.0f, 40.f);
    private float caution;
    private boolean spinning = true;
    private int markerEMETER = -1;
    private int markerTemp = -1;
    private BleDeviceHandlerForEis sensorHandler;
    private MarkerMapping markerMapping;

    public EisArRenderer() {
        super();
        this.sensorHandler = BleDeviceHandlerForEis.getInstance();
        this.markerMapping = new MarkerMapping();
    }

    @Override
    public boolean configureARScene() {
        // we are detecting 2D-matrix codes
        NativeInterface.arwSetPatternDetectionMode(NativeInterface.AR_MATRIX_CODE_DETECTION);
        // numeration of hamming63 range(0,7)
        NativeInterface.arwSetMatrixCodeType(NativeInterface.AR_MATRIX_CODE_3x3_HAMMING63);
        // more robust clustering based recognition than simple thresholding (see NativeInterface.java)
        NativeInterface.arwSetVideoThresholdMode(NativeInterface.AR_LABELING_THRESH_MODE_AUTO_OTSU);
        markerEMETER = ARToolKit.getInstance().addMarker("single_barcode;5;80");
        markerTemp = ARToolKit.getInstance().addMarker("single_barcode;4;80");
        return true;
    }

    @Override
    public void draw(GL10 gl) {
        gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glLoadMatrixf(ARToolKit.getInstance().getProjectionMatrix(), 0);
        gl.glEnable(GL10.GL_CULL_FACE); //possibly reduces performance when not enabled
        gl.glShadeModel(GL10.GL_SMOOTH);
        gl.glEnable(GL10.GL_DEPTH_TEST); //enable depth
        float zoffset = 40.0f;
        //visualization of EIS Sensoren
        for (EisSensor eisSensor : sensorHandler.getSensors()) {
            final int marker = markerMapping.getMarker(eisSensor.getAddress());
            if (ARToolKit.getInstance().queryMarkerVisible(marker)) {
                gl.glMatrixMode(GL10.GL_MODELVIEW);
                gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(marker), 0);
                // only show the arrow, when there is airflow
                if (Math.abs(eisSensor.getAirflow()) > 0.16) {
                    float scaleFactor = (Math.max(1, (float) (Math.log10(Constants.ARROW_SCALE) / Math.log10(6))))
                            * (float) (1 + Math.log1p(Math.abs(eisSensor.getAirflow())));
                    gl.glFrontFace(GL10.GL_CW);
                    gl.glPushMatrix();
                    gl.glTranslatef(0.0f, 0.0f, zoffset - 10 * scaleFactor);
                    //rotate the Matrix by 90° around z-axis
                    gl.glRotatef(90F, 0.0f, 0.0f, 1.0f);
                    //rotate the Matrix by angle calculated through Airflow
                    gl.glRotatef((float) eisSensor.getAngle(), 0.0f, 0.0f, -1.0f);
                    // Scale the Matrix in the direction of the Tip
                    gl.glScalef(scaleFactor/2, scaleFactor, scaleFactor/2);
                    arw.draw(gl);
                    gl.glPopMatrix();
                } else {
                    gl.glFrontFace(GL10.GL_CCW);
                    /*draw a prism when there is no airflow
                    needs to be GL_CCW (counterclockwise), not sure why, yet*/
                    gl.glPushMatrix();
                    gl.glTranslatef(0.0f, 0.0f, zoffset);
                    prism.draw(gl);
                    gl.glPopMatrix();
                }
            }
        }

                //visualization of EMeter
                if (ARToolKit.getInstance().queryMarkerVisible(markerEMETER)) {
                    if(EMeter.getEmeterPower()>0) {
                        gl.glMatrixMode(GL10.GL_MODELVIEW);
                        gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(markerEMETER), 0);
                        gl.glDisable(GL10.GL_CULL_FACE);
                        gl.glFrontFace(GL10.GL_CW);
                        gl.glPushMatrix();
                        //   gl.glTranslatef(0.0f, 10.0f + 40.f, 0.0f);
                        gl.glScalef(EMeter.getEmeterPower() / 2f, EMeter.getEmeterPower() / 2f, EMeter.getEmeterPower() / 1f);
                        gl.glTranslatef(0.0f, 0.0f, 0.0f);
                        //  gl.glRotatef(90F, 1.0f, 0.0f, 0.0f);
                        eMeter.draw(gl);
                    } else {
                        gl.glMatrixMode(GL10.GL_MODELVIEW);
                        gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(markerEMETER), 0);
                        gl.glFrontFace(GL10.GL_CCW);
                        //draw a prism when there is no airflow
                        //needs to be GL_CCW (counterclockwise), not sure why, yet
                        gl.glPushMatrix();
                        gl.glTranslatef(0.0f, 0.0f, zoffset);
                        yellowPrism.draw(gl);
                        gl.glPopMatrix();
                    }
            }






        /*Show the temperature in front of the printer marker
        visualization of Temperature*/
        Printer printer = sensorHandler.getPrinter();
        if (true) {
          //  float temp = (float) printer.getTemperature();
            float temp = (float) EMeter.getTempPrinter();
          //  final int marker = markerMapping.getMarker(sensorHandler.getPrinter().getAddress());
            float yOffset = 20f * (temp / 100f);
            if (ARToolKit.getInstance().queryMarkerVisible(markerTemp)) {
                gl.glMatrixMode(GL10.GL_MODELVIEW);
                gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(markerTemp), 0);

                gl.glDisable(GL10.GL_CULL_FACE);

                if (((TARGET_PRINTER - EPSILON_GREEN) <= temp) && (temp < (TARGET_PRINTER + EPSILON_GREEN))) {
                    gl.glFrontFace(GL10.GL_CW);
                    gl.glPushMatrix();
                    gl.glTranslatef(0.0f, yOffset + 40.f, 0.0f);
                    gl.glScalef(1, temp / 100f, 1);
                    gl.glTranslatef(0.0f, 0.0f, zoffset);
                    gl.glRotatef(90F, 1.0f, 0.0f, 0.0f);
                    greenCuboid.draw(gl);
                    gl.glPopMatrix();
                } else if (((TARGET_PRINTER - EPSILON_YELLOW) <= temp) && (temp < (TARGET_PRINTER + EPSILON_YELLOW))) {
                    gl.glFrontFace(GL10.GL_CW);
                    //draw the temperature bar
                    gl.glPushMatrix();
                    gl.glTranslatef(0.0f, yOffset + 40.f, 0.0f);
                    gl.glScalef(1, temp / 100f, 1);
                    gl.glTranslatef(0.0f, 0.0f, zoffset);
                    gl.glRotatef(90F, 1.0f, 0.0f, 0.0f);
                    yellowCuboid.draw(gl);
                    gl.glPopMatrix();
                } else {
                    gl.glFrontFace(GL10.GL_CW);
                    //draw exclamation mark
                    gl.glPushMatrix();
                    gl.glTranslatef(0.f, - yOffset * 2.f, 0.f);
                    gl.glTranslatef(0.0f, 0.0f, zoffset);
                    //Rotate to be parallel to the marker
                    gl.glRotatef(90F, 1.0f, 0.0f, 0.0f);
                    //spin
                    gl.glRotatef(caution, 0F, 0F, 1F);
                    mark.draw(gl);
                    gl.glPopMatrix();
                    //draw the temperature bar
                    gl.glPushMatrix();
                    gl.glTranslatef(0.0f, yOffset + 40.f, 0.0f);
                    gl.glScalef(1, temp / 100f, 1);
                    gl.glTranslatef(0.0f, 0.0f, zoffset);
                    gl.glRotatef(90F, 1.0f, 0.0f, 0.0f);
                    redCuboid.draw(gl);
                    gl.glPopMatrix();
                }

                if (spinning) {
                    caution += 5.0f;//enable continuos spinning
                }
            }
        }

    }
}




