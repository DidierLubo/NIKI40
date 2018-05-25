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

import android.util.Log;

/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * Preparing Historical Data
 */

public class PreparingData {

    private int[] distribution;
    private float[] centres;

    public PreparingData(int numberOfArrows, float [] values) {
        //calculating the distribution of the values
        distribution = clusteringByNumber(numberOfArrows, values, initiatingCentres(numberOfArrows));
        //calculating the centres of the arrows to be drawn
        centres = calculatingCentres(numberOfArrows, values, initiatingCentres(numberOfArrows));
        String m = "";
        for (int i = 0; i < distribution.length; i++) {
            m = m + " " + distribution[i] + " \n";
        }
        Log.d("Overall: ", m);

    }

    /**
     *
     * @param numberOfClusters the number of cluster to assign the values to
     * @param values the values to be assigned
     * @param centren the centres of the arrows
     * @return the distribution of the clusters
     */
    public static int[] clusteringByNumber(int numberOfClusters, float [] values, float[] centren) {
        int[] numberPerCluster = new int [numberOfClusters];
        for(int j = 0; j < values.length; j = j + 2) {
            numberPerCluster[nearestCentrum(centren, values[j], values[j + 1])]++;
        }
        return numberPerCluster;
    }

    /**
     *
     * @param centren the points of the centres
     * @param x the x value to check on
     * @param y the y value to check on
     * @return the index of the nearest centrum
     */
    public static int nearestCentrum(float[] centren, float x, float y) {
        float currentClosest = 100000000.0f;
        int closest = 0;
        int c = 0;
        String m = "";
        for (int i = 0; i < centren.length  ; i = i + 2) {
            if (Math.abs(distanceToLine(centren[i], centren[i + 1], x, y)) <= currentClosest) {
                if ((centren[i] >= 0 && x >= 0 && centren[i+1] >= 0 && y >= 0)
                        || (centren[i] <= 0 && x <= 0 && centren[i+1] <= 0 && y <= 0)
                        || (centren[i] <= 0 && x <= 0 && centren[i+1] >= 0 && y >= 0)
                        || (centren[i] >= 0 && x >= 0 && centren[i+1] <= 0 && y <= 0)) {
                    closest = c;
                    currentClosest = Math.abs(distanceToLine(centren[i], centren[i + 1], x, y));
                    m = m + x + " " + y + " " + closest + " " + currentClosest + " \n";
                }
            }
            c++;
        }
        Log.d("Closest: ", m);
        return closest;
    }

    /**
     *
     * @param cx the x Value of the centre
     * @param cy the y Value of the centre
     * @param xValue the x Value to check on
     * @param yValue the y Value to check on
     * @return the distance between centre and point
     */
    public static float distanceToLine(float cx, float cy, float xValue, float yValue) {
        float result = 0;
        if (cx != 0 && cy !=0) {
            //calculate orthogonal line
            float gradient = cy / cx;
            float d = yValue - (-1 / gradient) * xValue;
            float x = d / (gradient + (1 / gradient));
            float y = gradient * x;
            result = (float) Math.sqrt((xValue - x) * (xValue - x) + (yValue - y) * (yValue - y));
        }
        else if (cx != 0 && cy == 0) {
            result = yValue;
        }
        else if (cx == 0 && cy != 0) {
            result = xValue;
        }
        return result;
    }


    /**
     *
     * @param numberOfArrows the number of arrows to create
     * @return the points of the initial arrows
     */
    public float[] initiatingCentres (int numberOfArrows) {
        float angle = 360.0f / numberOfArrows;
        float[] centres = new float[numberOfArrows * 2];
        //cos(90) and sin(180) != 0, thanks to a peculiarity of java
        double precision = 0.0000000000001;
        for (int i = 0; i < centres.length; i = i + 2) {
            centres[i] = (float) Math.cos(Math.toRadians(angle));
            centres[i+1] = (float) Math.sin(Math.toRadians(angle));
            if ((centres[i] < precision && centres[i] > 0) || (centres[i] > -precision && centres[i] < 0)) {
                centres[i] = 0;
            } else if ((centres[i+1] < precision && centres[i+1] > 0) || (centres[i+1] > -precision && centres[i+1] < 0)) {
                centres[i+1] = 0;
            }
            angle = angle + (360.0f / numberOfArrows);
        }
        return centres;
    }

    /**
     *
     * @param numberOfClusters the number of clusters
     * @param values the values
     * @param initialCentres the array of the initial centres
     * @return
     */
    public float[] calculatingCentres(int numberOfClusters, float[] values, float[] initialCentres) {
        int[] numberPerCluster = new int [numberOfClusters];
        float[] length = new float[numberOfClusters];
        int index = 0;
        for(int j = 0; j < values.length; j = j + 2) {
            index = nearestCentrum(initialCentres, values[j], values[j + 1]);
            length[index] = length[index] + Maths.lengthOfVector(values[j], values[j+1]);
            numberPerCluster[index]++;
        }
        float[] median = normaliseLengths(calculateMedian(numberPerCluster, length));
        return recalculatingCentres(median, initialCentres);
    }

    public float[] normaliseLengths(float[] length) {
        float max = 0;
        for (int i = 0; i < length.length; i++) {
            if (Math.abs(length[i]) > max) {
                max = length[i];
            }
        }
        for (int i = 0; i < length.length; i++) {
            length[i] = length[i] * (100 / max);
            if (length[i] < 5.f) {
                length[i] = 0;
            }
        }
        return length;
    }


    /**
     *
     * @param length array of all median lengths of the arrows
     * @param centres array of the initial centres
     * @return
     */
    public float[] recalculatingCentres(float[] length, float[] centres) {
        float angle = 360.0f / length.length;
        float[] newCentres = new float[centres.length];
        int c = 0;
        for (int i = 0; i < centres.length; i = i + 2) {
            if (centres[i] != 0) {
                newCentres[i] = Maths.walkingDownTheLineX(length[c], angle);
            }
            if (centres[i+1] != 0) {
                newCentres[i + 1] = Maths.walkingDownTheLineY(length[c], angle);
            }
            angle = angle + (360.0f / length.length);
            c++;
        }
        return newCentres;
    }

    /**
     *
     * @param numberPerCluster array with number of assigned points
     * @param length array with added lengths of all points
     * @return array with the medians of all arrays
     */
    public float[] calculateMedian(int[] numberPerCluster, float length[]) {
        String m = "";
        for (int i = 0; i < numberPerCluster.length; i++) {
            if (length[i] != 0) {
                length[i] = length[i] / numberPerCluster[i];
                m = m + " " + length[i];
            }
        }
        Log.d("Normalise: ", m);
        return length;
    }

    /**
     *
     * @return the centres array
     */
    public float[] getCentres() {
        return centres;
    }

    /**
     *
     * @return the distribution array
     */
    public int[] getDistribution() {
        return distribution;
    }
}
