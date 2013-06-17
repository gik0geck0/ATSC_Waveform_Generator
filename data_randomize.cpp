// NOTES
// This fuction changes the bits in the vector
// The MSB is vector.at(0)


#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "common.cpp"
typedef uint8_t byte;
typedef bool bit;
using namespace std;

static const int NUM_BYTES = 187;
static const int BYTE_SIZE = 8;

void data_randomize(vector<byte>* mpeg_frame);

void data_randomize(vector<byte> *mpeg_frame)
{
    printf("Beginning data randomization\n");
	int i;
	bit feedbackBit;
	vector<bit>* mpeg_frame_bits;
	vector<byte>* temp;
	// Initialize to 0xF180 by ATSC Standards
	// In the format X + X^2 + X^3 + ... + X^16
	bit polynomial[16] = { 	0,0,0,0, //0x0
							0,0,0,1, //0x8
							1,0,0,0, //0x1
							1,1,1,1  //0xF
						 };
	// initialize the randomizing bytes
	bit fixedRandomizingBytes[8];
	fixedRandomizingBytes[0] = polynomial[1-1];
	fixedRandomizingBytes[1] = polynomial[3-1];
	fixedRandomizingBytes[2] = polynomial[4-1];
	fixedRandomizingBytes[3] = polynomial[7-1];
	fixedRandomizingBytes[4] = polynomial[11-1];
	fixedRandomizingBytes[5] = polynomial[12-1];
	fixedRandomizingBytes[6] = polynomial[13-1];
	fixedRandomizingBytes[7] = polynomial[14-1];
    printf("Initialization complete\n");

	// check to see if the mpg_frame is disible by 8
	if((mpeg_frame->size() % NUM_BYTES) != 0)
	{
		printf("WARNING!!! Transport stream not divisible by 187\n");
	}

    printf("Creating bits from byes\n");
	mpeg_frame_bits = makeBitsFromBytes(mpeg_frame);
	

    printf("Entering randomize loop\n");
	for(i = 0; i < mpeg_frame_bits->size(); i += BYTE_SIZE)
	{
		// XOR bytes
		(*mpeg_frame_bits)[i] 	 = (*mpeg_frame_bits)[i] 	  ^ fixedRandomizingBytes[7];
		(*mpeg_frame_bits)[i + 1] = (*mpeg_frame_bits)[i + 1] ^ fixedRandomizingBytes[6];
		(*mpeg_frame_bits)[i + 2] = (*mpeg_frame_bits)[i + 2] ^ fixedRandomizingBytes[5];
		(*mpeg_frame_bits)[i + 3] = (*mpeg_frame_bits)[i + 3] ^ fixedRandomizingBytes[4];
		(*mpeg_frame_bits)[i + 4] = (*mpeg_frame_bits)[i + 4] ^ fixedRandomizingBytes[3];
		(*mpeg_frame_bits)[i + 5] = (*mpeg_frame_bits)[i + 5] ^ fixedRandomizingBytes[2];
		(*mpeg_frame_bits)[i + 6] = (*mpeg_frame_bits)[i + 6] ^ fixedRandomizingBytes[1];
		(*mpeg_frame_bits)[i + 7] = (*mpeg_frame_bits)[i + 7] ^ fixedRandomizingBytes[0];

		// Increment polynomial
		feedbackBit = polynomial[16-1];

		polynomial[16-1] = polynomial[15-1];
		polynomial[15-1] = polynomial[14-1];
		polynomial[14-1] = polynomial[13-1] ^ feedbackBit;
		polynomial[13-1] = polynomial[12-1] ^ feedbackBit;
		polynomial[12-1] = polynomial[11-1] ^ feedbackBit;
		polynomial[11-1] = polynomial[10-1];
		polynomial[10-1] = polynomial[9-1 ];
		polynomial[9-1]  = polynomial[8-1 ];
		polynomial[8-1]  = polynomial[7-1 ] ^ feedbackBit;
		polynomial[7-1]  = polynomial[6-1 ] ^ feedbackBit;
		polynomial[6-1]  = polynomial[5-1 ];
		polynomial[5-1]  = polynomial[4-1 ];
		polynomial[4-1]  = polynomial[3-1 ] ^ feedbackBit;
		polynomial[3-1]  = polynomial[2-1 ];
		polynomial[2-1]  = polynomial[1-1 ] ^ feedbackBit;
		polynomial[1-1]  = feedbackBit;

		// reassignes the D values
		fixedRandomizingBytes[0] = polynomial[1-1];
		fixedRandomizingBytes[1] = polynomial[3-1];
		fixedRandomizingBytes[2] = polynomial[4-1];
		fixedRandomizingBytes[3] = polynomial[7-1];
		fixedRandomizingBytes[4] = polynomial[11-1];
		fixedRandomizingBytes[5] = polynomial[12-1];
		fixedRandomizingBytes[6] = polynomial[13-1];
		fixedRandomizingBytes[7] = polynomial[14-1];
	}
    printf("Ending randomize loop\n");

	//delete mpeg_frame;
	temp = makeBytesFromBits(mpeg_frame_bits);
	
	if(temp->size() != mpeg_frame->size())
        printf("Something went wrong!!!!!\n");

    printf("Saving temporaries into the mpeg frame\n");
	for(i = 0; i < temp->size(); i++)
	{
		mpeg_frame->at(i) = temp->at(i);
	}

	delete temp;
	delete mpeg_frame_bits;
}



// This is for testing purposes

// input:
// MSB on right
// 00110010110011010111011000100100

// output:
// 10101011001100011100000000100111
/*
int main()
{
	vector<byte>* testBit = new vector<byte>();

	for(int i = 0; i < 187; i++)
	{
		testBit->push_back(i);
	}

	for(int i = testBit->size() -1; i >=0; i--)
	{
		printf("%i",(int) testBit->at(i));
	}
	printf("\n");

	data_randomize(testBit);

	//printf("Size of testBit is : %i", testBit->size());
	for(int i = testBit->size() -1; i >=0; i--)
	{
		printf("%i",(int) testBit->at(i));
	}
	printf("\n");
	system("pause");
	return 0;
}
*/
