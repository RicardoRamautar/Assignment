#include <stdlib.h>
#include <stdint.h>
#include <xil_printf.h>
#include <timers.h>
#include "can_phy.h"
#include "can_mac.h"

#define MAX_BIT_SIZE 135
#define STUFFING_COUNT 6
#define DEBUG 0         //Turn debugging on or off (extra prints etc.)


CAN_SYMBOL * dec_to_bin(uint64_t number) {
    uint64_t temp[64];
    xil_printf("Line: 16, dec_to_bin\n");
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
            xil_printf("ERROR: Binary not 1 or 0! BIG PROBLEM!\n");
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
    free(bin2);
}

void printFromPointer(CAN_SYMBOL * pointer) {
    xil_printf("Array from pointer : ");
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        xil_printf("%d - ", *(pointer + i));
    }
    xil_printf("\n");
}

CAN_SYMBOL* append_to_array(CAN_SYMBOL* main, int fill, CAN_SYMBOL* sec, int idx) {
    for(int i=0; i<fill; i++) {
        main[idx] = DOMINANT;
        idx++;
    }

    for(int i=1; i<=sec[0]; i++) {
        main[idx] = sec[i];
        idx++;
    }
    
    return main;
}

CAN_SYMBOL* built_binary_frame(CAN_SYMBOL* id, CAN_SYMBOL* dlc, CAN_SYMBOL* data, int dlc_int) {
    int total_length = 19 + dlc_int*8 + 28;

    CAN_SYMBOL* binary_frame = (CAN_SYMBOL *) malloc(sizeof(CAN_SYMBOL)*(total_length+5));

    binary_frame[0] = total_length;                                                             // length of binary frame

    binary_frame[1] = DOMINANT;                                                                 // SOF

    int bin_frame_idx = 2;

    binary_frame = append_to_array(binary_frame, 11-id[0], id, bin_frame_idx);                  // ID
    bin_frame_idx += 11;

    binary_frame[bin_frame_idx] = DOMINANT;                                                     // RTR
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = DOMINANT;                                                     // IDE
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = DOMINANT;                                                     // ro
    bin_frame_idx++;
    
    binary_frame = append_to_array(binary_frame, 4-dlc[0], dlc, bin_frame_idx);                 // DLC
    bin_frame_idx += 4;
    
    binary_frame = append_to_array(binary_frame, 8*dlc_int-data[0], data, bin_frame_idx);      // DATA
    bin_frame_idx += 8*dlc_int;

    CAN_SYMBOL* crc = calc_crc(binary_frame);
    
    binary_frame = append_to_array(binary_frame, 15-crc[0], crc, bin_frame_idx);                // CRC
    bin_frame_idx += 15;

    // sendBinaryStream(binary_frame);

    binary_frame[bin_frame_idx] = RECESSIVE;                                                    // CRC delimiter bit
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = DOMINANT;                                                     // ACK field 1
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = RECESSIVE;                                                    // ACK field 2
    bin_frame_idx++;

    for(int i=0; i<10; i++) {
        binary_frame[bin_frame_idx] = RECESSIVE;                                                // EOF + INT
        bin_frame_idx++;
    }

    return binary_frame;
}

void sendBinaryStream(CAN_SYMBOL* stream) {
    if (DEBUG) xil_printf("Sending bitstream\n");
 
    CAN_SYMBOL last_bit = !stream[1];         //Start value != to first bit for successive counter
    int successive = 1;
    int max_index = stream[0] - 32;     //Max index to do bit stuffing to (exclude 32 bit CRC)
    for (int i = 1; i <= stream[0]; i++) {
        if (stream[i] == last_bit) successive++;
        else successive = 1;
        
        if (i <= max_index && successive > STUFFING_COUNT) {        //Double check i<= max_index
            can_phy_tx_symbol(!last_bit);
             
             if (DEBUG) {  
                 xil_printf(",");
                if (last_bit) putchar('0');       
                else putchar('1');                 
                xil_printf(",");
             }
             
            last_bit = !last_bit;
            successive = 1;
            i--;                        //Decrease i to not skip the current bit in stream[i]
            
        } else {
            (stream[i]);
            last_bit = stream[i];
            
            if (DEBUG) {
                if (stream[i]) putchar('1');      //TEMP
                else putchar('0');                 //TEMP
            }
            
        }
    }  
}

int testing(int k) {
    return k+1;
}

void can_mac_tx_frame(CAN_FRAME* txFrame) 
{ 
    
    xil_printf("TEST TEST\n\n\n");    //USE xil_printf() OTHERWISE IT CRASHES THE TEST CASES!!!!!!!!!!!!!!!!!!!!

    printFromPointer(dec_to_bin(txFrame->ID));
    printFromPointer(dec_to_bin(txFrame->DLC));
    printFromPointer(dec_to_bin(txFrame->Data));
    printFromPointer(dec_to_bin(txFrame->CRC));

    CAN_SYMBOL* id_bin = dec_to_bin(txFrame->ID);
    CAN_SYMBOL* dlc_bin = dec_to_bin(txFrame->DLC);
    CAN_SYMBOL* data_bin = dec_to_bin(txFrame->Data);
    CAN_SYMBOL* crc_bin = dec_to_bin(txFrame->CRC);

    CAN_SYMBOL* res = built_binary_frame(id_bin, dlc_bin, data_bin, txFrame->DLC);

    printFromPointer(res);

    sendBinaryStream(res); 
    
}

void can_mac_rx_frame(CAN_FRAME* rxFrame)
{
  /* must be implemented by students */
  
}
