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
        printf("%d", *(pointer + i));
    }
    printf("\n");
}

void checkEqual(CAN_SYMBOL* arr, CAN_SYMBOL* solution) {
    for(int i=1; i<=arr[0]; i++) {
        if(arr[i] != solution[i]) {
            printf("False\n");
            break;
        }
    }
    printf("True\n");
}

CAN_SYMBOL* remove_bitstuffing(CAN_SYMBOL* arr) {
    int prev = arr[1];
    int counter = 1;
    int length_counter = 1;
    int eof_check = arr[1];

    CAN_SYMBOL* eof_detected = (CAN_SYMBOL* ) malloc(2*sizeof(CAN_SYMBOL));
    eof_detected[0] = 1;
    eof_detected[1] = 0;

    CAN_SYMBOL new[135];

    new[0] = arr[0];
    new[1] = arr[1];

    for(int i=2; i<=arr[0]; i++) {
        if(arr[i] == 1) eof_check++; else eof_check=0;
        if(eof_check >= 7) return eof_detected;

        if(arr[i] == prev) {
            counter++;
        } else {
            counter = 1;
            prev = !prev;
        }

        if(counter <= 5) {
            length_counter++;
            new[length_counter] = arr[i];
        }

        if(counter >= 5) {
            if(arr[i+1] == 1) eof_check++; else eof_check=0;
            counter = 1;
            i++;
            prev = !prev;
        }
    }

    CAN_SYMBOL* result = (CAN_SYMBOL* ) malloc((length_counter+1)*sizeof(CAN_SYMBOL));

    result[0] = length_counter;

    for(int j=1; j<=length_counter; j++) {
        result[j] = new[j];
    }

    return result;
}

CAN_SYMBOL* remove_bitstuffing_scanf() {
    CAN_SYMBOL rx;
    printf("Enter a bit: ");
    scanf("%d", &rx); 

    int prev = rx;
    int counter = 1;
    int length_counter = 1;
    int eof_check = rx;

    CAN_SYMBOL* eof_detected = (CAN_SYMBOL* ) malloc(2*sizeof(CAN_SYMBOL));
    eof_detected[0] = 1;
    eof_detected[1] = 0;

    CAN_SYMBOL new[135];

    new[1] = rx;

    for(int i=1; i<135; i++) {
        printf("Enter a bit: ");
        scanf("%d", &rx); 

        if(rx == 2) break;

        if(rx == 1) eof_check++; else eof_check=0;
        if(eof_check >= 7) return eof_detected;

        if(rx == prev) {
            counter++;
        } else {
            counter = 1;
            prev = !prev;
        }

        if(counter <= 5) {
            length_counter++;
            new[length_counter] = rx;
        }

        if(counter >= 5) {
            printf("Enter a bit: ");
            scanf("%d", &rx); 
            if(rx == 1) eof_check++; else eof_check=0;

            counter = 1;
            i++;
            prev = !prev;
        }
    }

    CAN_SYMBOL* result = (CAN_SYMBOL* ) malloc((length_counter+1)*sizeof(CAN_SYMBOL));

    result[0] = length_counter;

    for(int j=1; j<=length_counter; j++) {
        result[j] = new[j];
    }

    return result;
}

int main() {
    // CAN_SYMBOL arr[2][19] = {
    //     {18,1,1,1,1,1,0,1,0,0,0,0,0,1,1,1,1,1,0},
    //     {18,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,0,1}
    // };

    // CAN_SYMBOL solutions[2][16] = {
    //     {15,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1},
    //     {15,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1}
    // };

    // for(int i=0; i<2; i++) {
    //     CAN_SYMBOL* result = remove_bitstuffing(arr[i]);
    //     checkEqual(result, solutions[i]);
    // }

    // CAN_SYMBOL eof[8] = {7,1,1,1,1,1,1,1};
    // CAN_SYMBOL* res = remove_bitstuffing(eof);
    // printFromPointer(res);

    CAN_SYMBOL* res = remove_bitstuffing_scanf();
    printFromPointer(res);

}