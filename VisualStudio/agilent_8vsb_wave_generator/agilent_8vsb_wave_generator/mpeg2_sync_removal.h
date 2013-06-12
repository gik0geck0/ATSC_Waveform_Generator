#pragma once
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
using namespace std;
typedef bool bit;
typedef uint8_t byte;

static const int TS_SIZE = 188;

void read_in_bytes(char* file_name, std::vector<byte>* input_stream);
void read_in_bits(char* file_name, std::vector<bit>* input_stream);
vector< vector<bit>* >* remove_sync_bit(vector<byte>* inputStream);
