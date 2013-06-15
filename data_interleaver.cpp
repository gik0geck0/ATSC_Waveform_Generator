/*
 * NOTES:
 * the data interleaver takes a pointer and reassignes the old pointer
 * and returns the new pointer
 *
 */

#include <vector>
#include <stdio.h>
#include <queue>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include "common.cpp"
typedef bool bit;
typedef uint8_t byte;
using namespace std;

static const int BYTES_PER_SEGMENT_FEC = 207;

void data_interleaving(vector<vector<byte>*>*);


void data_interleaving(vector<vector<byte>*>* bytes)
// WARNING: this function delets bitStream and returns a new interleaved bit Stream
{
	const int NUMBER_OF_QUEUE = 4;
	const int QUUEUE_BUFFER_OFFSET = 4;
	int clock, i, j;
	int start, end, current;
	int offsetCounter;
	int iter;
	vector<byte> normal;
	queue<byte> buffer[NUMBER_OF_QUEUE];

	clock = 0;
	for(i = 0; i < bytes->size(); i++)
	{	for(j = 0; j < bytes->at(i)->size(); j++)
		{	buffer[clock].push(bytes->at(i)->at(j));
			clock = (clock + 1) % NUMBER_OF_QUEUE;
		}
	}

	start = 0;
	end = 0;
	current = 0;

	offsetCounter = 0;

	while(true) // majic sorry
	{	normal.push_back(buffer[current].front());
		buffer[current].pop();

		if(!(current == end))
		{	current++;
			continue;
		}

		if(buffer[start].size() == 0)
		{	if(start == end)
			{	if(end == (NUMBER_OF_QUEUE - 1))
					break; // THIS SHOULD FIRE
				start++;
				end++;
				offsetCounter = 0;
				current = start;
				if(buffer[start].size() == 0) // if numbers don't fill up one slot
					break;
				continue;
			}
			else
			{	start++;
				if(!(offsetCounter == 4))
				{	offsetCounter++;
					current = start;
					continue;
				}
				else
				{	offsetCounter = 0;
					if(end == (NUMBER_OF_QUEUE - 1))
					{	current = start;
						continue;
					}
					else
					{	end++;
						current++;
						continue;
					}
				}
				continue;
			}
		}
		else
		{	if(!(offsetCounter == 4))
			{	offsetCounter++;
				current = start;
				continue;
			}
			else
			{	offsetCounter = 0;
				if(end == (NUMBER_OF_QUEUE - 1))
				{	current = start;
					continue;
				}
				else
				{	end++;
					current++;
					continue;
				}
			}
		}
	}

	iter = 0;
	for(i = 0; i < bytes->size(); i++)
	{	for(j = 0; j < bytes->at(i)->size(); j++)
		{
			bytes->at(i)->at(j) = normal.at(iter);
			iter++;
		}
	}
}



// for testing
/*
int main(void)
{
	int i;
	vector<byte> *bytes;
	vector<bit> testBit;
	vector<byte> *byteStream = new vector<byte>;
	vector<bit> *bitStream = new vector<bit>();
	for(i = 1; i < 208; i++)
		byteStream->push_back(i);
	bitStream = data_interleaving(makeBitsFromBytes(byteStream));
	delete byteStream;
	byteStream = makeBytesFromBits(bitStream);
	for(i= 0; i < byteStream->size(); i++)
	{
		printf("%i\n",byteStream->at(i));
	}

	// testing the function makeByteFromBits and makeBitsFromBytes
	
	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(1);

	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(0);
	testBit.push_back(1);
	testBit.push_back(0);
	testBit.push_back(1);
	testBit.push_back(0);
	testBit.push_back(1);

	bytes = makeBytesFromBits(&testBit);

	for(i = testBit.size() -1; i >=0; i--)
	{
		printf("%i",(int) testBit.at(i));
	}
	printf("\n");

	for(i = 0; i < bytes->size(); i++)
	{
		printf("%i\n",bytes->at(i) );
	}
	testBit = (*makeBitsFromBytes(bytes));

	for(i = testBit.size() -1; i >=0; i--)
	{
		printf("%i",(int) testBit.at(i));
	}
	printf("\n");

	return 0;

}
*/
