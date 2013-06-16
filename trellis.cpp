#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "common.cpp"
#include <set>


/*READ THIS
 *after finding out that the early part of code is supposed to work on a per data field basis, rather than a per segment
 *this code is going to be rather different, here is a couple rational for some of the steps so you remember them

 *there is now a struct called buffers that holds the bit buffers D1, D2, D3. This will represent the 12 different
 *trellis encoders
 *
 *Differential and convolution encoder do not take any differnt arguments. they shouldn't change either, dont touch these
 *
 *TrellisEncoder as a function is no longer a master function, instead it will only take a byte, and call convolution encoder 
 *and differntial encoder with just that bit. it also will take a buffers struct representing which trellis encoder it is
 *working with. it will be renamed to make the main call easier
 *
 *a new function called getByte will exist. it will take a byte from the entire field. 

 a new master function will have to be made. It will likely be renamed to trellisEncoder for the sake of the main file,
 but currently has the name master.
 */

















using namespace std;

typedef bool bit;
typedef bool shouldBeTrue;
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

//this struct will hold the buffers for each trellis encoder, can make each function work with a struct to determine which encoder it uses, rather than make a class with methods
//
struct buffers{
	vector<bit> *D1;
	vector<bit> *D2;
	vector<bit> *D3;
};

//returns the symbol pointer from stream
//	Precondition: bit stream is a multiple of 2
//	Postcondition: pointer to symbol on heap returned
symbol* getSymbol(uint8_t byte, int symbolCounter);

uint8_t getByte(vector<uint8_t> segment, int byteCounter);






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
vector<bit>* master(uint8_t byte, buffers* dSet, int byteCount);

vector<vector<int8_t>*>* trellisEncoder(vector<vector<uint8_t>*>* field);

//this main exists only for the purpose debuging. once the final product is finished this function will be deleted

/*
int main(){
	vector<vector<uint8_t>*>* field;
	field = new vector<vector<uint8_t>*>;
	vector<vector<int8_t>*>* answer; 
	vector<uint8_t>* segment = new vector<uint8_t>;
	std::set<int8_t> derp;
	for(int i =0; i < 312; i++){
		for(int j = 0; j < 207; j++){
			segment->push_back((uint8_t)((j*4+5*i)%255));
		}
		field->push_back(segment);
		segment = new vector<uint8_t>;
	}
	answer= trellisEncoder(field);
	for(int i = 0; i < answer->size(); i++){
		if(answer->at(i)->size() != 828)
			exit(1);
		cout << endl << endl << "segment size is " << answer->at(i)->size() << endl<< endl;
		for(int j = 0; j < answer->at(i)->size(); j++){
			cout << (int)answer->at(i)->at(j) << " ";
			derp.insert(answer->at(i)->at(j));
			cout << "i is " << i << ", j is " << j << endl;
		}
	}
	for(std::set<int8_t>::iterator it = derp.begin(); it != derp.end(); it++){
		cout << (int)(*it) << " ";
	}
	cout << answer->size();
	return 0;
}
*/



vector<vector<int8_t>*>* trellisEncoder(vector<vector<uint8_t>*>* field){
	shouldBeTrue x = false;
	vector<buffers*>* encoder = new vector<buffers*>;
	vector<vector<uint8_t>*>* currentField;
	vector<uint8_t>* currentSegment;
	vector<bit>* bits;
	vector<bit>* segmentBits;
	vector<vector<bit>*>* allSegmentBits;
	vector<int8_t>* voltageSegment = new vector<int8_t>;;
	vector<vector<int8_t>*>* allVoltageSegments= new vector<vector<int8_t>*>;
	buffers* buffer;
	segmentBits = new vector<bit>;
	cout << "making buffers" << endl;
	for(int i = 0; i < 12; i++){//intialize the 12 buffers 
		buffer = new buffers;
		buffer->D1 = new vector<bit>;
		buffer->D2 = new vector<bit>;
		buffer->D3 = new vector<bit>;
		for(int j = 0; j<12; j++){
			buffer->D1->push_back(0);
			buffer->D2->push_back(0);
			buffer->D3->push_back(0);
		}
		encoder->push_back(buffer);
	}
	cout << "made buffers" << endl;
	cout << "doing encoding" << endl;
	for(int j = 0; j < field->size(); j++){ //iterate over each segment
		cout << "J is now " << j << endl;
		currentSegment = field->at(j); 
		int segMod = j % 3;
		cout << "segMod is " << segMod << endl;
		if(segMod == 0){ //decides if I start at 0, 4, or 8 trellis encoder
			cout << "about to encode" << endl;
			for(int k= 0; k < currentSegment->size(); k++){
				cout << "k is now " << k << endl;
				bits = master(currentSegment->at(k), encoder->at(k%12), k); //bits is an itermediate place holder
				cout << "encoding is done for segment" << endl;
				cout << "pushing bits onto segmentbits" << endl;
				for(int n=0; n < bits->size(); n++){
					segmentBits->push_back(bits->at(n));//push bits generated from bit into the vector that holds all the bits of a segment.
				}
			}
		}
		else if (segMod == 1){
			for(int k = 0; k < currentSegment->size(); k++){
				bits = master(currentSegment->at(k), encoder->at((k+4)%12), k);
				for(int n=0; n < bits->size(); n++){
					segmentBits->push_back(bits->at(n));
				}
			}
		}
		else{
			for(int k = 0; k <currentSegment->size(); k++){
				cout << "k is " << k << endl;
				bits = master(currentSegment->at(k), encoder->at((k+8)%12),k);
				for(int n=0; n < bits->size(); n++){
					segmentBits->push_back(bits->at(n));
				}
			}
		}
		voltageSegment= bitsToLevel(segmentBits); //an intermediate step, just makes this already ugly function a little less ugly
		allVoltageSegments->push_back(voltageSegment);
		voltageSegment = new vector<int8_t>;
		segmentBits = new vector<bit>;
	}
	cout << "finished encoding" << endl;

	delete bits;
	delete voltageSegment;
	return allVoltageSegments;
}


vector<bit>* master(uint8_t byte, buffers* dSet, int byteCount){
	symbol* a; 
	symbol*	b; 
	symbol*	c; 
	symbol*	d;
	vector<bit>* returnBits = new vector<bit>;
	outputBits* outA; 
	outputBits* outB; 
	outputBits* outC; 
	outputBits* outD;
	a = getSymbol(byte, 0);
	b = getSymbol(byte, 1);
	c = getSymbol(byte, 2);
	d = getSymbol(byte, 3);
	differentialEncoder(a, dSet->D1, (byteCount *4));
	differentialEncoder(b, dSet->D1, (byteCount *4)+1);
	differentialEncoder(c, dSet->D1, (byteCount *4)+2);
	differentialEncoder(d, dSet->D1, (byteCount *4)+3);
	outA = convolutionEncoder(a, dSet->D2, dSet->D3, (byteCount%4));
	outB = convolutionEncoder(b, dSet->D2, dSet->D3, (byteCount%4)+1);
	outC = convolutionEncoder(c, dSet->D2, dSet->D3, (byteCount%4)+2);
	outD = convolutionEncoder(d, dSet->D2, dSet->D3, (byteCount%4)+3);
	returnBits->push_back(outA->z2);
	returnBits->push_back(outA->z1);
	returnBits->push_back(outA->z0);
	returnBits->push_back(outB->z2);
	returnBits->push_back(outB->z1);
	returnBits->push_back(outB->z0);
	returnBits->push_back(outC->z2);
	returnBits->push_back(outC->z1);
	returnBits->push_back(outC->z0);
	returnBits->push_back(outD->z2);
	returnBits->push_back(outD->z1);
	returnBits->push_back(outD->z0);
	return returnBits;


}

outputBits* convolutionEncoder(symbol* y, vector<bit> *D2, vector<bit>* D3, int symbolCounter){
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

symbol* getSymbol(uint8_t byte, int symbolCounter){
	symbol* outputSymbol = new symbol;
	vector<uint8_t>* derp=new vector<uint8_t>;
        derp->push_back(byte);	
	vector<bit>* bits;
	bits = makeBitsFromBytes(derp);
	outputSymbol->ms = (*bits)[symbolCounter*2];
	outputSymbol->ls = (*bits)[symbolCounter*2+1];	
	delete derp;
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
	if(bitStream->size() % BITS_PER_VOLT != 0){//check for size of stream, make sure its valid
		cout << "In function bitsToLevel: Requires a bitstream length that is a multiple of three\nABORTING" << endl;
		exit(1);
	}

	vector<int8_t>* voltageLevels = new vector<int8_t>;
	for(int i = 0; i < bitStream->size()/BITS_PER_VOLT; i++){ // iterate by every symbol(which is now 3 bits long)
		int level = 7; //voltage is defaulted to 7 or 111, will subtract as needed
		for(int j = 0; j < BITS_PER_VOLT; j++){
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

