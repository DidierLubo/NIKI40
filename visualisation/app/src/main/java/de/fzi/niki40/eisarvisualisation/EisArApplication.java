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

import android.app.Application;

import org.artoolkit.ar.base.assets.AssetHelper;

/**
 * Created by FZI Forschungszentrum Informatik on 06.04.2018.
 * Application class responsible for AR visualization
 */
public class EisArApplication extends Application {
    private static Application sInstance;
    public static Application getInstance(){ return sInstance;}

    @Override
    public void onCreate() {
        super.onCreate();
        sInstance = this;
        ((EisArApplication) sInstance).initializeInstance();
    }

    private void initializeInstance() {
        AssetHelper assetHelper = new AssetHelper(getAssets());
        assetHelper.cacheAssetFolder(getInstance(),"Data");
    }

}
