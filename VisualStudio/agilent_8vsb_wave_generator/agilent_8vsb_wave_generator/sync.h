#pragma once
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
