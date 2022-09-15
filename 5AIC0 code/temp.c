#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void printArray(int arr[5]) {
    printf("Array : ");
    for(int i = 0; i < 5; i++) {
        printf("%d - ", arr[i]);
    }
    printf("\n");
}

int * createArray() {
    int arr[5] = {1,2,3,4,5};

    int * arr_pointer = (int *) malloc(sizeof(arr));
    
    for(int i = 0; i < 5; i++) {
        arr_pointer[i] = arr[i];
    }

    return arr_pointer;
}

void printFromPointer(int * pointer) {
    printf("Array from pointer : ");
    for(int i = 0; i < 5; i++) {
        printf("%d - ", *(pointer + i));
    }
    printf("\n");
}

int main() {
    int x = 5;
    int * pointer = &x;
    printf("value of *pointer : %d \n", *pointer);
    printf("\n");

    int * arr_pointer = createArray();
    printFromPointer(arr_pointer);

}