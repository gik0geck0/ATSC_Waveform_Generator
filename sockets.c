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

#define SOCKETS_IP_ADDRESS			"169.254.5.21"
#define SOCKETS_PORT				5025
#define SOCKETS_BUFFER_SIZE			1024
#define SOCKETS_TIMEOUT				10

int WriteString(int MySocket,char string[])
{
	int retval;
	
	if((retval=send(MySocket,string,strlen(string),0))==-1) {
		printf("Error: Unable to send message (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	return retval;
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

int main(int argc,char **argv)
{
	int MySocket,MyControlPort;
	char SocketsBuffer[SOCKETS_BUFFER_SIZE];
	struct sockaddr_in MyAddress,MyControlAddress;
	unsigned int ControlPort;
	int status;
	int retval;
	
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
