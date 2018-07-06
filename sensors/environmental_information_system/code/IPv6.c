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
//************************************************************************************************************************
//    IPv6-ASCII-Ein-Ausgabe
//************************************************************************************************************************

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*
Adressnotation aus Wikipedia kopiert:

Die textuelle Notation von IPv6-Adressen ist in Abschnitt 2.2 von RFC 4291 beschrieben:
1. IPv6-Adressen werden für gewöhnlich hexadezimal (IPv4: dezimal) notiert, wobei die Zahl in acht Blöcke zu jeweils 16 Bit (4 Hexadezimalstellen) unterteilt wird.
   Diese Blöcke werden durch Doppelpunkte (IPv4: Punkte) getrennt notiert: 2001:0db8:85a3:08d3:1319:8a2e:0370:7344.
2. Führende Nullen innerhalb eines Blockes dürfen ausgelassen werden: 2001:0db8:0000:08d3:0000:8a2e:0070:7344 ist gleichbedeutend mit 2001:db8:0:8d3:0:8a2e:70:7344.
3. Mehrere aufeinander folgende Blöcke, deren Wert 0 (bzw. 0000) beträgt, dürfen ausgelassen werden. Dies wird durch zwei aufeinander folgende Doppelpunkte angezeigt:
   2001:0db8:0:0:0:0:1428:57ab ist gleichbedeutend mit 2001:db8::1428:57ab.[15]
4. Die Reduktion durch Regel 3 darf nur einmal durchgeführt werden, das heißt, es darf höchstens eine zusammenhängende Gruppe aus Null-Blöcken in der Adresse ersetzt werden.
   Die Adresse 2001:0db8:0:0:8d3:0:0:0 darf demnach entweder zu 2001:db8:0:0:8d3:: oder 2001:db8::8d3:0:0:0 gekürzt werden; 2001:db8::8d3:: ist unzulässig, da dies mehrdeutig ist
   und fälschlicherweise z. B. auch als 2001:db8:0:0:0:8d3:0:0 interpretiert werden könnte. Die Reduktion darf auch dann nicht mehrfach durchgeführt werden, wenn das Ergebnis eindeutig wäre,
   weil jeweils genau eine einzige 0 gekürzt wurde. Es empfiehlt sich, die Gruppe mit den meisten Null-Blöcken zu kürzen.
5. Ebenfalls darf für die letzten vier Bytes (also 32 Bits) der Adresse die herkömmliche dezimale Notation verwendet werden.
   So ist ::ffff:127.0.0.1 eine alternative Schreibweise für ::ffff:7f00:1. Diese Schreibweise wird vor allem bei Einbettung des IPv4-Adressraums in den IPv6-Adressraum verwendet.
*/

// Hilffunktionen um IPV6-Adresse zu parsen
// gefunden auf:  https://rosettacode.org/wiki/Parse_an_IP_Address

static uint32_t _parseDecimal ( const char **pchCursor )
{
	uint32_t nVal = 0;
	char chNow;
	while ( chNow = **pchCursor, chNow >= '0' && chNow <= '9' )
	{
		//shift digit in
		nVal *= 10;
		nVal += chNow - '0';

		++*pchCursor;
	}
	return nVal;
}

static uint32_t _parseHex ( const char **pchCursor )
{
	uint32_t nVal = 0;
	char chNow;
	while ( chNow = **pchCursor & 0x5f,	//(collapses case, but mutilates digits)
			(chNow >= ('0'&0x5f) && chNow <= ('9'&0x5f)) ||
			(chNow >= 'A' && chNow <= 'F')
			)
	{
		unsigned char nybbleValue;
		chNow -= 0x10;	//scootch digital values down; hex now offset by x31
		nybbleValue = ( chNow > 9 ? chNow - (0x31-0x0a) : chNow );
		//shift nybble in
		nVal <<= 4;
		nVal += nybbleValue;

		++*pchCursor;
	}
	return nVal;
}

//Parse a textual IPv4 or IPv6 address, optionally with port, into a binary
//array (for the address, in network order), and an optionally provided port.
//Also, indicate which of those forms (4 or 6) was parsed.
//Return 0 on success.  Otherwise Error-Code

//ppszText must be a nul-terminated ASCII string.  It will be
//updated to point to the character which terminated parsing (so you can carry
//on with other things.  abyAddr must be 16 bytes.  You can provide NULL for
//abyAddr, nPort, bIsIPv6, if you are not interested in any of those
//informations.  If we request port, but there is no port part port ist unchanged.
//There may be no whitespace leading or internal (though this may
//be used to terminate a successful parse.
//Note:  the binary address and integer port are in network order.

//===========================================================================================
//    IPv6_ParseIPv4OrIPv6
//===========================================================================================
//    ppszText:         in:  Zeiger auf den IPv6-String der mit 0 Terminiert sein muss!!
//    abyAddr:          out: Zeiger auf IPv6-Daten
//    pnPort:           out: Zeiger auf IPv6-Port
//    pbIsIPv6:         out: bool true wenn IPv6-Adresse erkannt wurde
//===========================================================================================
//    Return:           0=ok sonst Fehler-Nummer
//===========================================================================================
uint16_t IPv6_ParseIPv4OrIPv6 ( const char** ppszText,
		  uint8_t *abyAddr, uint16_t* pnPort, uint16_t *pbIsIPv6)
{
	unsigned char* abyAddrLocal;
	unsigned char abyDummyAddr[16];

	//find first colon, dot, and open bracket
	const char* pchColon = strchr ( *ppszText, ':' );
	const char* pchDot = strchr ( *ppszText, '.' );
	const char* pchOpenBracket = strchr ( *ppszText, '[' );
	const char* pchCloseBracket = NULL;

	//we'll consider this to (probably) be IPv6 if we find an open
	//bracket, or an absence of dots, or if there is a colon, and it
	//precedes any dots that may or may not be there
	int bIsIPv6local = NULL != pchOpenBracket || NULL == pchDot ||
			( NULL != pchColon && ( NULL == pchDot || pchColon < pchDot ) );
	//OK, now do a little further sanity check our initial guess...
	if ( bIsIPv6local )
	{
		//if open bracket, then must have close bracket that follows somewhere
		pchCloseBracket = strchr ( *ppszText, ']' );
		if ( NULL != pchOpenBracket && ( NULL == pchCloseBracket ||
				pchCloseBracket < pchOpenBracket ) )
			return 1;
	}
	else	//probably ipv4
	{
		//dots must exist, and precede any colons
		if ( NULL == pchDot || ( NULL != pchColon && pchColon < pchDot ) )
			return 2;
	}
   //we figured out this much so far....
	if ( NULL != pbIsIPv6 )
		*pbIsIPv6 = bIsIPv6local;

	//especially for IPv6 (where we will be decompressing and validating)
	//we really need to have a working buffer even if the caller didn't
	//care about the results.
	abyAddrLocal = abyAddr;	//prefer to use the caller's
	if ( NULL == abyAddrLocal )	//but use a dummy if we must
		abyAddrLocal = abyDummyAddr;

	//OK, there should be no correctly formed strings which are miscategorized,
	//and now any format errors will be found out as we continue parsing
	//according to plan.
	if ( ! bIsIPv6local )	//try to parse as IPv4
	{
		//4 dotted quad decimal; optional port if there is a colon
		//since there are just 4, and because the last one can be terminated
		//differently, I'm just going to unroll any potential loop.
		unsigned char* pbyAddrCursor = abyAddrLocal;
		uint32_t nVal;
		const char* pszTextBefore = *ppszText;
		nVal =_parseDecimal ( ppszText );			//get first val
		if ( '.' != **ppszText || nVal > 255 || pszTextBefore == *ppszText )	//must be in range and followed by dot and nonempty
			return 3;
		*(pbyAddrCursor++) = (unsigned char) nVal;	//stick it in addr
		++(*ppszText);	//past the dot

		pszTextBefore = *ppszText;
		nVal =_parseDecimal ( ppszText );			//get second val
		if ( '.' != **ppszText || nVal > 255 || pszTextBefore == *ppszText )
			return 4;
		*(pbyAddrCursor++) = (unsigned char) nVal;
		++(*ppszText);	//past the dot

		pszTextBefore = *ppszText;
		nVal =_parseDecimal ( ppszText );			//get third val
		if ( '.' != **ppszText || nVal > 255 || pszTextBefore == *ppszText )
			return 5;
		*(pbyAddrCursor++) = (unsigned char) nVal;
		++(*ppszText);	//past the dot

		pszTextBefore = *ppszText;
		nVal =_parseDecimal ( ppszText );			//get fourth val
		if ( nVal > 255 || pszTextBefore == *ppszText )	//(we can terminate this one in several ways)
			return 6;
		*(pbyAddrCursor++) = (unsigned char) nVal;

		if ( ':' == **ppszText && NULL != pnPort )	//have port part, and we want it
		{
			unsigned short usPortNetwork;	//save value in network order
			++(*ppszText);	//past the colon
			pszTextBefore = *ppszText;
			nVal =_parseDecimal ( ppszText );
			if ( nVal > 65535 || pszTextBefore == *ppszText )
				return 7;
			((unsigned char*)&usPortNetwork)[0] = ( nVal & 0xff00 ) >> 8;
			((unsigned char*)&usPortNetwork)[1] = ( nVal & 0xff );
			*pnPort = usPortNetwork;
			return 0;
		}
		else	//finished just with ip address
		{
			return 0;
		}
	}
	else	//try to parse as IPv6
	{
		unsigned char* pbyAddrCursor;
		unsigned char* pbyZerosLoc;
		int bIPv4Detected;
		int nIdx;
		//up to 8 16-bit hex quantities, separated by colons, with at most one
		//empty quantity, acting as a stretchy run of zeroes.  optional port
		//if there are brackets followed by colon and decimal port number.
		//A further form allows an ipv4 dotted quad instead of the last two
		//16-bit quantities, but only if in the ipv4 space ::ffff:x:x .
		if ( NULL != pchOpenBracket )	//start past the open bracket, if it exists
			*ppszText = pchOpenBracket + 1;
		pbyAddrCursor = abyAddrLocal;
		pbyZerosLoc = NULL;	//if we find a 'zero compression' location
		bIPv4Detected = 0;
		for ( nIdx = 0; nIdx < 8; ++nIdx )	//we've got up to 8 of these, so we will use a loop
		{
			const char* pszTextBefore = *ppszText;
			uint32_t nVal =_parseHex ( ppszText );		//get value; these are hex
			if ( pszTextBefore == *ppszText )	//if empty, we are zero compressing; note the loc
			{
				if ( NULL != pbyZerosLoc )	//there can be only one!
				{
					//unless it's a terminal empty field, then this is OK, it just means we're done with the host part
					if ( pbyZerosLoc == pbyAddrCursor )
					{
						--nIdx;
						break;
					}
					return 8;	//otherwise, it's a format error
				}
				if ( ':' != **ppszText )	//empty field can only be via :
					return 9;
				if ( 0 == nIdx )	//leading zero compression requires an extra peek, and adjustment
				{
					++(*ppszText);
					if ( ':' != **ppszText )
						return 10;
				}

				pbyZerosLoc = pbyAddrCursor;
				++(*ppszText);
			}
			else
			{
				if ( '.' == **ppszText )	               //special case of ipv4 convenience notation
				{
					//who knows how to parse ipv4?  we do!
					const char* pszTextlocal = pszTextBefore;	//back it up
					unsigned char abyAddrlocal[16];
					uint16_t bIsIPv6local=0;
					int bParseResultlocal = IPv6_ParseIPv4OrIPv6 ( &pszTextlocal, abyAddrlocal, NULL, &bIsIPv6local);
					*ppszText = pszTextlocal;	            //success or fail, remember the terminating char
					if ( bParseResultlocal || bIsIPv6local )	//must parse and must be ipv4
						return 11;
					//transfer addrlocal into the present location
					*(pbyAddrCursor++) = abyAddrlocal[0];
					*(pbyAddrCursor++) = abyAddrlocal[1];
					*(pbyAddrCursor++) = abyAddrlocal[2];
					*(pbyAddrCursor++) = abyAddrlocal[3];
					++nIdx;	                              //pretend like we took another short, since the ipv4 effectively is two shorts
					bIPv4Detected = 1;	                  //remember how we got here for further validation later
					break;	                              //totally done with address
				}

				if ( nVal > 65535 )	                     //must be 16 bit quantity
					return 12;
				*(pbyAddrCursor++) = nVal >> 8;		      //transfer in network order
				*(pbyAddrCursor++) = nVal & 0xff;
				if ( ':' == **ppszText )	               //typical case inside; carry on
				{
					++(*ppszText);
				}
				else	                                    //some other terminating character; done with this parsing parts
				{
					break;
				}
			}
		}

		//handle any zero compression we found
		if ( NULL != pbyZerosLoc )
		{
			int nHead = (int)( pbyZerosLoc - abyAddrLocal );	//how much before zero compression
			int nTail = nIdx * 2 - (int)( pbyZerosLoc - abyAddrLocal );	//how much after zero compression
			int nZeros = 16 - nTail - nHead;		//how much zeros
			memmove ( &abyAddrLocal[16-nTail], pbyZerosLoc, nTail );	//scootch stuff down
			memset ( pbyZerosLoc, 0, nZeros );		//clear the compressed zeros
		}

		//validation of ipv4 subspace ::ffff:x.x
		if ( bIPv4Detected )
		{
			static const unsigned char abyPfx[] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0xff,0xff };
			if ( 0 != memcmp ( abyAddrLocal, abyPfx, sizeof(abyPfx) ) )
				return 13;
		}

		//close bracket
		if ( NULL != pchOpenBracket )
		{
			if ( ']' != **ppszText )
				return 0;
			++(*ppszText);
		}

		if ( ':' == **ppszText && NULL != pnPort )	//have port part, and we want it
		{
			++(*ppszText);	//past the colon
			const char* pszTextBefore = *ppszText;
			uint32_t nVal;
			nVal =_parseDecimal ( ppszText );
			if ( nVal > 65535 || pszTextBefore == *ppszText )
				return 0;
			*pnPort = (uint16_t) nVal;
			return 0;
		}
      return 0;

	}
//   return 14;                                         // hier sollte man gar nicht hinkommen
}

//===========================================================================================
//    Parse ein ASCII-String mit einer IPv6-Adresse in die 16-BYTE-IPv6-Adresse
//===========================================================================================
//    pData:         Zeiger auf den IPv6-String der mit 0 Terminiert sein muss!!
//    pbIP:          Zeiger auf IPv6-Daten
//    pwPort:        Zeiger auf IPv6-Port
//===========================================================================================
//    Return:        0=ok sonst Fehler-Nummer
//===========================================================================================
uint32_t IPv6_ParseString( char *pData, uint8_t *pbIP, uint16_t *pwPort )
{
   uint16_t uiIsIPv6=0;
   return IPv6_ParseIPv4OrIPv6( (const char**)&pData, pbIP, pwPort, &uiIsIPv6);
}

//===========================================================================================
//    Wandelt eine IPv6 Adresse in einen lesbaren ASCII-String um
//===========================================================================================
//    pcBuffer:      Zeiger auf den Ausgabebuffer - Mindestlänge 52 Zeichen+Description-Länge
//    wLen:          Länge des Ausgabebuffers
//    pbDescription: Zeiger auf die Beschreibung der IPv6-Adresse - z.B. "Server-IP"
//    pbIPv6:        Zeiger auf die IPv6-Adresse
//    wPort:         Portnummer
//===========================================================================================
//    return:        Länge des Strings
//===========================================================================================
uint16_t IPv6_toString( char *pcBuffer, uint16_t wLen, char *pbDescription, uint8_t *pbIPv6, uint16_t wPort)
{
   wLen = snprintf( pcBuffer, wLen,  "%s; [%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X:%.2X%.2X]:%u",
                     pbDescription,
                     pbIPv6[0],  pbIPv6[1],  pbIPv6[2],  pbIPv6[3],
                     pbIPv6[4],  pbIPv6[5],  pbIPv6[6],  pbIPv6[7],
                     pbIPv6[8],  pbIPv6[9],  pbIPv6[10], pbIPv6[11],
                     pbIPv6[12], pbIPv6[13], pbIPv6[14], pbIPv6[15],
                     wPort
                     );
   return wLen;
}
