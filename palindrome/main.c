#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Check if a null-terminated string is a palindrome (ignoring spaces).
// `bool` from C23
bool isPalindrome(const char* start)
{
    size_t len = strlen(start);
    const char* end = start + len - 1;

    while (start < end) {
        while (*start == ' ') {
            start++;

            if (end <= start) {
                return true;
            }
        }

        while (*end == ' ') {
            end--;

            if (end <= start) {
                return true;
            }
        }

        if (*start != *end) {
            return false;
        }

        start++;
        end--;
    }

    return true;
}

int main(void)
{
    printf("Case 1: %d\n", isPalindrome("       a     bc   h   cb      a"));
    printf("Case 2: %d\n", isPalindrome("y idu s me4em sudiy"));
    printf("Case 2: %d\n", isPalindrome("krakozyabra"));

    return 0;
}
