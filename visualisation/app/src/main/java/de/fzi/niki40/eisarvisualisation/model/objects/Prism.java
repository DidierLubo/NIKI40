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

/**
 * Created by FZI Forschungszentrum Informatik on 16.09.2016.
 * Class implementing the visualisation object Prism - used for live data visualisation of wind flow
 */

import android.opengl.GLES10;

import org.artoolkit.ar.base.rendering.RenderUtils;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

import javax.microedition.khronos.opengles.GL10;



public class Prism {
    private FloatBuffer mVertexBuffer;
    private FloatBuffer mColorBuffer;
    private ByteBuffer mIndexBuffer;

    public Prism() {
        this(1.0F);
    }

    public Prism(float size) {
        this(size, 0.0F, 0.0F, 0.0F);
    }

    public Prism(float size, float x, float y, float z) {
        this.setArrays(size, x, y, z);
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

    private void setArrays(float size, float x, float y, float z) {
        float hs = size / 2.0F;
        float[] vertices = new float[]{

                x - hs , y - hs, z,//0
                x + hs, y -hs , z ,//1
                x +hs, y + hs , z ,//2
                x - hs , y+hs , z ,//3

                0 , 0 , z-3*hs,//4

                0, 0, z+3*hs//5
        };
        float c = 1.0F;
        float[] colors = new float[]{
                1, 1, 1,c,
                0, 0.467F, 0.186F,c,
                1, 1, 1,c,
                0, 0.467F, 0.286F,c,
                0, 0.467F, 0.486F,c,//fzigreen
                0, 0.467F, 0.486F,c,//fzigreen
        };
        byte[] indices = new byte[]{
                //triangles (numbers stand for vertices)
                (byte)0, (byte)4, (byte)1,
                (byte)1, (byte)4, (byte)2,
                (byte)2, (byte)4, (byte)3,
                (byte)3, (byte)4, (byte)0,
                (byte)5, (byte)0, (byte)1,
                (byte)5, (byte)1, (byte)2,
                (byte)5, (byte)2, (byte)3,
                (byte)5, (byte)3, (byte)0,
        };
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

        GLES10.glDrawElements(4, 24, 5121, this.mIndexBuffer);//18 = number of triangles*3 = number of indices



        GLES10.glDisableClientState('聶');
        GLES10.glDisableClientState('聴');
    }
}
