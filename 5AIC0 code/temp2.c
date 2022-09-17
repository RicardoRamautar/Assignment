#include <stdlib.h>
#include <stdio.h>    //Can we add this library?
#include <stdint.h>
#include <xil_printf.h>
#include <timers.h>
#include <string.h>    //Can we add this library?
#include "can_phy.h"
#include "can_mac.h"

#define MAX_BIT_SIZE 135
#define STUFFING_COUNT 6
#define DEBUG 0         //Turn debugging on or off (extra prints etc.)


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
    // i+3 instead of i+1, because apparently i was overwriting un-malloced space which caused the free() error

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

    CAN_SYMBOL* bin = (CAN_SYMBOL *) malloc((new_size+2)*sizeof(CAN_SYMBOL));
    // I got a reallocation error, so i decided to just use malloc

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

CAN_SYMBOL* calc_crc(CAN_SYMBOL* message) {
    int generator[] = {1,1,0,0,0,1,0,1,1,0,0,1,1,0,0,1};
    int msg_length = message[0];

    int concat_message[msg_length+15];
    memset( concat_message, 0, (msg_length+15)*sizeof(int));
    
    for(int i=1; i<=msg_length+15; i++) {
        concat_message[i-1] = message[i];
        if(i>msg_length) {
            concat_message[i-1] = 0;
        }
    }

    int curr = 0;
    int gen_curr = 0;
    int temp = 0;
    int shift = 0;
    while(curr < msg_length+15) {
        concat_message[curr]  ^= generator[gen_curr];

        if(concat_message[curr] == 0 && shift == 0) {
            temp++;
        }else if(concat_message[curr] == 1 && shift == 0) {
            shift = 1;
        }

        curr++;
        gen_curr++;

        if(gen_curr > 15) {
            if(temp >= msg_length) {
                break;
            }
            curr = temp;
            gen_curr = 0;
            shift = 0;
        }
    }

    CAN_SYMBOL* res = (CAN_SYMBOL *) malloc(15*sizeof(CAN_SYMBOL));

    int l = 0;
    for(int k=msg_length; k<msg_length+15; k++) {
        res[l] = concat_message[k];
        l++;
    }

    return res;
}

void printFromPointer(CAN_SYMBOL * pointer) {
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        printf("%d - ", *(pointer + i));
    }
    printf("\n");
}

void sendBinaryStream(CAN_SYMBOL* stream) {
    if (DEBUG) printf("Sending bitstream\n");
 
    CAN_SYMBOL last_bit = !stream[1];         //Start value != to first bit for successive counter
    int successive = 1;
    int max_index = stream[0] - 32;     //Max index to do bit stuffing to (exclude 32 bit CRC)
    for (int i = 1; i <= stream[0]; i++) {
        if (stream[i] == last_bit) successive++;
        else successive = 1;
        
        if (i <= max_index && successive > STUFFING_COUNT) {        //Double check i<= max_index
            can_phy_tx_symbol(!last_bit);
             
             if (DEBUG) {  
                 printf(",");
                if (last_bit) putchar('0');       
                else putchar('1');                 
                printf(",");
             }
             
            last_bit = !last_bit;
            successive = 1;
            i--;                        //Decrease i to not skip the current bit in stream[i]
            
        } else {
            can_phy_tx_symbol(stream[i]);
            last_bit = stream[i];
            
            if (DEBUG) {
                if (stream[i]) putchar('1');      //TEMP
                else putchar('0');                 //TEMP
            }
            
        }
    }  
}

void can_mac_tx_frame(CAN_FRAME* txFrame) 
{

	printFromPointer(dec_to_bin(txFrame->ID));
	printFromPointer(dec_to_bin(txFrame->DLC));
	printFromPointer(dec_to_bin(txFrame->Data));
	printFromPointer(dec_to_bin(txFrame->CRC));
	
    CAN_SYMBOL* id_bin = dec_to_bin(txFrame->ID);
    CAN_SYMBOL* dlc_bin = dec_to_bin(txFrame->DLC);
    CAN_SYMBOL* data_bin = dec_to_bin(txFrame->Data);
    CAN_SYMBOL* crc_bin = dec_to_bin(txFrame->CRC);
    CAN_SYMBOL* res =  concatenate_binary(id_bin, dlc_bin);
    CAN_SYMBOL* res2 =  concatenate_binary(res, data_bin);
    CAN_SYMBOL* res3 =  concatenate_binary(res2, crc_bin);
    
	printFromPointer(res3);
    sendBinaryStream(res3);
    
}

void can_mac_rx_frame(CAN_FRAME* rxFrame)
{
  /* must be implemented by students */
  
}
