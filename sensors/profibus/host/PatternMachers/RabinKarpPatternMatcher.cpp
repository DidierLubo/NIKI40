/*
        Copyright 2016-2018 NIKI 4.0 project team
        NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
        Project partners are FZI Forschungszentrum Informatik am Karlsruher
        Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
        für angewandte Forschung e.V. (www.hahn-schickard.de) and
        Hochschule Offenburg (www.hs-offenburg.de).
        This file was developed by Hahn-Schickard.
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
/*****************************************************************************************
 * RabinKarpPatternMatcher.cpp
 *
 *  Created on: 15.06.2018
 *      Author: Dovydas Girdvainis
 */

#include "RabinKarpPatternMatcher.h"
#include <string.h>

RabinKarpPatternMatcher::RabinKarpPatternMatcher(int hashingPrime, int alphabetSize, int dM)
{
    this->hashingPrime = hashingPrime;
    this->alphabetSize = alphabetSize;
    this->dM = dM;
}

RabinKarpPatternMatcher::RabinKarpPatternMatcher(int hashingPrime, int alphabetSize)
{
    RabinKarpPatternMatcher(hashingPrime, alphabetSize, 1);
}

RabinKarpPatternMatcher::RabinKarpPatternMatcher(int hashingPrime)
{
    RabinKarpPatternMatcher(hashingPrime, 256, 1);
}

RabinKarpPatternMatcher::RabinKarpPatternMatcher()
{
    RabinKarpPatternMatcher(7, 256, 1);
}

int RabinKarpPatternMatcher::search(const char needle[], const char haystack[], int indexBuffer[])
{
    int needleLenght = strlen(needle);
    int haystackLenght = strlen(haystack);
    int needleHash = 0;
    int haystackHash = 0;
    int bufferSize = 0;

    int i,j;

    // precompile dM
    for (i=0; i<needleLenght-1; i++){
	dM = (dM*alphabetSize)%hashingPrime;
    }   

    //Initial hash value calculation for the needle and the haystack
    for(i=0; i<needleLenght; i++){
	needleHash = (alphabetSize*needleHash + needle[i])%hashingPrime; 
	haystackHash = (alphabetSize*haystackHash + haystack[i])%hashingPrime;
    }

    //slide the needle window over the haystack
    for(i=0; i<=haystackLenght - needleLenght; i++){
		if(needleHash == haystackHash) {
			for(j=0; j<needleLenght; j++){
				if (haystack[i+j] != needle[j]){
					break;
				}
			}
			if(j == needleLenght) {
				*indexBuffer = i;
				indexBuffer++;
				bufferSize++;
			}
		}
		
		//calculate hash value for the remaining haystack
		if(i<haystackLenght-needleLenght){
			haystackHash = (alphabetSize*(haystackHash - haystack[i]*dM) + haystack[i+needleLenght])%hashingPrime;
			if (haystackHash < 0) 
				haystackHash = (haystackHash + hashingPrime);
		}
    }
    return bufferSize;
}