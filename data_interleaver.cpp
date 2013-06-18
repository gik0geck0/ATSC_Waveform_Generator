/*
 * NOTES:
 * the data interleaver takes a pointer and reassignes the old pointer
 * and returns the new pointer
 *
 */
#pragma once
#include <vector>
#include <stdio.h>
#include <queue>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include "common.cpp"

#define null (256)


typedef bool     bit;
typedef uint8_t  byte;
typedef uint16_t dbyte;

using namespace std;

static const int BYTES_PER_SEGMENT_FEC = 207;
static const int NUMBER_OF_QUEUES = 52;
void data_interleaving(vector<vector<byte>*>*);

// for testing
/*
int main(void)
{
	vector<byte>* test = new vector<byte>();
	for(int i = 0; i < 50; i++)
	{
		for(int j = 0; j < 10; j++)
		test->push_back(i*10 + j);
	}

	vector<vector<byte>*>* some = new vector<vector<byte>*>();
	some->push_back(test);

	data_interleaving(some);

	for(int i = 0; i < some->size(); i++)
		for(int j = 0; j < some->at(i)->size(); j++)
		{
			printf("%i\n",(int)some->at(i)->at(j));
		}
}

*/
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

