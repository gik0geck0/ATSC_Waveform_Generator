#pragma once
#include <vector>
#include <stdio.h>
#include <stdint.h>
using namespace std;

typedef bool bit;
typedef vector<int8_t> segment;

vector<vector<float>*>* pilot_insertion(vector<segment*>* data_frame);