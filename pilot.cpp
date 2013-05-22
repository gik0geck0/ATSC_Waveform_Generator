#pragma once
#include <vector>
#include <stdio.h>
#include <stdint.h>
using namespace std;

typedef bool bit;
typedef vector<int8_t> segment;

vector<vector<float>*>* pilot_insertion(vector<segment*>* data_frame)
{
	// This will delete the data_frame that is passed to it
	int i,j;
	vector<float>* field;
	vector<vector<float>*>* frameWithPilot;

	frameWithPilot = new vector<vector<float>*>();
	for(i = 0; i < data_frame->size(); i++)
	{
		field = new vector<float>();
		for(j = 0; j < data_frame->at(i)->size(); j++)
		{
			float value = (float)data_frame->at(i)->at(j) + 1.25f;
			field->push_back(value);
		}
		frameWithPilot->push_back(field);
	}

	// delete data_frame
	for(i = 0; i < data_frame->size(); i++)
	{
		delete data_frame->at(i);
	} 
	delete data_frame;
	data_frame = NULL;
	return frameWithPilot;
}

// for testing
/*
int main()
{
	int i,j;
	vector<vector<int8_t>*>* foo;
	vector<int8_t>* bar;
	vector<vector<float>*>* jar;

	foo = new vector<vector<int8_t>*>();
	for(i = 0; i < 100; i++)
	{
		bar = new vector<int8_t>();
		for(j = -8; j < 8; j++)
		{
			bar->push_back(j);
		}
		foo->push_back(bar);
	}

	jar = pilot_insertion(foo);

	for(i = 0; i < jar->size(); i++)
	{
		for(j = 0; j < jar->at(i)->size(); j++)
		{
			printf("%.2f   ", jar->at(i)->at(j));
		}
		printf("\n");
	}

	return 0;
}*/