#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define MAX_BIT_SIZE 135

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
        CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc(sizeof(CAN_SYMBOL));
        binary[0] = 1;
        binary[1] = number;
        return binary;
    }

    int i = 0;

    for( ; number > 0; i++) {
        temp[i] = number % 2;
        number = number / 2;
    }

    CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc(i*sizeof(CAN_SYMBOL));

    binary[0] = i;

    for (int j = 0; j < i; j++) {
        if(temp[j] == 0) {
            binary[i - j] = DOMINANT;
        } else if(temp[j] == 1) {
            binary[i - j] = RECESSIVE;
        } else {
            printf("ERROR: Binary not 1 or 0! BIG PROBLEM!/n");
        }
    }

    return binary;
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
    // free(bin2);
    return bin;
}

void printFromPointer(CAN_SYMBOL * pointer) {
    printf("Array from pointer : ");
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        printf("%d - ", *(pointer + i));
    }
    printf("\n");
}

void can_mac_tx_frame(CAN_FRAME* txFrame) 
{
    CAN_SYMBOL * bitstream = dec_to_bin(txFrame->ID);

    int dlc = txFrame->DLC;
    CAN_SYMBOL * substream = dec_to_bin(dlc);
    bitstream = concatenate_binary(bitstream, substream);

    int data = txFrame->Data;
    CAN_SYMBOL * substream2 = dec_to_bin(data);
    bitstream = concatenate_binary(bitstream, substream2);
    // bitstream = concatenate_binary(bitstream, dec_to_bin(txFrame->Data));
    // bitstream = concatenate_binary(bitstream, dec_to_bin(txFrame->CRC));
    
    printFromPointer(bitstream);
    
}

int WinMain() {
    CAN_FRAME frame = {10,20,43,52};
    CAN_FRAME * frame_pointer = &frame;
    can_mac_tx_frame(frame_pointer);
}
