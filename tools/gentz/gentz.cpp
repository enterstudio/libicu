/*
**********************************************************************
*   Copyright (C) 1999-2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   11/24/99    aliu        Creation.
*   09/26/00    aliu        Support for equivalency groups added.
*   01/31/01    aliu        Support for ISO 3166 country codes added.
**********************************************************************
*/

/* This program reads a text file full of parsed time zone data and
 * outputs a binary file, tz.dat, which then goes on to become part of
 * the memory-mapped (or dll) ICU data file.
 *
 * The data file read by this program is generated by a perl script,
 * tz.pl.  The input to tz.pl is standard unix time zone data from
 * ftp://elsie.nci.nih.gov.
 *
 * As a matter of policy, the perl script tz.pl wants to do as much of
 * the parsing, data processing, and error checking as possible, and
 * this program wants to just do the binary translation step.
 *
 * See tz.pl for the file format that is READ by this program.
 */

#include <stdio.h>
#include <stdlib.h>
#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "cmemory.h"
#include "cstring.h"
#include "filestrm.h"
#include "unewdata.h"
#include "uoptions.h"
#include "tzdat.h"

#define INPUT_FILE "tz.txt"
#define OUTPUT_FILE "tz.dat"

/* UDataInfo cf. udata.h */
static UDataInfo dataInfo = {
    sizeof(UDataInfo),
    0,

    U_IS_BIG_ENDIAN,
    U_CHARSET_FAMILY,
    sizeof(UChar),
    0,

    {TZ_SIG_0, TZ_SIG_1, TZ_SIG_2, TZ_SIG_3},
    {TZ_FORMAT_VERSION, 0, 0, 0},                 /* formatVersion */
    {0, 0, 0, 0} /* dataVersion - will be filled in with year.suffix */
};


class gentz {
    // These must match SimpleTimeZone!!!
    enum { WALL_TIME = 0,
           STANDARD_TIME,
           UTC_TIME
    };

    // The largest number of zones we accept as sensible.  Anything
    // larger is considered an error.  Adjust as needed.
    enum { MAX_ZONES = 1000 };

    // The maximum sensible GMT offset, in seconds
    static const int32_t MAX_GMT_OFFSET;

    static const char COMMENT;
    static const char CR;
    static const char LF;
    static const char MINUS;
    static const char SPACE;
    static const char TAB;
    static const char ZERO;
    static const char STANDARD_MARK;
    static const char DST_MARK;
    static const char SEP;
    static const char NUL;

    static const char* END_KEYWORD;

    enum { BUFLEN = 1024 };
    char buffer[BUFLEN];
    int32_t lineNumber;

    // Binary data that we construct from tz.txt and write out as tz.dat
    TZHeader              header;
    TZEquivalencyGroup*   equivTable;
    OffsetIndex*          offsetIndex;
    CountryIndex*         countryIndex;
    uint32_t*             nameToEquiv;
    char*                 nameTable;

    uint32_t equivTableSize;  // Total bytes in equivalency group table
    uint32_t offsetIndexSize; // Total bytes in offset index table
    uint32_t countryIndexSize; // Total bytes in country index table
    uint32_t nameToEquivSize; // Total bytes in nameToEquiv
    uint32_t nameTableSize;   // Total bytes in name table

    uint32_t maxPerOffset; // Maximum number of zones per offset
    uint32_t maxPerEquiv; // Maximum number of zones per equivalency group
    uint32_t equivCount; // Number of equivalency groups

    UBool useCopyright;
    UBool verbose;


public:
    int      MMain(int argc, char *argv[]);
private:
    int32_t  writeTzDatFile(const char *destdir);
    void     parseTzTextFile(FileStream* in);

    // High level parsing
    void          parseHeader(FileStream* in);

    TZEquivalencyGroup* parseEquivTable(FileStream* in);

    void          fixupNameToEquiv();

    void          parseDSTRule(char*& p, TZRule& rule);

    OffsetIndex*  parseOffsetIndexTable(FileStream* in);

    CountryIndex* parseCountryIndexTable(FileStream* in);

    char*         parseNameTable(FileStream* in);

    // Low level parsing and reading
    void     readEndMarker(FileStream* in);
    int32_t  readIntegerLine(FileStream* in, int32_t min, int32_t max);
    int32_t  _parseInteger(char*& p);
    int32_t  parseInteger(char*& p, char nextExpectedChar, int32_t, int32_t);
    int32_t  readLine(FileStream* in);

    // Error handling
    void    die(const char* msg);
};

int main(int argc, char *argv[]) {
    gentz x;

    U_MAIN_INIT_ARGS(argc, argv);

    return x.MMain(argc, argv);
}

const int32_t gentz::MAX_GMT_OFFSET = (int32_t)24*60*60; // seconds
const char    gentz::COMMENT        = '#';
const char    gentz::CR             = '\r';
const char    gentz::LF             = '\n';
const char    gentz::MINUS          = '-';
const char    gentz::SPACE          = ' ';
const char    gentz::TAB            = '\t';
const char    gentz::ZERO           = '0';
const char    gentz::SEP            = ',';
const char    gentz::STANDARD_MARK  = 's';
const char    gentz::DST_MARK       = 'd';
const char    gentz::NUL            = '\0';
const char*   gentz::END_KEYWORD    = "end";

static UOption options[]={
    UOPTION_HELP_H,
    UOPTION_HELP_QUESTION_MARK,
    UOPTION_COPYRIGHT,
    UOPTION_DESTDIR,
    UOPTION_VERBOSE
};

int gentz::MMain(int argc, char* argv[]) {
    /* preset then read command line options */
    options[3].value=u_getDataDirectory();
    argc=u_parseArgs(argc, argv, sizeof(options)/sizeof(options[0]), options);

    /* error handling, printing usage message */
    if(argc<0) {
        fprintf(stderr,
            "error in command line argument \"%s\"\n",
            argv[-argc]);
    } else if(argc<2) {
        argc=-1;
    }
    if(argc<0 || options[0].doesOccur || options[1].doesOccur) {
        fprintf(stderr,
            "usage: %s [-options] timezone-file\n"
            "\tread the timezone file produced by tz.pl and create " U_ICUDATA_NAME "_" TZ_DATA_NAME "." TZ_DATA_TYPE "\n"
            "options:\n"
            "\t-h or -? or --help  this usage text\n"
            "\t-v or --verbose     turn on verbose output\n"
            "\t-c or --copyright   include a copyright notice\n"
            "\t-d or --destdir     destination directory, followed by the path\n",
            argv[0]);
        return argc<0 ? U_ILLEGAL_ARGUMENT_ERROR : U_ZERO_ERROR;
    }

    /* get the options values */
    useCopyright=options[2].doesOccur;
    verbose = options[4].doesOccur;


    ////////////////////////////////////////////////////////////
    // Read the input file
    ////////////////////////////////////////////////////////////
    *buffer = NUL;
    lineNumber = 0;
    if (verbose) {
        fprintf(stdout, "Input file: %s\n", argv[1]);
    }
    FileStream* in = T_FileStream_open(argv[1], "r");
    if (in == 0) {
        die("Cannot open input file");
    }
    parseTzTextFile(in);
    T_FileStream_close(in);
    *buffer = NUL;

    ////////////////////////////////////////////////////////////
    // Write the output file
    ////////////////////////////////////////////////////////////
    int32_t wlen = writeTzDatFile(options[3].value);
    if (verbose) {
        fprintf(stdout, "Output file: %s.%s, %ld bytes\n",
            U_ICUDATA_NAME "_" TZ_DATA_NAME, TZ_DATA_TYPE, (long)wlen);
    }

    return 0; // success
}

int32_t gentz::writeTzDatFile(const char *destdir) {
    UNewDataMemory *pdata;
    UErrorCode status = U_ZERO_ERROR;

    // Careful: The order in which the tables are written must match the offsets.
    // Our order is:
    // - equiv table
    // - offset index
    // - country index
    // - name index (name to equiv map)
    // - name table (must be last!)
    header.equivTableDelta = sizeof(header);
    header.offsetIndexDelta = header.equivTableDelta + equivTableSize;
    header.countryIndexDelta = header.offsetIndexDelta + offsetIndexSize;
    header.nameIndexDelta = header.countryIndexDelta + countryIndexSize;
    // Must be last:
    header.nameTableDelta = header.nameIndexDelta + nameToEquivSize;

/*  // Don't need to check for negative values on unsigned numbers.
    if (header.equivTableDelta < 0 ||
        header.offsetIndexDelta < 0 ||
        header.countryIndexDelta < 0 ||
        header.nameIndexDelta < 0 ||
        header.nameTableDelta < 0) {
        die("Table too big -- negative delta");
    }
*/

    // Convert equivalency table indices to offsets.  This can only
    // be done after the header offsets have been set up.
    fixupNameToEquiv();

    // Fill in dataInfo with year.suffix
    *(uint16_t*)&(dataInfo.dataVersion[0]) = header.versionYear;
    *(uint16_t*)&(dataInfo.dataVersion[2]) = header.versionSuffix;

    pdata = udata_create(destdir, TZ_DATA_TYPE, U_ICUDATA_NAME "_" TZ_DATA_NAME, &dataInfo,
                         useCopyright ? U_COPYRIGHT_STRING : 0, &status);
    if (U_FAILURE(status)) {
        die("Unable to create data memory");
    }

    udata_writeBlock(pdata, &header, sizeof(header));
    udata_writeBlock(pdata, equivTable, equivTableSize);
    udata_writeBlock(pdata, offsetIndex, offsetIndexSize);
    udata_writeBlock(pdata, countryIndex, countryIndexSize);
    udata_writeBlock(pdata, nameToEquiv, nameToEquivSize);
    udata_writeBlock(pdata, nameTable, nameTableSize);

    uint32_t dataLength = udata_finish(pdata, &status);
    if (U_FAILURE(status)) {
        die("Error writing output file");
    }

    if (dataLength != (sizeof(header) + equivTableSize +
                       offsetIndexSize + countryIndexSize +
                       nameTableSize + nameToEquivSize
                       )) {
        die("Written file doesn't match expected size");
    }
    return dataLength;
}

void gentz::parseTzTextFile(FileStream* in) {
    parseHeader(in);

    // Read name table, create it, also create nameToEquiv index table
    // as a side effect.
    nameTable = parseNameTable(in);

    // Parse the equivalency groups
    equivTable = parseEquivTable(in);

    // Parse the GMT offset index table
    offsetIndex = parseOffsetIndexTable(in);

    // Parse the ISO 3166 country index table
    countryIndex = parseCountryIndexTable(in);
}

/**
 * Convert equivalency table indices to offsets.  The equivalency
 * table offset (in the header) must be set already.
 */
void gentz::fixupNameToEquiv() {
    uint32_t i;

    // First make a list that maps indices to offsets
    uint32_t *offsets = (uint32_t*) uprv_malloc(sizeof(uint32_t) * equivCount);
	/* test for NULL */
	if(offsets == NULL) {
		die("Out of memory");
	}
    offsets[0] = header.equivTableDelta;
    if (offsets[0] % 4 != 0) {
        die("Header size is not 4-aligned");
    }
    TZEquivalencyGroup *eg = equivTable;
    for (i=1; i<equivCount; ++i) {
        offsets[i] = offsets[i-1] + eg->nextEntryDelta;
        if (offsets[i] % 4 != 0) {
            die("Equivalency group table is not 4-aligned");
        }
        eg = (TZEquivalencyGroup*) (eg->nextEntryDelta + (int8_t*)eg);
    }

    // Now remap index values to offsets
    for (i=0; i<header.count; ++i) {
        uint32_t x = nameToEquiv[i];
        if (x >= equivCount) {
            die("Equiv index out of range");
        }
        nameToEquiv[i] = offsets[x];
    }

    uprv_free(offsets);
}

TZEquivalencyGroup* gentz::parseEquivTable(FileStream* in) {
    uint32_t n = readIntegerLine(in, 1, MAX_ZONES);
    if (n != equivCount) {
        die("Equivalency table count mismatch");
    }

    // We don't know how big the whole thing will be yet, but we can use
    // the maxPerEquiv number to compute an upper limit.
    //
    // The gmtOffset field within each struct must be
    // 4-aligned for some architectures.  To ensure this, we do two
    // things: 1. The entire struct is 4-aligned.  2. The gmtOffset is
    // placed at a 4-aligned position within the struct.  3. The size
    // of the whole structure is padded out to 4n bytes.  We achieve
    // this last condition by adding two bytes of padding after the
    // last entry, if necessary.  We adjust
    // the nextEntryDelta and add 2 bytes of padding if necessary.
    uint32_t maxPossibleSize = sizeof(TZEquivalencyGroup) +
        (maxPerEquiv-1) * sizeof(uint16_t);
    // Pad this out
    if ((maxPossibleSize % 4) != 0) {
        maxPossibleSize += 2;
    }
    if ((maxPossibleSize % 4) != 0) {
        die("Bug in 4-align code for equiv table");
    }
    maxPossibleSize *= n; // Get size of entire set of structs.

    int8_t *result = (int8_t*) uprv_malloc(sizeof(int8_t) * maxPossibleSize);
    if (result == 0) {
        die("Out of memory");
    }

    // Read each line and construct the corresponding entry
    TZEquivalencyGroup* eg = (TZEquivalencyGroup*)result;
    for (uint32_t i=0; i<n; ++i) {
        char *p;

        readLine(in);

        // Each line starts with 's,' or 'd,' to specify the zone type
        char flavor = buffer[0];
        if (buffer[1] != SEP) {
            die("Syntax error in equiv table");
        }
        p = buffer + 2;

        // This pointer will be adjusted to point to the start of the
        // list of zones in this group.
        uint16_t* pList = 0;

        switch (flavor) {
        case STANDARD_MARK:
            eg->isDST = 0;
            eg->u.s.zone.gmtOffset = 1000 * // Convert s -> ms
                parseInteger(p, SEP, -MAX_GMT_OFFSET, MAX_GMT_OFFSET);
            pList = &(eg->u.s.count);
            break;
        case DST_MARK:
            eg->isDST = 1;
            eg->u.d.zone.gmtOffset = 1000 * // Convert s -> ms
                parseInteger(p, SEP, -MAX_GMT_OFFSET, MAX_GMT_OFFSET);
            parseDSTRule(p, eg->u.d.zone.onsetRule);
            parseDSTRule(p, eg->u.d.zone.ceaseRule);
            eg->u.d.zone.dstSavings = (uint16_t) parseInteger(p, SEP, 0, 12*60);
            pList = &(eg->u.d.count);
            break;
        default:
            die("Invalid equiv table type marker (not s or d)");
        }

        // Now parse the list of zones in this group
        uint16_t egCount = (uint16_t) parseInteger(p, SEP, 1, maxPerEquiv);
        *pList++ = egCount;
        for (uint16_t j=0; j<egCount; ++j) {
            *pList++ = (uint16_t) parseInteger(p, (j==(egCount-1)) ? NUL : SEP,
                                               0, header.count-1);
        }

        // At this point pList points to the byte after the last byte of this
        // equiv group struct.  Time to 4-align it.
        uint16_t structSize = (uint16_t) (((int8_t*)pList) - ((int8_t*)eg));
        if ((structSize % 4) != 0) {
            // assert(structSize % 4 == 2);
            *pList++ = 0xFFFF; // Pad with invalid zone index
            structSize += 2;
        }

        // Set up next entry delta
        eg->nextEntryDelta = (i==(n-1)) ? (uint16_t) 0 : structSize;

        eg->reserved = 0; // ignored

        eg = (TZEquivalencyGroup*) (structSize + (int8_t*)eg);
    }
    equivTableSize = (int8_t*)eg - (int8_t*)result;
    readEndMarker(in);
    if (verbose) {
        fprintf(stdout, " Read %lu equivalency table entries, in-memory size %ld bytes\n",
            (unsigned long)equivCount, (long)equivTableSize);
    }
    return (TZEquivalencyGroup*)result;
}

OffsetIndex* gentz::parseOffsetIndexTable(FileStream* in) {
    uint32_t n = readIntegerLine(in, 1, MAX_ZONES);

    // We don't know how big the whole thing will be yet, but we can use
    // the maxPerOffset number to compute an upper limit.
    //
    // The gmtOffset field within each OffsetIndex struct must be
    // 4-aligned for some architectures.  To ensure this, we do two
    // things: 1. The entire struct is 4-aligned.  2. The gmtOffset is
    // placed at a 4-aligned position within the struct.  3. The size
    // of the whole structure is padded out to 4n bytes.  We achieve
    // this last condition by adding two bytes of padding after the
    // last zoneNumber, if count is _even_.  That is, the struct size
    // is 10+2count+padding, where padding is (count%2==0 ? 2:0).
    //
    // Note that we don't change the count itself, but rather adjust
    // the nextEntryDelta and add 2 bytes of padding if necessary.
    //
    // Don't try to compute the exact size in advance
    // (unless we want to avoid the use of sizeof(), which may
    // introduce padding that we won't actually employ).
    uint32_t maxPossibleSize = n * (sizeof(OffsetIndex) +
        (maxPerOffset-1) * sizeof(uint16_t));

    int8_t *result = (int8_t*) uprv_malloc(sizeof(int8_t) * maxPossibleSize);
    if (result == 0) {
        die("Out of memory");
    }

    // Read each line and construct the corresponding entry
    OffsetIndex* index = (OffsetIndex*)result;
    for (uint32_t i=0; i<n; ++i) {
        uint16_t alignedCount;
        readLine(in);
        char* p = buffer;
        index->gmtOffset = 1000 * // Convert s -> ms
            parseInteger(p, SEP, -MAX_GMT_OFFSET, MAX_GMT_OFFSET);
        index->defaultZone = (uint16_t)parseInteger(p, SEP, 0, header.count-1);
        index->count = (uint16_t)parseInteger(p, SEP, 1, maxPerOffset);
        uint16_t* zoneNumberArray = &(index->zoneNumber);
        UBool sawOffset = FALSE; // Sanity check - make sure offset is in zone list
        for (uint16_t j=0; j<index->count; ++j) {
            zoneNumberArray[j] = (uint16_t)
                parseInteger(p, (j==(index->count-1))?NUL:SEP,
                             0, header.count-1);
            if (zoneNumberArray[j] == index->defaultZone) {
                sawOffset = TRUE;
            }
        }
        if (!sawOffset) {
            die("Error: bad offset index entry; default not in zone list");
        }
        alignedCount = index->count;
        if((alignedCount%2)==0) /* force count to be ODD - see above */
        {
            // Use invalid zoneNumber for 2 bytes of padding
            zoneNumberArray[alignedCount++] = (uint16_t)0xFFFF;
        }
        int8_t* nextIndex = (int8_t*)&(zoneNumberArray[alignedCount]);

        index->nextEntryDelta = (uint16_t) ((i==(n-1)) ? 0 : (nextIndex - (int8_t*)index));
        index = (OffsetIndex*)nextIndex;
    }
    offsetIndexSize = (int8_t*)index - (int8_t*)result;
    if (offsetIndexSize > maxPossibleSize) {
        die("Yikes! Interal error while constructing offset index table");
    }
    readEndMarker(in);
    if (verbose) {
        fprintf(stdout, " Read %lu offset index table entries, in-memory size %ld bytes\n",
            (unsigned long)n, (long)offsetIndexSize);
    }
    return (OffsetIndex*)result;
}

CountryIndex* gentz::parseCountryIndexTable(FileStream* in) {
    uint32_t n = readIntegerLine(in, 1, MAX_ZONES);

    // We know how big the whole thing will be: Each zone occupies an
    // int, and each country adds 3 ints (one for the intcode, one for
    // next entry offset, one for the zone count).  Each int is 16
    // bits.
    //
    // Everything is 16-bits, so we don't 4-align the entries.
    // However, we do pad at the end of the table to make the whole
    // thing of size 4n, if necessary.
    uint32_t expectedSize = n*(sizeof(CountryIndex)-sizeof(uint16_t)) +
        header.count * sizeof(uint16_t);
    uint32_t pad = (4 - (expectedSize % 4)) % 4; // This will be 0 or 2
    int8_t *result = (int8_t*) uprv_malloc(sizeof(int8_t) * (expectedSize + pad));
    if (result == 0) {
        die("Out of memory");
    }

    // Read each line and construct the corresponding entry.
    // Along the way, make sure we don't write past 'limit'.
    CountryIndex* index = (CountryIndex*)result;
    int8_t* limit = ((int8_t*)result) + expectedSize; // Don't include pad
    uint32_t i;
    for (i=0; i<n && (int8_t*)(&index->zoneNumber) < limit; ++i) {
        readLine(in);
        char* p = buffer;
        index->intcode = (uint16_t)parseInteger(p, SEP, 0, 25*32+25 /*ZZ*/);
        index->count = (uint16_t)parseInteger(p, SEP, 0, header.count-1);
        uint16_t* zoneNumberArray = &(index->zoneNumber);
        if ((int8_t*)(&index->zoneNumber + index->count - 1) >= limit) {
            // Oops -- out of space
            break;
        }
        for (uint16_t j=0; j<index->count; ++j) {
            zoneNumberArray[j] = (uint16_t)
                parseInteger(p, (j==(index->count-1))?NUL:SEP,
                             0, header.count-1);
        }
        int8_t* nextIndex = (int8_t*)&(zoneNumberArray[index->count]);
        index->nextEntryDelta = (uint16_t) ((i==(n-1)) ? 0 : (nextIndex - (int8_t*)index));
        index = (CountryIndex*)nextIndex;
    }
    readEndMarker(in);

    // Make sure size matches expected value, and pad the total size
    countryIndexSize = (int8_t*)index - (int8_t*)result + pad;
    if (i != n || countryIndexSize != expectedSize) {
        die("Yikes! Interal error while constructing offset index table");
    }
    if (pad != 0) {
        countryIndexSize += pad;
        *(uint16_t*)index = 0; // Clear pad bits
    }
    if (verbose) {
        fprintf(stdout, " Read %lu country index table entries, in-memory size %ld bytes\n", (unsigned long)n, (long)countryIndexSize);
    }
    return (CountryIndex*)result;
}

void gentz::parseHeader(FileStream* in) {

    int32_t version = readIntegerLine(in, 0, 0xFFFF);
    if (version != TZ_FORMAT_VERSION) {
        die("Version mismatch between gentz and input file");
    }

    // Version string, e.g., "1999j" -> (1999<<16) | 10
    header.versionYear = (uint16_t) readIntegerLine(in, 1990, 0xFFFF);
    header.versionSuffix = (uint16_t) readIntegerLine(in, 0, 0xFFFF);

    header.count = readIntegerLine(in, 1, MAX_ZONES);
    equivCount = readIntegerLine(in, 1, header.count);
    maxPerOffset = readIntegerLine(in, 1, header.count);
    maxPerEquiv = readIntegerLine(in, 1, equivCount);

    // Size of name table in bytes
    // (0x00FFFFFF is an arbitrary upper limit; adjust as needed.)
    nameTableSize = readIntegerLine(in, 1, 0x00FFFFFF);

    readEndMarker(in);

    if (verbose) {
        fprintf(stdout, " Read header, data version %u(%u), in-memory size %ld bytes\n",
            header.versionYear, header.versionSuffix,
            (unsigned long)sizeof(header));
    }
}

void gentz::parseDSTRule(char*& p, TZRule& rule) {
    rule.month = (uint8_t) parseInteger(p, SEP, 0, 11);
    rule.dowim = (int8_t) parseInteger(p, SEP, -31, 31);
    rule.dow = (int8_t) parseInteger(p, SEP, -7, 7);
    rule.time = (uint16_t) parseInteger(p, SEP, 0, 24*60);
    rule.mode = *p++;
    if (*p++ != SEP) {
        die("Separator missing");
    }
    switch ((char)rule.mode) {
    case 'w':
        rule.mode = WALL_TIME;
        break;
    case 's':
        rule.mode = STANDARD_TIME;
        break;
    case 'u':
        rule.mode = UTC_TIME;
        break;
    default:
        die("Invalid rule time mode");
        break;
    }
}

/**
 * Parse the name table.
 * Each entry of the name table looks like this:
 * |36,Africa/Djibouti
 * The integer is an equivalency table index.  We build up a name
 * table, that just contains the names, and we return it.  We also
 * build up the name index, which indexes names to equivalency table
 * entries.  This is stored in the member variable nameToEquiv.
 */
char* gentz::parseNameTable(FileStream* in) {
    int32_t n = readIntegerLine(in, 1, MAX_ZONES);
    if (n != (int32_t)header.count) {
        die("Zone count doesn't match name table count");
    }
    char* names = (char*) uprv_malloc(sizeof(char) * nameTableSize);
    nameToEquiv = (uint32_t*) uprv_malloc(sizeof(uint32_t) * n);
    if (names == 0 || nameToEquiv == 0) {
        die("Out of memory");
    }
    nameToEquivSize = n * sizeof(nameToEquiv[0]);
    char* p = names;
    char* limit = names + nameTableSize;
    for (int32_t i=0; i<n; ++i) {
        readLine(in);
        char* q = buffer;
        // We store an index here for now -- later, in fixNameToEquiv,
        // we convert it to an offset.
        nameToEquiv[i] = (uint32_t) parseInteger(q, SEP, 0, equivCount-1);
        int32_t len = uprv_strlen(q);
        if ((p + len) <= limit) {
            uprv_memcpy(p, q, len);
            p += len;
            *p++ = NUL;
        } else {
            die("Name table longer than declared size");
        }
    }
    if (p != limit) {
        die("Name table shorter than declared size");
    }
    readEndMarker(in);
    if (verbose) {
        fprintf(stdout, " Read %ld names, in-memory size %ld bytes\n",
        (long)n, (long)nameTableSize);
    }
    return names;
}

/**
 * Read the end marker (terminates each list).
 */
void gentz::readEndMarker(FileStream* in) {
    readLine(in);
    if (uprv_strcmp(buffer, END_KEYWORD) != 0) {
        die("Keyword 'end' missing");
    }
}

/**
 * Read a line from the FileStream and parse it as an
 * integer.  There should be nothing else on the line.
 */
int32_t gentz::readIntegerLine(FileStream* in, int32_t min, int32_t max) {
    readLine(in);
    char* p = buffer;
    return parseInteger(p, NUL, min, max);
}

/**
 * Parse an integer from the given character buffer.
 * Advance p past the last parsed character.  Return
 * the result.  The integer must be of the form
 * /-?\d+/.
 */
int32_t gentz::_parseInteger(char*& p) {
    int32_t n = 0;
    int32_t digitCount = 0;
    int32_t digit;
    UBool negative = FALSE;
    if (*p == MINUS) {
        ++p;
        negative = TRUE;
    }
    for (;;) {
        digit = *p - ZERO;
        if (digit < 0 || digit > 9) {
            break;
        }
        n = 10*n + digit;
        p++;
        digitCount++;
    }
    if (digitCount < 1) {
        die("Unable to parse integer");
    }
    if (negative) {
        n = -n;
    }
    return n;
}

int32_t gentz::parseInteger(char*& p, char nextExpectedChar,
                            int32_t min, int32_t max) {
    int32_t n = _parseInteger(p);
    if (*p++ != nextExpectedChar) {
        die("Character following integer unexpected");
    }
    if (n < min || n > max) {
        die("Integer field out of range");
    }
    return n;
}

void gentz::die(const char* msg) {
    fprintf(stderr, "ERROR, %s\n", msg);
    if (*buffer) {
        fprintf(stderr, "Input file line %ld: \"%s\"\n", (long)lineNumber, buffer);
    }
    exit(1);
}

/**
 * Read a line.  Trim trailing comment and whitespace.  Ignore (skip)
 * blank lines, or comment-only lines.  Return the number of characters
 * on the line remaining.  On EOF, die.
 */
int32_t gentz::readLine(FileStream* in) {
    ++lineNumber;
    char* result = T_FileStream_readLine(in, buffer, BUFLEN);
    if (result == 0) {
        *buffer = 0;
        die("Unexpected end of file");
    }
    // Trim off trailing comment
    char* p = uprv_strchr(buffer, COMMENT);
    if (p != 0) {
        *p = NUL;
    }
    // Delete trailing whitespace
    p = buffer + uprv_strlen(buffer);
    while (p > buffer && (p[-1] == CR || p[-1] == LF ||
                          p[-1] == SPACE || p[-1] == TAB)) {
        p--;
    }
    *p = NUL;
    // If line is empty after trimming comments & whitespace,
    // then read the next line.
    return (*buffer == NUL) ? readLine(in) : uprv_strlen(buffer);
}
