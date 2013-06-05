#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>

#include "data_interleaver.cpp"
#include "data_randomize.cpp"
#include "mpeg2_sync_removal.cpp"
#include "pilot.cpp"
#include "reed_solomon.cpp"
#include "sync.cpp"
#include "trellis.cpp"

#ifdef OS_WINDOWS // for windows systems
   void remove()
   {
    system("del temp.ts");
   }
#else // for linux systems
  void remove()
   {
    system("rm temp.ts");
   }
#endif

typedef bool bit;
typedef uint8_t byte;

void read_in_file(char* file_name, std::vector<bit>* input_stream);
void read_in_bytes(char* file_name, std::vector<byte>* input_stream);
void read_mpeg(std::vector<byte>* input_stream);

int main() {

    char* file = "temp.ts";
    system("ffmpeg -i Testing/image.jpg -loglevel 0 -vcodec mpeg2video -f mpegts temp.ts");
    
    // read in MPEG
    std::vector<byte>* mpeg_stream = new vector<byte>();

    printf("Reading in file\n");
    read_in_bytes(file, mpeg_stream);

    remove(); // removes the temp transport stream 

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
    
    // Cleanup
    for (int i=0; i < vsb8_signal->size(); i++) {
        delete vsb8_signal->at(i);
    }
    delete vsb8_signal;
}
