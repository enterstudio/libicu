//---------------------------------------------------------------------------------
//
// Generated Header File.  Do not edit by hand.
//    This file contains the state table for RBBI rule parser.
//    It is generated by the Perl script "rbbicst.pl" from
//    the rule parser state definitions file "rbbirpt.txt".
//
//---------------------------------------------------------------------------------
#ifndef RBBIRPT_H
#define RBBIRPT_H

U_NAMESPACE_BEGIN
//
// Character classes for RBBI rule scanning.
//
    const uint8_t kRuleSet_digit_char = 128;
    const uint8_t kRuleSet_rule_char = 129;
    const uint8_t kRuleSet_white_space = 130;
    const uint8_t kRuleSet_name_char = 131;
    const uint8_t kRuleSet_name_start_char = 132;


enum RBBI_RuleParseAction {
    doExprOrOperator,
    doRuleErrorAssignExpr,
    doTagValue,
    doEndAssign,
    doRuleError,
    doVariableNameExpectedErr,
    doRuleChar,
    doLParen,
    doSlash,
    doStartTagValue,
    doDotAny,
    doExprFinished,
    doScanUnicodeSet,
    doExprRParen,
    doStartVariableName,
    doTagExpectedError,
    doTagDigit,
    doUnaryOpStar,
    doEndVariableName,
    doNOP,
    doUnaryOpQuestion,
    doExit,
    doStartAssign,
    doEndOfRule,
    doUnaryOpPlus,
    doExprStart,
    doExprCatOperator,
    doReverseDir,
    doCheckVarDef,
    rbbiLastAction};

//-------------------------------------------------------------------------------
//
//  RBBIRuleTableEl    represents the structure of a row in the transition table
//                     for the rule parser state machine.
//-------------------------------------------------------------------------------
struct RBBIRuleTableEl {
    RBBI_RuleParseAction          fAction;
    uint8_t                       fCharClass;       // 0-127:    an individual ASCII character
                                                    // 128-255:  character class index
    uint8_t                       fNextState;       // 0-250:    normal next-stat numbers
                                                    // 255:      pop next-state from stack.
    uint8_t                       fPushState;
    UBool                         fNextChar;
};

struct RBBIRuleTableEl gRuleParseStateTable[] = {
    {doNOP, 0, 0, 0, TRUE}
    , {doExprStart, 254, 12, 8, FALSE}     //  1      start
    , {doNOP, 130, 1,0,  TRUE}     //  2 
    , {doExprStart, 36 /*$*/, 70, 80, FALSE}     //  3 
    , {doReverseDir, 33 /*!*/, 11,0,  TRUE}     //  4 
    , {doNOP, 59 /*;*/, 1,0,  TRUE}     //  5 
    , {doNOP, 252, 0,0,  FALSE}     //  6 
    , {doExprStart, 255, 12, 8, FALSE}     //  7 
    , {doEndOfRule, 59 /*;*/, 1,0,  TRUE}     //  8      break-rule-end
    , {doNOP, 130, 8,0,  TRUE}     //  9 
    , {doRuleError, 255, 85,0,  FALSE}     //  10 
    , {doExprStart, 255, 12, 8, FALSE}     //  11      reverse-rule
    , {doRuleChar, 254, 21,0,  TRUE}     //  12      term
    , {doNOP, 130, 12,0,  TRUE}     //  13 
    , {doRuleChar, 129, 21,0,  TRUE}     //  14 
    , {doNOP, 91 /*[*/, 76, 21, FALSE}     //  15 
    , {doLParen, 40 /*(*/, 12, 21, TRUE}     //  16 
    , {doNOP, 36 /*$*/, 70, 20, FALSE}     //  17 
    , {doDotAny, 46 /*.*/, 21,0,  TRUE}     //  18 
    , {doRuleError, 255, 85,0,  FALSE}     //  19 
    , {doCheckVarDef, 255, 21,0,  FALSE}     //  20      term-var-ref
    , {doUnaryOpStar, 42 /***/, 25,0,  TRUE}     //  21      expr-mod
    , {doUnaryOpPlus, 43 /*+*/, 25,0,  TRUE}     //  22 
    , {doUnaryOpQuestion, 63 /*?*/, 25,0,  TRUE}     //  23 
    , {doNOP, 255, 25,0,  FALSE}     //  24 
    , {doExprCatOperator, 254, 12,0,  FALSE}     //  25      expr-cont
    , {doNOP, 130, 25,0,  TRUE}     //  26 
    , {doExprCatOperator, 129, 12,0,  FALSE}     //  27 
    , {doExprCatOperator, 91 /*[*/, 12,0,  FALSE}     //  28 
    , {doExprCatOperator, 40 /*(*/, 12,0,  FALSE}     //  29 
    , {doExprCatOperator, 36 /*$*/, 12,0,  FALSE}     //  30 
    , {doExprCatOperator, 46 /*.*/, 12,0,  FALSE}     //  31 
    , {doExprCatOperator, 47 /*/*/, 37,0,  FALSE}     //  32 
    , {doExprCatOperator, 123 /*{*/, 49,0,  TRUE}     //  33 
    , {doExprOrOperator, 124 /*|*/, 12,0,  TRUE}     //  34 
    , {doExprRParen, 41 /*)*/, 255,0,  TRUE}     //  35 
    , {doExprFinished, 255, 255,0,  FALSE}     //  36 
    , {doSlash, 47 /*/*/, 39,0,  TRUE}     //  37      look-ahead
    , {doNOP, 255, 85,0,  FALSE}     //  38 
    , {doExprCatOperator, 254, 12,0,  FALSE}     //  39      expr-cont-no-slash
    , {doNOP, 130, 25,0,  TRUE}     //  40 
    , {doExprCatOperator, 129, 12,0,  FALSE}     //  41 
    , {doExprCatOperator, 91 /*[*/, 12,0,  FALSE}     //  42 
    , {doExprCatOperator, 40 /*(*/, 12,0,  FALSE}     //  43 
    , {doExprCatOperator, 36 /*$*/, 12,0,  FALSE}     //  44 
    , {doExprCatOperator, 46 /*.*/, 12,0,  FALSE}     //  45 
    , {doExprOrOperator, 124 /*|*/, 12,0,  TRUE}     //  46 
    , {doExprRParen, 41 /*)*/, 255,0,  TRUE}     //  47 
    , {doExprFinished, 255, 255,0,  FALSE}     //  48 
    , {doNOP, 130, 49,0,  TRUE}     //  49      tag-open
    , {doStartTagValue, 128, 52,0,  FALSE}     //  50 
    , {doTagExpectedError, 255, 85,0,  FALSE}     //  51 
    , {doNOP, 130, 56,0,  TRUE}     //  52      tag-value
    , {doNOP, 125 /*}*/, 56,0,  FALSE}     //  53 
    , {doTagDigit, 128, 52,0,  TRUE}     //  54 
    , {doTagExpectedError, 255, 85,0,  FALSE}     //  55 
    , {doNOP, 130, 56,0,  TRUE}     //  56      tag-close
    , {doTagValue, 125 /*}*/, 59,0,  TRUE}     //  57 
    , {doTagExpectedError, 255, 85,0,  FALSE}     //  58 
    , {doExprCatOperator, 254, 12,0,  FALSE}     //  59      expr-cont-no-tag
    , {doNOP, 130, 59,0,  TRUE}     //  60 
    , {doExprCatOperator, 129, 12,0,  FALSE}     //  61 
    , {doExprCatOperator, 91 /*[*/, 12,0,  FALSE}     //  62 
    , {doExprCatOperator, 40 /*(*/, 12,0,  FALSE}     //  63 
    , {doExprCatOperator, 36 /*$*/, 12,0,  FALSE}     //  64 
    , {doExprCatOperator, 46 /*.*/, 12,0,  FALSE}     //  65 
    , {doExprCatOperator, 47 /*/*/, 37,0,  FALSE}     //  66 
    , {doExprOrOperator, 124 /*|*/, 12,0,  TRUE}     //  67 
    , {doExprRParen, 41 /*)*/, 255,0,  TRUE}     //  68 
    , {doExprFinished, 255, 255,0,  FALSE}     //  69 
    , {doStartVariableName, 36 /*$*/, 72,0,  TRUE}     //  70      scan-var-name
    , {doNOP, 255, 85,0,  FALSE}     //  71 
    , {doNOP, 132, 74,0,  TRUE}     //  72      scan-var-start
    , {doVariableNameExpectedErr, 255, 85,0,  FALSE}     //  73 
    , {doNOP, 131, 74,0,  TRUE}     //  74      scan-var-body
    , {doEndVariableName, 255, 255,0,  FALSE}     //  75 
    , {doScanUnicodeSet, 91 /*[*/, 255,0,  TRUE}     //  76      scan-unicode-set
    , {doScanUnicodeSet, 112 /*p*/, 255,0,  TRUE}     //  77 
    , {doScanUnicodeSet, 80 /*P*/, 255,0,  TRUE}     //  78 
    , {doNOP, 255, 85,0,  FALSE}     //  79 
    , {doNOP, 130, 80,0,  TRUE}     //  80      assign-or-rule
    , {doStartAssign, 61 /*=*/, 12, 83, TRUE}     //  81 
    , {doNOP, 255, 20, 8, FALSE}     //  82 
    , {doEndAssign, 59 /*;*/, 1,0,  TRUE}     //  83      assign-end
    , {doRuleErrorAssignExpr, 255, 85,0,  FALSE}     //  84 
    , {doExit, 255, 85,0,  TRUE}     //  85      errorDeath
 };
const char *RBBIRuleStateNames[] = {    0,
     "start",
    0,
    0,
    0,
    0,
    0,
    0,
     "break-rule-end",
    0,
    0,
     "reverse-rule",
     "term",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "term-var-ref",
     "expr-mod",
    0,
    0,
    0,
     "expr-cont",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "look-ahead",
    0,
     "expr-cont-no-slash",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "tag-open",
    0,
    0,
     "tag-value",
    0,
    0,
    0,
     "tag-close",
    0,
    0,
     "expr-cont-no-tag",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "scan-var-name",
    0,
     "scan-var-start",
    0,
     "scan-var-body",
    0,
     "scan-unicode-set",
    0,
    0,
    0,
     "assign-or-rule",
    0,
    0,
     "assign-end",
    0,
     "errorDeath",
    0};

U_NAMESPACE_END
#endif
