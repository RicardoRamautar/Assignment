#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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

void print_array(int msg[15], int x) {
    if(x==0){
        for(int i=0; i<11; i++) {
            printf("%d", msg[i]);
        }   
    }
}

CAN_SYMBOL* calc_crc(CAN_SYMBOL* message, int generator[]) {
    int msg_length = message[0];
    // int concat_message[15] = {0};
    int concat_message[msg_length+3];
    memset( concat_message, 0, (msg_length+3)*sizeof(int));
    
    for(int i=1; i<=msg_length; i++) {
        concat_message[i-1] = message[i];
    }
    concat_message[msg_length+1] = 0; concat_message[msg_length+2] = 0; concat_message[msg_length+3] = 0;

    int curr = 0;
    int gen_curr = 0;
    int temp = 0;
    int shift = 0;
    while(curr < msg_length+3) {
        printf("curr : %d \n", curr);
        printf("gen_curr : %d \n", gen_curr);
        printf("temp : %d \n", temp);

        concat_message[curr]  ^= generator[gen_curr];

        if(concat_message[curr] == 0 && shift == 0) {
            temp++;
        }else if(concat_message[curr] == 1 && shift == 0) {
            shift = 1;
        }

        curr++;
        gen_curr++;

        // if(curr >= msg_length+4) {
        //     print_array(concat_message,0);
        //     printf("\n");
        //     break;
        // }

        if(gen_curr > 3) {
            if(temp >= msg_length) {
                print_array(concat_message,0);
                printf("\n -------------------- \n");
                break;
            }
            curr = temp;
            gen_curr = 0;
            shift = 0;
        }

        print_array(concat_message,0);
        printf("\n1101");
        printf("\n \n");
    }

    CAN_SYMBOL* res = (CAN_SYMBOL *) malloc(3*sizeof(CAN_SYMBOL));

    int l = 0;
    for(int k=msg_length; k<msg_length+3; k++) {
        res[l] = concat_message[k];
        l++;
    }

    return res;
}

int WinMain() {
    int generator[] = {1,1,0,1};
    CAN_SYMBOL* message = dec_to_bin(154);
    printFromPointer(message);

    CAN_SYMBOL* crc = calc_crc(message, generator);
    for(int i=0; i<3; i++) {
        printf("%d", crc[i]);
    }
}