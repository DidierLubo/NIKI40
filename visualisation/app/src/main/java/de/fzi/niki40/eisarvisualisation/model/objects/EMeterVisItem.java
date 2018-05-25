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
 * Created by FZI Forschungszentrum Informatik on 25.04.2018.
 * Class implementing the visualisation object EMeterVisItem - used for live data visualisation of Emeter power
 */

public class EMeterVisItem {
    private FloatBuffer mVertexBuffer;
    private FloatBuffer mColorBuffer;
    private ByteBuffer mIndexBuffer;

    public EMeterVisItem() {
        this(1.0F);
    }

    public EMeterVisItem(float size) {
        this(size, 0.0F, 0.0F, 0.0F);
    }

    public EMeterVisItem(float size, float x, float y, float z) {
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
                //Bottom
                x-hs,y-4*hs, z-4*hs, //0
                x+hs,y-4*hs, z-4*hs, //1
                x+3*hs,y-3*hs, z-4*hs,//2
                x+4*hs,y-hs, z-4*hs,//3
                x+4*hs,y+hs, z-4*hs,//4
                x+3*hs,y+3*hs, z-4*hs,//5
                x+hs,y+4*hs, z-4*hs,//6
                x-hs,y+4*hs, z-4*hs,//7
                x-3*hs,y+3*hs, z-4*hs,//8
                x-4*hs,y+hs, z-4*hs,//9
                x-4*hs,y-hs, z-4*hs,//10
                x-3*hs,y-3*hs, z-4*hs,//11
                0,0,z-4*hs,//12

                //Top

                x-hs,y-4*hs, z+4*hs, //13
                x+hs,y-4*hs, z+4*hs, //14
                x+3*hs,y-3*hs, z+4*hs,//15
                x+4*hs,y-hs, z+4*hs,//16
                x+4*hs,y+hs, z+4*hs,//17
                x+3*hs,y+3*hs, z+4*hs,//18
                x+hs,y+4*hs, z+4*hs,//19
                x-hs,y+4*hs, z+4*hs,//20
                x-3*hs,y+3*hs, z+4*hs,//21
                x-4*hs,y+hs, z+4*hs,//22
                x-4*hs,y-hs, z+4*hs,//23
                x-3*hs,y-3*hs, z+4*hs,//24
                0,0,z+4*hs,//25
        };
        float c = 1.0F;
        float[] colors = new float[]{


                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,


                c,c,c,c,

                //Top
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,
                c, 0.75F, 0,c,
                c, 0.75F,0,0.5F,


                c,c,c,c,

        };
        byte[] indices = new byte[]{
                //triangles (numbers stand for vertices)
                //Bottom
                (byte)0, (byte)12, (byte)1,
                (byte)1, (byte)12, (byte)2,
                (byte)2, (byte)12, (byte)3,
                (byte)3, (byte)12, (byte)4,
                (byte)4, (byte)12, (byte)5,
                (byte)5, (byte)12, (byte)6,
                (byte)6, (byte)12, (byte)7,
                (byte)7, (byte)12, (byte)8,
                (byte)8, (byte)12, (byte)9,
                (byte)9, (byte)12, (byte)10,
                (byte)10, (byte)12, (byte)11,
                (byte)11, (byte)12, (byte)0,

                //sides
                (byte)0, (byte)13, (byte)14,
                (byte)0, (byte)14, (byte)1,
                (byte)1, (byte)14, (byte)15,
                (byte)1, (byte)15, (byte)2,
                (byte)2, (byte)15, (byte)16,
                (byte)2, (byte)16, (byte)3,
                (byte)3, (byte)16, (byte)17,
                (byte)3, (byte)17, (byte)4,
                (byte)4, (byte)17, (byte)18,
                (byte)4, (byte)18, (byte)5,
                (byte)5, (byte)18, (byte)19,
                (byte)5, (byte)19, (byte)6,
                (byte)6, (byte)19, (byte)20,
                (byte)6, (byte)20, (byte)7,
                (byte)7, (byte)20, (byte)21,
                (byte)7, (byte)21, (byte)8,
                (byte)8, (byte)21, (byte)22,
                (byte)8, (byte)22, (byte)9,
                (byte)9, (byte)22, (byte)23,
                (byte)9, (byte)23, (byte)10,
                (byte)10, (byte)23, (byte)24,
                (byte)10, (byte)24, (byte)11,
                (byte)11, (byte)24, (byte)13,
                (byte)11, (byte)13, (byte)0,


                //Top
                (byte)13, (byte)25, (byte)14,
                (byte)14, (byte)25, (byte)15,
                (byte)15, (byte)25, (byte)16,
                (byte)16, (byte)25, (byte)17,
                (byte)17, (byte)25, (byte)18,
                (byte)18, (byte)25, (byte)19,
                (byte)19, (byte)25, (byte)20,
                (byte)20, (byte)25, (byte)21,
                (byte)21, (byte)25, (byte)22,
                (byte)22, (byte)25, (byte)23,
                (byte)23, (byte)25, (byte)24,
                (byte)24, (byte)25, (byte)13,











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

        GLES10.glDrawElements(4, 144, 5121, this.mIndexBuffer);//number of triangles*3 = number of indices



        GLES10.glDisableClientState('聶');
        GLES10.glDisableClientState('聴');
    }
}
