#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum {DOMINANT = 0, RECESSIVE = 1, UNSPECIFIED = 2} CAN_SYMBOL;
typedef enum {SYNC_SEG, PROP_SEG, PHASE_SEG_1, PHASE_SEG_2} BIT_SEGMENT;
typedef struct _can_frame {
  uint32_t ID;
  uint32_t DLC;
  uint64_t Data;
  uint32_t CRC;
} CAN_FRAME;

CAN_SYMBOL * dec_to_bin(uint64_t number) {
    uint64_t temp[64];

    if (number == 0 || number == 1) {
        CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc(2*sizeof(CAN_SYMBOL));
        binary[0] = 1;
        binary[1] = number;
        return binary;
    }

    int i = 0;

    for( ; number > 0; i++) {
        temp[i] = number % 2;
        number = number / 2;
    }

    CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc((i+3)*sizeof(CAN_SYMBOL));

    binary[0] = i;

    for (int j = 0; j < i; j++) {
        if(temp[j] == 0) {
            binary[i - j] = DOMINANT;
        } else {
            binary[i - j] = RECESSIVE;
        }
    }

    return binary;
}

CAN_SYMBOL * concatenate_binary(CAN_SYMBOL * bin1, CAN_SYMBOL * bin2) {
    int size_bin1 = *bin1;
    int size_bin2 = *bin2;

    size_t new_size = size_bin1 + size_bin2 + 1;

    CAN_SYMBOL* bin = (CAN_SYMBOL *) malloc((new_size+2)*sizeof(CAN_SYMBOL));
    bin[0] = new_size-1;
    for(int i=1; i<=size_bin1; i++) {
        bin[i] = *(bin1 + i);
    }
    int j = 1;
    for(int k = size_bin1+1; k < new_size; k++) {
        bin[k] = *(bin2 + j);
        j++;
    }

    free(bin2);
    free(bin1);

    return bin;
}

void printFromPointer(CAN_SYMBOL * pointer) {
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        printf("%d - ", *(pointer + i));
    }
    printf("\n");
}

int WinMain() {
    CAN_SYMBOL * res1 = dec_to_bin(52);
    printf("First array : ");
    printFromPointer(res1);
    CAN_SYMBOL * res2 = dec_to_bin(2);
    printf("Second array : ");
    printFromPointer(res2);

    CAN_SYMBOL * res = concatenate_binary(res1, res2);
    printf("Concatenated array : ");
    printFromPointer(res);
}