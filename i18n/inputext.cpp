/*
 **********************************************************************
 *   Copyright (C) 2005-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 **********************************************************************
 */

#include "unicode/utypes.h"

#include "inputext.h"

#include "cmemory.h"
#include "cstring.h"

#include <string.h>

U_NAMESPACE_BEGIN

#define ARRAY_SIZE(array) (sizeof array / sizeof array[0])

#define NEW_ARRAY(type,count) (type *) uprv_malloc((count) * sizeof(type))
#define DELETE_ARRAY(array) uprv_free((void *) (array))

InputText::InputText()
    : fInputBytes(NEW_ARRAY(uint8_t, kBufSize)), // The text to be checked.  Markup will have been
                                                 //   removed if appropriate.
      fByteStats(NEW_ARRAY(int16_t, 256)),       // byte frequency statistics for the input text.
                                                 //   Value is percent, not absolute.
      fDeclaredEncoding(0),
      fRawInput(0),
      fRawLength(0)
{  

}

InputText::~InputText()
{
    DELETE_ARRAY(fDeclaredEncoding);
    DELETE_ARRAY(fByteStats);
    DELETE_ARRAY(fInputBytes);
}

void InputText::setText(const char *in, int32_t len)
{
    fInputLen  = 0;
    fC1Bytes   = FALSE;
    fRawInput  = (const uint8_t *) in;
    fRawLength = len;
}

void InputText::setDeclaredEncoding(const char* encoding, int32_t len)
{
    if(encoding) {
        len += 1;     // to make place for the \0 at the end.
        delete[] fDeclaredEncoding;		
        fDeclaredEncoding = NEW_ARRAY(char, len);
        uprv_strncpy(fDeclaredEncoding, encoding, len);
    }
}

UBool InputText::isSet() const 
{
    return fRawInput? TRUE : FALSE;
}

/**
*  MungeInput - after getting a set of raw input data to be analyzed, preprocess
*               it by removing what appears to be html markup.
* 
* @internal
*/
void InputText::MungeInput(UBool fStripTags) {
    int     srci = 0;
    int     dsti = 0;
    uint8_t b;
    bool    inMarkup = FALSE;
    int32_t openTags = 0;
    int32_t badTags  = 0;

    //
    //  html / xml markup stripping.
    //     quick and dirty, not 100% accurate, but hopefully good enough, statistically.
    //     discard everything within < brackets >
    //     Count how many total '<' and illegal (nested) '<' occur, so we can make some
    //     guess as to whether the input was actually marked up at all.
    if (fStripTags) {
        for (srci = 0; srci < fRawLength; srci += 1) {
            b = fRawInput[srci];

            if (b == (uint8_t)'<') {
                if (inMarkup) {
                    badTags += 1;
                }

                inMarkup = TRUE;
                openTags += 1;
            }

            if (! inMarkup) {
                fInputBytes[dsti++] = b;
            }

            if (b == (uint8_t)'>') {
                inMarkup = FALSE;
            }        
        }

        fInputLen = dsti;
    }

    //
    //  If it looks like this input wasn't marked up, or if it looks like it's
    //    essentially nothing but markup abandon the markup stripping.
    //    Detection will have to work on the unstripped input.
    //
    if (openTags<5 || openTags/5 < badTags || 
        (fInputLen < 100 && fRawLength>600)) {
            int32_t limit = fRawLength;

            if (limit > kBufSize) {
                limit = kBufSize;
            }

            for (srci=0; srci<limit; srci++) {
                fInputBytes[srci] = fRawInput[srci];
            }

            fInputLen = srci;
        }

        //
        // Tally up the byte occurence statistics.
        // These are available for use by the various detectors.
        //

        uprv_memset(fByteStats, 0, (sizeof fByteStats[0]) * 256);

        for (srci = 0; srci < fInputLen; srci += 1) {
            fByteStats[fInputBytes[srci]] += 1;
        }

        for (int32_t i = 0x80; i <= 0x9F; i += 1) {
            if (fByteStats[i] != 0) {
                fC1Bytes = TRUE;
                break;
            }
        }
}

U_NAMESPACE_END
