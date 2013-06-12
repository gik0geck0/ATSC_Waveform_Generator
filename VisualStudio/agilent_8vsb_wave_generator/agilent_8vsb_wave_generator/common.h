#pragma once
#include <vector>
#include <stdint.h>
typedef bool bit;
typedef uint8_t byte;

std::vector<byte>* makeBytesFromBits(std::vector<bit> *bitStream);
std::vector<bit> *makeBitsFromBytes(std::vector<byte> *byteStream);