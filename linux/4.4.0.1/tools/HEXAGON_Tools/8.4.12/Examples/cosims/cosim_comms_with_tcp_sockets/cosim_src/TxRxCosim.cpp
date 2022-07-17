/*****************************************************************
# Copyright (c) 2020 QUALCOMM INCORPORATED.
# All Rights Reserved.
*****************************************************************/
/*

    TxRx interface cosim module 

    This cosim module demonstrates a method for using TCP sockets
    as a communication channel between cosims. Two copies of this
    cosim are created to represent both the server (RX) and client
    (RX) side of a TCP socket. The cosim takes arguments that 
    specify the following values:
      1) Init/Ack address
      2) Read/Write buffer address
      3) Buffer size, in bytes
      4) Port number
      5) Cosim direction (RX or TX)

    The cosim is implemented as a C++ class in order to encapsulate
    data specific to the cosim's role as either a server (RX) or
    a client (TX). Arguments passed to the cosim are parsed in a 
    separate helper function prior to instantiation of the cosim 
    object.

    Two TCP ports are created and configured for use in setting up
    a communication channel between the cosims. For the sake of 
    simplicity, the entire example is structured to poll init/ack
    flags on one socket designated as a 'signal' before reading or
    writing a second socket designated as 'data'.

    The TCP port initalization is begun in the cosim constructors
    and completed within the context of one-shot timed callbacks
    that are called shortly after simulation begins. This approach
    is taken to ensure both cosims and both ports are properly
    instantiated and configured before any attempt to read or write
    to them is made. 

    Once the cosim constructors and one-shot timed callbacks have
    been called, the sockets are ready for use. The following 
    is a high-level description of how messages are passed in this
    example. 

      TX side:
      1) Hexagon ISS TX software writes message into memory-mapped
         buffer
      2) Hexagon ISS TX software writes 0x1 to txInit address
         to initiate message transfer
     3a) Write to txInit triggers TX cosim memory-was-written 
         callback
     3b) Mem-was-written callback reads message out of Hexagon ISS 
         memory into local cosim buffer via ReadMemory() API
     3c) Mem-was-written callback writes message to data socket
     3d) Mem-was-written callback writes 'INIT' to signal socket
      4) Hexagon ISS TX software waits for txInit value read
         from memory to clear, signaling message has been received
      5) TX cosim timed callback periodically checks signal socket 
         for 'ACK' message
      6) If 'ACK' message is read from signal socket, TX cosim
         timed callback writes 0x0 to txInit address using
         WriteMemory() API
      7) Hexagon ISS TX software loops back to step 1 
      
      RX side:
      1) Hexagon ISS RX software writes 0x1 to rxAck address to
         indicate 'wait for message' state
      2) Hexagon ISS RX software waits for rxAck value read from
         memory to clear, signaling a new message is available
     3a) RX cosim timed callback periodically checks signal socket 
         for 'INIT' message
     3b) If 'INIT' message is read from signal socket, RX cosim
         reads message from data socket into internal cosim buffer
     3c) RX cosim writes message into Hexagon ISS memory via 
         WriteMemory() API
     3d) RX cosim writes 'ACK' to signal socket
     3e) RX cosim writes 0x0 to rxAck address to signal message has
         been read successfully
      5) Hexagon ISS RX software loops back to step 1

*/

using namespace std;
#include "TxRxCosim.h"

extern "C" 
{

  EXPORT void *RegisterCosimArgs(char **name, HexagonWrapper *pHW, char *args)
  { 
    int retval;
    *name = "TxRxCosim";
    
    retval = ParseArgs(args);
    if (retval!=0)
    {   
        cout << "Cosim arg format: "\
        "0x<txrx_address> 0x<buffer_start> 0x<buffer_size> PORTNUM <direction>" << endl;
        exit(0);
    }
    //**********************************************
    // TCP sockets setup via constructor and callbacks
    //********************************************** 

    // Instantiate a new cosim object. Initial setup of the sockets is done in
    // the cosim constructor. The port connections are finalized in one-shot
    // timed callbacks registered below, which will be called once both hexagon
    // cores are initialized.  
    TxRxCosim *pTxRxCosim = new TxRxCosim();
    pTxRxCosim->pHexagonWrapper = pHW;

   // At this point the sockets have been initalized but not finalized. Both
   // hexagon cores need to be instantiated and both TX and RX cosims 
   // constructed before the sockets are finalized, otherwise one of the cosims
   // will hang on the first call to listen() since the other cosim will be
   // unable to respond.
   if (pHW->AddOneShotTimedCallback((void *) pTxRxCosim, 1, HEX_PICOSEC,
                                     OneShotTimedCallback)!= HEX_STAT_SUCCESS)
   {   
      fprintf(stderr, "FAIL: Error in setting OneShotTimedCallback\n");
   }


   //************************************************
   // TCP sockets communication handled via callbacks
   //************************************************

    if (pHW->AddTimedCallback((void *) pTxRxCosim, 1, HEX_MILLISEC, TimedCallBack)
        != HEX_STAT_SUCCESS)
    {
        fprintf(stderr, "FAIL: Error in setting AddTimedCallback\n");
    }
   if (pTxRxCosim->direction == "TX")
   {
       if (pHW->AddMemWasWrittenCallback((void *) pTxRxCosim,
                                      pTxRxCosim->txInit,
                                      pTxRxCosim->txInit,
                                      MemWrittenCallback) != HEX_STAT_SUCCESS)
       {
           fprintf(stderr, "FAIL: Error in setting AddMemWasWrittenCallback\n");
       }
   }       
   return pTxRxCosim;
  }
 
  EXPORT char * GetCosimVersion()
  { // Return the Hexagon wrapper version.
	return (char *)HEXAGON_WRAPPER_VERSION;
  }

  EXPORT void UnRegisterCosim(void *pTxRxCosim)
  {  // Call the destructor for TxRxCosim. 
	 delete (TxRxCosim *)pTxRxCosim;
  }
} // extern "C"

// ------------------------------------------------------
// TxRxCosim constructor. Socket construction begins here
// ------------------------------------------------------
TxRxCosim::TxRxCosim()
{
    txInit      = cmdLineArgs[0];
    rxAck       = cmdLineArgs[0];
    bufferStart = cmdLineArgs[1];
    bufferSize  = cmdLineArgs[2];
    portNum     = cmdLineArgs[3];
    direction   = _direction;

    // Timeout values for recv() call, set using setsockopt(). 
	// If recv() is called with no timeout value specified then the default
    // is a blocking call that will wait until a message is received. 
    // We do not want the call to block and cause the simulation to hang
    // so we set the timeout to the smallest possible supported value so it
    // returns almost immediately if there is no message waiting in the socket.

#if defined(_WIN32) || defined(_WIN64)	
int result;
// Initialize Winsock
result = WSAStartup(MAKEWORD(2,2), &wsaData);
if (result != 0) {
    printf("WSAStartup failed: %d\n", result);
	exit(result);
}
#else	
    nonBlock.tv_sec  = 0; 
    nonBlock.tv_usec = 1;  
#endif

    // Build TX or RX connection.
    // If RX, set up server-side socket connection. 
    // If TX, set up client-side connection. 
    if (direction == "RX")
    {
        cout << endl << "-----------------------------------------------" << endl;        
        cout << "RX COSIM: Constructor - Setting up server socket connection" << endl;
        cout << "-----------------------------------------------" << endl << endl;

        // --------------------------------------------------------------
        // Socket creation - All of the socket-specific stuff is using
        // functions defined in sys/socket.h and arpa/inet.h 
        // --------------------------------------------------------------    
        // int sockd = socket(domain, type, protocol)
        //    domain: AF_INET (IPv4 protocol)
        //    type: SOCK_STREAM (TCP connection)
        //    protocol: 0 (Internet Protocol)
        //    sockd: socket descriptor, similar to a file-handle 
#if defined(_WIN32) || defined(_WIN64)
        dataRx = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
		if (dataRx == INVALID_SOCKET)
		{
			wprintf(L"socket creation failed with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
		}	
        signalRx = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (signalRx == INVALID_SOCKET)
		{
			wprintf(L"socket creation failed with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
		}
#else 	
        if (((dataRx = socket(AF_INET, SOCK_STREAM, 0)) < 0) ||
             ((signalRx = socket(AF_INET, SOCK_STREAM, 0)) < 0)) 
        { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
#endif
        // ------------------
        // Set socket options
        // ------------------
        // int return = setsockopt(int sockd, int level, int optname, 
        //                         const void *optval, socklen_t optlen)
        //     sockd: socket descriptor
        //     level: indicates the protocol level that we want to modify
        //            (SOL_SOCKET is the socket layer)
        //     optname: name of option to be set (SO_REUSADDR allow reuse of
        //              local addresses)
        //     optval: value of option to be set, opt initialized to '1' in
        //             header file
        //     optlen: not relevant for SO_REUSADDR option
        //     ?opreturn: zero on success, -1 on error  
#if defined(_WIN32) || defined(_WIN64)
        const int optSize = sizeof(opt);
        if (setsockopt(dataRx, SOL_SOCKET, SO_REUSEADDR, &opt, optSize) == SOCKET_ERROR)
        {
			wprintf(L"setsockopt failed with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
        }
        if (setsockopt(signalRx, SOL_SOCKET, SO_REUSEADDR, &opt, optSize) == SOCKET_ERROR) 
        {
			wprintf(L"setsockopt failed with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
        }
#else
	    if ((setsockopt(dataRx, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) ||
            (setsockopt(signalRx, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)) 
        { 
            perror("setsockopt failed!"); 
            exit(EXIT_FAILURE); 
        }
#endif		
        // --------------------------------------------------------
        // Set socket options, continued. Same as previous, except:
        // --------------------------------------------------------
        //     optname: SO_RCVTIMEO specify receiving timeout value
        //     optval: nonBlock - timeval struct set to smallest possible
        //             timeout to avoid blocking on socket reads. If timeout is
        //             set to zero (default) then the operation will block till
        //             a message arrives on the socket
        //     optlen: not relevant for SO_RCVTIMEO option 
#if defined(_WIN32) || defined(_WIN64) 
        if(ioctlsocket(dataRx, FIONBIO, &iMode) == SOCKET_ERROR)
		{
			wprintf(L"ioctlsocket failed  with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
        }		
        if(ioctlsocket(signalRx, FIONBIO, &iMode) == SOCKET_ERROR)
        {
			wprintf(L"ioctlsocket failed with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
        }
#else
        if ((setsockopt(dataRx, SOL_SOCKET, SO_RCVTIMEO, &nonBlock, sizeof(nonBlock)) < 0) ||
            (setsockopt(signalRx, SOL_SOCKET, SO_RCVTIMEO, &nonBlock, sizeof(nonBlock)) < 0)) 
        {
            perror("setsockopt failed");
            exit(EXIT_FAILURE);
        } 
#endif
        // -------------------------------------------------------  
        // IP socket address format - combination of an IP address 
        // and a 16-bit port number
        // -------------------------------------------------------
        // struct sockaddr_in {
        //       sa_family_t    sin_family; (address family, always AF_INET)
        //       in_port_t      sin_port;   (16-bit port number)
        //       struct in_addr sin_addr;   (IP address)
        //   }
        // htons(portNum): converts port number from host byte order to network
        //                 byte order
        // INADDR_ANY: bind to any/all IP addresses on the local machine
        dataSockAddress.sin_family = AF_INET; 
        dataSockAddress.sin_port = htons(portNum); 
        sigSockAddress.sin_family = AF_INET;
        sigSockAddress.sin_port = htons(portNum+1);
        // Since RX and TX are running on the same machine, we can use the
        // localhost 'loopback' address of '127.0.0.1' as the address for our
        // TCP connection.
        // inet_pton(): converts IPv4/IPv6 addresses from text to binary form
        if ((inet_pton(AF_INET, "127.0.0.1", &dataSockAddress.sin_addr) <= 0) ||
            (inet_pton(AF_INET, "127.0.0.1", &sigSockAddress.sin_addr) <= 0)) 
        { 
            perror("Invalid address or address not supported"); 
            exit(EXIT_FAILURE);
        }   
 
        // --------------------------------------------  
        // Bind a socket descriptor to a socket address
        // -------------------------------------------- 
        // int return = bind(int sockd, const struct sockaddr *addr, 
        //                   socklen_t addrlen)
        //     sockd: socket descriptor
        //     addr:  pointer to sockaddr_in struct initialized in previous
        //            code block
        //     addrlen: size in bytes of address structure pointed to by addr
        //     return: zero on success, -1 on error 
        if ((bind(dataRx, (struct sockaddr *)&dataSockAddress, sizeof(dataSockAddress)) < 0) ||
            (bind(signalRx, (struct sockaddr *)&sigSockAddress, sizeof(sigSockAddress)) < 0))
        { 
            perror("bind failed"); 
            exit(EXIT_FAILURE); 
        } 

        // -----------------------------------------------------------
        // Listen - marks 'sockd' as a socket to be used for accepting
        // incoming connection requests
        // -----------------------------------------------------------
        // int return = listen(int sockd, int backlog)
        //     sockd: socket descriptor
        //     backlog: number of pending connections to allow before returning
        //              an error to clients requesting to connect
        //     return: zero on success, -1 on error
        if ((listen(dataRx, 1) < 0) || (listen(signalRx, 1) < 0))
        { 
            perror("listen failed"); 
            exit(EXIT_FAILURE); 
        } 
    }
    else if (direction == "TX") // client-side setup
    { 
        cout << endl << "-----------------------------------------------" << endl;        
        cout << "TX COSIM: Constructor - Setting up client socket connection" << endl;
        cout << "-----------------------------------------------" << endl << endl;

        // ------------------------------------------------------------------
        // Socket creation - same description as 'RX', but with unique socket
        // descriptors specific to the TX cosim
        // ------------------------------------------------------------------
        if (((dataTx = socket(AF_INET, SOCK_STREAM, 0)) < 0) ||
            ((signalTx = socket(AF_INET, SOCK_STREAM, 0)) < 0))  
        { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 

        // ---------------------------------------------
        // Set socket options - same description as 'RX'
        // ---------------------------------------------		
#if defined(_WIN32) || defined(_WIN64) 
        if(ioctlsocket(dataTx, FIONBIO, &iMode) == SOCKET_ERROR) 
        {
			wprintf(L"ioctlsocket1 failed  with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
        }
		
        //if (setsockopt(signalTx, SOL_SOCKET, SO_RCVTIMEO, (char*)&nonBlock, nonBlockSize) == SOCKET_ERROR) 
        if(ioctlsocket(signalTx, FIONBIO, &iMode) == SOCKET_ERROR) 
        {
			wprintf(L"ioctlsocket2 failed with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
        }		
#else
        if ((setsockopt(dataTx, SOL_SOCKET, SO_RCVTIMEO, &nonBlock, sizeof(nonBlock)) < 0) ||
            (setsockopt(signalTx, SOL_SOCKET, SO_RCVTIMEO, &nonBlock, sizeof(nonBlock)) < 0)) 
        {
            perror("setsockopt failed");
            exit(EXIT_FAILURE);
        } 
#endif

        // ----------------------------------------------------------------  
        // IP socket address format - same description and settings as 'RX'
        // ---------------------------------------------------------------- 
        dataSockAddress.sin_family = AF_INET; 
        dataSockAddress.sin_port = htons(portNum); 
        sigSockAddress.sin_family = AF_INET;
        sigSockAddress.sin_port = htons(portNum+1);
        if ((inet_pton(AF_INET, "127.0.0.1", &dataSockAddress.sin_addr) <= 0) ||
            (inet_pton(AF_INET, "127.0.0.1", &sigSockAddress.sin_addr) <= 0)) 
        { 
            perror("Invalid address or address not supported"); 
            exit(EXIT_FAILURE);
        } 
      
        // ---------------------------------------------------------------
        // connect - initiate a connection on a socket already established
        // by a server, which in our case is the RX cosim 
        // ---------------------------------------------------------------
        // int return = connect(int sockd, const struct sockaddr *addr,
        //                      socken_t addrlen)
        //     sockd: socket descriptor
        //     addr: socket address
        //     addrlen: size, in bytes, of socket address struct
        //     return: zero on success, -1 on error
#if defined(_WIN32) || defined(_WIN64) 		
		uint32_t connectReturn = 0;
		uint32_t selectReturn = 0;
		FD_SET writeSet;
		FD_ZERO(&writeSet);

		connectReturn = connect(dataTx, (struct sockaddr *)&dataSockAddress, sizeof(dataSockAddress));
        // winsock2 sockets can take a while to establish, might need to wait for socket to be writeable
        if (connectReturn == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
        {   
			FD_SET(dataTx, &writeSet); 
			// wait for socket to become writeable  
		    selectReturn = select(0, NULL, &writeSet, NULL, NULL);
            FD_ZERO(&writeSet);
		}
        else if (connectReturn == SOCKET_ERROR) 		
		{	
		    wprintf(L"connect to dataTx failed  with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
		}	

		connectReturn = connect(signalTx, (struct sockaddr *)&sigSockAddress, sizeof(sigSockAddress));
        if (connectReturn == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
        {
			FD_SET(signalTx, &writeSet); 
			// wait for socket to become writeable  
		    selectReturn = select(0, NULL, &writeSet, NULL, NULL);	
            FD_ZERO(&writeSet);
		}
        else if (connectReturn == SOCKET_ERROR) 		
		{	
		    wprintf(L"connect to signalTx failed  with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
		}		
#else	
        if ((connect(dataTx, (struct sockaddr *)&dataSockAddress, sizeof(dataSockAddress)) < 0) ||
            (connect(signalTx, (struct sockaddr *)&sigSockAddress, sizeof(sigSockAddress)) < 0)) 
        {	
            perror("Connect failure"); 
            exit(EXIT_FAILURE);
        }
#endif			

        // ------------------------------------
        // send a message on a connected socket
        // ------------------------------------
        // ssize_t return = send(int sockd, const void *buf, size_t len,
        //                       int flags)
        //     sockd: socket descriptor
        //     buf: character buffer containing message to be sent
        //     len: length, in bytes, of the message contained in 'buf'
        //     flags: bitwise OR of several options. 
        //            MSG_DONTWAIT enables nonblocking operation. If the socket
        //            is unable to accept a message then send() would ordinarly
        //            block until the socket is ready to send. We want send()
        //            to always return instead of potentially blocking the 
        //            cosim and by extension blocking the simulation from
        //            continuing to run.
        //     return: number of bytes sent on success, -1 on error. 
        if ((send(dataTx, HelloDataTx, strlen(HelloDataTx), MSG_DONTWAIT) < 0))
        {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }

		if ((send(signalTx, initMsg, strlen(initMsg), MSG_DONTWAIT) < 0))
		{
            perror("Send failed");
            exit(EXIT_FAILURE);
        }	
        cout << "TX COSIM: Constructor - Test messages sent on data and signal sockets" << endl;
    }
    // Error, neither TX or RX was specified. The constructor is the only place
    // the direction is explicitly checked for either RX or TX.  The rest of
    // the code just checks for direction==RX and assumes TX if the check fails
    else 
    {
        perror("Error in port direction setting, specify TX or RX direction in cosim cfg file");
        exit(EXIT_FAILURE);
    }
}

// ----------------------
// Cosim destructor
TxRxCosim::~TxRxCosim()
{	
    cout << "Destructing TxRX COSIM...";
    // Close all the socket descriptors
#if defined(_WIN32) || defined(_WIN64) 
    closesocket(signalTx);
    closesocket(signalRx);
    closesocket(dataTx);
    closesocket(dataRx);
#else	
    close(signalTx);
    close(signalRx);
    close(dataTx);
    close(dataRx);
#endif	
    cout << "done" << endl;
}

// -----------------------------------------------------------------
// By the time the one-shot timed callback is called, both cosims 
// have been constructed and the socket connections can be completed
// -----------------------------------------------------------------
void OneShotTimedCallback(void *handle)
{
    TxRxCosim *pTxRxCosim = (TxRxCosim*) handle;
 
    if (pTxRxCosim->direction == "RX")
    {

        cout << endl << "-----------------------------------------------------" << endl;
        cout << "  OneShotTimedCallbacks - Finalize setup of sockets" << endl;
        cout << "-----------------------------------------------------" << endl << endl;

        // ---------------------------------------------------------------------
        // accept - takes a pending connection request on a listening socket,
        // creates a new connected socket for it, and returns a new socket 
        // descriptor for the newly connected socket. This is a *different* 
        // socket descriptor than the one assigned to the original listening 
        // socket. A new descriptor is necessary because multiple connections
        // can be made via a single listening socket, so a unique handle is 
        // required for each established connection.
        // 
        // In this cosim use case the socket descriptors 'dataTx' and 'signalTx'
        // are doing double duty - in the TX version of the cosim (client) they
        // hold the client-side handles for the 'data' and 'signal' port
        // connections. In the RX version of the cosim (server), they hold the
        // server-side handles for the 'data' and 'signal' port connections.
        // -------------------------------------------------------------------
        // ssize_t return = accept(int sockd, struct sockaddr *addr,
        //                         socken_t *addrlen)  
        //     sockd: socket descriptor
        //     addr: pointer to structure with address of listening socket to
        //           be checked for a pending connection request
        //     addrlen: size, in bytes, of the sockaddr structure
        //
        //     By default, accept() will block until a connection request is
        //     made on the listening port. This is why accept() is being called
        //     within the context of a one-shot timed callback instead of in the
        //     cosim constructor. We need both cosims to already be instantiated
        //     and have both the server and client side port connections
        //     configured before we attempt to call accept(). In addition, we
        //     need to have requests pending on the listening port before
        //     calling accept() to avoid a blocking call, which is why the TX
        //     cosim sends test messages on both the 'signal and 'data' sockets
        //     within the context of the TX cosim constructor.
        if (((pTxRxCosim->dataTx = accept(pTxRxCosim->dataRx, 
                                          (struct sockaddr *)&pTxRxCosim->dataSockAddress,  
                                          (socklen_t*)&pTxRxCosim->addrLen)) < 0) ||
            ((pTxRxCosim->signalTx = accept(pTxRxCosim->signalRx,
                                            (struct sockaddr *)&pTxRxCosim->sigSockAddress,
                                            (socklen_t*)&pTxRxCosim->addrLen)) < 0)) 
        {   
            perror("accept failed"); 
            exit(EXIT_FAILURE); 
        }

        // -------------------------------------
        // recv messages from a connected socket
        // -------------------------------------
        // ssize_t return = recv(int sockd, void *buf, size_t len, int flags) 
        //     sockd: socket descriptor
        //     buf: character buffer containing message to be sent
        //     len: length, in bytes, of the message contained in 'buf'
        //     flags: bitwise OR of several options. 
        //     return: length, in bytes of message read, -1 if no message read
        // The test messages sent by the TX cosim on the 'signal' and 'data'
        // sockets should be available for reading, so we will read them out
        // of the socket buffer and print them to stdout. 
        pTxRxCosim->signalMsg = recv(pTxRxCosim->signalTx, pTxRxCosim->sigBuffer, 4, MSG_DONTWAIT);
        if (pTxRxCosim->signalMsg > 0)
            cout << "RX COSIM: OneShotTimedCallback - Test message received on signal port" << endl;
        else cout << "RX COSIM: ERROR! No test message received on signal port" << endl;

        pTxRxCosim->dataMsg = recv(pTxRxCosim->dataTx, pTxRxCosim->msgBuffer, 1024, MSG_DONTWAIT);
        if (pTxRxCosim->dataMsg > 0)
		{	
            cout << "RX COSIM: OneShotTimedCallback - Test message received on data port:" << endl;
            cout << "    " << pTxRxCosim->msgBuffer << endl;
        }
        else cout << "RX COSIM: ERROR! No test message received on data port" << endl;

        // ----------------------------------------------------------
        // send messages on a connected socket - described previously
        // ----------------------------------------------------------
        // 
        // Test messages sent by the TX cosim have been received, the
        // RX cosim will now send test replies. Not strictly necessary for
        // establishing/maintaining the port connection, but it helps 
        // demonstrate the use case and also helps confirm that the port
        // connection was established correctly. These replies will be
        // processed in the one-shot timed callback of the TX cosim.
        cout << "RX COSIM: OneShotTimedCallback - Sending data port test reply." << endl;
        send(pTxRxCosim->dataTx, pTxRxCosim->HelloDataRx, 
             strlen(pTxRxCosim->HelloDataRx), MSG_DONTWAIT);

        cout << "RX COSIM: OneShotTimedCallback - Sending signal port test reply." << endl;
        send(pTxRxCosim->signalTx, pTxRxCosim->ackMsg, strlen(pTxRxCosim->ackMsg), MSG_DONTWAIT); 
    }
    else // direction == "TX"
    {
        // ------------------------------------------------------------
        // recv messages from a connected socket - described previously
        // ------------------------------------------------------------
        //
        // At this point the one-shot callback for the RX cosim has been called,
        // so there should be reply messages from the RX cosim waiting in the
        // 'signal' and 'data' sockets. There is no tecnical requirement to have
        // a one-shot timed callback registered for the TX cosim, since at this
        // point the sockets have been successfully configured. The only purpose
        // this callback serves is to pull the test replies out of the sockets 
        // and print them to stdout. This does help verify the socket is 
        // sucessfully passing messages back and forth, so it's worth including. 
        pTxRxCosim->signalMsg = recv(pTxRxCosim->signalTx, pTxRxCosim->sigBuffer, 4, MSG_DONTWAIT);
        if (pTxRxCosim->signalMsg > 0)
            cout << "TX COSIM: OneShotTimedCallback - Test reply received on signal port" << endl;
        else cout << "TX COSIM: ERROR! No test reply received on signal port" << endl;

        pTxRxCosim->dataMsg = recv(pTxRxCosim->dataTx, pTxRxCosim->msgBuffer, 1024, MSG_DONTWAIT);
        if (pTxRxCosim->dataMsg > 0)
        {
            cout << "TX COSIM: OneShotTimedCallback - Test reply received on data port:" << endl;
            cout << "    " << pTxRxCosim->msgBuffer << endl;
        }
        else cout << "TX COSIM: ERROR! No test reply received on data port" << endl;

        cout << "--------------------------------" << endl;
        cout << "  Sockets setup is complete" << endl;
        cout << "--------------------------------" << endl << endl;
    }
}

// -----------------------------------------------------------------------------
// MemWrittenCallback is called when the software running on Hexagon ISS writes
// the memory-mapped addresses specified as 'txInit'. This callback is only 
// registerd by the TX cosim, so no need to check for direction. 
// -----------------------------------------------------------------------------
void MemWrittenCallback(void *handle, HEX_PA_t address, HEX_8u_t value, HEX_4u_t size)
{
    TxRxCosim *pTxRxCosim = (TxRxCosim*) handle;
    HexagonWrapper *pHW = pTxRxCosim->pHexagonWrapper;
    unsigned int i;
    unsigned long long data;
    char *buff = pTxRxCosim->msgBuffer;

    cout << "TX COSIM: MemWrittenCallback called for address 0x" << hex << address << endl;
    cout << "TX COSIM: MemWrittenCallback - Reading in data from TX memory-mapped buffer...";

    // The program running on the Hexagon ISS has initiated a data transfer from
    // one core (the 'TX' core) to another (the 'RX' core). The data will be
    // sent to the RX cosim in three steps:
    //
    // Step 1 - Read message from Hexagon ISS to the cosim's local buffer using
    //          the HexagonWrapper API 
    for (i=0; i<(pTxRxCosim->bufferSize); i+=8)
    {
           pHW->ReadMemory((pTxRxCosim->bufferStart+i), 8, (void*)&data);
           strncpy(buff+i,(const char*) &data, sizeof(data));
    }
    cout << "done" << endl;
		   

    // Step 2 - Write message out to the 'data' socket using TCP send() function
    stringstream txDataStream;
    txDataStream << pTxRxCosim->msgBuffer << endl;
    string txMsg = txDataStream.str();
    if(send(pTxRxCosim->dataTx, txMsg.c_str(), strlen(txMsg.c_str()), MSG_DONTWAIT) < 0)
        cout << "Error sending msg to data socket" << endl;

    // Step 3 - Write the 'init' message out on the 'signal' socket to signal
    //          to the RX cosim that a message is available on the data port
    if (send(pTxRxCosim->signalTx, pTxRxCosim->initMsg, strlen(pTxRxCosim->initMsg), MSG_DONTWAIT) < 0)
        cout << "Error sending msg to signal socket" << endl;
  
    cout << "TX COSIM: MemWrittenCallback - Data and init signal written to sockets" << endl;
    return;
}

// -----------------------------------------------------------------------------
// TimedCallback is registered for use by both the TX and RX versions of the
// cosim, in order to periodically check the signal socket for a message from
// the other cosim.
// -----------------------------------------------------------------------------
void TimedCallBack(void *handle)
{
    TxRxCosim *pTxRxCosim = (TxRxCosim*) handle;
    HexagonWrapper *pHW = pTxRxCosim->pHexagonWrapper;
    unsigned int i;
    unsigned long long data;
    char *pMsgBuff = pTxRxCosim->msgBuffer;

    if (pTxRxCosim->direction == "RX")
    {
        cout << "RX COSIM: TimedCallback - Check signal socket for INIT... ";

        // Periodically check the signal channel for 'INIT', indicating a
        // message is waiting
        
#if defined(_WIN32) || defined(_WIN64)
		int numBytes;
		int strLen = strlen(pTxRxCosim->initMsg);

        numBytes = (recv(pTxRxCosim->signalTx, pTxRxCosim->sigBuffer, strlen(pTxRxCosim->initMsg), MSG_DONTWAIT));
        if (numBytes == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
        {
            wprintf(L"Socket error on recv call in RX cosim with error = %d\n", WSAGetLastError());
            exit(WSAGetLastError());
        }
 		else if ((numBytes != SOCKET_ERROR) || strncmp(pTxRxCosim->initMsg, pTxRxCosim->sigBuffer, strlen(pTxRxCosim->initMsg)))
        {   
            cout << "no signal" << endl;
            return;  
        }
#else
        if ((recv(pTxRxCosim->signalTx, pTxRxCosim->sigBuffer, strlen(pTxRxCosim->initMsg), MSG_DONTWAIT) <= 0) || strncmp(pTxRxCosim->initMsg, pTxRxCosim->sigBuffer, strlen(pTxRxCosim->initMsg)))
        {   
            cout << "no signal" << endl;  
            return;  
        }
#endif	
        // If there is a message, there are four steps to reading the message
        // out of the data socket and writing it back to the 'TX' instance of
        // the Hexagon ISS.
        // 
        // Step 1 - Read the message out of the data socket into the local
        //          cosim buffer      
        if (recv(pTxRxCosim->dataTx, pTxRxCosim->msgBuffer, 1024, MSG_DONTWAIT) <= 0)
        {
            // There should be a message pending in the data socket if INIT has been 
            // written to the signal socket, so a data socket read fail is an error.
            cout << "Error reading message out of data socket" << endl;
            return;
        }
	
        // Step 2 - Write the message into the Hexagon ISS memory via 
        //          HexagonWrapper API   
        cout << "message received" << endl;
        cout << "RX COSIM: TimedCallback - Write data to simulator's memory mapped buffer..."; 
        for (i=0; i<(pTxRxCosim->bufferSize); i+=8)
        {
               strncpy((char*) &data, pMsgBuff+i, sizeof(data));
               pHW->WriteMemory((pTxRxCosim->bufferStart+i), 8, data);
        }
        cout << "...done" << endl;
   
        // Step 3 - Write 'ACK' message out on signal socket.
        cout << "RX COSIM: TimedCallback - Writing 'ACK' message out on signal socket...";
        send(pTxRxCosim->signalTx, pTxRxCosim->ackMsg, strlen(pTxRxCosim->ackMsg), MSG_DONTWAIT); 

        // Step 4 - Write 0x0 to rxAck address to indicate message was read
        //          successfully
        pHW->WriteMemory(pTxRxCosim->rxAck, 8, 0);
        cout << "...done" << endl;
    }
    else // direction == "TX"
    {
        cout << "TX COSIM: TimedCallback - Check signal socket for ACK... ";
         
        // Periodically check the signal channel for 'ACK', indicating the sent
        // message was received by the RX cosim
        if ((recv(pTxRxCosim->signalTx, pTxRxCosim->sigBuffer, strlen(pTxRxCosim->ackMsg), MSG_DONTWAIT) <= 0) || strncmp(pTxRxCosim->ackMsg, pTxRxCosim->sigBuffer, strlen(pTxRxCosim->ackMsg)))
        {
            cout << "no signal" << endl;  
            return; // No pending acknowledgement msg 
        }
     
        // Acknowledgement received, clear the memory-mapped txInit flag to
        // signal that the channel is ready for another message by writing a
        // zero to the txInit address mapped in Hexagon ISS memory using the
        // HexagonWrapper API
        cout << "received, clearing txInit flag" << endl;
        pHW->WriteMemory(pTxRxCosim->txInit, 8, 0);
    }
    return;
}
	

//------------------------------------------------------------
// Helper function that parses the cosim argument string into 
// individual arguments 
//------------------------------------------------------------
int ParseArgs(char *args)
{
    if(args == NULL) return -1;

    int i=0;
    string cString(args);
    string delimiter = " ";
    stringstream token;
    size_t pos = 0;

    // Save the hexadecimal address arguments using the 'space' character
    // as a delimiter to separate the individual arguments
    while (((pos = cString.find(delimiter)) != std::string::npos) && i<3) 
    {
        token.clear();
        token.str(cString.substr(0, pos));
		//cout << "arg: " << token.str() << endl;
        token >> hex >> cmdLineArgs[i++];
        cString.erase(0, pos + delimiter.length());
    }
    // Process the port number as a decimal value using the same method
    while (((pos = cString.find(delimiter)) != std::string::npos) && i<4)
    {
        token.clear();
        token.str(cString.substr(0, pos));
        token >> dec >> cmdLineArgs[i++];
		//cout << "arg: " << token.str() << endl;
        cString.erase(0, pos + delimiter.length());
    }
    // Last item in args list should be TX/RX direction.
    // Parse this value as a string
    {
       token.clear();
       token.str(cString);
       //cout << "arg: " << token.str() << endl;
       token >> _direction;
       i++;
    }    
 
    if (i!=5) return -1; // wrong number of arguments
    
    return 0;
}





