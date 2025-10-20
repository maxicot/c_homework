#include "../linked_list/linked_list.h"
#include <string.h>
#include <stddef.h>
#include <stdio.h>

// Return the opening opposite to one of the closing characters (0 if invalid)
char opener(char ch)
{
    switch (ch) {
        case ')':
            return '(';
        case ']':
            return '[';
        case '}':
            return '{';
        default:
            return 0;
    }
}

// Check whether parens, brackets and braces in the given null-terminated string are balanced
bool isBalanced(const char* str)
{
    LinkedList stack = lnew();

    for (; *str != 0; str++) {
        char ch = *str;

        switch (ch) {
            case '(':
            case '[':
            case '{':
                lpush(&stack, ch);
                break;
            case ')':
            case ']':
            case '}':
                PopResult last = lpop(&stack);

                if (!(last.valid && last.value == opener(ch))) {
                    return false;
                }

                break;
        }
    }

    return llen(&stack) == 0;
}

int main(void)
{
    const char* testCases[6] = {
        "",
        "(",
        ")",
        "()",
        "k((aa{a[b{ba[c(c)c]}]c}sajd)l)w",
        "(([{]}))",
    };

    for (size_t i = 0; i < 6; i++) {
        printf("%d: '%s'\n", isBalanced(testCases[i]), testCases[i]);
    }

    return 0;
}
