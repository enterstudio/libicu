/*
* Copyright (C) {1997-2001}, International Business Machines Corporation and others. All Rights Reserved.
********************************************************************************
*
* File MSGFMT.H
*
* Modification History:
*
*   Date        Name        Description
*   02/19/97    aliu        Converted from java.
*   03/20/97    helena      Finished first cut of implementation.
*   07/22/98    stephen     Removed operator!= (defined in Format)
*   08/19/2002  srl         Removing Javaisms
********************************************************************************
*/
// *****************************************************************************
// This file was generated from the java source file MessageFormat.java
// *****************************************************************************

#ifndef MSGFMT_H
#define MSGFMT_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/format.h"
#include "unicode/locid.h"
#include "unicode/parseerr.h"

U_NAMESPACE_BEGIN

class NumberFormat;

/**
 * Provides means to produce concatenated messages in language-neutral way.
 * Use this for all concatenations that show up to end users.
 * <P>
 * Takes a set of objects, formats them, then inserts the formatted
 * strings into the pattern at the appropriate places.
 * <P>
 * Here are some examples of usage:
 * Example 1:
 * <pre>
 * \code
 *     UErrorCode success = U_ZERO_ERROR;
 *     GregorianCalendar cal(success);
 *     Formattable arguments[] = {
 *         7L,
 *         Formattable( (Date) cal.getTime(success), Formattable::kIsDate),
 *         "a disturbance in the Force"
 *     };
 *
 *     UnicodeString result;
 *     MessageFormat::format(
 *          "At {1,time} on {1,date}, there was {2} on planet {0,number}.",
 *          arguments, 3, result, success );
 *
 *     cout << "result: " << result << endl;
 *     //<output>: At 4:34:20 PM on 23-Mar-98, there was a disturbance
 *     //             in the Force on planet 7.
 * \endcode
 * </pre>
 * Typically, the message format will come from resources, and the
 * arguments will be dynamically set at runtime.
 * <P>
 * Example 2:
 * <pre>
 *  \code
 *     success = U_ZERO_ERROR;
 *     Formattable testArgs[] = {3L, "MyDisk"};
 *
 *     MessageFormat* form = new MessageFormat(
 *         "The disk \"{1}\" contains {0} file(s).", success );
 *
 *     UnicodeString string;
 *     FieldPosition fpos = 0;
 *     cout &lt;&lt; "format: " &lt;&lt; form->format(testArgs, 2, string, fpos, success ) &lt;&lt; endl;
 *
 *     // output, with different testArgs:
 *     // output: The disk "MyDisk" contains 0 file(s).
 *     // output: The disk "MyDisk" contains 1 file(s).
 *     // output: The disk "MyDisk" contains 1,273 file(s).
 *     delete form;
 *  \endcode
 *  </pre>
 *
 *  The pattern is of the following form.  Legend:
 *  <pre>
 * \code
 *       {optional item}
 *       (group that may be repeated)*
 * \endcode
 *  </pre>
 *  Do not confuse optional items with items inside quotes braces, such
 *  as this: "{".  Quoted braces are literals.
 *  <pre>
 *  \code
 *       messageFormatPattern := string ( "{" messageFormatElement "}" string )*
 *
 *       messageFormatElement := argument { "," elementFormat }
 *
 *       elementFormat := "time" { "," datetimeStyle }
 *                      | "date" { "," datetimeStyle }
 *                      | "number" { "," numberStyle }
 *                      | "choice" "," choiceStyle
 *
 *       datetimeStyle := "short"
 *                      | "medium"
 *                      | "long"
 *                      | "full"
 *                      | dateFormatPattern
 *
 *       numberStyle :=   "currency"
 *                      | "percent"
 *                      | "integer"
 *                      | numberFormatPattern
 *
 *       choiceStyle :=   choiceFormatPattern
 * \endcode
 * </pre>
 * If there is no elementFormat, then the argument must be a string,
 * which is substituted. If there is no dateTimeStyle or numberStyle,
 * then the default format is used (e.g.  NumberFormat::createInstance(),
 * DateFormat::createTimeInstance(DateFormat::kDefault, ...) or DateFormat::createDateInstance(DateFormat::kDefault, ...). For
 * a ChoiceFormat, the pattern must always be specified, since there
 * is no default.
 * <P>
 * In strings, single quotes can be used to quote the "{" sign if
 * necessary. A real single quote is represented by ''.  Inside a
 * messageFormatElement, quotes are [not] removed. For example,
 * {1,number,$'#',##} will produce a number format with the pound-sign
 * quoted, with a result such as: "$#31,45".
 * <P>
 * If a pattern is used, then unquoted braces in the pattern, if any,
 * must match: that is, "ab {0} de" and "ab '}' de" are ok, but "ab
 * {0'}' de" and "ab } de" are not.
 * <P>
 * The argument is a number from 0 to 9, which corresponds to the
 * arguments presented in an array to be formatted.
 * <P>
 * It is ok to have unused arguments in the array.  With missing
 * arguments or arguments that are not of the right class for the
 * specified format, a failing UErrorCode result is set.
 * <P>
 * For more sophisticated patterns, you can use a ChoiceFormat to get
 * output such as:
 * <pre>
 * \code
 *     UErrorCode success = U_ZERO_ERROR;
 *     MessageFormat* form = new MessageFormat("The disk \"{1}\" contains {0}.", success);
 *     double filelimits[] = {0,1,2};
 *     UnicodeString filepart[] = {"no files","one file","{0,number} files"};
 *     ChoiceFormat* fileform = new ChoiceFormat(filelimits, filepart, 3);
 *     form->setFormat(1, *fileform); // NOT zero, see below
 *
 *     Formattable testArgs[] = {1273L, "MyDisk"};
 *
 *     UnicodeString string;
 *     FieldPosition fpos = 0;
 *     cout << form->format(testArgs, 2, string, fpos, success) << endl;
 *
 *     // output, with different testArgs
 *     // output: The disk "MyDisk" contains no files.
 *     // output: The disk "MyDisk" contains one file.
 *     // output: The disk "MyDisk" contains 1,273 files.
 * \endcode
 * </pre>
 * You can either do this programmatically, as in the above example,
 * or by using a pattern (see ChoiceFormat for more information) as in:
 * <pre>
 * \code
 *    form->applyPattern(
 *      "There {0,choice,0#are no files|1#is one file|1<are {0,number,integer} files}.");
 * \endcode
 * </pre>
 * <P>
 * [Note:] As we see above, the string produced by a ChoiceFormat in
 * MessageFormat is treated specially; occurances of '{' are used to
 * indicated subformats, and cause recursion.  If you create both a
 * MessageFormat and ChoiceFormat programmatically (instead of using
 * the string patterns), then be careful not to produce a format that
 * recurses on itself, which will cause an infinite loop.
 * <P>
 * [Note:] Formats are numbered by order of variable in the string.
 * This is [not] the same as the argument numbering!
 * <pre>
 * \code
 *    For example: with "abc{2}def{3}ghi{0}...",
 *
 *    format0 affects the first variable {2}
 *    format1 affects the second variable {3}
 *    format2 affects the second variable {0}
 * \endcode
 * </pre>
 * and so on.
 */
class U_I18N_API MessageFormat : public Format {
public:
    /**
     * Enum type for kMaxFormat.
     * @obsolete ICU 3.0.  The 10-argument limit was removed as of ICU 2.6,
     * rendering this enum type obsolete.
     */
    enum EFormatNumber {
        /**
         * The maximum number of arguments.
         * @obsolete ICU 3.0.  The 10-argument limit was removed as of ICU 2.6,
         * rendering this constant obsolete.
         */
        kMaxFormat = 10
    };

    /**
     * Construct a new MessageFormat using the given pattern.
     *
     * @param pattern   Pattern used to construct object.
     * @param status    Output param to receive success code.  If the
     *                  pattern cannot be parsed, set to failure code.
     * @stable ICU 2.0
     */
    MessageFormat(const UnicodeString& pattern,
                  UErrorCode &status);

    /**
     * Constructor that allows locale specification.
     * @param pattern   Pattern used to construct object.
     * @param newLocale The locale to use for formatting dates and numbers.
     * @param status    Output param to receive success code.  If the
     *                  pattern cannot be parsed, set to failure code.
     * @stable ICU 2.0
     */
    MessageFormat(const UnicodeString& pattern,
                  const Locale& newLocale,
                        UErrorCode& success);
    /**
     * Constructor that allows locale specification.
     * @param pattern   Pattern used to construct object.
     * @param newLocale The locale to use for formatting dates and numbers.
     * @param parseError Struct to recieve information on position 
     *                   of error if an error is encountered
     * @param success    Output param to receive success code.  If the
     *                  pattern cannot be parsed, set to failure code.
     * @stable ICU 2.0
     */
    MessageFormat(const UnicodeString& pattern,
                  const Locale& newLocale,
                  UParseError& parseError,
                  UErrorCode& success);
    /**
     * Copy constructor.
     * @stable ICU 2.0
     */
    MessageFormat(const MessageFormat&);

    /**
     * Assignment operator.
     * @stable ICU 2.0
     */
    const MessageFormat& operator=(const MessageFormat&);

    /**
     * Destructor.
     * @stable ICU 2.0
     */
    virtual ~MessageFormat();

    /**
     * Clone this Format object polymorphically. The caller owns the
     * result and should delete it when done.
     * @stable ICU 2.0
     */
    virtual Format* clone(void) const;

    /**
     * Return true if the given Format objects are semantically equal.
     * Objects of different subclasses are considered unequal.
     * @param other  the object to be compared with.
     * @return       true if the given Format objects are semantically equal.
     * @stable ICU 2.0
     */
    virtual UBool operator==(const Format& other) const;

    /**
     * Sets the locale. This locale is used for fetching default number or date
     * format information.
     * @param theLocale    the new locale value to be set.
     * @stable ICU 2.0
     */
    virtual void setLocale(const Locale& theLocale);

    /**
     * Gets the locale. This locale is used for fetching default number or date
     * format information.
     * @return    the locale of the object.
     * @stable ICU 2.0
     */
    virtual const Locale& getLocale(void) const;

    /**
     * Apply the given pattern string to this message format.
     *
     * @param pattern   The pattern to be applied.
     * @param status    Output param set to success/failure code on
     *                  exit. If the pattern is invalid, this will be
     *                  set to a failure result.
     * @stable ICU 2.0
     */
    virtual void applyPattern(const UnicodeString& pattern,
                              UErrorCode& status);
    /**
     * Sets the pattern.
     * @param pattern    The pattern to be applied.
     * @param parseError Struct to recieve information on position 
     *                   of error if an error is encountered
     * @param status     Output param set to success/failure code on
     *                   exit. If the pattern is invalid, this will be
     *                   set to a failure result.
     * @draft ICU 2.0
     */
    virtual void applyPattern(const UnicodeString& pattern,
                             UParseError& parseError,
                             UErrorCode& status);

    /**
     * Gets the pattern. See the class description.
     * @param appendTo  Output parameter to receive the pattern.
     *                  Result is appended to existing contents.
     * @return          Reference to 'appendTo' parameter.
     * @stable ICU 2.0
     */
    virtual UnicodeString& toPattern(UnicodeString& appendTo) const;

    /**
     * Sets formats to use on parameters.
     * See the class description about format numbering.
     * The caller should not delete the Format objects after this call.
     * Note that the array formatsToAdopt is not itself adopted. Its
     * ownership is retained by the caller. If the call fails because
     * memory cannot be allocated, then the formats will be deleted
     * by this method, and this object will remain unchanged.
     * 
     * @stable ICU 2.0
     * @param formatsToAdopt    the format to be adopted.
     * @param count             the size of the array.
     */
    virtual void adoptFormats(Format** formatsToAdopt, int32_t count);

    /**
     * Sets formats to use on parameters.
     * See the class description about format numbering.
     * Each item in the array is cloned into the internal array.
     * If the call fails because memory cannot be allocated, then this
     * object will remain unchanged.
     * 
     * @stable ICU 2.0
     * @param newFormats the new format to be set.
     * @param cnt        the size of the array.
     */
    virtual void setFormats(const Format** newFormats,int32_t cnt);


    /**
     * Sets formats individually to use on parameters.
     * See the class description about format numbering.
     * The caller should not delete the Format object after this call.
     * If the number is over the number of formats already set,
     * the item will be deleted and ignored.
     * @stable ICU 2.0
     * @param formatNumber     index of the parameter.
     * @param formatToAdopt    the format to be adopted.
     */
    virtual void adoptFormat(int32_t formatNumber, Format* formatToAdopt);

    /**
     * Sets formats individually to use on parameters.
     * See the class description about format numbering.
     * If the number is over the number of formats already set,
     * the item will be ignored.
     * @param variable         index of the parameter.
     * @param newFormat    the format to be set.
     * @stable ICU 2.0
     */
    virtual void setFormat(int32_t variable, const Format& newFormat);

    /**
     * Gets formats that were set with setFormats.
     * See the class description about format numbering.
     * @stable ICU 2.0
     * @param count    the size of the array.
     */
    virtual const Format** getFormats(int32_t& count) const;

    /**
     * Returns pattern with formatted objects.  Does not take ownership
     * of the Formattable* array; just reads it and uses it to generate
     * the format string.
     *
     * @param source    An array of objects to be formatted & substituted.
     * @param count     the size of the array.
     * @param appendTo  Output parameter to receive result.
     *                  Result is appended to existing contents.
     * @param ignore    No useful status is returned.
     * @param success   Output param set to success/failure code
     * @return          Reference to 'appendTo' parameter.
     * @stable ICU 2.0
     */
    UnicodeString& format(  const Formattable* source,
                            int32_t count,
                            UnicodeString& appendTo,
                            FieldPosition& ignore,
                            UErrorCode& success) const;

    /**
     * Convenience routine.  Avoids explicit creation of
     * MessageFormat, but doesn't allow future optimizations.
     * @param pattern   the pattern.
     * @param source    An array of objects to be formatted & substituted.
     * @param count     the size of the array.
     * @param appendTo  Output parameter to receive result.
     *                  Result is appended to existing contents.
     * @param success   Output param set to success/failure code
     * @return          Reference to 'appendTo' parameter.
     * @stable ICU 2.0
     */
    static UnicodeString& format(   const UnicodeString& pattern,
                                    const Formattable* arguments,
                                    int32_t count,
                                    UnicodeString& appendTo,
                                    UErrorCode& success);

    /**
     * Format an object to produce a message.  This method handles
     * Formattable objects of type kArray. If the Formattable
     * object type is not of type kArray, then it returns a failing
     * UErrorCode.
     *
     * @param obj       The object to format
     * @param appendTo  Output parameter to receive result.
     *                  Result is appended to existing contents.
     * @param pos       On input: an alignment field, if desired.
     *                  On output: the offsets of the alignment field.
     * @param status    Output param filled with success/failure status.
     * @return          Reference to 'appendTo' parameter.
     * @stable ICU 2.0
     */
    virtual UnicodeString& format(const Formattable& obj,
                                  UnicodeString& appendTo,
                                  FieldPosition& pos,
                                  UErrorCode& status) const;

    /**
     * Redeclared Format method.
     * @param obj       The object to format
     * @param appendTo  Output parameter to receive result.
     *                  Result is appended to existing contents.
     * @param status    Output param filled with success/failure status.
     * @return          Reference to 'appendTo' parameter.
     * @stable ICU 2.0
     */
    UnicodeString& format(const Formattable& obj,
                          UnicodeString& appendTo,
                          UErrorCode& status) const;

    /**
     * Parses the string.
     * <P>
     * Caveats: The parse may fail in a number of circumstances.  For
     * example:
     * <P>
     * If one of the arguments does not occur in the pattern.
     * <P>
     * If the format of an argument is loses information, such as with
     * a choice format where a large number formats to "many".
     * <P>
     * Does not yet handle recursion (where the substituted strings
     * contain {n} references.)
     * <P>
     * Will not always find a match (or the correct match) if some
     * part of the parse is ambiguous.  For example, if the pattern
     * "{1},{2}" is used with the string arguments {"a,b", "c"}, it
     * will format as "a,b,c".  When the result is parsed, it will
     * return {"a", "b,c"}.
     * <P>
     * If a single argument is formatted twice in the string, then the
     * later parse wins.
     *
     * @param source    String to be parsed.
     * @param status    On input, starting position for parse. On output,
     *                  final position after parse.
     * @stable ICU 2.0
     */
    virtual Formattable* parse( const UnicodeString& source,
                                ParsePosition& status,
                                int32_t& count) const;

    /**
     * Parses the string. Does not yet handle recursion (where
     * the substituted strings contain {n} references.)
     *
     * @param source    String to be parsed.
     * @param count     Output param to receive size of returned array.
     * @param status    Output param to receive success/error code.
     * @stable ICU 2.0
     */
    virtual Formattable* parse( const UnicodeString& source,
                                int32_t& count,
                                UErrorCode& status) const;

    /**
     * Parse a string to produce an object.  This methods handles
     * parsing of message strings into arrays of Formattable objects.
     * Does not yet handle recursion (where the substituted strings
     * contain %n references.)
     * <P>
     * Before calling, set parse_pos.index to the offset you want to
     * start parsing at in the source. After calling, parse_pos.index
     * is the end of the text you parsed.  If error occurs, index is
     * unchanged.
     * <P>
     * When parsing, leading whitespace is discarded (with successful
     * parse), while trailing whitespace is left as is.
     * <P>
     * See Format::parseObject() for more.
     *
     * @param source    The string to be parsed into an object.
     * @param result    Formattable to be set to the parse result.
     *                  If parse fails, return contents are undefined.
     * @param parse_pos The position to start parsing at. Upon return
     *                  this param is set to the position after the
     *                  last character successfully parsed. If the
     *                  source is not parsed successfully, this param
     *                  will remain unchanged.
     * @return          A newly created Formattable* object, or NULL
     *                  on failure.  The caller owns this and should
     *                  delete it when done.
     * @stable ICU 2.0
     */
    virtual void parseObject(const UnicodeString& source,
                             Formattable& result,
                             ParsePosition& parse_pos) const;

public:
    /**
     * Returns a unique class ID POLYMORPHICALLY.  Pure virtual override.
     * This method is to implement a simple version of RTTI, since not all
     * C++ compilers support genuine RTTI.  Polymorphic operator==() and
     * clone() methods call this method.
     *
     * @return          The class ID for this object. All objects of a
     *                  given class have the same class ID.  Objects of
     *                  other classes have different class IDs.
     * @stable ICU 2.0
     */
    virtual UClassID getDynamicClassID(void) const;

    /**
     * Return the class ID for this class.  This is useful only for
     * comparing to a return value from getDynamicClassID().  For example:
     * <pre>
     * .   Base* polymorphic_pointer = createPolymorphicObject();
     * .   if (polymorphic_pointer->getDynamicClassID() ==
     * .      Derived::getStaticClassID()) ...
     * </pre>
     * @return          The class ID for all objects of this class.
     * @stable ICU 2.0
     */
    static UClassID getStaticClassID(void) { return (UClassID)&fgClassID; }
    
    /**
     * Returns array of formattable types in the parsed pattern 
     * for use in C API
     * @param listCount  Output parameter to receive the size of array
     * @return           The array of formattable types in the pattern
     * @internal
     */
    const Formattable::Type* getFormatTypeList(int32_t& listCount){
        argTypeCount;
        return argTypes; 
    }

private:
    static const char fgClassID;

    Locale              fLocale;
    UnicodeString       fPattern;
    Format              *fFormats[kMaxFormat];

    /**
     * A structure representing one subformat of this MessageFormat.
     * Each subformat has a Format object, an offset into the plain
     * pattern text fPattern, and an argument number.  The argument
     * number corresponds to the array of arguments to be formatted.
     */
    class Subformat {
    public:
        Format* format; // formatter
        int32_t offset; // offset into fPattern
        int32_t arg;    // 0-based argument number

        // Clone that.format and assign it to this.format
        // Do NOT delete this.format
        Subformat& operator=(const Subformat& that) {
            format = that.format ? that.format->clone() : NULL;
            offset = that.offset;
            arg = that.arg;
            return *this;
        }

        UBool operator==(const Subformat& that) const {
            // Do cheap comparisons first
            return offset == that.offset &&
                   arg == that.arg &&
                   ((format == that.format) || // handles NULL
                    (*format == *that.format));
        }

        UBool operator!=(const Subformat& that) const {
            return !operator==(that);
        }
    };

    /**
     * A MessageFormat contains an array of subformats.  This array
     * needs to grow dynamically if the MessageFormat is modified.
     */
    Subformat* subformats;
    int32_t    subformatCount;
    int32_t    subformatCapacity;

    /**
     * A MessageFormat formats an array of arguments.  Each argument
     * has an expected type, based on the pattern.  For example, if
     * the pattern contains the subformat "{3,number,integer}", then
     * we expect argument 3 to have type Formattable::kLong.  This
     * array needs to grow dynamically if the MessageFormat is
     * modified.
     */
    Formattable::Type* argTypes;
    int32_t            argTypeCount;
    int32_t            argTypeCapacity;

    // Variable-size array management
    UBool allocateSubformats(int32_t capacity);
    UBool allocateArgTypes(int32_t capacity);

    /**
     * Finds the word s, in the keyword list and returns the located index.
     * @param s the keyword to be searched for.
     * @param list the list of keywords to be searched with.
     * @return the index of the list which matches the keyword s.
     */
    static int32_t findKeyword( const UnicodeString& s,
                                const UChar * const *list);

    /**
     * Formats the array of arguments and copies the result into the
     * result buffer, updates the field position.
     *
     * @param arguments The formattable objects array.
     * @param cnt       The array count.
     * @param appendTo  Output parameter to receive result.
     *                  Result is appended to existing contents.
     * @param status    Field position status.
     * @param recursionProtection
     *                  Initially zero. Bits 0..9 are used to indicate
     *                  that a parameter has already been seen, to
     *                  avoid recursion.  Currently unused.
     * @param success   The error code status.
     * @return          Reference to 'appendTo' parameter.
     */
    UnicodeString&  format( const Formattable* arguments,
                            int32_t cnt,
                            UnicodeString& appendTo,
                            FieldPosition& status,
                            int32_t recursionProtection,
                            UErrorCode& success) const;

    void             makeFormat(int32_t offsetNumber,
                                UnicodeString* segments,
                                UParseError& parseError,
                                UErrorCode& success);

    /**
     * Convenience method that ought to be in NumberFormat
     */
    NumberFormat* createIntegerFormat(const Locale& locale, UErrorCode& status) const;

    /**
     * Checks the range of the source text to quote the special
     * characters, { and ' and copy to target buffer.
     * @param source
     * @param start the text offset to start the process of in the source string
     * @param end the text offset to end the process of in the source string
     * @param appendTo  Output parameter to receive result.
     *                  Result is appended to existing contents.
     */
    static void copyAndFixQuotes(const UnicodeString& appendTo, int32_t start, int32_t end, UnicodeString& target);
};

inline UClassID
MessageFormat::getDynamicClassID() const
{
    return MessageFormat::getStaticClassID();
}

inline UnicodeString&
MessageFormat::format(const Formattable& obj,
                      UnicodeString& appendTo,
                      UErrorCode& status) const {
    return Format::format(obj, appendTo, status);
}
U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_FORMATTING */

#endif // _MSGFMT
//eof
