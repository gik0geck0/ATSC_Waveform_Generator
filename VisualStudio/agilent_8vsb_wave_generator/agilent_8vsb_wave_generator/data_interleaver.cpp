/*
 * NOTES:
 * the data interleaver takes a pointer and reassignes the old pointer
 * and returns the new pointer
 *
 */
#include "data_interleaver.h"

void data_interleaving(vector<vector<byte>*>* bytes)
{
	int i,j,clockCounter, iter;
	int numberOfEmptyQueues;
	bool allQueuesEmpty = false;
	vector<byte> normal;
	queue<dbyte> interleaver[52];

	// initialize interleaver
	for(i = 0; i < NUMBER_OF_QUEUES; i++)
		for(j = 0; j < i*4; j++)
			interleaver[i].push(null);


	clockCounter = 0;
	for(i = 0; i < bytes->size(); i++)
	{
		for(j = 0; j < bytes->at(i)->size(); j++)
		{
			interleaver[clockCounter].push(bytes->at(i)->at(j));
			clockCounter = (clockCounter + 1) % NUMBER_OF_QUEUES;
		}
	}

	while(!allQueuesEmpty)
	{
		numberOfEmptyQueues = 0;
		for(i = 0; i < NUMBER_OF_QUEUES; i++)
		{
			if(interleaver[i].size() != 0)
			{
				if(interleaver[i].front() == null)
				{
					interleaver[i].pop();
				}
				else
				{
					normal.push_back(interleaver[i].front());
					interleaver[i].pop();
				}
			}
			else
			{
				numberOfEmptyQueues++;
			}
		}
		if(numberOfEmptyQueues == 52)
			allQueuesEmpty = true;
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
