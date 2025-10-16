#include "../linked_list/linked_list.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef enum {
    INTEGER,
    OPERATOR,
} TokenType;

typedef struct {
    TokenType type;
    int32_t value;
} Token;

static_assert(sizeof(Token) <= sizeof(size_t));

// sizeof(Token) <= sizeof(size_t)
typedef union {
    size_t size;
    Token token;
} TokenTransmutation;

// sizeof(Token) <= sizeof(size_t)
Token asToken(size_t token)
{
    TokenTransmutation value = {.size = token};
    return value.token;
}

// sizeof(Token) <= sizeof(size_t)
size_t fromToken(Token token) {
    TokenTransmutation value = {.token = token};
    return value.size;
}

// Produce a token stream from the given string
LinkedList tokenize(const char* str)
{
    LinkedList list = lnew();
    size_t index = 0;
    size_t len = strlen(str);

    while (index < len) {
        char ch = str[index];
        TokenType type = -1;
        int32_t value = -1;

        switch (ch) {
            case '0'...'9':
                type = INTEGER;
                value = atol(str + index);

                while (str[index] >= '0' && str[index] <= '9') {
                    index++;
                }

                break;
            case '(':
            case ')':
            case '+':
            case '-':
            case '*':
            case '/':
                type = OPERATOR;
                value = ch;
                index++;
                break;
            default:
                index++;
                break;
        }

        if (type != -1) {
            Token token = {type, value};
            lpush(&list, fromToken(token));
        }
    }

    return list;
}

// Return an operator's precedence
uint8_t precedence(char operator)
{
    switch (operator) {
        case '('...')':
            return 255;
        case '*':
        case '/':
            return 253;
        case '+':
        case '-':
            return 252;
    }
}

// Convert an arithmetical expression into reverse Polish notation
LinkedList parse(const char* str)
{
    LinkedList tokenStream = tokenize(str);
    LinkedList operators = lnew();
    LinkedList output = lnew();

    for (size_t idx = 0; idx < llen(&tokenStream); idx++) {
        Token token = asToken(*lget(&tokenStream, idx));

        switch (token.type) {
            case INTEGER:
                lpush(&output, fromToken(token));
                break;
            case OPERATOR:
                switch (token.value) {
                    case '(':
                        lpush(&operators, '(');
                        break;
                    case ')':
                        bool met = false;

                        while (llen(&operators) > 0) {
                            char last = lpop(&operators).value;

                            if (last != '(') {
                                Token newToken = {OPERATOR, last};
                                lpush(&output, fromToken(newToken));
                            } else {
                                met = true;
                            }
                        }

                        if (!met) {
                            fprintf(stderr, "unbalanced parenthesis\n");
                            abort();
                        }

                        break;
                    default:
                        while (llen(&operators) > 0) {
                            char last = *lpeek(&operators);
                            if (last != '(' && precedence(last) >= precedence(token.value)) {
                                lpop(&operators);
                                Token newToken = {OPERATOR, last};
                                lpush(&output, fromToken(newToken));
                            } else {
                                break;
                            }
                        }
                        lpush(&operators, token.value);
                        break;
                }
                break;
        }
    }

    while (llen(&operators) > 0) {
        char last = lpop(&operators).value;

        if (last == '(') {
            fprintf(stderr, "unbalanced parenthesis\n");
            abort();
        }

        Token newToken = {OPERATOR, last};
        lpush(&output, fromToken(newToken));
    }

    ldelete(tokenStream);

    return output;
}

// Parse and print an arithmetical expression in reverse Polish notation
void printRpn(const char* str)
{
    LinkedList list = parse(str);

    for (size_t i = 0; i < llen(&list); i++) {
        Token token = asToken(*lget(&list, i));
        printf((token.type == INTEGER) ? "%d " : "%c ", token.value);
    }

    printf("\n");
}

int main(void)
{
    printRpn("(1 + 1) * 2");
    printRpn("6 / (2 + 1) * 3");

    return 0;
}
