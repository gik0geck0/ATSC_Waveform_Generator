#include <stdint.h>

vector<int16_t>* convert_to_16bit_int(vector<vector<float>*>* data_segments) {
    vector<int16_t>* signal_map = new vector<int16_t>();
    
    for (int i=0; i < data_segments->size(); i++) {
        for (int j=0; j < data_segments->at(i)->size(); j++) {
            signal_map->push_back((int16_t)(data_segments->at(i)->at(j)*(32767.0/10.0)));
        }
    }

    return signal_map;
}
