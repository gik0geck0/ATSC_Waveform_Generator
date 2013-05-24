#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <vector>
#include "common.cpp"

typedef uint8_t byte;
typedef bool bit;

uint8_t gmul(uint8_t a, uint8_t b);
uint8_t gsub(uint8_t a, uint8_t b);
uint8_t gadd(uint8_t a, uint8_t b);

void reed_solomon_parity(std::vector<bit>* input_stream);
byte solomon_iteration(byte outputs[21], byte input_byte, bool gate_open);

// This is defined specifically for the ATSC standard
byte alphas[] = { 174, 165, 121, 121, 198, 228, 22, 187, 36, 69, 150, 112, 220, 6, 99, 111, 5, 240, 186, 152, 1 };

/*
int main() {
    uint8_t a = 0x53;
    uint8_t b = 0xCA;

    bool arr[] = { 0,1,0,1,0,1,0,1, 0,1,0,1,1,1,0,1, 1,1,0,1,0,0,0,1 };
    std::vector<bit> data_stream;
    for (int i=0; i < 24; i++) {
        data_stream.push_back(arr[i]);
    }

    reed_solomon_parity(&data_stream);
    printf("Data Bytes {\n");
    for (int i=0; i < data_stream.size(); i+=8) {
        for (int j=0; j < 8; j++) {
            printf("%i, ", (int) data_stream.at(i+j));
        }
        printf("\n");
    }
    printf("}\n");
}
*/

void test_reed_solomon() {
    
}

/*
 *  Takes a segment of bits, and appends Reed-Solomon parity bits to the end
 *  Length of Input vector is expected to be 187 bytes ( 1496 bits )
 */
void reed_solomon_parity(std::vector<bit>* input_bits) {
    
    /*
    if (input_bits->size() != 1496) {
        printf("Reed Solomon Parity for ATSC is expected to apply to a single segment (1496 bits).\n");
        printf("Expected Input Length: 1496, but got %i\n", input_bits->size());
        exit(-1);
    }
    */

    // This can be thought of as X.
    // The FIRST element is what's stored in the GATE
    // index = power of X
    byte outputs[21];
    for (int i=0; i < 22; i++) {
        outputs[i] = 0;
    }

    std::vector<byte>* input_bytes = makeBytesFromBits(input_bits);

    for (int i=0; i < input_bytes->size(); i++) {

        solomon_iteration(outputs, (*input_bytes)[i], true);

        printf("X array: { ");
        for (int j=0; j < 21; j++) {
            printf("%i, ", outputs[j]);
        }
        printf("}\n");
        
    }

    // outputs now contains the parity bytes
    std::vector<byte> parity_bytes;
    for (int i=1; i < 21; i++) {
        parity_bytes.push_back(outputs[i]);
    }

    std::vector<bit>* parity_bits = makeBitsFromBytes(&parity_bytes);
    
    for ( int i=0; i < parity_bits->size(); i++) {
        input_bits->push_back(parity_bits->at(i));
    }
}

/*
 *  Performs a single modification to the output bytes, given an input byte.
 *  Originally, it was intended to work after the input bytes have been read in, but I'm not sure it's necesary anymore
 */
byte solomon_iteration(byte outputs[21], byte input_byte, bool gate_open) {
    if (gate_open) {
        // The GATE is open, so the feedback is active
        outputs[0] = gadd(outputs[19], input_byte);
    } else {
        printf("Resetting gate to 0\n");
        outputs[0] = 0;
    }

    for (int i=21; i > 0; i--) {
        /*
        printf("%i: ", i);
        printf("%i -> ", outputs[i]);
        */
        if (i > 1) {
            outputs[i] = gadd(outputs[i-1], gmul(alphas[i], outputs[0]));
        } else {
            outputs[i] = gmul(alphas[i], outputs[0]);
        }
        /*
        printf("%i\n", outputs[i]);
        */
    }


    // the last output will be output. X^20 is output
    return outputs[20];
}

// Comes from wikipedia page on Finite Field Arithmetic
//
/* Add two numbers in a GF(2^8) finite field */
uint8_t gadd(uint8_t a, uint8_t b) {
        return a ^ b;
}
 
/* Subtract two numbers in a GF(2^8) finite field */
uint8_t gsub(uint8_t a, uint8_t b) {
        return a ^ b;
}
 
/* Multiply two numbers in the GF(2^8) finite field defined 
 * by the polynomial x^8 + x^4 + x^3 + x + 1 */
uint8_t gmul(uint8_t a, uint8_t b) {
        uint8_t p = 0;
        uint8_t counter;
        uint8_t carry;
        for (counter = 0; counter < 8; counter++) {
                if (b & 1) 
                        p ^= a;
                carry = (a & 0x80);
                a <<= 1;
                if (carry) 
                        a ^= 0x011D; /* what x^8 is modulo x^8 + x^4 + x^3 + x^2 + 1 */
                b >>= 1;
        }
        return p;
}
