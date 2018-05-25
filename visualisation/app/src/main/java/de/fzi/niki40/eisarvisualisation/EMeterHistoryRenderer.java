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

import de.fzi.niki40.eisarvisualisation.helper.OPCDataHelp;
import de.fzi.niki40.eisarvisualisation.model.objects.CandelHistorical;


/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * Renderer responsible for the visualization of the EMeter or Temp history data received from the Server
 * Renderer is used in class HistoryVisualizationArActivity
 */

public class EMeterHistoryRenderer extends ARRenderer {

    public int markerID = -1;
    public int markerIDtemp = -1;
    private CandelHistorical candel = new CandelHistorical(HistoryVisualizationArActivity.getArrayEmeter());

    /**
     * Markers can be configured here.
     */
    @Override
    public boolean configureARScene() {
        NativeInterface.arwSetPatternDetectionMode(NativeInterface.AR_MATRIX_CODE_DETECTION);
        NativeInterface.arwSetMatrixCodeType(NativeInterface.AR_MATRIX_CODE_3x3_HAMMING63);
        markerIDtemp = ARToolKit.getInstance().addMarker("single_barcode;4;80");
        return true;
    }

    /**
     * Override the draw function from ARRenderer.
     */
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
       /* If the marker is visible, apply its transformation, and draws the candel
          check if users has selected a sensor corresponding to the EMeter,
          draws the Emeter visualizaion*/
        if (OPCDataHelp.getSensorName().contains("EMETER")) {

            if (ARToolKit.getInstance().queryMarkerVisible(markerID)) {

                gl.glMatrixMode(GL10.GL_MODELVIEW);
                gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(markerID), 0);
                candel.draw(gl);
            }
            /*else if a visualization of the Temp data is intended,
            draws the Candel with the Temp data, rotated by 90 degrees*/
        } else {
            if (ARToolKit.getInstance().queryMarkerVisible(markerIDtemp)) {

                gl.glMatrixMode(GL10.GL_MODELVIEW);
                gl.glLoadMatrixf(ARToolKit.getInstance().queryMarkerTransformation(markerIDtemp), 0);
                gl.glRotatef(90F, 1.0f, 0.0f, 0.0f);
                candel.draw(gl);
            }
        }

    }
}
