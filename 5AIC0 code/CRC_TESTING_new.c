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

void printFromPointer(CAN_SYMBOL * pointer) {
    int size = *pointer;
    for(int i = 1; i <= size; i++) {
        printf("%d", pointer[i]);
    }
    printf("\n");
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
    }

    CAN_SYMBOL * binary = (CAN_SYMBOL*) malloc((i+1)*sizeof(CAN_SYMBOL));

    binary[0] = i;

    for (int j = 0; j < i; j++) {
        if(temp[j] == 0) {
            binary[i - j] = DOMINANT;
        } else if(temp[j] == 1) {
            binary[i - j] = RECESSIVE;
        } else {
            printf("ERROR: Binary not 1 or 0! BIG PROBLEM!\n");
        }
    }

    return binary;
}

CAN_SYMBOL* calc_crc(CAN_SYMBOL* message) {
    int generator[] = {1,1,0,0,0,1,0,1,1,0,0,1,1,0,0,1};
    int msg_length = message[0];

    int concat_message[msg_length+15];
    memset(concat_message, 0, (msg_length+15)*sizeof(int));
    
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

void check_answer(CAN_SYMBOL* answer, CAN_SYMBOL* result) {
    int check = 0;
    for(int i=1; i<=15; i++) {
        if(answer[i-1] != result[i]) {
            printf("WRONG \n");
            printf("index: %d \ncorrect: %d \nresult: %d", i, answer[i], result[i]);
            check = 1;
            break;
        }
    }

    if(check == 0) printf("CORRECT\n");
}

void testing() {
    CAN_SYMBOL* message = dec_to_bin(98651);
    CAN_SYMBOL* res = calc_crc(message);
    CAN_SYMBOL answer[15] = {0,1,0,1,1,0,1,0,1,0,1,1,1,1,1};
    check_answer(answer, res);

    CAN_SYMBOL* message2 = dec_to_bin(337030483);
    CAN_SYMBOL* res2 = calc_crc(message2);
    CAN_SYMBOL answer2[15] = {1,1,1,0,0,0,0,1,1,1,0,1,1,0,0};
    check_answer(answer2, res2);

    CAN_SYMBOL* message3 = dec_to_bin(15337727443532368772);
    CAN_SYMBOL* res3 = calc_crc(message3);
    CAN_SYMBOL answer3[15] = {1,1,0,0,1,0,0,0,1,1,1,0,0,0,0};
    check_answer(answer3, res3);

    CAN_SYMBOL* message4 = dec_to_bin(12398654147350661206);
    CAN_SYMBOL* res4 = calc_crc(message4);
    CAN_SYMBOL answer4[15] = {0,0,1,0,0,0,1,0,0,1,0,0,0,0,0};
    check_answer(answer4, res4);

    CAN_SYMBOL* message5 = dec_to_bin(9119675365003015960);
    CAN_SYMBOL* res5 = calc_crc(message5);
    CAN_SYMBOL answer5[15] = {1,1,1,1,1,1,1,1,0,1,1,1,0,1,0};
    check_answer(answer5, res5);

    CAN_SYMBOL* message6 = dec_to_bin(4163797588621573825);
    CAN_SYMBOL* res6 = calc_crc(message6);
    CAN_SYMBOL answer6[15] = {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0};
    check_answer(answer6, res6);

    CAN_SYMBOL* message7 = dec_to_bin(17854790980495639815);
    CAN_SYMBOL* res7 = calc_crc(message7);
    CAN_SYMBOL answer7[15] = {1,0,0,0,0,1,1,0,1,1,1,0,1,1,1};
    check_answer(answer7, res7);

    CAN_SYMBOL* message8 = dec_to_bin(13971574139096012541);
    CAN_SYMBOL* res8 = calc_crc(message8);
    CAN_SYMBOL answer8[15] = {1,0,0,1,0,0,0,1,0,0,0,0,1,1,1};
    check_answer(answer8, res8);

    CAN_SYMBOL* message9 = dec_to_bin(3633767881898878188);
    CAN_SYMBOL* res9 = calc_crc(message9);
    CAN_SYMBOL answer9[15] = {0,1,0,1,0,0,1,1,1,1,1,0,0,0,0};
    check_answer(answer9, res9);
}

int main() {
    testing();
}
