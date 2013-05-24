
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

    printf("Mod 188? %i\n", inputStream->size() % 188);
	if(!((inputStream->size() % 188) == 0)) {
        printf("Input stream was of an invalid size\n");
		exit(1);
    }

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

void read_in_bits(char* file_name, std::vector<bit>* input_stream) {
    FILE* f = fopen(file_name, "rb");
    fseek(f, 0L, SEEK_END);
    long filelen = ftell(f);
    rewind(f);

    char* tape = (char*) malloc(filelen); // Check for out-of-memory errors!
    fread((void*)tape, filelen, 1, f);

    for (int i=0; i < filelen; i++) {
        for (int i=0; i < 8; i++) {
            input_stream->push_back( (tape[i] << i) & 0x80);
        }
    }

    fclose(f);
}

void read_in_bytes(char* file_name, std::vector<byte>* input_stream) {
    FILE* f = fopen(file_name, "rb");
    fseek(f, 0L, SEEK_END);
    long filelen = ftell(f);
    rewind(f);

    char* tape = (char*) malloc(filelen); // Check for out-of-memory errors!
    fread((void*)tape, filelen, 1, f);

    for (int i=0; i < filelen; i++) {
        input_stream->push_back((byte) tape[i]);
    }

    fclose(f);
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
