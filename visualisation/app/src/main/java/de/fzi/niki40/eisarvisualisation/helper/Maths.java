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

package de.fzi.niki40.eisarvisualisation.helper;

import org.opcfoundation.ua.builtintypes.DataValue;

/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * Maths for Historical Data
 */

public class Maths {
    /**
     *
     * @param currentValue the x Value
     * @param size the distance to go in one direction
     * @param angle the direction to go to
     * @return
     */
    public static float calculateNewX(float currentValue, float size, float angle) {
        return ((float) (Math.sin(Math.toRadians(angle)) * size + currentValue));
    }

    /**
     *
     * @param currentValue the x Value
     * @param size the distance to go in one direction
     * @param angle the direction to go to
     * @return
     */
    public static float calculateNewY(float currentValue, float size, float angle) {
        return ((float) Math.cos(Math.toRadians(angle)) * size + currentValue);
    }


    /**
     * function to solve the WWS problem in a triangle
     * @param angle1 the size of the first arrow
     * @param angle2 the size of the second arrow
     * @param edge1 the length of the one edge
     * @return the length of the edge which is opposite to angle1
     */
    public static float wws(float angle1, float angle2, float edge1) {
        float angle3 = 180.0f - angle1 - angle2;
        float edge2 = (float) (Math.sin(Math.toRadians(angle1)) / ((Math.sin(Math.toRadians(angle3)) / edge1)));
        float edge3 = (float) Math.sqrt(edge1 * edge1 - edge2 * edge2);
        return edge2;
    }

    /**
     *
     * @param x x Value
     * @param y y Value
     * @return the length of the given vector (all relative to the zero point coordinate system)
     */
    public static float lengthOfVector(float x, float y) {
        return (float) Math.sqrt((x * x) + (y * y));
    }

    /**
     *
     * @param length the length of the distance to walk
     * @param angle the angle in which direction to walk
     * @return x value of the point that was walked to (all relative to the zero point coordinate system)
     */
    public static float walkingDownTheLineX(float length, float angle) {
        return (float) (length * Math.cos(Math.toRadians(angle)));
    }

    /**
     *
     * @param length the length of the distance to walk
     * @param angle the angle in which direction to walk
     * @return y value of the point that was walked to (all relative to the zero point coordinate system)
     */
    public static float walkingDownTheLineY(float length, float angle) {
        return (float) (length * Math.sin(Math.toRadians(angle)));
    }

    /**
     *
     * @param xValues the array of the x values
     * @param yValues the array of the y values
     * @return an array consiting of the x and y values in the order x1, y1, x2, y2, ...
     */
    public static float[] setArray(DataValue[] xValues, DataValue[] yValues) {
        float[] values = new float[xValues.length * 2];
        for (int i = 0; i < values.length; i = i + 1) {
            values [i] = xValues[i].getValue().floatValue();
            values [i+1] = (float) yValues[i+1].getValue().floatValue();
        }
        return values;
    }
}
