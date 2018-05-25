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

import javax.microedition.khronos.opengles.GL10;

/**
 * Created by FZI Forschungszentrum Informatik on 18.04.2018.
 * Class implementing the visualisation object Candel - used for visualisation of historical Temp and Emeter data
 */

public class CandelHistorical {

    private FloatBuffer mVertexBuffer;
    private FloatBuffer mColorBuffer;
    private ByteBuffer mIndexBuffer;
    private int numberOfCandels;
    private static float spaceBetweenCandels = 4.0F;
    private static float width = 10.0F;
    private static float hs = width / 2.0f;



    public CandelHistorical(float[] heights) {
        this.setArrays(heights);
    }

    public FloatBuffer getmVertexBuffer() {
        return this.mVertexBuffer;
    }
    public FloatBuffer getmColorBuffer() {
        return this.mColorBuffer;
    }
    public ByteBuffer getmIndexBuffer() {
        return this.mIndexBuffer;
    }

    private void setArrays(float[] heightOfCandals ) {
        this.numberOfCandels = heightOfCandals.length;
        float[] vertices = calculateAllVertices(heightOfCandals);
        float[] colors = defineColorsOfCandels();
        byte[] indices =  calculateAllIndices();
        this.mVertexBuffer = RenderUtils.buildFloatBuffer(vertices);
        this.mColorBuffer = RenderUtils.buildFloatBuffer(colors);
        this.mIndexBuffer = RenderUtils.buildByteBuffer(indices);
    }

    public void draw(GL10 unused) {
        GLES10.glColorPointer(4, 5126, 0, this.mColorBuffer);
        //4= content of a color (REDVALUE, GREENVALUE, BLUEVALUE, ALPHAVALUE(Transparency))
        GLES10.glVertexPointer(3, 5126, 0, this.mVertexBuffer);
        GLES10.glEnableClientState('聶');
        GLES10.glEnableClientState('聴');
        GLES10.glDrawElements(4, 36*numberOfCandels, 5121, this.mIndexBuffer);//36 = number of triangles*3 = number of indices
        GLES10.glDisableClientState('聶');
        GLES10.glDisableClientState('聴');
    }

    private float[]  calculateAllVertices (float[] heightOfCandals){

        float[] vertices = new float[numberOfCandels*8*3];
        float x = -((numberOfCandels*width)+(spaceBetweenCandels*numberOfCandels))/2;
        float y = 0.0F;
        float z = 0.0F;
        int curruntPosition = 0;
        for (int i = 0; i < numberOfCandels; i++) {
            float[] tempVertices = createOneCandel(heightOfCandals[i],x,y,z);
            for (int j = 0; j <24 ; j++) {
                vertices[curruntPosition] = tempVertices[j];
                curruntPosition++;
            }
            x = x+width+spaceBetweenCandels;
        }
        return vertices;
    }

    private float[] createOneCandel(float height, float x, float y, float z){
        float[] vertices = new float[]{
                x - hs, y - hs, z ,
                x + hs, y - hs, z ,
                x + hs, y + hs, z ,
                x - hs, y + hs, z ,
                x - hs, y - hs, z + height,
                x + hs, y - hs, z + height,
                x + hs, y + hs, z + height,
                x - hs, y + hs, z + height
        };
        return vertices;
    }

    private byte[] calculateAllIndices (){
        int currentPosition = 0;
        byte[] indices = new byte[numberOfCandels*36];
        byte[] indicesss = new byte[]{0, 4, 5, 0, 5, 1, 1, 5, 6, 1, 6, 2, 2, 6, 7, 2, 7, 3, 3, 7, 4, 3, 4, 0, 4, 7, 6, 4, 6, 5, 3, 0, 1, 3, 1, 2};

        for (int i = 0; i < numberOfCandels; i++) {
            /*byte[] tempIndices = calculateNextIndices*/
            for (byte currentByte: indicesss) {
                indices[currentPosition] = (byte)( (int)currentByte +(i*8));
                currentPosition++;
            }
        }
        return indices;
    }

    private float[] defineColorsOfCandels(){
                float c = 1.0F;
                float[] colors = new float[]{
                1.0F, 1.0F, 0.2F,c,
                1.0F, 1.0F, 0.2F,c,
                1.0F, 1.0F, 0.2F,c,
                1.0F, 1.0F, 0.2F,c,
                1.0F, 1.0F, 0.2F,c,
                1.0F, 1.0F, 0.2F,c,
                1.0F, 1.0F, 0.2F,c,
                1.0F, 1.0F, 0.2F,c,
        };

        float[] colorsOfAllCandels = new float[numberOfCandels*4*8];
        int positionInArr = 0;
        for (int i = 0; i < numberOfCandels; i++) {

            for (float temp: colors) {
                colorsOfAllCandels[positionInArr]= temp;
                positionInArr++;
            }
        }
        return colorsOfAllCandels;
    }

}
