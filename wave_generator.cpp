#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>

typedef bool bit;

void read_in_file(char* file_name, std::vector<bit>* input_stream);

int main() {

    char* file = "input.txt";

    // read in MPEG
    std::vector<bit> mpeg_stream;
    read_in_file(file, &mpeg_stream);

    /*
    for (int i=0; i < mpeg_stream.size(); i++) {
        //std::cout << "Bit " << i << ": " << mpeg_stream[i] << std::endl;
        printf("Bit %i: %i\n", i, (int) mpeg_stream[i]);
    }
    */

    // divide into 188 byte segments, with the first byte as the sync byte
    // get rid of the MPEG sync byte, and replace it with the ATSC sync byte (later)

    //for ( vector<bit> mpeg_frame in mpeg_stream) {

        // randomize the 187 bytes
        //data_randomize(mpeg_frame);

        // create the 20 byte reed solomon pairities
        //add_reed_solomon_parity(mpeg_frame);

        // use trellis encoding to go from symbols to 8-VSB levels
        //trellis_encode(&mpeg_frame);
        
        //add_segment_sync(&mpeg_frame);
    //}

    // synchronize the fields
    // The first of a set of 313 segments is a field sync
    
    // Pilot insertion

    // Send to Wave-form generator
    
}

void read_in_file(char* file_name, std::vector<bit>* input_stream) {
    std::ifstream file;
    file.open(file_name);

    char input_byte;
    while ( file >> input_byte ) {
        //printf("Reading in byte: %c\n", input_byte);
        for (int i=0; i < 8; i++) {
            input_stream->push_back(input_byte & 0x80);
            input_byte = input_byte << 1;
        }
    }

    file.close();
}
