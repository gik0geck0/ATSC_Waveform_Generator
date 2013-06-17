
#pragma once
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.cpp"
using namespace std;
typedef bool bit;
typedef uint8_t byte;

static const int TS_SIZE = 188;

vector< vector<bit>* >* remove_sync_bit(vector<byte>* inputStream)
{
	// This function will delete the inputSteam
	int i, j, numberOfPackets;
	vector<byte>* byteSteam;
	vector< vector<bit>* >* bitPackage;

    printf("Mod 188? %i\n", inputStream->size() % TS_SIZE);
	if(!((inputStream->size() % TS_SIZE) == 0)) {
        printf("Input stream was of an invalid size\n");
		exit(1);
    }

	numberOfPackets = inputStream->size() / TS_SIZE;
	bitPackage = new vector<vector<bit>*>();
	for(i = 0; i < numberOfPackets; i++)
	{
		byteSteam = new vector<byte>();
		for(j = 1; j < TS_SIZE; j++)
		{
			byteSteam->push_back(inputStream->at(i*TS_SIZE+j));
		}
		bitPackage->push_back(makeBitsFromBytes(byteSteam));
		delete byteSteam;
	}
	byteSteam = NULL;
	
	delete inputStream;
	inputStream = NULL;
	return bitPackage;
}

vector<byte>* remove_sync_byte(vector<byte>* inputStream)
{
	// This function will delete the inputSteam
	int i, j, numberOfPackets;

    //printf("Mod 188? %i\n", inputStream->size() % TS_SIZE);
	if(!((inputStream->size() % TS_SIZE) == 0)) {
        printf("Input stream was of an invalid size\n");
		exit(1);
    }

	numberOfPackets = inputStream->size() / TS_SIZE;

    /*
	for(i = 0; i < numberOfPackets; i++)
	{
        inputStream->erase(inputStream->begin() + numberOfPackets * TS_SIZE);
	}
    */

    return inputStream;
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

    delete tape;

    fclose(f);
}

void read_in_bytes(char* file_name, std::vector<byte>* input_stream) {
    FILE* f = fopen(file_name, "rb");

    if (!f) {
        printf("Could not open file %s\n", file_name);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    long filelen = ftell(f);
    rewind(f);

    char* tape = (char*) malloc(filelen); // Check for out-of-memory errors!
    fread((void*)tape, filelen, 1, f);

    for (int i=0; i < filelen; i++) {
        input_stream->push_back((byte) tape[i]);
    }

    free(tape);

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
