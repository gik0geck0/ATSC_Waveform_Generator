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

void add_reed_solomon_parity(std::vector<byte>* input_bytes);
std::vector<std::vector<byte>*>* divide_into_segments(std::vector<byte>* field_bytes);
byte solomon_iteration(byte outputs[21], byte input_byte, bool gate_open);

int main() {

}

// This is defined specifically for the ATSC standard
byte alphas[] = { 174, 165, 121, 121, 198, 228, 22, 187, 36, 69, 150, 112, 220, 6, 99, 111, 5, 240, 186, 152, 1 };

/*
 *  Takes a segment of bits, and appends Reed-Solomon parity bits to the end
 *  Length of Input vector is expected to be 187 bytes ( 1496 bits )
 *
 *  Input bytes will contain an entire data-field (312 segments) of bytes
 */
void add_reed_solomon_parity(std::vector<byte>* input_bytes) {

    std::vector<std::vector<byte>*>* field_segments = divide_into_segments(input_bytes);
    delete input_bytes;

    for (int i=0; i < field_segments->size(); i++) {

        // This can be thought of as X.
        // The FIRST element is what's stored in the GATE
        // index = power of X
        byte outputs[21];
        for (int i=0; i < 22; i++) {
            outputs[i] = 0;
        }

        for (int j=0; j < field_segments->at(i)->size(); j++) {

            solomon_iteration(outputs, field_segments->at(i)->at(j), true);

            /*
            printf("X array: { ");
            for (int j=0; j < 21; j++) {
                printf("%i, ", outputs[j]);
            }
            printf("}\n");
            */
        }

        // outputs now contains the parity bytes
        for (int j=1; j < 22; j++) {
            field_segments->at(i)->push_back(outputs[j]);
        }
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

std::vector<std::vector<byte>*>* divide_into_segments(std::vector<byte>* field_bytes) {
    std::vector<std::vector<byte>*>* field_segments = new std::vector<std::vector<byte>*>();
    int num_segments = field_bytes->size()/187;
    if (num_segments < 312) {
        printf("Warning:    Reed-solomon has come across an incomplete data-field. Expected 312 segments, but got %i\n", num_segments);
    }

    for (int i=0; i < num_segments; i++) {
        std::vector<byte>* my_segment = new std::vector<byte>();
        
        for (int j=0; j < 187; j++) {
            my_segment->push_back(field_bytes->at(i*j));
        }

        field_segments->push_back(my_segment);
    }
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
