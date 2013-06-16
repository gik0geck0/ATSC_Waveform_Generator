#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <set>
#include <vector>

#include "agilent_wave_interface.cpp"
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
typedef std::vector<vector<byte>*> field;

void verify_data(std::vector<int16_t>* data);
void read_in_file(char* file_name, std::vector<bit>* input_stream);
void read_in_bytes(char* file_name, std::vector<byte>* input_stream);
void read_mpeg(std::vector<byte>* input_stream);

int main() {
    char* file = "tmp/kittens.mpg";
   // char* file = "~/downloads/imp18.jpg";
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

    vector<byte>* fieldBytes = new vector<byte>; //holds all the bytes of a field
    vector<vector<byte>*>* oneField = new vector<vector<byte>*>; // will hold at most 312 data segments
    vector<vector<byte>*>* fieldAllBytes = new vector<vector<byte>*>;
    vector<vector<vector<byte>*>*>* seperatedFields = new vector<vector<vector<byte>*>*>;; //holds the data fields

	vector<byte>*tempBytes; //for temp step

	for(int i=0; i < mpeg_packets->size(); i++){
		tempBytes = makeBytesFromBits(mpeg_packets->at(i)); //get the bytes
		fieldBytes->insert(fieldBytes->end(), tempBytes->begin(), tempBytes->end()); //add bytes to stream
		if(i !=0 && i%312 == 0){
			fieldAllBytes->push_back(fieldBytes);
			fieldBytes = new vector<byte>();
		}
	}	
	if(fieldBytes->size() != 0){
		fieldAllBytes->push_back(fieldBytes);
	}
	for(int i = 0; i < fieldAllBytes->size(); i++){
		data_randomize(fieldAllBytes->at(i));
		oneField = add_reed_solomon_parity(fieldAllBytes->at(i));
		data_interleaving(oneField);
        vector<vector<int8_t>*>* field_vsb8 = trellisEncoder(oneField);
        for (int j=0; j < field_vsb8->size(); j++) {
            vsb8_packets->push_back(field_vsb8->at(i));
        }
	}

    delete mpeg_packets;
    delete tempBytes;

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
