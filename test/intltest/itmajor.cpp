/********************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1998-2001, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

/**
 * MajorTestLevel is the top level test class for everything in the directory "IntlWork".
 */

/***********************************************************************
* Modification history
* Date        Name        Description
* 02/14/2001  synwee      Release collation for testing.
***********************************************************************/

#include "unicode/utypes.h"
#include "itmajor.h"

#include "itutil.h"
#include "tscoll.h"
#include "ittxtbd.h"
#include "itformat.h"
#include "itconv.h"
#include "ittrans.h"
#include "itrbbi.h"
#include "itrbnf.h"
#include "itrbnfrt.h"
#include "normconf.h"
#include "tstnorm.h"
#include "canittst.h"

#define CASE_SUITE(id, suite) case id:                  \
                          name = #suite;                \
                          if(exec) {                    \
                              logln(#suite "---");      \
                              suite test;               \
                              callTest(test, par);      \
                          }                             \
                          break

void MajorTestLevel::runIndexedTest( int32_t index, UBool exec, const char* &name, char* par )
{
    switch (index) {
        case 0: name = "utility"; 
                if (exec) { 
                    logln("TestSuite Utilities---"); logln();
                    IntlTestUtilities test;
                    callTest( test, par );
                }
                break;

        case 1: name = "convert"; 
                if (exec) {
                    logln("TestSuite Convert---"); logln();
                    IntlTestConvert test;
                    callTest( test, par );
                }
                break;

        case 2: name = "normalize"; 
                if (exec) {
                    logln("TestSuite Normalize---"); logln();
                    IntlTestNormalize test;
                    callTest( test, par );
                }
                break;

        case 3: name = "collate"; 
                if (exec) {
                    logln("TestSuite Collator---"); logln();
                    IntlTestCollator test;
                    callTest( test, par );
                }
                break;

        case 4: name = "textbounds"; 
                if (exec) {
                    logln("TestSuite TextBoundary---"); logln();
                    IntlTestTextBoundary test;
                    callTest( test, par );
                }
                break;

        case 5: name = "format"; 
                if (exec) {
                    logln("TestSuite Format---"); logln();
                    IntlTestFormat test;
                    callTest( test, par );
                }
                break;

        case 6: name = "translit"; 
                if (exec) {
                    logln("TestSuite Transliterator---"); logln();
                    IntlTestTransliterator test;
                    callTest( test, par );
                }
                break;

        case 7: name = "rbbi"; 
                if (exec) {
                    logln("TestSuite RuleBasedBreakIterator---"); logln();
                    IntlTestRBBI test;
                    callTest( test, par );
                }
                break;
		case 8: name = "rbnf";
			if (exec) {
					logln("TestSuite RuleBasedNumberFormat----"); logln();
					IntlTestRBNF test;
					callTest(test, par);
			}
			break;
		case 9: name = "rbnfrt";
			if (exec) {
					logln("TestSuite RuleBasedNumberFormat RT----"); logln();
					RbnfRoundTripTest test;
					callTest(test, par);
			}
			break;

        default: name = ""; break;
    }
}

void IntlTestNormalize::runIndexedTest( int32_t index, UBool exec, const char* &name, char* par )
{
    if(exec) logln("TestSuite Normalize:");
    switch (index) {
        CASE_SUITE(0, BasicNormalizerTest);
        CASE_SUITE(1, NormalizerConformanceTest); // this takes a long time
        CASE_SUITE(2, CanonicalIteratorTest);
        default:
            name="";
            break;
    }
}
