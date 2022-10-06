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
    //  Receive first bit
    CAN_SYMBOL rx;
    printf("Enter a bit: "); scanf("%d", &rx); 

    //  initialize variables
    CAN_SYMBOL prev = rx, eof_check = rx, temp[135];
    int counter = 1, length_counter = 1;

    //  Array that is returned when EOF has been detected
    CAN_SYMBOL* eof_detected = (CAN_SYMBOL* ) malloc(2*sizeof(CAN_SYMBOL));
    eof_detected[0] = 1; eof_detected[1] = 0;

    //  Insert first element into temporary array
    temp[1] = rx;

    for(int i=1; i<135; i++) {
        printf("Enter a bit: ");  scanf("%d", &rx); 

        // For testing : when inputting 2, function will stop and return array
        if(rx == 2) break;

        //  Increment "eof_check" if 1 bit is received, otherwise set "eof_check" to zero
        if(rx == 1) eof_check++; else eof_check=0;

        //  Check if EOF has been received
        if(eof_check >= 7) return eof_detected;

        //  Increment counter if curr==prev, otherwise set counter to 1 and update "prev"
        if(rx == prev) {
            counter++;
        } else {
            counter = 1;
            prev = !prev;
        }

        //  Add to temporary array and increment "length_counter", tracking its current index
        if(counter <= 5) {
            length_counter++;
            temp[length_counter] = rx;
        }

        if(counter >= 5) {
            //  Check for possible EOF bits
            printf("Enter a bit: "); scanf("%d", &rx); 
            if(rx == 1) eof_check++; else eof_check=0;

            //  Set counter back to 1, skip a bit, update "prev"
            counter = 1;
            i++;
            prev = !prev;
        }
    }

    CAN_SYMBOL* result = (CAN_SYMBOL* ) malloc((length_counter+1)*sizeof(CAN_SYMBOL));

    result[0] = length_counter;

    for(int j=1; j<=length_counter; j++) {
        result[j] = temp[j];
    }

    return result;
}

void test(int i) {
    if(i==1) {
        CAN_SYMBOL arr[19] = {18,1,1,1,1,1,0,1,0,0,0,0,0,1,1,1,1,1,0};
        CAN_SYMBOL solution[16] = {15,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1};
        CAN_SYMBOL* result = remove_bitstuffing(arr);
        checkEqual(result, solution);
    }

    if(i==2) {
        CAN_SYMBOL arr[19] = {18,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,0,1};
        CAN_SYMBOL solution[16] = {15,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1};
        CAN_SYMBOL* result = remove_bitstuffing(arr);
        checkEqual(result, solution);
    }

    if(i==3) {
        CAN_SYMBOL eof[8] = {7,1,1,1,1,1,1,1};
        CAN_SYMBOL* res = remove_bitstuffing(eof);
        printFromPointer(res);
    }
}

int main() {
    test(1);
    test(2);
    test(3);

    // CAN_SYMBOL* res = remove_bitstuffing_scanf();
    // printFromPointer(res);

}