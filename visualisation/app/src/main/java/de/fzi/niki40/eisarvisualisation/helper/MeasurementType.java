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

import de.fzi.niki40.eisarvisualisation.EisArActivity;

/**
 * Created by FZI Forschungszentrum Informatik on 28.09.2016.
 */
public enum MeasurementType {
    AIR_FLOW (de.fzi.niki40.eisarvisualisation.R.string.airflow_string),
    TEMPERATURE (de.fzi.niki40.eisarvisualisation.R.string.temperature_string);

    private int resourceId;

    private MeasurementType(int resourceId) {
        this.resourceId = resourceId;
    }

    @Override
    public String toString() {
        return EisArActivity.context.getResources().getString(resourceId);
    }

    /**
     * Compares a given string with all existing values of the MeasurementType enum
     * @param string string representation of the enum
     * @return fitting enum object if it exists, null otherwise
     */
    public static MeasurementType fromString(String string) {
        MeasurementType result = null;
        if (string.equals(EisArActivity.context.getResources().getString(de.fzi.niki40.eisarvisualisation.R.string.airflow_string))) {
            result = MeasurementType.AIR_FLOW;
        } else if (string.equals(EisArActivity.context.getResources().getString(de.fzi.niki40.eisarvisualisation.R.string.temperature_string))) {
            result = MeasurementType.TEMPERATURE;
        }
        return result;
    }
}
