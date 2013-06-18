#ifdef _WIN32
#else
#include <stdint.h>
#include <iostream>
#include <cstdlib>
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


std::vector<int16_t>* convert_to_16bit_int(std::vector<std::vector<float>*>* data_segments);
char* custom_strdup(const char* input_string, int nchar);
int WriteString(int MySocket,char string[], int size);
int WriteString(int MySocket,char string[]);
int WaitForData(int MySocket);
int ReadString(int MySocket,char *buffer);
void ReadoutErrors(int MySocket);
void DeviceClear(int MySocket,char *buffer);
void SetNODELAY(int MySocket);
int send_data_to_generator(std::vector<int16_t>* integer_data);
#endif