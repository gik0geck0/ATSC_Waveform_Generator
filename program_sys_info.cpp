
typedef bool bit;

vector<bit>* make_stt();
vector<bit>* make_mgt();
vector<bit>* make_tvct();
vector<bit>* make_eit();

// System Time Table
vector<bit>* make_stt() {
    vector<bit>* stt_table = new vector<bit>();

    // table id
    // 8 bits: always 0xCD
    for (int i=7; i>=0; i--) {
        stt_table.push_back((0xCD >> i) & 0x01);
    }

    // section syntax indicator
    stt_table.push_back(true);

    // private indicator
    // 1 bit: 1
    stt_table.push_back(true);

    // reserved
    // 2 bits: 11
    stt_table.push_back(true);
    stt_table.push_back(true);

    // section length
    // 12 bits describing the length of the remaining bytes in this section (counting after the section length bits)
    // With 0 descriptors, the section length will be 17 bytes remaining = 0x11
    for (int i=11; i>=0; i--) {
        stt_table.push_back((0x11 >> i) & 0x01);
    }

    // table id extension
    // 16 bits: 0x00 00
    for (int i=0; i < 16; i++) {
        stt_table.push_back(false);
    }

    // reserved
    stt_table.push_back(true);
    stt_table.push_back(true);
    
    // version number
    // 5 bits: 0 00 00
    for (int i=0; i < 5; i++) {
        stt_table.push_back(false);
    }
    
    // current_next_indicator
    // 1 bit: 1
    stt_table.push_back(true);
    
    // section number
    // 8 bits: 0x00
    for (int i=0; i < 8; i++) {
        stt_table.push_back(false);
    }

    // last section number
    // 8 bits: 0x00
    for (int i=0; i < 8; i++) {
        stt_table.push_back(false);
    }
    
    // protocol version
    // 8 bits: Will be used in the future. Currently, the only protocol is 0
    for (int i=0; i < 8; i++) {
        stt_table.push_back(false);
    }
    
    // system time
    // 32-bit unsigned int describing the number of GPS seconds since 00:00:00 UTC 6 January 1980
    // This could get complicated. We will be repeating the video over and over, and cannot change the time. So I guess for now, it's going to be the 1980s...
    for (int i=0; i < 32; i++) {
        stt_table.push_back(false);
    }
    
    // gps utc offset
    // 8-bit unsigned int defining the offset from GPS to UTC.
    // UTC = GPS_time - GPS_offset
    for (int i=0; i < 8; i++) {
        stt_table.push_back(false);
    }
    
    // daylight saving
    for (int i=0; i < 16; i++) {
        stt_table.push_back(false);
    }

    
    // for: descriptors
    // Uhhhh... i guess we have 0 descriptors?
    // cool
    
    // CRC 32
    // TODO
    
    return stt_table
}

vector<bit>* make_mgt() {
    vector<bit>* mgt_table = new vector<bit>();

    // table id
    // 8 bits: always 0xC7
    for (int i=7; i>=0; i--) {
        mgt_table.push_back((0xC7 >> i) & 0x01);
    }

    // section syntax indicator
    mgt_table.push_back(true);

    // private indicator
    // 1 bit: 1
    mgt_table.push_back(true);

    // reserved
    // 2 bits: 11
    mgt_table.push_back(true);
    mgt_table.push_back(true);

    // section length
    // 12 bits describing the length of the remaining bytes in this section (counting after the section length bits)
    // With 0 descriptors, the section length will be 17 bytes remaining = 0x11
    // TODO
    for (int i=11; i>=0; i--) {
        mgt_table.push_back((0x11 >> i) & 0x01);
    }

    // table id extension
    // 16 bits: 0x00 00
    for (int i=0; i < 16; i++) {
        mgt_table.push_back(false);
    }

    // reserved
    mgt_table.push_back(true);
    mgt_table.push_back(true);
    
    // version number
    // 5 bits: 0 00 00
    // We can assume that the MGT will never change, so the version number should not change beyond 0
    for (int i=0; i < 5; i++) {
        mgt_table.push_back(false);
    }
    
    // current_next_indicator
    // 1 bit: 1
    mgt_table.push_back(true);
    
    // section number
    // 8 bits: 0x00
    for (int i=0; i < 8; i++) {
        mgt_table.push_back(false);
    }

    // last section number
    // 8 bits: 0x00
    for (int i=0; i < 8; i++) {
        mgt_table.push_back(false);
    }
    
    // protocol version
    // 8 bits: Will be used in the future. Currently, the only protocol is 0
    for (int i=0; i < 8; i++) {
        mgt_table.push_back(false);
    }

    // tables defined
    // 16 bits: defines the number of tables that will be described
    // Terrestrial: 6 - 370 tables

    // table type
    // 16 bits: the type of this table
    
    // table type PID
    // 13 bits: PID for specified table type

    // CRC 32
    // TODO
    
    return mgt_table
    
}
