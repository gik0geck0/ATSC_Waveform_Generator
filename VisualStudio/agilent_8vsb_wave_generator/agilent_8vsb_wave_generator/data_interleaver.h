#pragma once
#include <vector>
#include <stdio.h>
#include <queue>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include "common.h"

#define null (256)


typedef bool     bit;
typedef uint8_t  byte;
typedef uint16_t dbyte;

using namespace std;

static const int BYTES_PER_SEGMENT_FEC = 207;
static const int NUMBER_OF_QUEUES = 52;
void data_interleaving(vector<vector<byte>*>*);