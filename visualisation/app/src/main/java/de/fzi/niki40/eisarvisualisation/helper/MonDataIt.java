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

/**
 * Created by FZI Forschungszentrum Informatik on 03.04.2018.
 * The following class represents a sensor, which is to be search in the node set
 * of the OPC UA Server
 */

public class MonDataIt {

    String Name;





    String IdentifierFlowX;
    String IdentifierFlowY;

    String Description;

    public String getDescription() {
        return Description;
    }

    public void setDescription(String description) {
        Description = description;
    }

    public String getName() {
        return Name;
    }

    public void setName(String name) {
        Name = name;
    }

    public String getIdentifierFlowX() {
        return IdentifierFlowX;
    }

    public void setIdentifierFlowX(String identifierFlowX) {
        IdentifierFlowX = identifierFlowX;
    }

    public String getIdentifierFlowY() {
        return IdentifierFlowY;
    }

    public void setIdentifierFlowY(String identifierFlowY) {
        IdentifierFlowY = identifierFlowY;
    }
}
