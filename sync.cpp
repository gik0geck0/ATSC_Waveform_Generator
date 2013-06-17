#include <vector>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
using namespace std;

/*
  Type defs are for making the flow of the program make more sense in the prespective of the standard. It does not encompass all types, 
  ie. a segment is a vector<int8_t>*, but make pn511 does not return a segment as defined by the ATSC standard, so it returns a vector<int8_t>*, not a segment*. makeNewField will however make a segment in terms of the ATSC standard, and will return a segment*, not a vector<int8_t>*
*/

typedef vector<int8_t> segment; // this is a complete, encoded transport stream after going through everyting up to the sync
typedef vector<segment*> dataFrame; //this is a completed dataframe with syncs. 
typedef bool bit;


/*
  Constants, these numbers have specific meanings and too increase readablity of code, have been defined.
*/
static const int PN511_BLOCK_SIZE = 511;
static const int PN63_STANDARD_BLOCK_SIZE = 63;
static const int PN63_RESERVED_BLOCK_SIZE = 92;
static const int NUMBER_SEGMENTS_BEFORE_FIELD = 312;
static const int NUMBER_SEGMENTS_AFTER_FIELD = 313;
static const int SYMBOLS_PER_SEGMENT_BEFORE_SYNC = 828;

/*
 This function is the Master function call. Calling this function will return a data frame as defined by the ATSC A/53 Standard page 10 figure 5.2 
 
 Precondition: a vector containg any number of 828 symbol segments is provided
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
  This function will return a vector of length 12 that contains the last 12 symbols of the segment pointer provided

  Precondition: lastSegment is valid
  Postcondition: only the last 12 symbols of lastSegment are returned. 
*/
vector<int8_t>* getLast12Symbols(segment* lastSegment);


/*
  This function is simply to abstract away the hardcoding of the 24 level mode definition. It is always going to be 8VSB trellis coded which is 0000 1010 0101 1111 0101 1010 as defined by the ATSC Standard A/53 Part2 page 22 table 5.3

  Precondition: None
  Postcondition: None
*/
vector<int8_t>* assignModeFlag();

/*
  This function is simply to abstract away all the function calls associated in making a field sync

  Precondition: Pointer to last segment has at least 12 symbols
  Postcondition: Pointer to field sync is returned
*/
segment* makeNewField(int fieldSyncNum, segment* lastSegment);

/*
  This function will prepend the segment sync of 5 -5 -5 5 to a particular segment and modify the segment to prepend the segment sync to it

  Precondition: None
  Postcondition: Orginal segment modified with segment sync prepended
*/
void segSync(segment* seg);


vector<int8_t>* pn511(){
	//intiliation
	vector<int8_t>* sequence = new vector<int8_t>;
	bit x9(0), x8(1), x7(0), x6(0), x5(0), x4(0), x3(0), x2(0), x1(0);
	bit temp;
	
	//The implementation of the PN511 sequence as defined by the ATSC A/53 part 2 standard
	for(int i = 0; i < PN511_BLOCK_SIZE; i++){
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
	//intiliazation
	vector<int8_t>* sequence = new vector<int8_t>;
	bit x6(1), x5(0), x4(0), x3(1), x2(1), x1(1);
	bit temp;

	//Implementation of the PN63 sequence as defined by the ATSC A/53 part 2 standard. Note that on every other field sync, the middle pn63 block will be inverted. 
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

vector<int8_t>* assignModeFlag(){
	//intilization
	vector<int8_t>* mode = new vector<int8_t>;
	
	//this sequence is defined by the ATSC A/53 part 2. It is the same for every 8VSB trellis encoded wave.
	mode->push_back(-5);
	mode->push_back(-5);
	mode->push_back(-5);
	mode->push_back(-5);

	mode->push_back(5);
	mode->push_back(-5);
	mode->push_back(5);
	mode->push_back(-5);

	mode->push_back(-5);
	mode->push_back(5);
	mode->push_back(-5);
	mode->push_back(5);

	mode->push_back(5);
	mode->push_back(5);
	mode->push_back(5);
	mode->push_back(5);

	mode->push_back(-5);
	mode->push_back(5);
	mode->push_back(-5);
	mode->push_back(5);

	mode->push_back(5);
	mode->push_back(-5);
	mode->push_back(5);
	mode->push_back(-5);

	return mode;
}

segment* makeNewField(int fieldSyncNum, segment* lastSegment){
	//initilization
	vector<int8_t>* fieldSync = new vector<int8_t>;
	vector<int8_t>* mode = assignModeFlag();
	vector<int8_t>* pn511Block = pn511();
	vector<int8_t>* pn63Block; // will be created later, due to need to iterate
	vector<int8_t>* last12Symbols = getLast12Symbols(lastSegment);
	
	//checks to see that the sequence generated is right length
	if(pn511Block->size() != PN511_BLOCK_SIZE){
		cout << "In function makeNewField: pn511Block is not correct size\nExpected 511\nGot " << pn511Block->size() << "\nABORTING";
		exit(1);
	}

	fieldSync->insert(fieldSync->end(), pn511Block->begin(), pn511Block->end()); //append the pn511Block to field sync

	//make the pn63 blocks
	for(int i = 0; i < 3; i++){
		pn63Block = pn63(PN63_STANDARD_BLOCK_SIZE, fieldSyncNum, i+1); 
		//checks to see that sequence generates is right length
		if(pn63Block->size() != PN63_STANDARD_BLOCK_SIZE){
			cout << "In function makeNewField: pn63Block number " << i+1 << " was not correct size\nExpected 63\nGot " << pn63Block->size() << "\nABORTING";
			exit(1);
		}
		fieldSync->insert(fieldSync->end(), pn63Block->begin(), pn63Block->end());//append the pn63Block to field sync
		delete pn63Block; // free up iterative memory
	}

	fieldSync->insert(fieldSync->end(), mode->begin(), mode->end());//append the mode block to field sync
	pn63Block = pn63(PN63_RESERVED_BLOCK_SIZE, fieldSyncNum, 4);// can use any block num, but use 4 just for good measure
	fieldSync->insert(fieldSync->end(), pn63Block->begin(), pn63Block->end()); // insert the 92 length pn63 block;
	fieldSync->insert(fieldSync->end(), last12Symbols->begin(), last12Symbols->end()); //add last 12 symbols
	//destructors
	delete mode;
	delete pn511Block;
	delete pn63Block;
	delete last12Symbols;
	mode = NULL;
	pn511Block = NULL;
	pn63Block = NULL;
	last12Symbols = NULL;


	return fieldSync;
}

void segSync(segment* seg){
	seg->insert(seg->begin(), 5);
	seg->insert(seg->begin()+1, -5);
	seg->insert(seg->begin()+2, -5);
	seg->insert(seg->begin()+3, 5);
}	

vector<int8_t>* getLast12Symbols(segment* lastSegment){
	vector<int8_t>* last12Symbols = new vector<int8_t>;
	for(int i = lastSegment->size()-12; i < lastSegment->size(); i++){
		last12Symbols->push_back((*lastSegment)[i]);
	}
	return last12Symbols;
}

dataFrame* syncMux(vector<segment*>* dataSegments){
	vector<segment*>* fieldSyncs = new vector<segment*>; //will hold fieldsyncs to be inserted later
	segment* lastSegment;
	int fieldSyncNum = 1;//will always have at least 1 field sync 
	bool evenMult = false; //to check if it is an even amount of 312 or not

	for(int i = 0; i < dataSegments->size(); i++){//for each data segment
		if(i%NUMBER_SEGMENTS_BEFORE_FIELD == 0 && i != 0){
			fieldSyncs->push_back(makeNewField(fieldSyncNum, lastSegment));//append field sync into vector
			fieldSyncNum++; //may need to make another field sync
			evenMult = true;	
		}
		else if(i%NUMBER_SEGMENTS_BEFORE_FIELD != 0){
			evenMult = false;
		}
		lastSegment = (*dataSegments)[i];
	}
	if(!evenMult){ //if number of segments is not a multiple of 312, then we need to add another field sync
		fieldSyncs->push_back(makeNewField(fieldSyncNum, lastSegment));
	}

	int fieldToInsert=0; //the index of the field to insert from the vector fieldSyncs
	for(int i = 0; i < dataSegments->size(); i++){
		if(i%NUMBER_SEGMENTS_AFTER_FIELD == 0){
			dataSegments->insert(dataSegments->begin()+i, (*fieldSyncs)[fieldToInsert]);
			fieldToInsert++;
		}
	}

    printf("Adding segment syncs\n");
    for (int i=0; i < dataSegments->size(); i++) {
        if ( dataSegments->at(i)->size() != 828 ) {
            printf("Segment %i does not have 828 bytes. Instead: %i\n", i, dataSegments->at(i)->size());
        }
        
        //printf("Making new vector\n");
        segment* seg_sync = new segment();
		//segSync(dataSegments->at(j));
        //
        //Make a new vector, and put the segment sync at the beginning
        seg_sync->push_back(5);
        seg_sync->push_back(-5);
        seg_sync->push_back(-5);
        seg_sync->push_back(5);

        //printf("Adding the provious data onto it\n");
        // Push the data from the old segment into the new vector, after the sync
        for (int j=0; j < dataSegments->at(i)->size(); j++) {
            seg_sync->push_back(dataSegments->at(i)->at(j));
        }

        //printf("Deleting the old data\n");
        //delete (*dataSegments)[i];

        //printf("Saving the new segment to replace the old\n");
        (*dataSegments)[i] = seg_sync;
        if ( dataSegments->at(i)->size() != 832 ) {
            printf("Segment %i does not have 832 bytes after the segment sync was inserted. Instead: %i\n", i, dataSegments->at(i)->size());
        }
    }

    /*
	for(int i = 0; i < dataSegments->size(); i++){
		if((*dataSegments)[i]->size() != SYMBOLS_PER_SEGMENT_BEFORE_SYNC){
			cout << "In function syncMux: Encountered unexpected size of segment before segment sync addon at index " << i << endl << "Expected 828\nGot " << (*dataSegments)[i]->size()<<endl;
		}
		segSync((*dataSegments)[i]);
	}
    */

	return dataSegments;
	
}

/*
int main(){
	vector<segment*>* test = new vector<segment*>;
	segment* derp;
	for(int i = 0,j = -128; i<727; i++, j+=2){
		derp = new vector<int8_t>;
		for(int k = 0; k < 828; k++){
			derp->push_back(j);
		}
		test->push_back(derp);
	}
	dataFrame* greg = syncMux(test);
	for(int i = 0; i < greg->size(); i++){
		for(int j = 0; j < 832; j++){
			cout << (int)(*(*greg)[i])[j] << " ";
		}
		cout << endl;
	}

	return 0;
}
*/
