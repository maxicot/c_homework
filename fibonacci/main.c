#include <stdio.h>

// Iterative fibonacci implementation that summarizes numbers instead
unsigned solution()
{
    unsigned sum = 0;
    unsigned i = 0;
    unsigned j = 1;

    while (i <= 1000000) {
        if (i % 2 == 0) {
            sum += i;
        }

        unsigned temp = i + j;
        i = j;
        j = temp;
    }

    return sum;
}

int main(void)
{
    printf("%d", solution());
}
