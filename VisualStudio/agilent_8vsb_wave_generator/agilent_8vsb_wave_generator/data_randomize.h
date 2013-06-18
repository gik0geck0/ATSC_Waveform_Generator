#pragma once
// NOTES
// This fuction changes the bits in the vector
// The MSB is vector.at(0)


#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "common.h"
typedef uint8_t byte;
typedef bool bit;
using namespace std;

static const int NUM_BYTES = 187;
static const int BYTE_SIZE = 8;

void data_randomize(vector<byte>* mpeg_frame);
