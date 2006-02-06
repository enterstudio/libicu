/*
 **********************************************************************
 *   Copyright (C) 2005-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 **********************************************************************
 */

#include "unicode/utypes.h"

#include "csrutf8.h"

U_NAMESPACE_BEGIN

CharsetRecog_UTF8::~CharsetRecog_UTF8()
{
    // nothing to do
}

const char *CharsetRecog_UTF8::getName() const
{
    return "UTF-8";
}

int32_t CharsetRecog_UTF8::match(InputText* det) {
    bool hasBOM = FALSE;
    int32_t numValid = 0;
    int32_t numInvalid = 0;
    const uint8_t *input = det->fRawInput;
    int32_t i;
    int32_t trailBytes = 0;
    int32_t confidence;

    if (det->fRawLength >= 3 && 
        input[0] == 0xEF && input[1] == 0xBB && input[2] == 0xBF) {
            hasBOM = TRUE;
        }

        // Scan for multi-byte sequences
        for (i=0; i < det->fRawLength; i += 1) {
            int32_t b = input[i];

            if ((b & 0x80) == 0) {
                continue;   // ASCII
            }

            // Hi bit on char found.  Figure out how long the sequence should be
            if ((b & 0x0E0) == 0x0C0) {
                trailBytes = 1;                
            } else if ((b & 0x0F0) == 0x0E0) {
                trailBytes = 2;
            } else if ((b & 0x0F8) == 0xF0) {
                trailBytes = 3;
            } else {
                numInvalid += 1;

                if (numInvalid > 5) {
                    break;
                }

                trailBytes = 0;
            }

            // Verify that we've got the right number of trail bytes in the sequence
            for (;;) {
                i += 1;

                if (i >= det->fRawLength) {
                    break;
                }

                b = input[i];

                if ((b & 0xC0) != 0x080) {
                    numInvalid += 1;
                    break;
                }

                if (--trailBytes == 0) {
                    numValid += 1;
                    break;
                }
            }

        }

        // Cook up some sort of confidence score, based on presense of a BOM
        //    and the existence of valid and/or invalid multi-byte sequences.
        confidence = 0;
        if (hasBOM && numInvalid == 0) {
            confidence = 100;
        } else if (hasBOM && numValid > numInvalid*10) {
            confidence = 80;
        } else if (numValid > 3 && numInvalid == 0) {
            confidence = 100;            
        } else if (numValid > 0 && numInvalid == 0) {
            confidence = 80;
        } else if (numValid == 0 && numInvalid == 0) {
            // Plain ASCII.  
            confidence = 10;            
        } else if (numValid > numInvalid*10) {
            // Probably corruput utf-8 data.  Valid sequences aren't likely by chance.
            confidence = 25;
        }

        return confidence;
}

U_NAMESPACE_END
