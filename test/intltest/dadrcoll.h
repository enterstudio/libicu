/********************************************************************
 * COPYRIGHT: 
 * Copyright (c) 2002, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

/**
 * DataDrivenCollatorTest is a test class that uses data stored in resource
 * bundles to perform testing. For more details on data structure, see
 * source/test/testdata/DataDrivenCollationTest.txt
 */

#ifndef _INTLTESTDATADRIVENCOLLATOR
#define _INTLTESTDATADRIVENCOLLATOR

#include "unicode/utypes.h"

#if !UCONFIG_NO_COLLATION

#include "tscoll.h"
#include "uvector.h"
#include "unicode/coll.h"
#include "unicode/tblcoll.h"
#include "unicode/sortkey.h"
#include "unicode/schriter.h"
#include "unicode/ures.h"
#include "tstdtmod.h"

class SeqElement {
public:
  UnicodeString source;
  UCollationResult relation;
};

class DataDrivenCollatorTest: public IntlTestCollator {
    void runIndexedTest(int32_t index, UBool exec, const char* &name, char* par = NULL );
public:
    DataDrivenCollatorTest();
    virtual ~DataDrivenCollatorTest();
protected:

    void DataDrivenTest(char *par);
    void processSequence(Collator* col, const UnicodeString &sequence);
    void processTest(TestData *testData);
    void processArguments(Collator *col, const UChar *start, int32_t optLen);
    UBool setTestSequence(const UnicodeString &setSequence, SeqElement &el);
    UBool getNextInSequence(SeqElement &el);
private:
  StringCharacterIterator seq;
  TestDataModule *driver;
  UErrorCode status;
  UVector sequences;
};

#endif /* #if !UCONFIG_NO_COLLATION */

#endif
