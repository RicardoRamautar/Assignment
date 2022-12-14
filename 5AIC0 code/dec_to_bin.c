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

    CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc((i+1)*sizeof(CAN_SYMBOL));

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

void printFromPointer(CAN_SYMBOL * pointer) {
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        printf("%d", *(pointer + i));
    }
    printf("\n");
}

CAN_SYMBOL * concatenate_binary(CAN_SYMBOL * bin1, CAN_SYMBOL * bin2) {
    int size_bin1 = *bin1;
    int size_bin2 = *bin2;

    size_t new_size = size_bin1 + size_bin2 + 1;

    CAN_SYMBOL * bin = (CAN_SYMBOL *) realloc(bin1, new_size);

    bin[0] = new_size-1;

    int j = 1;
    for(int i = size_bin1+1; i < new_size; i++) {
        bin[i] = *(bin2 + j);
        j++;
    }

    // FREE MEMORY ALLOCATED TO BIN2

    return bin;
}

void can_max_tx_frame(CAN_FRAME* txFrame)
{
    uint64_t id = txFrame->ID;
    uint64_t dlc = txFrame->DLC;
    uint64_t data = txFrame->Data;
    uint64_t crc = txFrame->CRC;

    printFromPointer(dec_to_bin(id));
    printFromPointer(dec_to_bin(dlc));
    printFromPointer(dec_to_bin(data));
    printFromPointer(dec_to_bin(crc));

    CAN_SYMBOL* id_bin = dec_to_bin(id);
    CAN_SYMBOL* dlc_bin = dec_to_bin(dlc);
    CAN_SYMBOL* data_bin = dec_to_bin(data);
    CAN_SYMBOL* crc_bin = dec_to_bin(crc);
    CAN_SYMBOL* res =  concatenate_binary(id_bin, dlc_bin);
    printFromPointer(res);
    CAN_SYMBOL* res2 =  concatenate_binary(res, data_bin);
    printFromPointer(res2);
    CAN_SYMBOL* res3 =  concatenate_binary(res2, crc_bin);
    // printFromPointer(res3);
}

int WinMain() {
    CAN_FRAME frame = {10,20,43,52};
    CAN_FRAME * frame_pointer = &frame;
    can_max_tx_frame(frame_pointer);
}

// int WinMain() {
//     CAN_SYMBOL * res1 = dec_to_bin(15);
//     printf("First array : ");
//     printFromPointer(res1);
//     CAN_SYMBOL * res2 = dec_to_bin(2);
//     printf("Second array : ");
//     printFromPointer(res2);

//     CAN_SYMBOL * res = concatenate_binary(res1, res2);
//     printf("Concatenated array : ");
//     printFromPointer(res);
// }