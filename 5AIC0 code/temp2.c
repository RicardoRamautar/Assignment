#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef enum {DOMINANT = 0, RECESSIVE = 1, UNSPECIFIED = 2} CAN_SYMBOL;
typedef enum {SYNC_SEG, PROP_SEG, PHASE_SEG_1, PHASE_SEG_2} BIT_SEGMENT;
typedef struct _can_frame {
  uint32_t ID;
  uint32_t DLC;
  uint64_t Data;
  uint32_t CRC;
} CAN_FRAME;

int convert_bin_to_dec(CAN_SYMBOL* stream) {
  int size = stream[0];
  
  uint64_t ID_bin = 0;
  for (int i=1; i <= size; i++) {
    ID_bin = 10 * ID_bin + stream[i];
  }
  
  int dec = 0, i = 0, temp;

  while (ID_bin!=0) {
    temp = ID_bin % 10;
    ID_bin /= 10;
    dec += temp * pow(2, i);
    ++i;
  }

  return dec;
} 

CAN_FRAME* return_frame() {
    CAN_SYMBOL ID_stream[5] = {4,1,2,0,0};
    CAN_SYMBOL DLC_stream[5] = {4,1,0,0,0};
    CAN_SYMBOL Data_stream[5] = {4,1,0,0,1};
    CAN_SYMBOL CRC_stream[5] = {4,1,0,1,0};

    CAN_FRAME* rxFrame = (CAN_FRAME*) malloc (sizeof(CAN_FRAME));
    rxFrame->ID = convert_bin_to_dec(ID_stream);
    rxFrame->DLC = convert_bin_to_dec(DLC_stream);
    rxFrame->Data = convert_bin_to_dec(Data_stream);
    rxFrame->CRC = convert_bin_to_dec(CRC_stream);

    return rxFrame;
}

int main(){
    CAN_FRAME* frame = return_frame;

    printf("%d \n", frame->ID);
    printf("%d \n", frame->ID);
    printf("%d \n", frame->ID);
    printf("%d", frame->ID);

}