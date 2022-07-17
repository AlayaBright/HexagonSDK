/*****************************************************************
# Copyright (c) 2020 QUALCOMM INCORPORATED.
# All Rights Reserved.
*****************************************************************/
#ifndef __TxRxCosim_H
#define __TxRxCosim_H
#include <HexagonWrapper.h>
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <string.h>
#include <sstream>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#define MSG_DONTWAIT 0
#define close(x) closesocket(x)
#define EXPORT __declspec( dllexport ) 
#pragma comment(lib, "Ws2_32.lib")
#else 
#define EXPORT
#endif

class TxRxCosim {
public:
    int signalRx, signalTx, signalMsg; 
    int dataRx, dataTx, dataMsg;
    struct sockaddr_in dataSockAddress;
    struct sockaddr_in sigSockAddress;
#if defined(_WIN32) || defined(_WIN64)
    const char opt = 1;
	int nonBlock = 100;
	WSADATA wsaData;
	u_long iMode = 1;
#else	
    int opt = 1;
    struct timeval nonBlock;
#endif
    int addrLen = sizeof(sockaddr_in);
    unsigned int  txInit;
    unsigned int  rxAck;
	unsigned int  bufferStart;
    unsigned long bufferSize;
    unsigned int  portNum;
    string        direction;
    char          msgBuffer[1024] = {0};
    char          sigBuffer[10]; 
    char          *HelloDataTx = "Hello from TX cosim";
    char          *HelloDataRx = "Hello from RX cosim";
    char          *ackMsg   = "ACK";
    char          *initMsg  = "INIT";
    unsigned int  msgCount = 0;

 
	HexagonWrapper *pHexagonWrapper;
	TxRxCosim();
	~TxRxCosim();
};

unsigned int cmdLineArgs[4];
string       _direction;

void TimedCallBack(void *);
void MemWrittenCallback(void *, HEX_PA_t, HEX_8u_t, HEX_4u_t);
void OneShotTimedCallback(void *);
int ParseArgs(char*); 

#endif // __TxRx_H

