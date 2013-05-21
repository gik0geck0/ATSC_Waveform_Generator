#include <cstdio>
#include <cstdlib>
#include <cstdint>

typedef uint8_t byte
typedef bool bit

uint8_t gmul(uint8_t a, uint8_t b);
uint8_t gsub(uint8_t a, uint8_t b);
uint8_t gadd(uint8_t a, uint8_t b);

void reed_solomon_parity(vector<bit>* input_stream);

// This is defined specifically for the ATSC standard
byte alphas = {
    174,
    165,
    121,
    121,
    198,
    228,
    22,
    187,
    36,
    69,
    150,
    112,
    220,
    6,
    99,
    111,
    5,
    240,
    186,
    152,
    1
}

int main() {
    uint8_t a = 0x53;
    uint8_t b = 0xCA;

    printf("A ^ D -> %i\n", res);
}

void reed_solomon_parity(vector<bit>* input_stream) {

    // This can be thought of as X.
    // The FIRST element is what's stored in the GATE
    // index = power of X
    byte outputs[21];

    for (int i=0; i < input_stream.size(); i++) {
        output_stream->push_back(input_stream->at(i));
        byte input_byte;
        for (int j = 0; j < 8; j++) {
            byte current_bit = (byte)(input_stream[i+j]);
            current_bit <<= j;
            input_byte |= current_bit;
        }

        solomon_iteration(outputs, input_byte, true);
    }

    for (int i=0; i < 20; i++) {
        byte output = solomon_iteration(outputs, 0, false);
        printf("Output parity byte: %i\n", output);
    }
}

byte solomon_iteration(byte outputs[21], byte input_byte, bool gate_open) {
    if (gate_open) {
        // The GATE is open, so the feedback is active
        outputs[20] = gadd(outputs[19], input_byte);
    }

    for (int i=1; i < 21; i++) {
        if (i > 0) {
            outputs[i] = gadd(outputs[i-1], gmul(alphas[i], outputs[0]));
        } else {
            outputs[i] = gmul(alphas[i], outputs[0]);
        }
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
