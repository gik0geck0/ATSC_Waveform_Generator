#include "agilent_windows_interface.h"

std::vector<int16_t>* convert_to_16bit_int(std::vector<std::vector<float>*>* data_segments) {
    std::vector<int16_t>* signal_map = new std::vector<int16_t>();
    
    for (int i=0; i < data_segments->size(); i++) {
        for (int j=0; j < data_segments->at(i)->size(); j++) {
            signal_map->push_back((int16_t)((data_segments->at(i)->at(j)-1.25)*(32767.0/7)));
        }
    }

    return signal_map;
}


char* custom_strdup(const char* input_string, int nchar) {
    char *dup = (char*) malloc(sizeof(char)*nchar);
    if (dup == NULL)
        return NULL;
    memcpy (dup, input_string, nchar);
    return dup;
}

int init_winsock(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	wVersionRequested = MAKEWORD(2, 0);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we couldn't find a useable */
		/* winsock.dll. */
		fprintf(stderr, "Cannot initialize Winsock 1.1.\n");
		return -1;
	}
	return 0;
}


int close_winsock(void)
{
	WSACleanup();
	return 0;
}



/***************************************************************************
> $Function: openSocket$
*
* $Description: open a TCP/IP socket connection to the instrument $
*
* $Parameters: $
* (const char *) hostname . . . . Network name of instrument.
* This can be in dotted decimal notation.
* (int) portNumber . . . . . . . The TCP/IP port to talk to.
* Use 5025 for the SCPI port.
*
* $Return: (int) . . . . . . . . A file descriptor similar to open(1).$
*
* $Errors: returns -1 if anything goes wrong $
*
***************************************************************************/

SOCKET openSocket(const char *hostname, int portNumber)
{
	struct hostent *hostPtr;
	struct sockaddr_in peeraddr_in;
	SOCKET s;

	memset(&peeraddr_in, 0, sizeof(struct sockaddr_in));
	/***********************************************/

	/* map the desired host name to internal form. */

	/***********************************************/
	hostPtr = gethostbyname(hostname);
	if (hostPtr == NULL)
	{
		fprintf(stderr,"unable to resolve hostname '%s'\n", hostname);
		return INVALID_SOCKET;
	}

	/*******************/
	/* create a socket */
	/*******************/

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		fprintf(stderr,"unable to create socket to '%s': %s\n",
			hostname, strerror(errno));
		return INVALID_SOCKET;
	}

	memcpy(&peeraddr_in.sin_addr.s_addr, hostPtr->h_addr, hostPtr->h_length);
	peeraddr_in.sin_family = AF_INET;
	peeraddr_in.sin_port = htons((unsigned short)portNumber);
	if (connect(s, (const struct sockaddr*)&peeraddr_in,
		sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		fprintf(stderr,"unable to create socket to '%s': %s\n",
			hostname, strerror(errno));
		return INVALID_SOCKET;
	}
	return s;
}


/***************************************************************************
*
> $Function: commandInstrument$
*
* $Description: send a SCPI command to the instrument.$
*
* $Parameters: $
* (FILE *) . . . . . . . . . file pointer associated with TCP/IP socket.
* (const char *command) . . SCPI command string.
* $Return: (char *) . . . . . . a pointer to the result string.
*
* $Errors: returns 0 if send fails $
*
***************************************************************************/
int commandInstrument(SOCKET sock,
					  const char command[], int size)
{
	int count;
	/* fprintf(stderr, "Sending \"%s\".\n", command); */
	if (strchr(command, '\n') == NULL) {
		fprintf(stderr, "Warning: missing newline on command %s.\n", command);
	}

	count = send(sock, command, size, 0);
	if (count == SOCKET_ERROR) {
		return COMMAND_ERROR;
	}

	return NO_CMD_ERROR;
}

int commandInstrument(SOCKET sock, const char command[])
{
	return commandInstrument(sock, command, strlen(command));
}

/**************************************************************************
* recv_line(): similar to fgets(), but uses recv()
**************************************************************************/
char * recv_line(SOCKET sock, char * result, int maxLength)
{
	int cur_length = 0;
	int count;
	char * ptr = result;
	int err = 1;

	while (cur_length < maxLength) {
		/* Get a byte into ptr */
		count = recv(sock, ptr, 1, 0);
		/* If no chars to read, stop. */
		if (count < 1) {
			break;
		}
		cur_length += count;

		/* If we hit a newline, stop. */
		if (*ptr == '\n') {
			ptr++;
			err = 0;
			break;
		}
		ptr++;
	}
	*ptr = '\0';
	if (err) 
		return NULL;

	return result;
}

/***************************************************************************
*
> $Function: queryInstrument$
*
* $Description: send a SCPI command to the instrument, return a response.$
*
* $Parameters: $
* (FILE *) . . . . . . . . . file pointer associated with TCP/IP socket.
* (const char *command) . . SCPI command string.
* (char *result) . . . . . . where to put the result.
* (size_t) maxLength . . . . maximum size of result array in bytes.
*
* $Return: (long) . . . . . . . The number of bytes in result buffer.
*
* $Errors: returns 0 if anything goes wrong. $
*
***************************************************************************/
long queryInstrument(SOCKET sock,
					 const char *command, char *result, size_t maxLength)
{
	long ch;
	char tmp_buf[8];
	long resultBytes = 0;
	int command_err;
	int count;

	/*********************************************************
	* Send command to analyzer
	*********************************************************/
	command_err = commandInstrument(sock, command);
	if (command_err) return COMMAND_ERROR;

	/*********************************************************
	* Read response from analyzer
	********************************************************/
	count = recv(sock, tmp_buf, 1, 0); /* read 1 char */
	ch = tmp_buf[0];

	if ((count < 1) || (ch == EOF) || (ch == '\n'))
	{
		*result = '\0'; /* null terminate result for ascii */
		return 0;
	}

	/* use a do-while so we can break out */
	do
	{
		if (ch == '#')
		{
			/* binary data encountered - figure out what it is */
			long numDigits;
			long numBytes = 0;
			/* char length[10]; */
			count = recv(sock, tmp_buf, 1, 0); /* read 1 char */
			ch = tmp_buf[0];
			if ((count < 1) || (ch == EOF)) break; /* End of file */

			if (ch < '0' || ch > '9') break; /* unexpected char */
			numDigits = ch - '0';

			if (numDigits)
			{
				/* read numDigits bytes into result string. */
				count = recv(sock, result, (int)numDigits, 0);
				result[count] = 0; /* null terminate */
				numBytes = atol(result);
			}

			if (numBytes)
			{
				resultBytes = 0;
				/* Loop until we get all the bytes we requested. */
				/* Each call seems to return up to 1457 bytes, on HP-UX 9.05 */
				do {
					int rcount;
					rcount = recv(sock, result, (int)numBytes, 0);
					resultBytes += rcount;
					result += rcount; /* Advance pointer */
				} while ( resultBytes < numBytes );


				/************************************************************

				* For LAN dumps, there is always an extra trailing newline

				* Since there is no EOI line. For ASCII dumps this is

				* great but for binary dumps, it is not needed.

				***********************************************************/

				if (resultBytes == numBytes)
				{
					char junk;
					count = recv(sock, &junk, 1, 0);
				}
			}
			else
			{
				/* indefinite block ... dump til we read only a line feed */
				do
				{
					if (recv_line(sock, result, maxLength) == NULL) break;
					if (strlen(result)==1 && *result == '\n') break;
					resultBytes += strlen(result);
					result += strlen(result);
				} while (1);

			}
		}
		else
		{
			/* ASCII response (not a binary block) */
			*result = (char)ch;
			if (recv_line(sock, result+1, maxLength-1) == NULL) return 0;

			/* REMOVE trailing newline, if present. And terminate string. */
			resultBytes = strlen(result);
			if (result[resultBytes-1] == '\n') resultBytes -= 1;
			result[resultBytes] = '\0';
		}
	} while (0);

	return resultBytes;
}


/*************************************************************************
*
> $Function: showErrors$
*
* $Description: Query the SCPI error queue, until empty. Print results. $
*
* $Return: (void)
*
*************************************************************************/
void showErrors(SOCKET sock)
{
	const char * command = "SYST:ERR?\n";
	char result_str[256];
	do {
		queryInstrument(sock, command, result_str, sizeof(result_str)-1);
		/******************************************************************
		* Typical result_str:
		* -221,"Settings conflict; Frequency span reduced."
		* +0,"No error"
		* Don't bother decoding.
		******************************************************************/
		if (strncmp(result_str, "+0,", 3) == 0) {
			/* Matched +0,"No error" */
			break;
		}
		puts(result_str);
	} while (1);
}


/***************************************************************************
> $Function: main$
*
* $Description: Read command line arguments, and talk to analyzer.
Send query results to stdout. $
*
* $Return: (int) . . . non-zero if an error occurs
*
***************************************************************************/
int send_data_to_generator(std::vector<int16_t>* integer_data)
{
	if (integer_data->size() > 16000000) {
        printf("Yo dawg. Whatcha doin? You're givin me way more ints than I can handle! I got %i, but I can only take %i\n", integer_data->size(), 16000000);
        exit(1);
    }


	SOCKET MySocket;
	char *charBuf = (char *) malloc(INPUT_BUF_SIZE);
	int chr;
	char command[1024];
	char *destination;
	unsigned char quiet = 0;
	unsigned char show_errs = 0;
	int number = 0;

	std::stringstream data_str_builder;
	char* str_num_chars;
	char* data_pointer;
	int num_chars;
	int lenght_num_chars;
	int data_arb_header_length;
	int length_num_chars;
	std::set<std::pair<char, char> > my_set;

	destination = "169.254.5.21";
	strcpy(command, "");

	/**********************************************/
	/* open a socket connection to the instrument */
	/**********************************************/

	if (init_winsock() != 0)
	{
		system("pause");
		exit(1);
	}


	MySocket = openSocket(destination, SCPI_PORT);
	if (MySocket == INVALID_SOCKET) {
		fprintf(stderr, "Unable to open socket.\n");
		system("pause");
		return 1;

	}

	fprintf(stderr, "Socket opened.\n");

	// place instructions here
	commandInstrument(MySocket,"*CLS;*RST\n");
	num_chars = integer_data->size()*2;
	data_pointer = (char*) malloc(num_chars*sizeof(char));
    for (int i=0; i < integer_data->size(); i++) {
        int16_t data_point = integer_data->at(i);
        if (data_point == 32767) {
            data_point = 32511;
        }
        data_pointer[i*2] = (char) (data_point >> 8) & 0x00FF;
        data_pointer[i*2+1] = (char) data_point & 0x00FF;
        //data_pointer[i*2+1] = data_pointer[i*2];
        //data_pointer[i*2] = data_pointer[i*2+1];
       // printf("At %i, Integer %i -> %i + %i\n", i, data_point, (int) data_pointer[i*2], (int) data_pointer[i*2+1]);
    }

    //((char*) ((void*) integer_data));

    str_num_chars = (char*) calloc(10, sizeof(char));
    length_num_chars = sprintf(str_num_chars, "%i", num_chars);
    printf("Number of chars: %s. Length of that as a string: %i\n", str_num_chars, length_num_chars);
    if (length_num_chars > 9 ) {
        printf("Having more than 9 characters representing the number of bytes is unsupported.\n");
        exit(1);
    }
    
    //data_str_builder << "DATA:ARB:DAC atsc_wave,500,400,368,336,304,272,257,496,32767,-32767\n";
    data_str_builder << "DATA:ARB:DAC atsc_wave,#";
    data_str_builder << length_num_chars;
    data_str_builder << str_num_chars;
    data_arb_header_length = data_str_builder.str().length();
    printf("Header '%s' has a length: %i\n", data_str_builder.str().c_str(), data_arb_header_length);

    for (int i=0; i < num_chars; i++) {
        if (i >= 99999 && i <= 100002) {
            //printf("Byte %i: %i\n", i, (int) data_pointer[i]);
        }
        data_str_builder << data_pointer[i];
    }
    data_str_builder << '\n';

    for ( int i=0; i < num_chars; i+=2) {
        my_set.insert(std::pair<char,char>(data_pointer[i], data_pointer[i+1]));
    }

    printf("Distinct levels: %i\n", my_set.size());
    for ( std::set<std::pair<char,char> >::iterator iter = my_set.begin(); iter != my_set.end(); iter++) {
        printf("Byte set: %i,%i\n", (int) (*iter).first, (int) (*iter).second);
    }

    //printf("Last char in string: %c\n", data_str_builder.str().c_str()[num_chars]);
    printf("Using the wave string:\n");
    


    printf("Reading function related errors\n");
	showErrors(MySocket);

    // Function and data setup
    commandInstrument(MySocket, "FORM:BORD NORM\n");
    commandInstrument(MySocket, "FUNC:ARB:PTP 1.0045\n");
    commandInstrument(MySocket, "FUNC ARB\n");
    commandInstrument(MySocket, "FUNC:ARB:FILTER OFF\n");

    // Voltage range setup
    commandInstrument(MySocket, "VOLT:UNIT VPP\n");
    commandInstrument(MySocket, "VOLT 1.0 Vpp\n");
    commandInstrument(MySocket, "VOLT:OFFS 0.0\n");

    commandInstrument(MySocket, "VOLT:LOW 0.0\n");
    commandInstrument(MySocket, "VOLT:HIGH 1.0\n");

    //commandInstrument(MySocket, "VOLT:LIM:LOW 0.0\n");
    //commandInstrument(MySocket, "VOLT:LIM:HIGH 1.0\n");

    commandInstrument(MySocket, "VOLT:LIM:STAT ON\n");

    commandInstrument(MySocket, "FUNC:ARB:SRAT 10763260\n");

    printf("Reading errors after setting voltages\n");
    showErrors(MySocket);

    // upload the wave, and set to display it
    printf("Total bytes to write to socket: %i\n", num_chars+data_arb_header_length+1);

    commandInstrument(MySocket, custom_strdup(data_str_builder.str().c_str(), num_chars+data_arb_header_length+1), num_chars+data_arb_header_length+1);
    /*commandInstrument(MySocket, custom_strdup(data_str_builder.str().c_str(), 400000 + 32), 400000 + 32);*/

    commandInstrument(MySocket, "FUNC:ARB atsc_wave\n");

    printf("Reading errors related to uploading the waveform\n");
	showErrors(MySocket);


    // Show the wave
    commandInstrument(MySocket, "OUTP ON\n");


	// clean up
	free(charBuf);
	delete data_pointer;
		data_pointer = NULL;
	delete str_num_chars;
		str_num_chars = NULL;
	closesocket(MySocket);
	system("pause");
	return 0;
}

/* End of lanio.c */