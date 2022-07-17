/*******************************************************************************
# Copyright (c) 2020 QUALCOMM INCORPORATED.
# All Rights Reserved.
*******************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <hexagon_standalone.h>

//-----------------------------------------------------------------------
//    SocketSim cosim example - Hexagon ISS driver program
//-----------------------------------------------------------------------
/*
   The following program is a contrived example meant to demonstrate
   the use of TCP sockets as a communication channel between Hexagon
   ISS cosimulation modules.
   
   The program uses a set of fixed addresses to represent a memory-mapped 
   communication channel between cores. As such, the program uses a 
   simplistic (and inefficient) 'polling' scheme to coordinate message 
   passing between the 'TX' and 'RX' portions of the main loop. A "real"
   implementation would most likely rely on interrupts to handle the 
   communcations channel rather than polling to avoid wasting cycles 
   waiting for something to happen.
    
   The program is designed to run in one of two modes: 
      Loopback - A single instance of the Hexagon ISS loads both the
                 RX and TX versions of the cosim and runs one
                 copy of the test program with direction set as 'BD'
                 (bi-directional). Messages will loop from the TX 
                 portion of the program -> TX cosim -> RX cosim -> RX
                 portion of the program. Loopback mode is invoked by
                 executing 'make' from within the '/socketsim/cosim_src'
                 directory.
      Mutisim  - Two instances of the Hexagon ISS are created and
                 configured via the HexagonWrapper API. The same program
                 is loaded in each instance of the simulator, but each
                 with a different direction (RX or TX). The 'RX' simulator
                 will load the RX version of the cosim and the 'TX' 
                 simulator will load the TX version of the cosim. Messages
                 will be passed from TX ISS -> TX cosim -> RX cosim ->
                 RX ISS. Multisim mode is invoked by executing 'make' from
                 the top-level '/socketsim' directory.

-------------------------------------------------------------------------*/

// These values represent the memory-mapped communication channel that will 
// be used to read/write messages passed between the RX and TX portions of
// the program
#define TXINITADDR 0xdead0000
#define TXBUFFADDR 0xdead1000
#define RXACKADDR 0xbeef0000
#define RXBUFFADDR 0xbeef1000

// Buffers used to transmit and store messages
const char TXMSG[] = "Current value of msgCount: ";
char       RXMSG[1024] = {0};
char       tmpMSG[1024] = {0};

int main(int argc, char **argv)
{

    char *txBuff, *txInit;
    volatile char *rxBuff, *rxAck; 
    int msgCount;
    char intString[50] = {0};
    const char *direction = argv[1];
    if (NULL == direction) 
    {
        printf("Arguments: TxRxTest <direction>\n");
        printf("  where direction is either 'TX', 'RX, or 'BD'\n");
        exit(1);
    } 

    // Set up uncached memory mapping for TX and RX buffers to be
    // read/written by cosims. This step is not necessary for the 
    // purposes of our example since we're relying on the simulator
    // HexagonWrapper API to handle reads/writes to these locations. 
    // However, they would be needed if we actually wanted to do 
    // something similar on a real target, so it doesn't hurt to 
    // leave them in. 
    if ((add_translation_extended(1, (void*)TXINITADDR, TXINITADDR, 8, 8, 6, 2, 0, 3)) < 0)
        printf("WARN: Could not add TX translation\n");
    if ((add_translation_extended(2, (void*)RXACKADDR, RXACKADDR, 8, 8, 6, 2, 0, 3)) < 0)
        printf("WARN: Could not add RX translation\n");

    // Set up pointers to the comms channel addresses for ease of use 
    txInit = (char*)TXINITADDR;
    txBuff = (char*)TXBUFFADDR;
    rxAck = (char*)RXACKADDR;
    rxBuff = (char*)RXBUFFADDR;

    // The following loop is where the communication over the TCP socket
    // channel happens. The messages passed on the communcation channel
    // are strings constructed of a prefix plus an update to a running
    // counter. I chose strings as data since they can be easily read
    // and understood, not because the data must consist of strings. 
    // **IMPORTANT** 
    // There is *NO* requirement that the data passed over the channel
    // consist of strings - the TCP channel can be used to pass *ANY*
    // data as long as both sides of the channel understand what is
    // being passed and are able to parse it correctly. 
    for (msgCount=0; msgCount<5; msgCount++)
    {
        if (!strcmp(direction, "TX") || !strcmp(direction, "BD"))
        {
            printf("HEXAGON ISS TX: Sending message on data socket...");
            strcat(tmpMSG, TXMSG);
            sprintf(intString, "%d\n", msgCount);
            strcat(tmpMSG, intString); 
            strncpy(txBuff, tmpMSG, sizeof(tmpMSG));
            memset(tmpMSG, 0 , sizeof(tmpMSG));
            printf("done\n");
            *txInit = 1;
            printf("HEXAGON ISS TX: Waiting for ack on signal socket...\n");
            while(*txInit) ; // Address will be updated by cosim when msg is acknowledged
            printf("HEXAGON ISS TX: ...ack received on signal socket\n");
        }
        if (!strcmp(direction, "RX") || !strcmp(direction, "BD"))
        {
            printf("HEXAGON ISS RX: Waiting for message on data socket...\n");
            *rxAck = 1;
#if MULTISIM==1 
            // Wait for signal from cosim confirming arrival of message. Only necessary
            // when running multisim test, we *do not* want to wait when running the
            // loopback test. In loopback mode all the code is running on one simulator,
            // and therefore this while loop  will prevent code execution from returning
            // to the 'TX' section above to transmit another message.
            while (*rxAck) ;
#endif
            strncpy(RXMSG, (const char*)rxBuff, sizeof(RXMSG));
            printf("HEXAGON ISS RX: ...message received on data socket\n");
            printf("HEXAGON ISS RX: Contents of message:   %s", RXMSG);
        }
    }
    return 0;
}
