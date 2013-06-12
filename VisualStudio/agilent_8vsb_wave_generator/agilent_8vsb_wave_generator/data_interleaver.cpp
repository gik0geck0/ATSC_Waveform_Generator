/*
 * NOTES:
 * the data interleaver takes a pointer and reassignes the old pointer
 * and returns the new pointer
 *
 */
#include "data_interleaver.h"

vector<bit>* data_interleaving(vector<bit> *bitStream)
// WARNING: this function delets bitStream and returns a new interleaved bit Stream
{
	int i,j;
	int bytestreamSize = BYTES_PER_SEGMENT_FEC;
	vector<bit> *interleavedBits;
	vector<byte> *byteStream;
	byte convolutionalInterleaver[52][4];

	// convert bits to bytes
	byteStream = makeBytesFromBits(bitStream);
	delete bitStream;
	bitStream = NULL;
	
	if(byteStream->size() != BYTES_PER_SEGMENT_FEC)
	{
		printf("Byte Stream is %i long and not 207 bytes long.\n", (int)byteStream->size());
	}
	//interleave the bytes
	for(i = 0; i < BYTES_PER_SEGMENT_FEC; i++)
	{
		convolutionalInterleaver[i % 52][i/52] = byteStream->at(i);
	}
	delete byteStream;
	byteStream = NULL;

	// new byte stream to pull data form the interleaver
	byteStream = new vector<byte>();
	for(i = 0; i < BYTES_PER_SEGMENT_FEC; i++)
			byteStream->push_back(convolutionalInterleaver[i/4][i%4]);
	// make bit stream
	interleavedBits = makeBitsFromBytes(byteStream);
	delete byteStream;
	byteStream = NULL;
	return interleavedBits;
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
