#pragma once
#include <vector>
#include <stdio.h>
#include <queue>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include "common.h"
typedef bool bit;
typedef uint8_t byte;
using namespace std;

static const int BYTES_PER_SEGMENT_FEC = 207;

vector<bit>* data_interleaving(vector<bit> *bitStream);