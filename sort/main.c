#include <stdint.h>
#include <stdio.h>

extern int i32QuickSort(const int32_t* buf, uint64_t len);

int main() {
    int arr[100] = {0};
    int num = 0;
    int len = 0;

    while (scanf("%d", &num) == 1) {
        arr[len] = num;
        len++;

        if (getchar() == '\n') {
            break;
        }
    }

    int res = i32QuickSort(&arr[0], len);

    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }

    printf("\n");

    return res;
}
