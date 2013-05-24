
#pragma once
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.cpp"
using namespace std;
typedef bool bit;
typedef uint8_t byte;

vector< vector<bit>* >* remove_sync_bit(vector<byte>* inputStream)
{
	// This function will delete the inputSteam
	int i, j, numberOfPackets;
	vector<byte>* byteSteam;
	vector< vector<bit>* >* bitPackage;

	if(!((inputStream->size() % 188) == 0))
		exit(1);

	numberOfPackets = inputStream->size() / 188;
	bitPackage = new vector<vector<bit>*>();
	for(i = 0; i < numberOfPackets; i++)
	{
		byteSteam = new vector<byte>();
		for(j = 1; j < 188; j++)
		{
			byteSteam->push_back(inputStream->at(i*188+j));
		}
		bitPackage->push_back(makeBitsFromBytes(byteSteam));
		delete byteSteam;
	}
	byteSteam = NULL;
	
	delete inputStream;
	inputStream = NULL;
	return bitPackage;
}
// test
/*
int main()
{
	int i;
	int someNumber = 188*16;
	vector< vector<bit>* >* moreStuff;
	vector<byte> *bytes = new vector<byte>();
	for(i = 0; i < someNumber; i++)
	{
		bytes->push_back(i);
	}
	moreStuff = remove_sync_bit(bytes);
	bytes = NULL;
	printf("Size: %i\n",(int)moreStuff->size());
	for(i = 0; i < moreStuff->size(); i++)
	{
		printf("\t%f\n",(float)moreStuff->at(i)->size()/8.0f);
	}
	for(i = 0; i < moreStuff->size(); i++)
	{
		delete moreStuff->at(i);
	}
	delete moreStuff;
	moreStuff = NULL;
	return 0;
}
*/