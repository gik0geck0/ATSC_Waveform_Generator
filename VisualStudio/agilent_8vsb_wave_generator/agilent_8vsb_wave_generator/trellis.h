#pragma once
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <stdint.h>

using namespace std;

typedef bool bit;
const static int SYMBOL_DELAY = 12; // 12 symbol delay in trellis encoding
const static int BIT_STREAM_LENGTH = 1656; // number of bits in 207 bytes (187 from transport stream + 20 Reed solomon)
const static int RESULTING_BIT_STREAM_LENGTH = 2484;//number of bits that the resulting bit stream should have(1656 * 3/2)
const static int BITS_PER_VOLT = 3;
const static int BITS_PER_SYMBOL = 2;

/*
 A symbol is defined as 2 bits by ATSC standard before trellis encoding
 */
struct symbol {		
	bit ms; //Most significant bit
	bit ls;	//Least significant bit
};


//This is the struct containing the output bits from the trellis encoder
struct outputBits {
	bit z2;
	bit z1;
	bit z0;
};

//returns the symbol pointer from stream
//	Precondition: bit stream is a multiple of 2
//	Postcondition: pointer to symbol on heap returned
symbol* getSymbol(vector<bit> *bitStream, int symbolCounter);



/*
	The differentialEncoder function performs the operation defined in the A/53 part 2 page 15 differential encoder
       	The function modifies the most significant bit of the incoming symbol by XOR'ing it with the output of the symbol buffer D1

	Precondition:None
	Postcondition:X has had its most significant bit modified. Least significant bit unmodified

*/
void differentialEncoder(symbol* x, vector<bit>* D1, int symbolCounter);



/*
	The convolutionEncoder function peforms the Convolution encoding as defined in the A/53 part 2 page 15 convolution encoder
	This function will return a pointer to a struct containing the output bits

	Precondition:None
        Postcondition:Return a pointer to a struct made on the heap. 	
*/
outputBits* convolutionEncoder(symbol* y, vector<bit>* D2, vector<bit>* D3, int symbolCounter);


/*
	This function will convert 3 bits into a voltage level as defined by the ATSC A/53 part 2 document

	Precondition: bitStream is a multiple of 3
	Postcondition: new vector of int8_t is produced and a pointer to it is returned.

*/
vector<int8_t>* bitsToLevel(vector<bit>* bitStream);

/*
	This function is the Master function. One call to this will preform all the necessary steps and modify the bitstream with the trellis encoding, returning a vector of int8_t

	Precondition:Bit steam is a multiple of 2
	Postcondition:The bit stream is altered with the trellis encoding done.
*/
vector<int8_t>* trellisEncoder(vector<bit>* bitStream);
