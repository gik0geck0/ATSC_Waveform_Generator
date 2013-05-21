vector<byte>* makeBytesFromBits(vector<bit> *bitStream)
{
	// Does not delete bitStream
	int i,j;
	byte b;
	vector<byte> *byteStream = new vector<byte>();
	for(i = 0; i < bitStream->size(); i+=8)
	{
		b =  (*bitStream)[i];
		for(j = 1; j < 8; j++)
		{
			b <<= 1;
			b += (*bitStream)[i + j]; 	
		}
		byteStream->push_back(b);
	}
	return byteStream;
}

vector<bit> *makeBitsFromBytes(vector<byte> *byteStream)
{
	// Does not delete byteStream
	byte i, j, mask;
	vector<bit> *bitStream = new vector<bit>();

	for(i = 0; i < byteStream->size(); i++)
	{
		mask = 0x80;
		for(j = 8; j > 0; j--)
		{
			bitStream->push_back(byteStream->at(i) & mask);
			mask >>= 1;
		}
	}
	return bitStream;
}
