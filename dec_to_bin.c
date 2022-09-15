#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {DOMINANT = 0, RECESSIVE = 1, UNSPECIFIED = 2} CAN_SYMBOL;
typedef enum {SYNC_SEG, PROP_SEG, PHASE_SEG_1, PHASE_SEG_2} BIT_SEGMENT;
typedef struct _can_frame {
  uint32_t ID;
  uint32_t DLC;
  uint64_t Data;
  uint32_t CRC;
} CAN_FRAME;

CAN_SYMBOL * dec_to_bin(int number) {
    int temp[32];

    if (number == 0) {
        CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc(sizeof(CAN_SYMBOL));
        binary[0] = 1;
        binary[1] = DOMINANT;
        return binary;
    }

    if (number == 1) {
        CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc(sizeof(CAN_SYMBOL));
        binary[0] = 1;
        binary[1] = RECESSIVE;
        return binary;
    }

    int i = 0;

    for( ; number > 0; i++) {
        temp[i] = number % 2;
        number = number / 2;
    }

    for(int k=0;k<i;k++) {
        printf("%d", temp[k]);
    }
    printf("\n");

    CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc(i*sizeof(CAN_SYMBOL));

    int bit = 0;

    binary[0] = i;

    for (int j = 0; j < i; j++) {
        if(temp[j] == 0) {
            binary[i - j] = DOMINANT;
        } else {
            binary[i - j] = RECESSIVE;
        }
    }
    printf("\n");

    return binary;
}

void printFromPointer(CAN_SYMBOL * pointer) {
    printf("Array from pointer : ");
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        printf("%d - ", *(pointer + i));
    }
    printf("\n");
}

int main() {
    CAN_SYMBOL * res = dec_to_bin(10);
    printFromPointer(res);
}