#include "TokenInfo_AffixTable_wEvaluation.h"
#include "TokenAffix.h"
#include "Token.h"
#include "Stack.h"
#include "Exception.h"
#include "Error.h"
#include "ShuntingYard.h"

#define PREFIX_TYPE 4
#define INFIX_TYPE 2
#define SUFFIX_TYPE 1     // Suffix == Postfix

TokenInfo AffixPossibilities[50] = {
  ['+'] = {.Attribute = PREFIX_TYPE | INFIX_TYPE | SUFFIX_TYPE}, // 7
  ['-'] = {.Attribute = PREFIX_TYPE | INFIX_TYPE | SUFFIX_TYPE}, // 7
  ['*'] = {.Attribute = INFIX_TYPE}, // 2
  ['/'] = {.Attribute = INFIX_TYPE},  // 2
  ['('] = {.Attribute = PREFIX_TYPE}, // 4
  [')'] = {.Attribute = SUFFIX_TYPE}, // 1
};
// If operator is '+', '-' , '*' or '/' return the
// affix possibilities (in int) else throw error
TokenInfo *getTokenInfo(Token *token){
  char operatorSymbol;
  operatorSymbol = *((OperatorToken*)token)->str;

  if(operatorSymbol == '+'){
    return &AffixPossibilities['+'];
  }
  else if(operatorSymbol == '-'){
    return &AffixPossibilities['-'];
  }
  else if(operatorSymbol == '*'){
    return &AffixPossibilities['*'];
  }
  else if(operatorSymbol == '/'){
    return &AffixPossibilities['/'];
  }
  else if(operatorSymbol == '('){
    return &AffixPossibilities['('];
  }
  else if(operatorSymbol == ')'){
    return &AffixPossibilities[')'];
  }
  else{
    throwException(ERR_INVALID_OPERATOR, token , "'%c'is an invalid operator", operatorSymbol);
  }
}

// This function will check currrent operator and next operator possibilities
// The following table show the AffixPossibilities
//   ---------------------------------------------
//  | currOperator | nextOperator| Acceptable (*)| Yes - * / No - Blank
//   ---------------------------------------------
//  |     '+'      |      '+'    |       *       |
//  |     '+'      |      '-'    |       *       |
//  |     '+'      |      '*'    |               |
//  |     '+'      |      '/'    |               |
//  ----------------------------------------------
//  |     '-'      |      '+'    |       *       |
//  |     '-'      |      '-'    |       *       |
//  |     '-'      |      '*'    |               |
//  |     '-'      |      '/'    |               |
//  ----------------------------------------------
//  |     '*'      |      '+'    |       *       |
//  |     '*'      |      '-'    |       *       |
//  |     '*'      |      '*'    |               |
//  |     '*'      |      '/'    |               |
//  ----------------------------------------------
//  |     '/'      |      '+'    |       *       |
//  |     '/'      |      '-'    |       *       |
//  |     '/'      |      '*'    |               |
//  |     '/'      |      '/'    |               |
//  ----------------------------------------------
int checkOperatorsAffixPossibilities(Token *currToken, Tokenizer *nextTokens){
  int compareWithTableResult;
  Token *nextToken;
  TokenInfo *currTokenInfo;
  TokenInfo *nextTokenInfo;
  TokenType currTokenType;

  currTokenType = getTokenType(currToken);

  if(currTokenType == TOKEN_OPERATOR_TYPE){
    currTokenInfo = getTokenInfo(currToken);
    nextToken = getToken(nextTokens);

    if(nextToken->type == TOKEN_OPERATOR_TYPE){
      nextTokenInfo = getTokenInfo(nextToken);
      compareWithTableResult = compareCurrTokenAndNextTokenWithTable(currTokenInfo, nextTokenInfo);

      switch (compareWithTableResult) {
        case 0 : pushBackToken(nextTokens, nextToken);
                 return 0;
        case 1 : pushBackToken(nextTokens, nextToken);
                 return 1;
        // compareCurrTokenAndNextTokenWithTable only return 1 or 0
        // else error will be thrown in getTokenInfo
        //default : throwSimpleError(ERR_INVALID_ANSWER, "Invalid answer from compareCurrTokenAndNextTokenWithTable");
      }
    }
    else if(nextToken->type == TOKEN_INTEGER_TYPE || nextToken->type == TOKEN_FLOAT_TYPE){
		    pushBackToken(nextTokens, nextToken);
		    return 1 ;
    }
    else if (nextToken->type == TOKEN_NULL_TYPE){
      pushBackToken(nextTokens, nextToken);
      return 1 ;
    }

    else{
      // getTokenInfo only accept '+', '-', '*', '/'
      // else error already thrown at getTokenInfo
      throwException(ERR_INVALID_OPERATOR, nextToken ,"'%s' is an invalid operator", nextToken->str);
    }
  }
  else{
    // getTokenInfo only accept '+', '-', '*', '/'
    // else error already thrown at getTokenInfo
    throwException(ERR_INVALID_OPERATOR, currToken ,"'%s' is an invalid operator", nextToken->str);
  }

}

/*   leftAffix    thisAffix      Validity      Example
 *   INFIX        INFIX           FALSE        2 + *
 *   INFIX        PREFIX          TRUE         2 + -
 *   INFIX        SUFFIX          FALSE        2 + )
 *
 *   PREFIX       INFIX           FALSE        - +
 *   PREFIX       PREFIX          TRUE         - -2
 *   PREFIX       SUFFIX          FALSE        - )
 *
 *   SUFFIX       INFIX           TRUE         ) +
 *   SUFFIX       PREFIX          FALSE        ) -2
 *   SUFFIX       SUFFIX          TRUE         ) )
 */
int areAffixesCombinationValid(Affix leftAffix, Affix thisAffix){
  Token *token ;
  if(leftAffix == INFIX){
    switch (thisAffix) {
      case PREFIX : return 1;
      default : return 0;
      //default : throwException(ERR_INVALID_AFFIX, token," Combination of 'leftAffix' and 'thisAffix' is invalid");
    }
  }
  else if(leftAffix == PREFIX){
    switch (thisAffix) {
      case PREFIX : return 1;
      default : return 0;
      //throwException(ERR_INVALID_AFFIX,token, " Combination of 'leftAffix' and 'thisAffix' is invalid");
    }
  }
  else{
    switch (thisAffix) {
      case INFIX : return 1;
      case SUFFIX : return 1;
      default : return 0;
      //throwException(ERR_INVALID_AFFIX, token, " Combination of 'leftAffix' and 'thisAffix' is invalid");
    }
  }

}


// This function will compare currOperator and nextOperator the Attribute from AffixPossibilities
int compareCurrTokenAndNextTokenWithTable(TokenInfo *currTokenInfo, TokenInfo *nextTokenInfo){
  if(currTokenInfo->Attribute == 7){         // 4 | 2 | 1 = 7  (refer to #define)
    switch (nextTokenInfo->Attribute) {
      case 7 : return 1;
      case 4 : return 1;
      case 2 : return 0;
      case 1 : return 0;
      // getTokenInfo only give 2 or 7
      // else error already thrown at getTokenInfo
      default: throwException(ERR_INVALID_TOKENINFO, nextTokenInfo,"Invalid attribute '%d' from TokenInfo", nextTokenInfo->Attribute);
    }
  }
  else if(currTokenInfo->Attribute == 4){
    switch (nextTokenInfo->Attribute) {
      case 7 : return 1;
      case 4 : return 1;
      case 2 : return 0;
      case 1 : return 0;
      // getTokenInfo only give 2 or 7
      // else error already thrown at getTokenInfo
      default: throwException(ERR_INVALID_TOKENINFO, nextTokenInfo,"Invalid attribute '%d' from TokenInfo", nextTokenInfo->Attribute);
    }
}
  else if(currTokenInfo->Attribute == 2){
    switch (nextTokenInfo->Attribute) {
      case 7 : return 1;
      case 4 : return 1;
      case 2 : return 0;
      case 1 : return 0;
      // getTokenInfo only give 2 or 7
      // else error already thrown at getTokenInfo
      default: throwException(ERR_INVALID_TOKENINFO, nextTokenInfo,"Invalid attribute '%d' from TokenInfo", nextTokenInfo->Attribute);
    }
  }
    else if(currTokenInfo->Attribute == 1){
      switch (nextTokenInfo->Attribute) {
        case 7 : return 1;
        case 4 : return 0;
        case 2 : return 1;
        case 1 : return 1;
        // getTokenInfo only give 2 or 7
        // else error already thrown at getTokenInfo
        default: throwException(ERR_INVALID_TOKENINFO, nextTokenInfo,"Invalid attribute '%d' from TokenInfo", nextTokenInfo->Attribute);
      }
  }
  // getTokenInfo only give 2 or 7
  // else error already thrown at getTokenInfo
  else{
    throwException(ERR_INVALID_TOKENINFO, nextTokenInfo,"Invalid attribute '%d' from TokenInfo", currTokenInfo->Attribute);
  }

}

int isOperatorSymbolValid(Token* token){
  char operatorSymbol;
  operatorSymbol = *((OperatorToken*)token) -> str;

  if(operatorSymbol == '+' || operatorSymbol == '-' || operatorSymbol == '*' || operatorSymbol == '/' || operatorSymbol == '(' || operatorSymbol == ')'){
      return 1;
  }
  else{
      throwException(ERR_INVALID_OPERATOR, token, "'%s' is an invalid operator", token->str);
  }
}
// If infix pop two TOKEN prefix pop one token
// The function check the affix of first operator token of tokenizer
Affix checkTokenAffix(Tokenizer *tokenizer, Token *prevToken){
  TokenType prevTokenType;
  int  PossibleAffixCombination;
  char operatorSymbol;

  prevTokenType = prevToken->type;
  Token *nextToken = NULL;

  OperatorType currTokenOperatorType;
  nextToken = getToken(tokenizer);
  operatorSymbol = *((OperatorToken*)nextToken) -> str;

  // (2)(+) / (2)(-) / (2)(*) / (2)(/)  return operator infix
  if(prevTokenType == TOKEN_FLOAT_TYPE || prevTokenType == TOKEN_INTEGER_TYPE){
    if(nextToken->type == TOKEN_OPERATOR_TYPE){
      if(operatorSymbol == ')'){
        encodeAffix(nextToken, SUFFIX);
        pushBackToken(tokenizer, nextToken);
        return SUFFIX;
      }
      else{
        encodeAffix(nextToken, INFIX);
        pushBackToken(tokenizer, nextToken);
        return INFIX;
      }
    }
    else{
      pushBackToken(tokenizer, nextToken);
    //  throwSimpleError(ERR_INVALID_AFFIX, "Invalid affix (currentToken and nextToken is not OperatorType)");
      throwException(ERR_INVALID_AFFIX, nextToken,"Combination of '%s' and '%s' is invalid", prevToken->str, nextToken->str);
    }
  }
  // For example (+)(-) [(+) is preToken and (-) is nextToken]
  // (-) should be infix
  else if (prevTokenType == TOKEN_OPERATOR_TYPE){
    if(nextToken->type == TOKEN_OPERATOR_TYPE){
      pushBackToken(tokenizer, nextToken);
      PossibleAffixCombination = checkOperatorsAffixPossibilities(prevToken, tokenizer);
      if(PossibleAffixCombination == 1){
          if(operatorSymbol == ')'){
            encodeAffix(nextToken, SUFFIX);
            return SUFFIX;
          }
          else{
            encodeAffix(nextToken, PREFIX);
            return PREFIX;
          }
      }
      else{
      //  throwSimpleError(ERR_INVALID_AFFIX, "current operator is valid (INFIX or PREFIX) but nextToken is either '*' or '/' (INFIX)");
        throwException(ERR_INVALID_AFFIX, nextToken,"Combination of '%s' and '%s' is invalid", prevToken->str, nextToken->str);
      }
    } // Example (-)(2) return PREFIX
    else{
      encodeAffix(nextToken, PREFIX);
      pushBackToken(tokenizer, nextToken);
      return PREFIX;
    }

  }
  else if(prevTokenType == TOKEN_NULL_TYPE){
    if(nextToken->type == TOKEN_OPERATOR_TYPE){
      pushBackToken(tokenizer, nextToken);
      encodeAffix(nextToken, PREFIX);
      return PREFIX;
    }
  }
  else{
    //throwSimpleError(ERR_INVALID_OPERATOR, "Invalid operator found");
    throwException(ERR_INVALID_OPERATOR, nextToken,"Combination of '%s' and '%s' is invalid", prevToken->str, nextToken->str);
  }

}


void encodeTokenAffix(Token *token, Token *prevToken, Tokenizer *tokenizer,TokenType prevTokenType){
  int  PossibleAffixCombination;
  char operatorSymbol;
  char prevOperatorSymbol;

  operatorSymbol = *((OperatorToken*)token) -> str;
  if(prevTokenType == TOKEN_OPERATOR_TYPE){
      prevOperatorSymbol =*((OperatorToken*)prevToken) -> str ;
  }


  if(isOperatorSymbolValid(token)){
    // (2)(+) / (2)(-) / (2)(*) / (2)(/)  return operator infix
    if(prevTokenType == TOKEN_FLOAT_TYPE || prevTokenType == TOKEN_INTEGER_TYPE){
      if(token->type == TOKEN_OPERATOR_TYPE){
        if(operatorSymbol == ')'){
          encodeAffix(token, SUFFIX);
        }
        else if(operatorSymbol == '('){
          throwException(ERR_INVALID_AFFIX, token," Did not expect '%s'' ", token->str);
        }
        else{
          PossibleAffixCombination = checkOperatorsAffixPossibilities(token, tokenizer);
            if(PossibleAffixCombination == 1){
              encodeAffix(token, INFIX);
            }
            else{
              throwException(ERR_INVALID_AFFIX, token,"Did not expect '%s'", token->str);
            }
        }
      }

      else{
        throwException(ERR_INVALID_AFFIX, token,"Combination of  '%s' and '%s' is invalid ", token->str);
      }
    }
    // For example (+)(-) [(+) is preToken and (-) is nextToken]
    // (-) should be infix
    else if (prevTokenType == TOKEN_OPERATOR_TYPE){
      if(token->type == TOKEN_OPERATOR_TYPE){
        PossibleAffixCombination = checkOperatorsAffixPossibilities(token, tokenizer);
        if(PossibleAffixCombination == 1){
            if(operatorSymbol == ')'){
              encodeAffix(token, SUFFIX);
            }
            else if (prevOperatorSymbol == ')'){
              encodeAffix(token, INFIX);
            }
            else{
              encodeAffix(token, PREFIX);
            }
        }
        else{
          throwException(ERR_INVALID_AFFIX, token,"Did not expect '%s'", token->str);
        }
      } // Example (-)(2) return PREFIX
      else{
        encodeAffix(token, PREFIX);
      }

    }
    else if(prevTokenType == TOKEN_NULL_TYPE){
      if(token->type == TOKEN_OPERATOR_TYPE){
        encodeAffix(token, PREFIX);
      }
      else{
        //throwSimpleError(ERR_INVALID_OPERATOR, "Invalid operator found");
        throwException(ERR_INVALID_OPERATOR, token,"Did not expect '%s'", token->str);
      }
    }
  }

}

void checkTokenAffixAndEncodeAffix(Token *token, Tokenizer *tokenizer,TokenType prevTokenType){
  int  PossibleAffixCombination;
  char operatorSymbol;

  operatorSymbol = *((OperatorToken*)token) -> str;

  if(isOperatorSymbolValid(token)){
    // (2)(+) / (2)(-) / (2)(*) / (2)(/)  return operator infix
    if(prevTokenType == TOKEN_FLOAT_TYPE || prevTokenType == TOKEN_INTEGER_TYPE){
      if(token->type == TOKEN_OPERATOR_TYPE){
        if(operatorSymbol == ')'){
          encodeAffix(token, SUFFIX);
        }
        else if(operatorSymbol == '('){
          throwException(ERR_INVALID_AFFIX, token,"'%s' should not be here ", token->str);
        }
        else{
          PossibleAffixCombination = checkOperatorsAffixPossibilities(token, tokenizer);
            if(PossibleAffixCombination == 1){
              encodeAffix(token, INFIX);
            }
            else{
              throwException(ERR_INVALID_AFFIX, token,"'%s' should not be here ", token->str);
            }
        }
      }

      else{
        throwException(ERR_INVALID_AFFIX, token,"Combination of  '%s' and '%s' is invalid ", token->str);
      }
    }
    // For example (+)(-) [(+) is preToken and (-) is nextToken]
    // (-) should be infix
    else if (prevTokenType == TOKEN_OPERATOR_TYPE){
      if(token->type == TOKEN_OPERATOR_TYPE){
        PossibleAffixCombination = checkOperatorsAffixPossibilities(token, tokenizer);
        if(PossibleAffixCombination == 1){
            if(operatorSymbol == ')'){
              encodeAffix(token, SUFFIX);
            }
            else if (operatorSymbol == '*'|| operatorSymbol == '/'){
              encodeAffix(token, INFIX);
            }
            else{
              encodeAffix(token, PREFIX);
            }
        }
        else{
          throwException(ERR_INVALID_AFFIX, token,"'%s' should not be here ", token->str);
        }
      } // Example (-)(2) return PREFIX
      else{
        encodeAffix(token, PREFIX);
      }

    }
    else if(prevTokenType == TOKEN_NULL_TYPE){
      if(token->type == TOKEN_OPERATOR_TYPE){
        encodeAffix(token, PREFIX);
      }
      else{
        //throwSimpleError(ERR_INVALID_OPERATOR, "Invalid operator found");
        throwException(ERR_INVALID_OPERATOR, token,"'%s' should not be here ", token->str);
      }
    }
  }

}

// This function will combine the prefix and the number together
// For example, (-)(2)---->(-2)
Token *combinePrefixWithOperandToken(Token *prefixToken, Token *operandToken){
  char  prefixSymbol;
  TokenType prefixTokenType;
  TokenType operandTokenType;

  prefixTokenType = getTokenType(prefixToken);
  operandTokenType = getTokenType(operandToken);

  if(prefixTokenType == TOKEN_OPERATOR_TYPE){
    prefixSymbol = *((OperatorToken*)prefixToken)->str;
    // If nextToken is numbers then do the operation else throw error
    if(operandTokenType == TOKEN_INTEGER_TYPE || operandTokenType== TOKEN_FLOAT_TYPE){
      switch(operandTokenType){
        case TOKEN_INTEGER_TYPE : if(prefixSymbol == '+'){
                                    return operandToken;
                                  }
                                  else if (prefixSymbol == '-'){
                                    int operand;
                                    operand = ((IntegerToken*)operandToken)->value;
                                    operand = -operand;
                                    ((IntegerToken*)operandToken)->value = operand;
                                    return operandToken;
                                  }
                                  else{
                                    throwException(ERR_INVALID_OPERATOR, prefixToken,"Operator '%s' is an invalid prefix" , prefixToken->str);
                                  }

        case TOKEN_FLOAT_TYPE    : if(prefixSymbol == '+'){
                                    return operandToken;
                                  }
                                  else if (prefixSymbol == '-'){
                                    float operand;
                                    operand = ((FloatToken*)operandToken)->value;
                                    operand = -operand;
                                    ((FloatToken*)operandToken)->value = operand;
                                    return operandToken;
                                  }
                                  else{
                                        throwException(ERR_INVALID_OPERATOR, prefixToken,"Operator '%s' is an invalid prefix" , prefixToken->str);
                                  }

          default :               throwException(ERR_INVALID_TOKENTYPE, operandToken,"Combination of '%s' and '%s' is invalid" , prefixToken->str, operandToken->str);
      }
    }
  }

    else{
    throwException(ERR_INVALID_OPERATOR, operandToken,"Combination of '%s' and '%s' is invalid" , prefixToken->str, operandToken->str);
    }
}
