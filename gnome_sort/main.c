#include <stdint.h>
#include <stdio.h>

void intSwap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Gnome sort implementation (https://en.wikipedia.org/wiki/Gnome_sort)
void gnomeSort(int* array, size_t len)
{
    size_t i = 1;
    size_t j = 2;

    while (i < len) {
        if (array[i - 1] < array[i]) {
            i = j;
            j++;
        } else {
            intSwap(array + i - 1, array + i);
            i--;
            if (i == 0) {
                i = j;
                j++;
            }
        }
    }
}

int main(void)
{
    int array[7] = {9, 15, 2, 6, 0, 1, 5};
    gnomeSort(&array[0], 7);

    for (int i = 0; i < 7; i++) {
        printf("%d ", array[i]);
    }

    return 0;
}
