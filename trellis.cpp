#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

typedef bit bool;
const static int SYMBOL_DELAY = 12; // 12 symbol delay in trellis encoding

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

//returns the symbol from stream as well as increments symbolCounter
//Precondition: bit stream is a multiple of 2
//Postcondition: symbol counter is properly incremented
symbol* getSymbol(vector<bit> bitStream, int& symbolCounter);



/*
	The DifferentialEncoder function performs the operation defined in the A/53 part 2 page 15 differential encoder
       	The function modifies the most significant bit of the incoming symbol by XOR'ing it with the output of the symbol buffer D1

	Precondition:None
	Postcondition:X has had its most significant bit modified. Least significant bit unmodified

*/
void DifferentialEncoder(symbol x, vector<bit> D1);

void trellisEncoder(vector<bit> bitStream){
	int symbolCounter = 0;
	vector<bit> D1;
	vector<bit> D2;
	vector<bit> D3;
	for(i = 0; i < SYMBOL_DELAY; i++){
		D1.push_back(0);
		D2.push_back(0);
		D3.push_back(0);
	}
	symbol* currentSymbol = getSymbol(bitStream);



}

symbol* getSymbol(vector<bool>, int& symbolCounter){

}

/*
	The DifferentialEncoder function performs the operation defined in the A/53 part 2 page 15 differential encoder
       	The function modifies the most significant bit of the incoming symbol by XOR'ing it with the output of the symbol buffer D1

	Precondition:none
	Postcondition:X has had its most significant bit modified. Least significant bit unmodified

*/
void DifferentialEncoder(symbol x, vector<bit> D1){
	outputBit = x.ms ^ D1[symbolCounter % SYMBOL_DELAY];
	D1[symbolCounter % SYMBOL_DELAY] = outputBit;
	x.ms = outputBit;
}
