#include <vector>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
using namespace std;

typedef vector<int8_t> segment; // this is a complete, encoded transport stream after going through everyting up to the sync
typedef vector<segment*> dataFrame; //this is a completed dataframe with syncs. 
typedef bool bit;

/*
 This function is the Master function call. Calling this function will return a data frame as defined by the ATSC A/53 Standard page 10 figure 5.2 
 
 Precondition: a vector containg 624, 828 symbol segments is provided
 Postcondition: a pointer to a vector containing segment pointers is returned with field syncs inserted before proper segments and segment syncs prepended to every segment. 
*/
dataFrame* syncMux(vector<segment*>* dataSegments);

/*
  This function will perform the pn511 psuedo random number generated sequence as defined by ATSC A/53 standard part 2 page 21 figure 5.10, and return a vector of levels.

  Precondition:None
  Postcondition:None
*/
vector<int8_t>* pn511();


/*
  This function will perform the pn63 psuedo random number generated sequence of length length as defined by ATSC A/53 standard part 2 page 21 figure 5.10, and return a vector of levels. Will invert if an even number field sync and second 63 block length

  Precondition:Valid length provided
  Postcondition:returned vector is inverted if it is the middle block and an even number field sync
*/
vector<int8_t>* pn63(int length, int currentFieldSyncNum, int blockNum);


/*
  This function will return a vector of length 12 that contains the last 12 symbols of the previous segment

  Precondition: lastSegment is valid or NULL
  Postcondition: only the last 12 symbols of lastSegment are returned. 
*/
vector<int8_t>* getLast12Symbols(segment* lastSegment);


vector<int8_t>* pn511(){
	vector<int8_t>* sequence = new vector<int8_t>;
	bit x9(0), x8(1), x7(0), x6(0), x5(0), x4(0), x3(0), x2(0), x1(0);
	bit temp;
	for(int i = 0; i < 511; i++){
		if(x1)
			sequence->push_back(5);
		else
			sequence->push_back(-5);
		temp = x1;
		temp = temp ^ x2;
		temp = temp ^ x4;
		temp = temp ^ x5;
		temp = temp ^ x7;
		temp = temp ^ x8;
		x1 = x2;
		x2 = x3;
		x3 = x4;
		x4 = x5;
		x5 = x6;
		x6 = x7;
		x7 = x8;
		x8 = x9;
		x9 = temp;
	}

	return sequence;
}


vector<int8_t>* pn63(int length, int currentFieldSyncNum, int blockNum){
	vector<int8_t>* sequence = new vector<int8_t>;
	bit x6(1), x5(0), x4(0), x3(1), x2(1), x1(1);
	bit temp;
	for(int i = 0; i < length; i++){
		if(currentFieldSyncNum % 2 == 0){ // if im an even field sync
			if(blockNum == 2){ // if im the block that might be invereted
				if(x1) //output inverted info
					sequence->push_back(-5); 
				else
					sequence->push_back(5);
			}
			else{ //otherwise output normal info
				if(x1)
					sequence->push_back(5);
				else 
					sequence->push_back(-5);
			}

		}
		else{
			if(x1)
				sequence->push_back(5);
			else
				sequence->push_back(-5);
		}
		temp = x1;
		temp = temp ^ x2;
		x1 = x2;
		x2 = x3;
		x3 = x4;
		x4 = x5;
		x5 = x6;
		x6 = temp;
	}
	return sequence;
}

int main(){
	return 0;
}

