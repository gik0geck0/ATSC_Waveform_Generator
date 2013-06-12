#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bits/socket.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>
#include <sstream>
#include <utility>

#define SOCKETS_IP_ADDRESS			"169.254.5.21"
#define SOCKETS_PORT				5025
#define SOCKETS_BUFFER_SIZE			1024
#define SOCKETS_TIMEOUT				10


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


// sockets.c
// Demonstrates basic instrument control through TCP
// Copyright (C) 2007 Agilent Technologies
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// The GNU General Public License is available at
// http://www.gnu.org/copyleft/gpl.html.
int WriteString(int MySocket,char string[], int size)
{
	int retval;
	
	if((retval=send(MySocket, string, size, 0))==-1) {
		printf("Error: Unable to send message (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	return retval;
}

int WriteString(int MySocket,char string[]) {
    WriteString(MySocket, string, strlen(string));
}

int WaitForData(int MySocket)
{
	fd_set MyFDSet;
	struct timeval tv;
	int retval;
	
	// Wait for data to become available
	FD_ZERO(&MyFDSet); // Initialize fd_set structure
	FD_SET(MySocket,&MyFDSet); // Add socket to "watch list"
	tv.tv_sec=SOCKETS_TIMEOUT; tv.tv_usec=0; // Set timeout
	retval=select(MySocket+1,&MyFDSet,NULL,NULL,&tv); // Wait for change
	
	// Interpret return value
	if(retval==-1) {
		printf("Error: Problem with select (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	return retval; // 0 = timeout, 1 = socket status has changed
}

int ReadString(int MySocket,char *buffer)
{
	int actual;
	
	// Wait for data to become available
	if(WaitForData(MySocket)==0)
	{
		// Timeout
		printf("Warning: Timeout...\n");
		return 0;
	}
	
	// Read data
	if((actual=recv(MySocket,buffer,200,0))==-1) {
		printf("Error: Unable to receice data (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	else {
      	buffer[actual]=0;
	}
	return actual;
}

void ReadoutErrors(int MySocket) {
	char* SocketsBuffer = (char*) calloc(SOCKETS_BUFFER_SIZE, sizeof(char));

    while ( strcmp(SocketsBuffer, "+0,\"No error\"\n")!=0 ) {
        WriteString(MySocket, "SYST:ERR?\n");
        ReadString(MySocket, SocketsBuffer);
        printf("Any errors? %s\n", SocketsBuffer);
    }

}

void DeviceClear(int MySocket,char *buffer)
{
	WriteString(MySocket,"DCL\n");
	if(ReadString(MySocket,buffer)==0)
		return;
	if(strcmp(buffer,"DCL\n")==0)
		printf("DCL\\n received back from instrument...\n");
	else {
		printf("Warning: DCL response: %s\n",buffer);
	}
	return;
}

void SetNODELAY(int MySocket)
{
	int StateNODELAY = 1; // Turn NODELAY on
	int ret;
	
	ret=setsockopt(MySocket, // Handle to socket connection
		IPPROTO_TCP, // Protocol level (TCP)
		TCP_NODELAY, // Option on this level (NODELAY)
		(void *)&StateNODELAY, // Pointer to option variable
		sizeof StateNODELAY); // Size of option variable
	
	if(ret==-1) {
		printf("Error: Unable to set NODELAY option (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	return;
}

int send_data_to_generator(std::vector<int16_t>* integer_data) {
    if (integer_data->size() > 16000000) {
        printf("Yo dawg. Whatcha doin? You're givin me way more ints than I can handle! I got %i, but I can only take %i\n", integer_data->size(), 16000000);
        exit(1);
    }

	int MySocket,MyControlPort;
	char SocketsBuffer[SOCKETS_BUFFER_SIZE];
	struct sockaddr_in MyAddress,MyControlAddress;
	unsigned int ControlPort;
	int status;
	int retval;
    std::stringstream data_str_builder;
    char* data_pointer;
    //char* data_as_chars;
    int num_chars;
    char* str_num_chars;
    int length_num_chars;
    int data_arb_header_length;
    std::set<std::pair<char, char> > my_set;
	
	// Create socket (allocate resources)
	if((MySocket=socket(
		PF_INET, // IPv4
		SOCK_STREAM, // TCP
		0))==-1) {
 		printf("Error: Unable to create socket (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	
	// Establish TCP connection
	memset(&MyAddress,0,sizeof(struct sockaddr_in)); // Set structure to zero
	MyAddress.sin_family=PF_INET; // IPv4
	MyAddress.sin_port=htons(SOCKETS_PORT); // Port number (in network order)
	MyAddress.sin_addr.s_addr=
		inet_addr(SOCKETS_IP_ADDRESS); // IP address (in network order)
	if(connect(MySocket,(struct sockaddr *)&MyAddress,
		sizeof(struct sockaddr_in))==-1) {
		printf("Error: Unable to establish connection to socket (%i)...\n",
			errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	
	// Minimize latency by setting TCP_NODELAY option
	SetNODELAY(MySocket);
	
	// Clear status and reset instrument
	WriteString(MySocket,"*CLS;*RST\n");

	// Get instrument's ID string
	WriteString(MySocket,"*IDN?\n");
	ReadString(MySocket,SocketsBuffer);
	printf("Instrument ID: %s\n",SocketsBuffer);
	
	// Ask for control port
	WriteString(MySocket,"SYST:COMM:TCPIP:CONTROL?\n");
	if(ReadString(MySocket,SocketsBuffer)==0)
	{
		printf("Warning: No response from instrument (control port).\n");
		retval=0;
		goto SocketMainClose;
	}
	sscanf(SocketsBuffer,"%u",&ControlPort);
	printf("Control Port: %u\n",ControlPort);
	
	// Create socket for control port
	if((MyControlPort=socket(PF_INET,SOCK_STREAM,0))==-1)
	{
		printf("Error: Unable to create control port socket (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		retval=1;
		goto SocketMainClose;
	}

	// Establish TCP connection to control port
	memset(&MyControlAddress,0,
		sizeof(struct sockaddr_in)); // Set structure to zero
	MyControlAddress.sin_family=PF_INET; // IPv4
	MyControlAddress.sin_port=
		htons(ControlPort); // Port number (in network order)
	MyControlAddress.sin_addr.s_addr=
		inet_addr(SOCKETS_IP_ADDRESS); // IP address (in network order)
	if(connect(MyControlPort,(struct sockaddr *)&MyControlAddress,
		sizeof(struct sockaddr_in))==-1) {
		printf(
			"Error: Unable to establish connection to control port (%i)...\n",
			errno);
		perror("sockets"); // Print error message based on errno
		retval=1;	
		goto SocketMainClose;
	}
	
	// Do a device clear
	DeviceClear(MyControlPort,SocketsBuffer);
    num_chars = integer_data->size()*2;
    //num_chars = 99999;
    //num_chars = 100000;
    //= (void*) malloc((integer_data->size()*2+1)*sizeof(char));
    //data_as_chars = (char*) calloc(num_chars, sizeof(char));
    
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

    /*
    //data_str_builder << "220";

    // 500
    for (int i=0; i < 100000; i++) {
        // 32767
        data_str_builder << (char) 0x7F;
        data_str_builder << (char) 0xFF;

        // -32767
        data_str_builder << (char) 0x80;
        data_str_builder << (char) 0x01;
    }

    // 400
    data_str_builder << (char) 0x01;
    data_str_builder << (char) 0x90;

    // 368
    data_str_builder << (char) 0x01;
    data_str_builder << (char) 0x70;

    // 336
    data_str_builder << (char) 0x01;
    data_str_builder << (char) 0x50;

    // 304
    data_str_builder << (char) 0x01;
    data_str_builder << (char) 0x30;

    // 272
    data_str_builder << (char) 0x01;
    data_str_builder << (char) 0x10;

    // 257
    data_str_builder << (char) 0x01;
    data_str_builder << (char) 0x01;

    // 496
    data_str_builder << (char) 0x01;
    data_str_builder << (char) 0xF0;

    // 32767
    data_str_builder << (char) 0x7F;
    data_str_builder << (char) 0xFF;

    // -32767
    data_str_builder << (char) 0x80;
    data_str_builder << (char) 0x01;

    data_str_builder << '\n';
    */

    for (int i=0; i < num_chars; i++) {
        if (i >= 99999 && i <= 100002) {
            printf("Byte %i: %i\n", i, (int) data_pointer[i]);
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
    /*
    for (int i=0; i < num_chars+data_arb_header_length+1; i++) {
        printf("Byte %i: %i\n", i, (int) data_str_builder.str().c_str()[i]);
    }
    */
    //printf("%s\n", data_str_builder.str().c_str());
	
    // Initial wave setup
    //WriteString(MySocket, "DATA:ARB:DAC atsc_wave,500,600,1000,12000,-8000,9000,-20000,30000\n");
    
    // Frequency range setup
    


    printf("Reading function related errors\n");
    ReadoutErrors(MySocket);

    // Function and data setup
    WriteString(MySocket, "FORM:BORD NORM\n");
    WriteString(MySocket, "FUNC:ARB:PTP 1.0045\n");
    WriteString(MySocket, "FUNC ARB\n");
    WriteString(MySocket, "FUNC:ARB:FILTER OFF\n");

    // Voltage range setup
    WriteString(MySocket, "VOLT:UNIT VPP\n");
    WriteString(MySocket, "VOLT 1.0 Vpp\n");
    WriteString(MySocket, "VOLT:OFFS 0.0\n");

    WriteString(MySocket, "VOLT:LOW 0.0\n");
    WriteString(MySocket, "VOLT:HIGH 1.0\n");

    //WriteString(MySocket, "VOLT:LIM:LOW 0.0\n");
    //WriteString(MySocket, "VOLT:LIM:HIGH 1.0\n");

    WriteString(MySocket, "VOLT:LIM:STAT ON\n");

    WriteString(MySocket, "FUNC:ARB:SRAT 10763260\n");

    printf("Reading errors after setting voltages\n");
    ReadoutErrors(MySocket);

    // upload the wave, and set to display it
    printf("Total bytes to write to socket: %i\n", num_chars+data_arb_header_length+1);

    WriteString(MySocket, custom_strdup(data_str_builder.str().c_str(), num_chars+data_arb_header_length+1), num_chars+data_arb_header_length+1);
    /*WriteString(MySocket, custom_strdup(data_str_builder.str().c_str(), 400000 + 32), 400000 + 32);*/

    WriteString(MySocket, "FUNC:ARB atsc_wave\n");

    printf("Reading errors related to uploading the waveform\n");
    ReadoutErrors(MySocket);


    //
    // Show the wave
    WriteString(MySocket, "OUTP ON\n");
    //WriteString(MySocket, "SYST:ERR?\n");
    //ReadString(MySocket, SocketsBuffer);
    
    /*
    WriteString(MySocket, "SYST:ERR?\n");
    ReadString(MySocket, SocketsBuffer);
    */

    /*
    printf("Any errors? %i\n", (int) strcmp(SocketsBuffer, "+0,\"No Error\"\n"));
    printf("'%s'\n", SocketsBuffer);
    printf("'%s'\n", "+0,\"No Error\"\n");
    */


    /*
	// Demonstrate SRQ... First, tell the instrument when to generate an SRQ
	// NOTE: The following commands are dependant of the instrument
	// This example is for the Agilent L4410A, L4411A,34410A or 34411A
	WriteString(MySocket,
		"*ESE 1\n"); // Operation Complete causes Standard Event
	WriteString(MySocket,
		"*SRE 32\n"); // Standard Event causes SRQ
	
	// Now perform some operation(s) and raise SRQ when done
	WriteString(MySocket,"CONF:FREQ\n"); // Go to frequency measurement mode
	WriteString(MySocket,"*OPC\n"); // Tell us when you're ready!
   
	// Now wait until data becomes available on the control link
	if(WaitForData(MyControlPort)==0)
	{
		printf("Warning: Instrument did not generate SRQ.\n");
		retval=0;
		goto SocketClose;
	}
	
	// Data available... Read and interpret!
	ReadString(MyControlPort,SocketsBuffer);
	printf("Data read on control port: %s\n",SocketsBuffer);
	if(strncmp(SocketsBuffer,"SRQ",3)!=0) {
		printf("Warning: Didn't receive an SRQ!\n");
		retval=0;
		goto SocketClose;
	}
	sscanf(SocketsBuffer,"SRQ%i",&status);
	printf("Status is: %d",status);

    */
SocketClose:	
	
	// Close control port
	if(close(MyControlPort)==-1) {
		printf("Error: Unable to close control port socket (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		retval=1;
	}

SocketMainClose:
	
	// Close main port
	if(close(MySocket)==-1) {
		printf("Error: Unable to close socket (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		retval=1;;
	}

	exit(retval);
}
