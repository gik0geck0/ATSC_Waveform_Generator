#pragma once
#include <vector>
#include <stdint.h>
typedef bool bit;
typedef uint8_t byte;

std::vector<byte>* makeBytesFromBits(std::vector<bit> *bitStream)
{
	// Does not delete bitStream
	int i,j;
	byte b;
	std::vector<byte> *byteStream = new std::vector<byte>();
	for(i = 0; i < bitStream->size(); i+=8)
	{
		b =  (*bitStream)[i];
		for(j = 1; j < 8; j++)
		{
			b <<= 1;
			b += (*bitStream)[i + j]; 	
		}
		byteStream->push_back(b);
	}
	return byteStream;
}

std::vector<bit> *makeBitsFromBytes(std::vector<byte> *byteStream)
{
	// Does not delete byteStream
	byte i, j, mask;
	std::vector<bit> *bitStream = new std::vector<bit>();

	for(i = 0; i < byteStream->size(); i++)
	{
		mask = 0x80;
		for(j = 8; j > 0; j--)
		{
			bitStream->push_back(byteStream->at(i) & mask);
			mask >>= 1;
		}
	}
	return bitStream;
}
