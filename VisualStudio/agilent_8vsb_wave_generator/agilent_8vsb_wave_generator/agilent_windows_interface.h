/***************************************************************************
* $Header: socketio.c,v 1.5 96/10/04 20:29:32 roger Exp $
* $Revision: 1.5 $
* $Date: 96/10/04 20:29:32 $
*
* $Contributor: LSID, MID $
*
* $Description: Functions to talk to an Agilent E4406A transmitter
* tester via TCP/IP. Uses command-line arguments.
*
* A TCP/IP connection to port 5025 is established and
* the resultant file descriptor is used to "talk" to the
* instrument using regular socket I/O mechanisms. $
*
*
*
* E4406A Examples:
*
* Query the center frequency:
* lanio 15.4.43.5 'sens:freq:cent?'
*
* Query X and Y values of marker 1 and marker 2 (assumes they are on):
* lanio my4406 'calc:spec:mark1:x?;y?; :calc:spec:mark2:x?;y?'
*
* Check for errors (gets one error):
* lanio my4406 'syst:err?'
*
* Send a list of commands from a file, and number them:
* cat scpi_cmds | lanio -n my4406
*
****************************************************************************
*
* This program compiles and runs under
* - HP-UX 10.20 (UNIX), using HP cc or gcc:
* + cc -Aa -O -o lanio lanio.c
* + gcc -Wall -O -o lanio lanio.c
*
* - Windows 95, using Microsoft Visual C++ 4.0 Standard Edition
* - Windows NT 3.51, using Microsoft Visual C++ 4.0
* + Be sure to add WSOCK32.LIB to your list of libraries!
* + Compile both lanio.c and getopt.
* + Consider re-naming the files to lanio.cpp and getopt.cpp
*
* Considerations:
* - On UNIX systems, file I/O can be used on network sockets.
* This makes programming very convenient, since routines like
* getc(), fgets(), fscanf() and fprintf() can be used. These
* routines typically use the lower level read() and write() calls.
*
* - In the Windows environment, file operations such as read(), write(),
* and close() cannot be assumed to work correctly when applied to
* sockets. Instead, the functions send() and recv() MUST be used.
*/

/* Support both Win32 and HP-UX UNIX environment */
#pragma once
#include <stdio.h> /* for fprintf and NULL */
#include <string.h> /* for memcpy and memset */
#include <stdlib.h> /* for malloc(), atol() */
#include <errno.h> /* for strerror */
#include <winsock2.h> // BSD-style socket functions
#include <vector>
#include <stdint.h>
#include <utility>
#include <sstream>
#include <set>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define COMMAND_ERROR (1)
#define NO_CMD_ERROR (0)
#define SCPI_PORT 5025
#define INPUT_BUF_SIZE (64*1024)



char* custom_strdup(const char* input_string, int nchar);
int init_winsock(void);
int close_winsock(void);
SOCKET openSocket(const char *hostname, int portNumber);
int commandInstrument(SOCKET sock,
					  const char command[], int size);
int commandInstrument(SOCKET sock, const char command[]);
char * recv_line(SOCKET sock, char * result, int maxLength);
long queryInstrument(SOCKET sock,
					 const char *command, char *result, size_t maxLength);
void showErrors(SOCKET sock);
int send_data_to_generator(std::vector<int16_t>* integer_data);
