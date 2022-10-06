#include <stdlib.h>
#include <stdint.h>
#include <xil_printf.h>
#include <timers.h>
#include "can_phy.h"
#include "can_mac.h"


#define MAX_BIT_SIZE 135
#define STUFFING_COUNT 5
#define DEBUG 1         //Turn debugging on or off (extra prints etc.)
#define RX_BUFFERSIZE 6



void printFromPointer(CAN_SYMBOL * pointer) {
    xil_printf("Array from pointer : ");
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        xil_printf("%d", pointer[i]);
    }
    xil_printf("\n");
}

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
        //xil_printf("temp[%d] = %d,  number: %d\n", i, temp[i], number);
    }

    CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc((i+1)*sizeof(CAN_SYMBOL));

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

    CAN_SYMBOL* res = (CAN_SYMBOL *) malloc(16*sizeof(CAN_SYMBOL));

    int l = 1;
    res[0] = 15;
    for(int k=msg_length; k<msg_length+15; k++) {
        res[l] = concat_message[k];
        l++;
    }

    return res;
}

int check_crc(CAN_SYMBOL* msg) {
    CAN_SYMBOL* crc = calc_crc(msg);

    for(int i=1; i<16; i++) {
        if(crc[i] == 1) {
            xil_printf("INCORRECT CRC\n");
            return 1;
        }
    }
    xil_printf("CORRECT CRC\n");
    return 0;
}

void append_to_array(CAN_SYMBOL* main, int fill, CAN_SYMBOL* sec, int idx) {
    for(int i=0; i<fill; i++) {
        main[idx] = DOMINANT;
        idx++;
    }

    for(int i=1; i<=sec[0]; i++) {
        main[idx] = sec[i];
        idx++;
    }
}

bool sendBinaryStream(CAN_SYMBOL* stream) {
    
    CAN_SYMBOL rx;
         
    CAN_SYMBOL last_bit = !stream[1];         //Start value != to first bit for successive counter
    int successive = 1;
    int max_index = stream[0];     //Max index to do bit stuffing to (exclude 32 bit CRC)
    for (int i = 1; i <= stream[0]; i++) {
        if (stream[i] == last_bit) successive++;
        else successive = 1;
        
        if (i <= max_index && successive > STUFFING_COUNT) {        //Double check i<= max_index
            can_phy_tx_symbol(!last_bit);          //stuffed bit
            can_phy_rx_symbol(&rx);
            
            if (!last_bit != rx) {                //check for collision
                return false;
            }            
             
            last_bit = !last_bit;
            successive = 1;
            i--;                        //Decrease i to not skip the current bit in stream[i]
            
        } else {
            
            CAN_SYMBOL temp = stream[i];
                        
            can_phy_tx_symbol(temp);      //Send data to the bus
            can_phy_rx_symbol(&rx);
            
            if (temp != rx) {                //check for collision
                return false;
            }
                        
            last_bit = stream[i];
            
        }
    }  
    xil_printf("\n");
    return true;
}

CAN_SYMBOL* build_binary_frame(CAN_SYMBOL* id, CAN_SYMBOL* dlc, CAN_SYMBOL* data, int dlc_int) {
    
    printFromPointer(data);
    
    int total_length = 19 + dlc_int*8 + 28;
    xil_printf("\n%d \n", total_length);

    CAN_SYMBOL* binary_frame = (CAN_SYMBOL *) calloc((total_length+1), sizeof(CAN_SYMBOL));
    
    
    
    binary_frame[0] = total_length;                 // length of binary frame
    
    xil_printf("malloc'd binary frame: ");
    printFromPointer(binary_frame);

    binary_frame[1] = DOMINANT;                     // SOF

    int bin_frame_idx = 2;
  
    
    for(int i=0; i<(11-id[0]); i++) {               // fill DX with zeros up to 11 bits
        binary_frame[bin_frame_idx] = DOMINANT;
        bin_frame_idx++;
    }
    
    xil_printf("fill DX: ");
    printFromPointer(binary_frame);
    
    for(int i=1; i<=id[0]; i++) {                   // ID
        binary_frame[bin_frame_idx] = id[i];
        bin_frame_idx++;
    }
    
    free(id);
    
    xil_printf("fill ID: ");
    printFromPointer(binary_frame);
    
    binary_frame[bin_frame_idx] = DOMINANT;         // RTR
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = DOMINANT;         // IDE
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = DOMINANT;         // ro
    bin_frame_idx++;
    
    xil_printf("Added RTR, IDE, ro: ");
    printFromPointer(binary_frame);
    
    for(int i=0; i<(4-dlc[0]); i++) {               // fill DLC with zeros up to 4 bits
        binary_frame[bin_frame_idx] = DOMINANT;
        bin_frame_idx++;
    }
    
    xil_printf("DLC zeroes: ");
    printFromPointer(binary_frame);

    for(int i=1; i<=dlc[0]; i++) {                   // DLC
        binary_frame[bin_frame_idx] = dlc[i];
        bin_frame_idx++;
    }
    
    printFromPointer(dlc);
    free(dlc);
    
    xil_printf("DLC: ");
    printFromPointer(binary_frame);
    xil_printf("dlc_int: %d,  size: %d \n", dlc_int, data[0]);
    
    if (8*dlc_int - data[0] < 0) xil_printf("ERROR! DLC Size NOT SUFFICIENT FOR DATA!\n\n");
    
    for(int i=0; i<(8*dlc_int-data[0]); i++) {      // fill DATA with zeros up to DCL*8 bits
        binary_frame[bin_frame_idx] = DOMINANT;
        bin_frame_idx++;
    }
    
    xil_printf("DATA zeroes: ");
    printFromPointer(binary_frame);
    xil_printf("\n");
    
    for(int i=1; i<=data[0]; i++) {                   // DATA
        binary_frame[bin_frame_idx] = data[i];
        bin_frame_idx++;
        //("index: %d,  data[i]: %d\n", bin_frame_idx, data[i]);
    }
    
    free(data);
    
    xil_printf("Data: ");
    printFromPointer(binary_frame);
    xil_printf("\n");
    

    CAN_SYMBOL* crc = calc_crc(binary_frame);

    for(int i=0; i<(15-crc[0]); i++) {               // fill CRC with zeros up to 15 bits
        binary_frame[bin_frame_idx] = DOMINANT;
        bin_frame_idx++;
    }

    for(int i=1; i<=crc[0]; i++) {                   // CRC
        binary_frame[bin_frame_idx] = crc[i];
        bin_frame_idx++;
    }

    binary_frame[bin_frame_idx] = RECESSIVE;         // CRC delimiter bit
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = DOMINANT;         // ACK field 1
    bin_frame_idx++;

    binary_frame[bin_frame_idx] = RECESSIVE;         // ACK field 2
    bin_frame_idx++;

    for(int i=0; i<10; i++) {
        binary_frame[bin_frame_idx] = RECESSIVE;     // EOF + INT
        bin_frame_idx++;
    }
    printFromPointer(binary_frame);
    return binary_frame;
}

void updateBuffer(CAN_SYMBOL* array, CAN_SYMBOL new_entry) {
	
	for(int i = 1; i <= RX_BUFFERSIZE; i++) {
		array[i] = array[i-1];
	}
	
	array[0] = new_entry;
}

bool can_mac_tx_frame(CAN_FRAME* txFrame) 
{ 
    
  //USE xil_printf() OTHERWISE IT CRASHES THE TEST CASES!!!!!!!!!!!!!!!!!!!!
    
    CAN_SYMBOL* id_bin = dec_to_bin(txFrame->ID);
    CAN_SYMBOL* dlc_bin = dec_to_bin(txFrame->DLC);
    CAN_SYMBOL* data_bin = dec_to_bin(txFrame->Data);
    //CAN_SYMBOL* crc_bin = dec_to_bin(txFrame->CRC);      When do you use this?
    
    /*
    printFromPointer(id_bin);
    printFromPointer(dlc_bin);
    printFromPointer(data_bin);
    */
    
    
    
    
    CAN_SYMBOL* bitstream = build_binary_frame(id_bin, dlc_bin, data_bin, txFrame->DLC);
    
    printFromPointer(bitstream);
    
    
    
    //CAN_SYMBOL tempstream[25] = {DOMINANT, DOMINANT, RECESSIVE,DOMINANT, DOMINANT, RECESSIVE,DOMINANT, DOMINANT, RECESSIVE,DOMINANT, DOMINANT, RECESSIVE,DOMINANT, DOMINANT, RECESSIVE};
    
    if (!sendBinaryStream(bitstream)) {
        return false;
    }
    
    xil_printf("\n");
    return true;
}

void can_mac_rx_frame(CAN_FRAME* rxFrame)
{
  /* must be implemented by students */
  xil_printf("rxFrame function \n\n");
  
  CAN_SYMBOL bit_memory[RX_BUFFERSIZE];
  CAN_SYMBOL symbol;
  
  
  for (int i=0; i< 111; i++) {
      can_phy_rx_symbol(&symbol);
      xil_printf("%d", symbol);
      
  }
  xil_printf("\n\n");
  
  //updateBuffer(bit_memory, symbol);
  
  
  
}
