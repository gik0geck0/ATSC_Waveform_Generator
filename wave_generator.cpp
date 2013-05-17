
typedef bool bit

int main {

    // read in MPEG
    vector<bit> mpeg_stream;

    // divide into 188 byte segments, with the first byte as the sync byte
    // get rid of the MPEG sync byte, and replace it with the ATSC sync byte

    for each segement {

        // randomize the 187 bytes
        data_randomize(mpeg_stream);

        // create the 20 byte reed solomon pairities
        add_reed_solomon_parity(mpeg_stream);

        // use trellis encoding to go from symbols to 8-VSB levels
        trellis_encode(&mpeg_stream);
        
        add_segment_sync(&mpeg_stream);
    }

    // synchronize the fields
    // The first of a set of 313 segments is a field sync
    
    // Pilot insertion

    // Send to Wave-form generator
    
}
