/*
 *******************************************************************************
 *
 *   Copyright (C) 1999-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  gendata.cpp
 *
 *   created on: 11/03/2000
 *   created by: Eric R. Mader
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "LETypes.h"
#include "LEScripts.h"
#include "LayoutEngine.h"

#include "sfnt.h"
#include "PortableFontInstance.h"

#include "unicode/uscript.h"

U_NAMESPACE_USE

#define ARRAY_LENGTH(array) (sizeof array / sizeof array[0])

struct TestInput
{
    const char *fontName;
    LEUnicode  *text;
    le_int32    textLength;
    le_int32    scriptCode;
    le_bool     rightToLeft;
};

/* 
 * FIXME: should use the output file name and the current date.
 */
const char *header =
    "/*\n"
    " *******************************************************************************\n"
    " *\n"
    " *   Copyright (C) 1999-%4.4d, International Business Machines\n"
    " *   Corporation and others.  All Rights Reserved.\n"
    " *\n"
    " *   WARNING: THIS FILE IS MACHINE GENERATED. DO NOT HAND EDIT IT\n"
    " *   UNLESS YOU REALLY KNOW WHAT YOU'RE DOING.\n"
    " *\n"
    " *******************************************************************************\n"
    " *\n"
    " *   file name:    testdata.cpp\n"
    " *   generated on: %s\n"
    " *   generated by: gendata.cpp\n"
    " */\n"
    "\n"
    "#include \"LETypes.h\"\n"
    "#include \"LEScripts.h\"\n"
    "#include \"letest.h\"\n"
    "\n";

LEUnicode devaText[] =
{
    0x0936, 0x094d, 0x0930, 0x0940, 0x092e, 0x0926, 0x094d, 0x0020,
    0x092d, 0x0917, 0x0935, 0x0926, 0x094d, 0x0917, 0x0940, 0x0924,
    0x093e, 0x0020, 0x0905, 0x0927, 0x094d, 0x092f, 0x093e, 0x092f,
    0x0020, 0x0905, 0x0930, 0x094d, 0x091c, 0x0941, 0x0928, 0x0020,
    0x0935, 0x093f, 0x0937, 0x093e, 0x0926, 0x0020, 0x092f, 0x094b,
    0x0917, 0x0020, 0x0927, 0x0943, 0x0924, 0x0930, 0x093e, 0x0937,
    0x094d, 0x091f, 0x094d, 0x0930, 0x0020, 0x0909, 0x0935, 0x093E,
    0x091A, 0x0943, 0x0020, 0x0927, 0x0930, 0x094d, 0x092e, 0x0915,
    0x094d, 0x0937, 0x0947, 0x0924, 0x094d, 0x0930, 0x0947, 0x0020,
    0x0915, 0x0941, 0x0930, 0x0941, 0x0915, 0x094d, 0x0937, 0x0947,
    0x0924, 0x094d, 0x0930, 0x0947, 0x0020, 0x0938, 0x092e, 0x0935,
    0x0947, 0x0924, 0x093e, 0x0020, 0x092f, 0x0941, 0x092f, 0x0941,
    0x0924, 0x094d, 0x0938, 0x0935, 0x0903, 0x0020, 0x092e, 0x093e,
    0x092e, 0x0915, 0x093e, 0x0903, 0x0020, 0x092a, 0x093e, 0x0923,
    0x094d, 0x0921, 0x0935, 0x093e, 0x0936, 0x094d, 0x091a, 0x0948,
    0x0935, 0x0020, 0x0915, 0x093f, 0x092e, 0x0915, 0x0941, 0x0930,
    0x094d, 0x0935, 0x0924, 0x0020, 0x0938, 0x0902, 0x091c, 0x0935
};

le_int32 devaTextLength = ARRAY_LENGTH(devaText);

LEUnicode arabText[] =
{
    0x0623, 0x0633, 0x0627, 0x0633, 0x064B, 0x0627, 0x060C, 0x0020, 
    0x062A, 0x062A, 0x0639, 0x0627, 0x0645, 0x0644, 0x0020, 
    0x0627, 0x0644, 0x062D, 0x0648, 0x0627, 0x0633, 0x064A, 0x0628, 
    0x0020, 0x0641, 0x0642, 0x0637, 0x0020, 0x0645, 0x0639, 0x0020, 
    0x0627, 0x0644, 0x0623, 0x0631, 0x0642, 0x0627, 0x0645, 0x060C, 
    0x0020, 0x0648, 0x062A, 0x0642, 0x0648, 0x0645, 0x0020, 0x0628, 
    0x062A, 0x062E, 0x0632, 0x064A, 0x0646, 0x0020, 0x0627, 0x0644, 
    0x0623, 0x062D, 0x0631, 0x0641, 0x0020, 0x0648, 0x0627, 0x0644, 
    0x0645, 0x062D, 0x0627, 0x0631, 0x0641, 0x0020, 0x0627, 0x0644, 
    0x0623, 0x062E, 0x0631, 0x0649, 0x0020, 0x0628, 0x0639, 0x062F, 
    0x0020, 0x0623, 0x0646, 0x0020, 0x062A, 0x064F, 0x0639, 0x0637, 
    0x064A, 0x0020, 0x0631, 0x0642, 0x0645, 0x0627, 0x0020, 0x0645, 
    0x0639, 0x064A, 0x0646, 0x0627, 0x0020, 0x0644, 0x0643, 0x0644, 
    0x0020, 0x0648, 0x0627, 0x062D, 0x062F, 0x0020, 0x0645, 0x0646, 
    0x0647, 0x0627, 0x002E, 0x0020, 0x0648, 0x0642, 0x0628, 0x0644, 
    0x0020, 0x0627, 0x062E, 0x062A, 0x0631, 0x0627, 0x0639, 0x0020, 
    0x0022, 0x064A, 0x0648, 0x0646, 0x0650, 0x0643, 0x0648, 0x062F, 
    0x0022, 0x060C, 0x0020, 0x0643, 0x0627, 0x0646, 0x0020, 0x0647, 
    0x0646, 0x0627, 0x0643, 0x0020, 0x0645, 0x0626, 0x0627, 0x062A, 
    0x0020, 0x0627, 0x0644, 0x0623, 0x0646, 0x0638, 0x0645, 0x0629, 
    0x0020, 0x0644, 0x0644, 0x062A, 0x0634, 0x0641, 0x064A, 0x0631, 
    0x0020, 0x0648, 0x062A, 0x062E, 0x0635, 0x064A, 0x0635, 0x0020, 
    0x0647, 0x0630, 0x0647, 0x0020, 0x0627, 0x0644, 0x0623, 0x0631, 
    0x0642, 0x0627, 0x0645, 0x0020, 0x0644, 0x0644, 0x0645, 0x062D, 
    0x0627, 0x0631, 0x0641, 0x060C, 0x0020, 0x0648, 0x0644, 0x0645, 
    0x0020, 0x064A, 0x0648, 0x062C, 0x062F, 0x0020, 0x0646, 0x0638, 
    0x0627, 0x0645, 0x0020, 0x062A, 0x0634, 0x0641, 0x064A, 0x0631, 
    0x0020, 0x0648, 0x0627, 0x062D, 0x062F, 0x0020, 0x064A, 0x062D, 
    0x062A, 0x0648, 0x064A, 0x0020, 0x0639, 0x0644, 0x0649, 0x0020, 
    0x062C, 0x0645, 0x064A, 0x0639, 0x0020, 0x0627, 0x0644, 0x0645, 
    0x062D, 0x0627, 0x0631, 0x0641, 0x0020, 0x0627, 0x0644, 0x0636, 
    0x0631, 0x0648, 0x0631, 0x064A, 0x0629
    
    /* The next few sentences...
    0x002E, 0x0020, 0x0648, 
    0x0639, 0x0644, 0x0649, 0x0020, 0x0633, 0x0628, 0x064A, 0x0644, 
    0x0020, 0x0627, 0x0644, 0x0645, 0x062B, 0x0627, 0x0644, 0x060C, 
    0x0020, 0x0641, 0x0625, 0x0646, 0x0020, 0x0627, 0x0644, 0x0627, 
    0x062A, 0x062D, 0x0627, 0x062F, 0x0020, 0x0627, 0x0644, 0x0623, 
    0x0648, 0x0631, 0x0648, 0x0628, 0x064A, 0x0020, 0x0644, 0x0648, 
    0x062D, 0x062F, 0x0647, 0x060C, 0x0020, 0x0627, 0x062D, 0x062A, 
    0x0648, 0x0649, 0x0020, 0x0627, 0x0644, 0x0639, 0x062F, 0x064A, 
    0x062F, 0x0020, 0x0645, 0x0646, 0x0020, 0x0627, 0x0644, 0x0634, 
    0x0641, 0x0631, 0x0627, 0x062A, 0x0020, 0x0627, 0x0644, 0x0645, 
    0x062E, 0x062A, 0x0644, 0x0641, 0x0629, 0x0020, 0x0644, 0x064A, 
    0x063A, 0x0637, 0x064A, 0x0020, 0x062C, 0x0645, 0x064A, 0x0639, 
    0x0020, 0x0627, 0x0644, 0x0644, 0x063A, 0x0627, 0x062A, 0x0020, 
    0x0627, 0x0644, 0x0645, 0x0633, 0x062A, 0x062E, 0x062F, 0x0645, 
    0x0629, 0x0020, 0x0641, 0x064A, 0x0020, 0x0627, 0x0644, 0x0627, 
    0x062A, 0x062D, 0x0627, 0x062F, 0x002E, 0x0020, 0x0648, 0x062D, 
    0x062A, 0x0649, 0x0020, 0x0644, 0x0648, 0x0020, 0x0627, 0x0639, 
    0x062A, 0x0628, 0x0631, 0x0646, 0x0627, 0x0020, 0x0644, 0x063A, 
    0x0629, 0x0020, 0x0648, 0x0627, 0x062D, 0x062F, 0x0629, 0x060C, 
    0x0020, 0x0643, 0x0627, 0x0644, 0x0644, 0x063A, 0x0629, 0x0020, 
    0x0627, 0x0644, 0x0625, 0x0646, 0x062C, 0x0644, 0x064A, 0x0632, 
    0x064A, 0x0629, 0x060C, 0x0020, 0x0641, 0x0625, 0x0646, 0x0020, 
    0x062C, 0x062F, 0x0648, 0x0644, 0x0020, 0x0634, 0x0641, 0x0631, 
    0x0629, 0x0020, 0x0648, 0x0627, 0x062D, 0x062F, 0x0020, 0x0644, 
    0x0645, 0x0020, 0x064A, 0x0643, 0x0641, 0x0020, 0x0644, 0x0627, 
    0x0633, 0x062A, 0x064A, 0x0639, 0x0627, 0x0628, 0x0020, 0x062C, 
    0x0645, 0x064A, 0x0639, 0x0020, 0x0627, 0x0644, 0x0623, 0x062D, 
    0x0631, 0x0641, 0x0020, 0x0648, 0x0639, 0x0644, 0x0627, 0x0645, 
    0x0627, 0x062A, 0x0020, 0x0627, 0x0644, 0x062A, 0x0631, 0x0642, 
    0x064A, 0x0645, 0x0020, 0x0648, 0x0627, 0x0644, 0x0631, 0x0645, 
    0x0648, 0x0632, 0x0020, 0x0627, 0x0644, 0x0641, 0x0646, 0x064A, 
    0x0629, 0x0020, 0x0648, 0x0627, 0x0644, 0x0639, 0x0644, 0x0645, 
    0x064A, 0x0629, 0x0020, 0x0627, 0x0644, 0x0634, 0x0627, 0x0626, 
    0x0639, 0x0629, 0x0020, 0x0627, 0x0644, 0x0627, 0x0633, 0x062A, 
    0x0639, 0x0645, 0x0627, 0x0644, 0x002E */
};
le_int32 arabTextLength = ARRAY_LENGTH(arabText);


LEUnicode thaiSample[] =
{
    0x0E1A, 0x0E17, 0x0E17, 0x0E35, 0x0E48, 0x0E51, 0x0E1E, 0x0E32,
    0x0E22, 0x0E38, 0x0E44, 0x0E0B, 0x0E42, 0x0E04, 0x0E25, 0x0E19,
    0x0E42, 0x0E14, 0x0E42, 0x0E23, 0x0E18, 0x0E35, 0x0E2D, 0x0E32, 
    0x0E28, 0x0E31, 0x0E22, 0x0E2D, 0x0E22, 0x0E39, 0x0E48, 0x0E17,
    0x0E48, 0x0E32, 0x0E21, 0x0E01, 0x0E25, 0x0E32, 0x0E07, 0x0E17,
    0x0E38, 0x0E48, 0x0E07, 0x0E43, 0x0E2B, 0x0E0D, 0x0E48, 0x0E43,
    0x0E19, 0x0E41, 0x0E04, 0x0E19, 0x0E0B, 0x0E31, 0x0E2A, 0x0E01, 
    0x0E31, 0x0E1A, 0x0E25, 0x0E38, 0x0E07, 0x0E40, 0x0E2E, 0x0E19,
    0x0E23, 0x0E35, 0x0E0A, 0x0E32, 0x0E27, 0x0E44, 0x0E23, 0x0E48,
    0x0E41, 0x0E25, 0x0E30, 0x0E1B, 0x0E49, 0x0E32, 0x0E40, 0x0E2D,
    0x0E47, 0x0E21, 0x0E20, 0x0E23, 0x0E23, 0x0E22, 0x0E32, 0x0E0A,
    0x0E32, 0x0E27, 0x0E44, 0x0E23, 0x0E48, 0x0E1A, 0x0E49, 0x0E32,
    0x0E19, 0x0E02, 0x0E2D, 0x0E07, 0x0E1E, 0x0E27, 0x0E01, 0x0E40, 
    0x0E02, 0x0E32, 0x0E2B, 0x0E25, 0x0E31, 0x0E07, 0x0E40, 0x0E25,
    0x0E47, 0x0E01, 0x0E40, 0x0E1E, 0x0E23, 0x0E32, 0x0E30, 0x0E44,
    0x0E21, 0x0E49, 0x0E2A, 0x0E23, 0x0E49, 0x0E32, 0x0E07, 0x0E1A,
    0x0E49, 0x0E32, 0x0E19, 0x0E15, 0x0E49, 0x0E2D, 0x0E07, 0x0E02, 
    0x0E19, 0x0E21, 0x0E32, 0x0E14, 0x0E49, 0x0E27, 0x0E22, 0x0E40,
    0x0E01, 0x0E27, 0x0E35, 0x0E22, 0x0E19, 0x0E40, 0x0E1B, 0x0E47,
    0x0E19, 0x0E23, 0x0E30, 0x0E22, 0x0E30, 0x0E17, 0x0E32, 0x0E07,
    0x0E2B, 0x0E25, 0x0E32, 0x0E22, 0x0E44, 0x0E21, 0x0E25, 0x0E4C
    /* A few more lines...
    0x0E1A, 0x0E49, 0x0E32, 0x0E19, 0x0E21, 0x0E35, 0x0E2A, 0x0E35,
    0x0E48, 0x0E1D, 0x0E32, 0x0E21, 0x0E35, 0x0E1E, 0x0E37, 0x0E49,
    0x0E19, 0x0E01, 0x0E31, 0x0E1A, 0x0E2B, 0x0E25, 0x0E31, 0x0E07,
    0x0E04, 0x0E32, 0x0E23, 0x0E27, 0x0E21, 0x0E17, 0x0E33, 0x0E40,
    0x0E1B, 0x0E47, 0x0E19, 0x0E2B, 0x0E49, 0x0E2D, 0x0E07, 0x0E40,
    0x0E14, 0x0E35, 0x0E22, 0x0E27, 0x0E43, 0x0E19, 0x0E2B, 0x0E49, 
    0x0E2D, 0x0E07, 0x0E21, 0x0E35, 0x0E17, 0x0E31, 0x0E49, 0x0E07,
    0x0E40, 0x0E15, 0x0E32, 0x0E2B, 0x0E38, 0x0E07, 0x0E15, 0x0E49,
    0x0E21, 0x0E17, 0x0E35, 0x0E48, 0x0E2A, 0x0E19, 0x0E34, 0x0E21,
    0x0E14, 0x0E39, 0x0E02, 0x0E36, 0x0E49, 0x0E19, 0x0E40, 0x0E25,
    0x0E2D, 0x0E30, 0x0E21, 0x0E35, 0x0E15, 0x0E39, 0x0E49, 0x0E43, 
    0x0E2A, 0x0E48, 0x0E16, 0x0E49, 0x0E27, 0x0E22, 0x0E0A, 0x0E32,
    0x0E21, 0x0E42, 0x0E15, 0x0E4A, 0x0E30, 0x0E40, 0x0E01, 0x0E49,
    0x0E32, 0x0E2D, 0x0E35, 0x0E49, 0x0E2A, 0x0E32, 0x0E21, 0x0E2B,
    0x0E23
    */
};

le_int32 thaiSampleLength = ARRAY_LENGTH(thaiSample);

TestInput testInputs[] = {
    {"raghu.ttf",             devaText,   devaTextLength,   devaScriptCode, FALSE},
    {"CODE2000.TTF",          arabText,   arabTextLength,   arabScriptCode, TRUE},
    {"LucidaSansRegular.ttf", arabText,   arabTextLength,   arabScriptCode, TRUE},
    {"angsd___.ttf",          thaiSample, thaiSampleLength, thaiScriptCode, FALSE}
};

#define TEST_COUNT ARRAY_LENGTH(testInputs)

le_int32 testCount = TEST_COUNT;

void dumpShorts(FILE *file, const char *label, le_int32 id, le_uint16 *shorts, le_int32 count) {
    char lineBuffer[8 * 8 + 2];
    le_int32 bufp = 0;

    fprintf(file, label, id);

    for (int i = 0; i < count; i += 1) {
        if (i % 8 == 0 && bufp != 0) {
            fprintf(file, "    %s\n", lineBuffer);
            bufp = 0;
        }

        bufp += sprintf(&lineBuffer[bufp], "0x%4.4X, ", shorts[i]);
    }

    if (bufp != 0) {
        lineBuffer[bufp - 2] = '\0';
        fprintf(file, "    %s\n", lineBuffer);
    }

    fprintf(file, "};\n\n");
}

void dumpLongs(FILE *file, const char *label, le_int32 id, le_int32 *longs, le_int32 count) {
    char lineBuffer[8 * 12 + 2];
    le_int32 bufp = 0;

    fprintf(file, label, id);

    for (int i = 0; i < count; i += 1) {
        if (i % 8 == 0 && bufp != 0) {
            fprintf(file, "    %s\n", lineBuffer);
            bufp = 0;
        }

        bufp += sprintf(&lineBuffer[bufp], "0x%8.8X, ", longs[i]);
    }

    if (bufp != 0) {
        lineBuffer[bufp - 2] = '\0';
        fprintf(file, "    %s\n", lineBuffer);
    }

    fprintf(file, "};\n\n");
}

void dumpFloats(FILE *file, const char *label, le_int32 id, float *floats, le_int32 count) {
    char lineBuffer[8 * 16 + 2];
    le_int32 bufp = 0;

    fprintf(file, label, id);

    for (int i = 0; i < count; i += 1) {
        if (i % 8 == 0 && bufp != 0) {
            fprintf(file, "    %s\n", lineBuffer);
            bufp = 0;
        }

        bufp += sprintf(&lineBuffer[bufp], "%fF, ", floats[i]);
    }

    if (bufp != 0) {
        lineBuffer[bufp - 2] = '\0';
        fprintf(file, "    %s\n", lineBuffer);
    }

    fprintf(file, "};\n\n");
}

const char *getShortName(le_int32 scriptCode)
{
    static char shortName[5];
    const char *name = uscript_getShortName((UScriptCode) scriptCode);

    shortName[0] = tolower(name[0]);
    shortName[1] = tolower(name[1]);
    shortName[2] = tolower(name[2]);
    shortName[3] = tolower(name[3]);
    shortName[4] = '\0';

    return shortName;
}

int main(int /*argc*/, char *argv[])
{
    le_int32 test;
    FILE *outputFile = fopen(argv[1], "w");
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    const char *tmFormat = "%m/%d/%Y %I:%M:%S %p %Z";
    char  tmString[64];

    strftime(tmString, 64, tmFormat, local);
    fprintf(outputFile, header, local->tm_year + 1900, tmString);

    for (test = 0; test < testCount; test += 1) {
        LEErrorCode fontStatus = LE_NO_ERROR;
        PortableFontInstance fontInstance(testInputs[test].fontName, 12, fontStatus);

        if (LE_FAILURE(fontStatus)) {
            printf("ERROR: test case %d, could not get a font instance for %s\n", test, testInputs[test].fontName);
            continue;
        }

        const char *nameString = fontInstance.getNameString(NAME_VERSION_STRING, PLATFORM_MACINTOSH, MACINTOSH_ROMAN, MACINTOSH_ENGLISH);

        fprintf(outputFile, "const char *fontVersionString%d = \"%s\";\n", test, nameString);
        fprintf(outputFile, "le_uint32 fontChecksum%d = 0x%8.8X;\n\n", test, fontInstance.getFontChecksum());

        fontInstance.deleteNameString(nameString);

        LEErrorCode success = LE_NO_ERROR;
        LayoutEngine *engine = LayoutEngine::layoutEngineFactory(&fontInstance, testInputs[test].scriptCode, -1, success);
        le_uint32  glyphCount;
        LEGlyphID *glyphs;
        le_int32  *indices;
        float     *positions;

        if (LE_FAILURE(success)) {
            printf("ERROR: test case %d, could not create a LayoutEngine for script %s.\n", test, uscript_getName((UScriptCode) testInputs[test].scriptCode));
            continue;
        }

        glyphCount = engine->layoutChars(testInputs[test].text, 0, testInputs[test].textLength, testInputs[test].textLength, testInputs[test].rightToLeft, 0, 0, success);

        glyphs    = new LEGlyphID[glyphCount];
        indices   = new le_int32[glyphCount];
        positions = new float[glyphCount * 2 + 2];

        engine->getGlyphs(glyphs, success);
        engine->getCharIndices(indices, success);
        engine->getGlyphPositions(positions, success);

        //fprintf(outputFile, "font: %s\n", testInputs[test].fontName);
        dumpShorts(outputFile, "LEUnicode inputText%d[] =\n{\n", test, (le_uint16 *) testInputs[test].text, testInputs[test].textLength);

        dumpLongs(outputFile, "LEGlyphID resultGlyphs%d[] =\n{\n", test, (le_int32 *) glyphs, glyphCount);
        fprintf(outputFile, "le_int32 resultGlyphCount%d = %d;\n\n", test, glyphCount);

        dumpLongs(outputFile, "le_int32 resultIndices%d[] =\n{\n", test, indices, glyphCount);

        dumpFloats(outputFile, "float resultPositions%d[] =\n{\n", test, positions, glyphCount * 2 + 2);

        fprintf(outputFile, "\n");

        delete[] positions;
        delete[] indices;
        delete[] glyphs;
        delete   engine;
    }

    fprintf(outputFile, "TestInput testInputs[] = \n{\n");

    for (test = 0; test < testCount; test += 1) {
        fprintf(outputFile, "    {\"%s\", fontVersionString%d, fontChecksum%d, inputText%d, %d, %sScriptCode, %s},\n",
            testInputs[test].fontName, test, test, test, testInputs[test].textLength, getShortName(testInputs[test].scriptCode),
            testInputs[test].rightToLeft? "TRUE" : "FALSE");
    }

    fprintf(outputFile, "};\n\nle_int32 testCount = ARRAY_LENGTH(testInputs);\n\n");

    fprintf(outputFile, "TestResult testResults[] = \n{\n");

    for (test = 0; test < testCount; test += 1) {
        fprintf(outputFile, "    {resultGlyphCount%d, resultGlyphs%d, resultIndices%d, resultPositions%d},\n",
            test, test, test, test);
    }

    fprintf(outputFile, "};\n\n");

    fclose(outputFile);
    return 0;
}
