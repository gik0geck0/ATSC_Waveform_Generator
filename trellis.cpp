#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

typedef bool bit;
const static int SYMBOL_DELAY = 12; // 12 symbol delay in trellis encoding
const static int BIT_STREAM_LENGTH = 1656; // number of bits in 207 bytes (187 from transport stream + 20 Reed solomon)
const static int RESULTING_BIT_STEAM_LENGTH = 2484;//number of bits that the resulting bit stream should have(1656 * 3/2)

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
//Precondition: bit stream is a multiple of 2
//Postcondition: pointer to symbol on heap returned
symbol* getSymbol(vector<bit> bitStream, int symbolCounter);



/*
	The differentialEncoder function performs the operation defined in the A/53 part 2 page 15 differential encoder
       	The function modifies the most significant bit of the incoming symbol by XOR'ing it with the output of the symbol buffer D1

	Precondition:None
	Postcondition:X has had its most significant bit modified. Least significant bit unmodified

*/
void differentialEncoder(symbol* x, vector<bit> D1, int symbolCounter);



/*
	The convolutionEncoder function peforms the Convolution encoding as defined in the A/53 part 2 page 15 convolution encoder
	This function will return a pointer to a struct containing the output bits

	Precondition:None
        Postcondition:Return a pointer to a struct made on the heap. 	
*/
outputBits* convolutionEncoder(symbol* y, vector<bit> D2, vector<bit> D3, int symbolCounter);

/*
	This function is the Master function. One call to this will preform all the necessary steps and modify the bitstream with the trellis encoding

	Precondition:Bit steam is a multiple of 2
	Postcondition:The bit stream is altered with the trellis encoding done.
*/
vector trellisEncoder(vector<bit> bitStream);

void trellisEncoder(vector<bit>* bitStream){
	//check for a valid bitStream size
	if(bitStream.size()%2){
		cout << "In function trellisEncoder: Not given a bit stream whose length is a multiple of 2.\nGiven bit stream length is " << bitStream.size() << "\nAborting" << endl;
		exit(0);
	}
	//A warning for incorrect bitStream size, will still proceede even if wrong however
	if(bitStream.size() != BIT_STREAM_LENGTH){
		cout << "In function trellisEncoder: Unexpected length of bit stream.\nExpected length of " << BIT_STEAM_LENGTH << "\nGot length of " << bitStream.size() << "\nContiuing with function, output may be improper\n" << endl;
	}
	//initilization block
	int symbolCounter = 0;
	vector<bit> D1;
	vector<bit> D2;
	vector<bit> D3;
	vector<bit>* newBitStream = new vector<bit>;
	outputBits* output;
	symbol* currentSymbol;
	//push 0's onto every location of symbol buffer
	for(i = 0; i < SYMBOL_DELAY; i++){
		D1.push_back(0);
		D2.push_back(0);
		D3.push_back(0);
	}
	for(int i = 0; i < bitStream.size(); i++){
		currentSymbol = getSymbol(bitStream*, symbolCounter);
		differentialEncoder(currentSymbol, D1, symbolCounter);
		output = convolutionEncoder(currentSymbol, D2, D3, symbolCounter);
		newBitStream->push_back(output->z2);
		newBitStream->push_back(output->z1);
		newBitStream->push_back(output->z0);
		symbolCounter++;
		delete output;
		delete currentSymbol;
		output=NULL;
		currentSymbol=NULL:
	}

	//deconstructors
	delete currentSymbol;
	currentSymbol = NULL;
	delete output;
	output = NULL;

	if(newBitStream.size() != RESULTING_BIT_STREAM_LENGTH){
		cout << "In function trellisEncoder: Unexpected resulting bit stream length\nExpected " << RESULTING_BIT_STREAM_LENGTH << "\nGot " << newBitStream.size()<< endl;
	}
	delete bitStream; //remove old data
	bitStream = newBitStream;
}

symbol* getSymbol(vector<bit> bitStream, int symbolCounter){
	symbol* outputSymbol = new symbol;
	outputSymbol->ms = bitStream[symbolCounter*2];
	outputSymbol->ls = bitStream[symbolCounter*2+1];	
	return outputSymbol;
}

/*
	The DifferentialEncoder function performs the operation defined in the A/53 part 2 page 15 differential encoder
       	The function modifies the most significant bit of the incoming symbol by XOR'ing it with the output of the symbol buffer D1

	Precondition:none
	Postcondition:X has had its most significant bit modified. Least significant bit unmodified

*/
void differentialEncoder(symbol* x, vector<bit> D1, int symbolCounter){
	outputBit = x->ms ^ D1[symbolCounter % SYMBOL_DELAY];
	D1[symbolCounter % SYMBOL_DELAY] = outputBit;
	x->ms = outputBit;
}
