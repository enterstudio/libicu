/*
**********************************************************************
* Copyright (c) 2002, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* $Source: /xsrl/Nsvn/icu/icu/source/i18n/unicode/Attic/currency.h,v $ 
* $Revision: 1.2 $
**********************************************************************
*/
#ifndef UCURRENCY_H
#define UCURRENCY_H

#include "unicode/unistr.h"

U_NAMESPACE_BEGIN

class Locale;

/**
 * A class encapsulating information about a currency, as defined by
 * ISO 4217.  A currency is represented by a 3-character string
 * containing its ISO 4217 code.  This class can return various data
 * necessary the proper display of a currency:
 *
 * <ul><li>A display symbol, for a specific locale
 * <li>The number of fraction digits to display
 * <li>A rounding increment
 * </ul>
 *
 * The <tt>DecimalFormat</tt> class uses these data to display
 * currencies.
 * @author Alan Liu
 * @since ICU 2.2
 */
class U_I18N_API UCurrency {

    // TODO:? Make the string a char* instead of a UnicodeString ?

 public:

    /**
     * Returns a currency object for the default currency in the given
     * locale.
     */
    static UnicodeString forLocale(const Locale& locale,
                                   UErrorCode& ec);

    /**
     * Returns the display string for this currency object in the
     * given locale.  For example, the display string for the USD
     * currency object in the en_US locale is "$".
     */
    static UnicodeString getSymbol(const UnicodeString& currency,
                                   const Locale& locale);

    /**
     * Returns the number of the number of fraction digits that should
     * be displayed for this currency.
     * @return a non-negative number of fraction digits to be
     * displayed
     */
    static int32_t getDefaultFractionDigits(const UnicodeString& currency);

    /**
     * Returns the rounding increment for this currency, or 0.0 if no
     * rounding is done by this currency.
     * @return the non-negative rounding increment, or 0.0 if none
     */
    static double getRoundingIncrement(const UnicodeString& currency);
};

U_NAMESPACE_END

#endif
