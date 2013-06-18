#pragma once
#include <cstdio>
#include <stdint.h>
#include <cstdlib>
#include <stdint.h>
#include <vector>
#include "common.h"

using namespace std;

typedef uint8_t byte;
typedef bool bit;

uint8_t gmul(uint8_t a, uint8_t b);
uint8_t gsub(uint8_t a, uint8_t b);
uint8_t gadd(uint8_t a, uint8_t b);

vector<vector<byte>*>* add_reed_solomon_parity(std::vector<byte>* input_bytes);
std::vector<std::vector<byte>*>* divide_into_segments(std::vector<byte>* field_bytes);
byte solomon_iteration(byte outputs[21], byte input_byte, bool gate_open);


// This is defined specifically for the ATSC standard
const byte alphas[] = { 174, 165, 121, 121, 198, 228, 22, 187, 36, 69, 150, 112, 220, 6, 99, 111, 5, 240, 186, 152, 1 };
