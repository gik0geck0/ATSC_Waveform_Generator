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

//this main exists only for the purpose debuging. once the final product is finished this function will be deleted
/*
int main(){
	vector<bit>* test = new vector<bit>; //a test stream of bits
	test->push_back(1);
	test->push_back(0);

	test->push_back(1);
	test->push_back(1);

	test->push_back(0);
	test->push_back(1);

	test->push_back(1);
	test->push_back(0);

	test->push_back(0);
	test->push_back(0);

	test->push_back(1);
	test->push_back(0);

	test->push_back(0);
	test->push_back(1);

	test->push_back(0);
	test->push_back(1);

	test->push_back(1);
	test->push_back(1);

	test->push_back(0);
	test->push_back(1);

	test->push_back(1);
	test->push_back(0);

	test->push_back(0);
	test->push_back(1);

	test->push_back(1);
	test->push_back(1);

	test->push_back(0);
	test->push_back(1);

	test->push_back(0);
	test->push_back(0);

	test->push_back(0);
	test->push_back(0);

	test->push_back(0);
	test->push_back(0);

	test->push_back(1);
	test->push_back(1);

	test->push_back(1);
	test->push_back(0);

	test->push_back(1);
	test->push_back(0);

	test->push_back(0);
	test->push_back(0);

	test->push_back(1);
	test->push_back(1);

	test->push_back(0);
	test->push_back(1);

	test->push_back(1);
	test->push_back(1);
	cout << "Input is\n";
	for(int i = 0; i < test->size(); i++){
		cout << ((*test)[i]);
		if(i%2 == 1)
			cout << " ";
	}
	cout << "\n";
	cout << "Output is\n";
	vector<int8_t>* derp;
	derp = trellisEncoder(test);
	for(int i = 0; i < derp->size(); i++){
		cout << (int)(*derp)[i] << " "; 
	}
	return 0;
}
*/


vector<int8_t>* trellisEncoder(vector<bit>* bitStream){
	//check for a valid bitStream size
	if(bitStream->size()%2){
		cout << "In function trellisEncoder: Not given a bit stream whose length is a multiple of 2.\nGiven bit stream length is " << bitStream->size() << "\nAborting" << endl;
		exit(1);
	}
	//A warning for incorrect bitStream size, will still proceede even if wrong however
	if(bitStream->size() != BIT_STREAM_LENGTH){
		cout << "In function trellisEncoder: Unexpected length of bit stream.\nExpected length of " << BIT_STREAM_LENGTH << "\nGot length of " << bitStream->size() << "\nContiuing with function, output may be improper\n" << endl;
	}
	//initilization block
	int symbolCounter = 0;
	vector<bit>* D1=new vector<bit>;
	vector<bit>* D2=new vector<bit>;
	vector<bit>* D3=new vector<bit>;
	vector<bit>* newBitStream = new vector<bit>;
	outputBits* output;
	symbol* currentSymbol;
	//push 0's onto every location of symbol buffer
	for(int i = 0; i < SYMBOL_DELAY; i++){
		D1->push_back(0);
		D2->push_back(0);
		D3->push_back(0);
	}
	for(int i = 0; i < bitStream->size()/2; i++){ //iterate by symbol(which is currently 2 bits)
		//get the current 2 bit symbol from bit stream
		currentSymbol = getSymbol(bitStream, symbolCounter);
		
		//generate output bits
		differentialEncoder(currentSymbol, D1, symbolCounter);
		output = convolutionEncoder(currentSymbol, D2, D3, symbolCounter);

		//push output bits onto new bit stream
		newBitStream->push_back(output->z2);
		newBitStream->push_back(output->z1);
		newBitStream->push_back(output->z0);

		//increment the symbol counter
		symbolCounter++;

		//free up memory, pointers will be reassigned
		delete output;
		delete currentSymbol;

		//just for good coding practice, removing any dangling pointerss. 
		output=NULL; 
		currentSymbol=NULL;

		//debug block
		/*
		if(symbolCounter %12 == 0){
			cout << "D1 is currently at\n";
			for(int i = 0; i < 12; i++){
				cout<<(*D1)[i] << " ";
			}
			cout << "\nD2 is currently at\n";
			for(int i = 0; i < 12; i++){
				cout<<(*D2)[i] << " ";
			}
			cout <<"\nD3 is currently at \n";
			for(int i = 0; i < 12; i++){
				cout <<(*D3)[i] << " ";
			}
			cout << endl;
		}
		*/
	}

	//deconstructors
	delete currentSymbol;
	currentSymbol = NULL;
	delete output;
	output = NULL;
	delete D1, D2, D3;
	D1 = NULL; D2 = NULL; D3 = NULL;

	//make sure the resulting bit stream is the right length, splurts out a warning if not, but will still return result
	if(newBitStream->size() != RESULTING_BIT_STREAM_LENGTH){
		cout << "In function trellisEncoder: Unexpected resulting bit stream length\nExpected " << RESULTING_BIT_STREAM_LENGTH << "\nGot " << newBitStream->size()<< endl;
	}

	delete bitStream; //remove old data
	bitStream = newBitStream; //reassign pointers
	vector<int8_t>* levels; // the returned pointer of vectors
	levels = bitsToLevel(bitStream); //convert stream of bits to voltage levels
	delete bitStream; //delete old bitstream
	bitStream = NULL; //reassign dangling pointer
	return levels; 
}

outputBits* convolutionEncoder(symbol* y, vector<bit>* D2, vector<bit>* D3, int symbolCounter){
	outputBits* output = new outputBits;
	output->z2 = y->ms;//most sig bit is unaffected
	output->z1 = y->ls;//z1 bit has no funny operation

	//convolution implementation as defined by the ATSC A/53 part 2 standard
	bit temp = (*D2)[symbolCounter%12] ^ y->ls;
	bit z = (*D3)[symbolCounter%12];
	(*D3)[symbolCounter%12] = temp;
	(*D2)[symbolCounter%12] = z;
	output->z0 = z;

	return output;
}

symbol* getSymbol(vector<bit>* bitStream, int symbolCounter){
	symbol* outputSymbol = new symbol;
	outputSymbol->ms = (*bitStream)[symbolCounter*2];
	outputSymbol->ls = (*bitStream)[symbolCounter*2+1];	
	return outputSymbol;
}

void differentialEncoder(symbol* x, vector<bit>* D1, int symbolCounter){
	bit outputBit;

	//differential encoder (aka precoder) as defined by the ATSC A/53 part 2 standard
	outputBit = x->ms ^ (*D1)[symbolCounter % SYMBOL_DELAY];
	(*D1)[symbolCounter % SYMBOL_DELAY] = outputBit;

	x->ms = outputBit;
}





vector<int8_t>* bitsToLevel(vector<bit>* bitStream){
	if(bitStream->size() % 3 != 0){//check for size of stream, make sure its valid
		cout << "In function bitsToLevel: Requires a bitstream length that is a multiple of three\nABORTING" << endl;
		exit(1);
	}

	vector<int8_t>* voltageLevels = new vector<int8_t>;
	for(int i = 0; i < bitStream->size()/3; i++){ // iterate by every symbol(which is now 3 bits long)
		int level = 7; //voltage is defaulted to 7 or 111, will subtract as needed
		for(int j = 0; j < 3; j++){
			if(j == 0){ // most sig bit
				if((*bitStream)[i*3+j] == false)
					level -= 8;
			}
			else if(j == 1){ //mid bit
				if((*bitStream)[i*3+j] == false)
					level -= 4;	
			}
			else{ // least sig bit
				if((*bitStream)[i*3+j] == false)
					level -= 2;
			}
			
		}
		voltageLevels->push_back(level);

	}
	return voltageLevels;	
}
