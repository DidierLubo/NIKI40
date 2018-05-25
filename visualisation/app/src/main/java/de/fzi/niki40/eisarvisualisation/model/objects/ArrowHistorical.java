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

package de.fzi.niki40.eisarvisualisation.model.objects;

import android.opengl.GLES10;

import org.artoolkit.ar.base.rendering.RenderUtils;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.opengles.GL10;

import de.fzi.niki40.eisarvisualisation.helper.Maths;

/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * Class implementing the visualisation object ArrowHistorical - used for visualisation of historical wind flow data
 */

public class ArrowHistorical {private FloatBuffer mVertexBuffer;
    private FloatBuffer mColorBuffer;
    private ByteBuffer mIndexBuffer;
    private int numberOfArrows;

    public ArrowHistorical(float size, float x, float y, float z, int numberOfArrows, float[] arrows, int[] distribution) {
        setArrays(size, x, y, z, numberOfArrows, arrows, distribution);
    }

    /**
     *
     * @param size the size of the segment
     * @param x the x coordinate of the centre
     * @param y the y coordinate of the centre
     * @param z the z coordinate of the centre
     * @param numberOfArrows the number of arrows which shall be rendered
     * @param arrows the points that determine the centres of the arrows (in order x1,y1,x2,y1....)
     * @param distribution the distribution of the values
     */
    private void setArrays(float size, float x, float y, float z, int numberOfArrows, float[] arrows, int[] distribution) {

        this.numberOfArrows = numberOfArrows;

        float hs = size / 2.0f;

        float[] valuesPerArrow = lengthAndAngle(arrows);

        float[] vertices = vertices(arrows, valuesPerArrow, z, hs);

        float[] colors = colors(distribution);

        byte[] indices = indices(numberOfArrows);


        this.mVertexBuffer = RenderUtils.buildFloatBuffer(vertices);
        this.mColorBuffer = RenderUtils.buildFloatBuffer(colors);
        this.mIndexBuffer = RenderUtils.buildByteBuffer(indices);
    }

    public void draw(GL10 unused) {
        GLES10.glColorPointer(4, GLES10.GL_FLOAT, 0, mColorBuffer);
        GLES10.glVertexPointer(3, GLES10.GL_FLOAT, 0, mVertexBuffer);

        GLES10.glEnableClientState(GLES10.GL_COLOR_ARRAY);
        GLES10.glEnableClientState(GLES10.GL_VERTEX_ARRAY);

        GLES10.glDrawElements(GLES10.GL_TRIANGLES, numberOfArrows * 18, GLES10.GL_UNSIGNED_BYTE, mIndexBuffer);

        GLES10.glDisableClientState(GLES10.GL_COLOR_ARRAY);
        GLES10.glDisableClientState(GLES10.GL_VERTEX_ARRAY);
    }


    /**
     *
     * @param arrows points of the arrows
     * @return the length and angle of each arrow in one array
     */
    private float[] lengthAndAngle(float[]  arrows) {
        //calculating Length and Angle for each Arrow
        float valuePerArrow[] = new float[arrows.length];
        for (int i = 0; i < numberOfArrows * 2; i = i + 2) {
            float xVal = arrows[i];
            float yVal = arrows[i + 1];
            float length = (float) Math.sqrt((xVal * xVal) + (yVal * yVal));
            float angle = (float) Math.toDegrees(Math.atan2((double) yVal, (double) xVal));
            if (angle < 0) {
                angle = angle + 360;
            }
            valuePerArrow[i] = length;
            valuePerArrow[i + 1] = angle;
        }
        return valuePerArrow;
    }


    /**
     *
     * @param arrows points of the arrows
     * @param valuesPerArrow the length and angle per arrow
     * @param z the lower z-coordinate of the arrows
     * @param hs the height of the arrows
     * @return
     */
    private float[] vertices(float arrows[], float[] valuesPerArrow, float z, float hs) {
        //defining the width of each arrow
        float sizeOfArrow = 360.0f / numberOfArrows;

        //list to dynamically add arrows
        List list = new ArrayList();

        int c = 1;

        for (int i = 0; i < arrows.length; i = i + 2) {
            float angle1 = 360.0f - valuesPerArrow[i + 1];
            float angle2 = 360.0f - valuesPerArrow[i + 1] + 180.0f;
            if (angle2 > 360.0f) {
                angle2 = angle2 - 360.0f;
            }

            //the new x Value in direction of the angle
            float firstXValue = Maths.calculateNewX(arrows[i], Maths.wws(sizeOfArrow / 2.0f, 90.0f, valuesPerArrow[i]), angle1);
            //the second x Value in opposite direction of the first value
            float secondXValue = Maths.calculateNewX(arrows[i], Maths.wws(sizeOfArrow / 2.0f, 90.0f, valuesPerArrow[i]), angle2);
            //the new y Value in direction of the angle
            float firstYValue = Maths.calculateNewY(arrows[i+1], Maths.wws(sizeOfArrow / 2.0f, 90.0f, valuesPerArrow[i]), angle1);
            //the second y Value in opposite direction of the first value
            float secondYValue = Maths.calculateNewY(arrows[i+1], Maths.wws(sizeOfArrow / 2.0f, 90.0f, valuesPerArrow[i]), angle2);
            //0
            list.add(firstXValue);
            list.add(firstYValue);
            list.add(z);

            //1
            list.add(secondXValue);
            list.add(secondYValue);
            list.add(z);

            //2
            list.add(firstXValue);
            list.add(firstYValue);
            list.add(z + hs);

            //3
            list.add(secondXValue);
            list.add(secondYValue);
            list.add(z + hs);

            //4
            list.add(0.0f);
            list.add(0.0f);
            list.add(z + (hs / 2));

            c++;
        }


        float[] vertices = new float[list.size()];
        for (int i = 0; i < list.size(); i++) {
            vertices[i] = (float) list.get(i);
        }
        return vertices;
    }

    /**
     *
     * @param cluster the distribution of points per arrow
     * @return the color for each arrow
     */
    private float[] colors(int[] cluster) {
        int numberOfValues = 0;
        for (int i = 0; i < cluster.length; i++) {
            numberOfValues = numberOfValues + cluster[i];
        }
        float oneThird = numberOfValues / 5;
        float twoThird = numberOfValues / 3;
        float[] colors = new float[numberOfArrows * 4 * 5];
        for (int i = 0; i < cluster.length; i++) {
            int index = i * 20;
            for (int j = 0; j < 16; j = j + 4) {
                if (cluster[i] < oneThird) {
                    colors[index + j] = 0;
                    colors[index + j + 1] = 0.39f + (0.78f / oneThird * cluster[i]);
                    colors[index + j + 2] = 0;
                    colors[index + j + 3] = 1.0f;
                } else if (cluster[i] < twoThird) {
                    colors[index + j] = 0.78f + (1/0.22f / twoThird * cluster[i]);
                    colors[index + j + 1] = 0.58f + (0.42f / twoThird * cluster[i]);
                    colors[index + j + 2] = 0;
                    colors[index + j + 3] = 1.0f;

                } else {
                    colors[index + j] = 0.39f + (0.39f/255 * numberOfValues * cluster[i]);
                    colors[index + j + 1] = 0;
                    colors[index + j + 2] = 0;
                    colors[index + j + 3] = 1.0f;

                }
            }
            colors[index + 16] = 0.0f;
            colors[index + 17] = 0.0f;
            colors[index + 18] = 0.0f;
            colors[index + 19] = 1.0f;
        }
        return colors;
    }


    /**
     *
     * @param numberOfArrows the number of Arrows that will be rendered
     * @return the byte array of indices
     */
    private byte[] indices (int numberOfArrows){
        int v = 0;
        byte[] indices = new byte[numberOfArrows * 18];
        for (int i = 0; i < indices.length; i = i + 18) {
            //Back 1
            indices[i] = (byte) (v);
            indices[i + 1] = (byte) (v + 2);
            indices[i + 2] = (byte) (v + 1);
            //Back 2
            indices[i + 3] = (byte) (v + 1);
            indices[i + 4] = (byte) (v + 2);
            indices[i + 5] = (byte) (v + 3);

            //Side left
            indices[i + 6] = (byte) (v + 3);
            indices[i + 7] = (byte) (v + 4);
            indices[i + 8] = (byte) (v + 1);
            //Side bottom
            indices[i + 9] = (byte) (v + 1);
            indices[i + 10] = (byte) (v + 4);
            indices[i + 11] = (byte) (v);
            //Side right
            indices[i + 12] = (byte) (v + 4);
            indices[i + 13] = (byte) (v + 2);
            indices[i + 14] = (byte) (v);
            //Side top
            indices[i + 15] = (byte) (v + 2);
            indices[i + 16] = (byte) (v + 4);
            indices[i + 17] = (byte) (v + 3);

            v = v + 5;

        }
        return indices;
    }
}
