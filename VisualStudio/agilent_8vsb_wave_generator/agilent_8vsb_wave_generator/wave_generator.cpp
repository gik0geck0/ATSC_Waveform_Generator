#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <set>
#include <vector>
#include <string>

#include "mpeg2_sync_removal.h"
#include "data_randomize.h"
#include "reed_solomon.h"
#include "data_interleaver.h"
#include "trellis.h"
#include "sync.h"
#include "pilot.h"


#ifdef _WIN32 // for windows systems
	#include "agilent_windows_interface.h"
	void remove()
	{
	system("del temp.ts");
	}
#else // for linux systems
//#include "agilent_wave_interface.cpp"
  void remove()
   {
    system("rm temp.ts");
   }
#endif

typedef bool bit;
typedef uint8_t byte;

void verify_data(std::vector<int16_t>* data);
void read_in_file(char* file_name, std::vector<bit>* input_stream);
void read_in_bytes(char* file_name, std::vector<byte>* input_stream);
void read_mpeg(std::vector<byte>* input_stream);
std::vector<int16_t>* convert_to_16bit_int(std::vector<std::vector<float>*>* data_segments);
void exitProgram(int exit); // to make command line stay open after program is finished

int main(int argc, char* argv[]) {

	char* file;
	string systemCall;
	if(argc != 2) {
		printf("Please input a file name\n");
	//	exitProgram(1);
	}
	file = "imgres.jpg";
    
	systemCall = "ffmpeg -i ";
	systemCall += file;
	systemCall += " -loglevel 0 -vcodec mpeg2video -f mpegts temp.ts"; // 
	// char* file = "~/downloads/imp18.jpg";
	system(systemCall.c_str());
	// read in MPEG
    std::vector<byte>* mpeg_stream = new vector<byte>();

    printf("Reading in file\n");
    read_in_bytes("temp.ts", mpeg_stream);

   // remove(); // removes the temp transport stream 

    // Prints out every 188th byte. It should be 0x47
    /*
    printf("Bits read in:\n");
    for (int i=0; i < mpeg_stream.size(); i+=188*8) {
        for (int j=0; j < 8; j+=4) {
            for (int k=0; k < 4; k++) {
                printf("%i", (int) mpeg_stream.at(i+j+k));
            }
            printf(" ");
        }
        printf("\n");
    }
    */

    // get rid of the MPEG sync byte, so it can later be replaced by the ATSC sync byte
    // divide into 187 byte segments, WITHOUT any sync byte
    printf("Removing MPEG sync byte, and dividing into mpeg packets\n");
    std::vector<vector<bit>*>* mpeg_packets = remove_sync_bit(mpeg_stream);

    vector<vector<int8_t>*>* vsb8_packets = new vector<vector<int8_t>*>();

    for ( int i=0; i < mpeg_packets->size(); i++) {

        // randomize the 187 bytes
        printf("Randomizing packet %i\n", i);
        data_randomize(mpeg_packets->at(i));

        // create the 20 byte reed solomon pairities
        printf("Adding parity to packet %i\n", i);
        add_reed_solomon_parity(mpeg_packets->at(i));


        // Mix up the bits with interleaving
        printf("Interleaving packet %i\n", i);
        mpeg_packets->at(i) = data_interleaving(mpeg_packets->at(i));

        // use trellis encoding to go from symbols to 8-VSB levels
        printf("Trellis Encoding packet %i\n", i);
        vsb8_packets->push_back(trellisEncoder(mpeg_packets->at(i)));
    }

    delete mpeg_packets;

    // synchronize the fields
    printf("Syncing packets\n");
    syncMux(vsb8_packets);
    
    // Pilot insertion
    printf("Performing Pilot insertion\n");
    vector<vector<float>*>* vsb8_signal = pilot_insertion(vsb8_packets);

    // Send to Wave-form generator
    std::vector<int16_t>* as_int16 = convert_to_16bit_int(vsb8_signal);
    printf("Number of ints: %i\n", as_int16->size());
 
    verify_data(as_int16);
    send_data_to_generator(as_int16);
    
    // Cleanup
    for (int i=0; i < vsb8_signal->size(); i++) {
        delete vsb8_signal->at(i);
    }
    delete vsb8_signal;
	system("pause");
	return EXIT_SUCCESS;
}

void verify_data(std::vector<int16_t>* data) {
    std::set<int16_t> my_set;
    for ( int i=0; i < data->size(); i++) {
        my_set.insert(data->at(i));
    }

    printf("Distinct levels: %i\n", my_set.size());
    for ( std::set<int16_t>::iterator iter = my_set.begin(); iter != my_set.end(); iter++) {
        printf("In set: %i\n", *iter);
    }
}

std::vector<int16_t>* convert_to_16bit_int(std::vector<std::vector<float>*>* data_segments) {
    std::vector<int16_t>* signal_map = new std::vector<int16_t>();
    
    for (int i=0; i < data_segments->size(); i++) {
        for (int j=0; j < data_segments->at(i)->size(); j++) {
            signal_map->push_back((int16_t)((data_segments->at(i)->at(j)-1.25)*(32767.0/7)));
        }
    }

    return signal_map;
}

void exitProgram(int exit)
{
	system("pause");
	if(exit == 0)
		std::exit(EXIT_SUCCESS);
	std::exit(EXIT_FAILURE);
}