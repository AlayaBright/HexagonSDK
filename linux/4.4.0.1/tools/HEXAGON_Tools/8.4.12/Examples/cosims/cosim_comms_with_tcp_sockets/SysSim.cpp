/*******************************************************************************
# Copyright (c) 2020 QUALCOMM INCORPORATED.
# All Rights Reserved.
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HexagonWrapper.h"

#ifndef HEX_CORE
#define HEX_CORE HEX_CPU_V68
#endif

#ifndef TIMING_MODE
#define TIMING_MODE HEX_TIMING
#endif

// This code instantiates two instances of the Hexagon ISS in 'virtual
// platform' mode, where the ISS is configured and controlled by a separate
// entity. Each core is configured to load one copy of the TXRX Cosim code
// and each core is loaded with the same copy of the Hexagon program written
// as a driver for this example. However, each core passes different direction
// arguments to the cosims and the Hexagon program in order to differentiate
// between the server (RX) and client (TX) sides of a TCP socket.
// 
// The 'SysSim' executable takes three arguments:
//    1) full path to Hexagon binary
//    2) full path to cosim configuration file for TX cosim
//    3) full path to cosim configuration file for RX cosim  
int main(int argc, char** argv) 
{
   int errors = 0;
   
   if(argc!=4){
       printf("Usage: SysSim.exe <path to TX/RX test> <path to TX cosim cfg file> <path to RX cosim cfg file>\n");
       exit(1);
   }

   HexagonWrapper* q6Core1 = new HexagonWrapper(HEX_CORE);
   HexagonWrapper* q6Core2 = new HexagonWrapper(HEX_CORE);

   char *elfFile = (char*) argv[1];
   char *txCosimCfg = (char*) argv[2];
   char *rxCosimCfg = (char*) argv[3];

   if (q6Core1->ConfigureTimingMode(TIMING_MODE) != HEX_STAT_SUCCESS)
        fprintf(stderr,"FAIL: Error in setting timing mode\n");

   if (q6Core2->ConfigureTimingMode(TIMING_MODE) != HEX_STAT_SUCCESS)
        fprintf(stderr,"FAIL: Error in setting timing mode\n");

//---------------------------------------------------------------------------
// Configure executables - the same Hexagon binary is loaded into both cores 
//---------------------------------------------------------------------------
   if (q6Core1->ConfigureExecutableBinary(elfFile) != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on ConfigureExecutableBinary()\n");
      errors++;
   }

   if (q6Core2->ConfigureExecutableBinary(elfFile) != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on ConfigureExecutableBinary()\n");
      errors++;
   }

//--------------------------------------------------------------------------
// Configure arguments passed to Hexagon binary. Different functionality
// is enabled depending on which side of the TCP socket the software is 
// intended to communicate with
//--------------------------------------------------------------------------
   char * RX[] = {(char*)"TxRxTest", (char*)"RX"};
   if (q6Core1->ConfigureAppCommandLine(2, RX) != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on ConfigureAppCommandLine()\n");
      errors++;
   }

   char * TX[] = {(char*)"TxRxTest", (char*)"TX"}; 
   if (q6Core2->ConfigureAppCommandLine(2, TX) != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on ConfigureAppCommandLine()\n");
      errors++;
   }

//---------------------------------------------------------------------------
// Configure cosim config files 
//---------------------------------------------------------------------------
   if (q6Core1->ConfigureCosim(rxCosimCfg) != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on ConfigureCosim()\n");
      errors++;
   }

   if (q6Core2->ConfigureCosim(txCosimCfg) != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on ConfigureCosim()\n");
      errors++;
   }

//---------------------------------------------------------------------------
// End of configuration 
//---------------------------------------------------------------------------

   if (q6Core1->EndOfConfiguration() != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on EndOfConfiguration()\n");
      errors++;
   }

   if (q6Core2->EndOfConfiguration() != HEX_STAT_SUCCESS) {
      fprintf(stderr,"Error on EndOfConfiguration()\n");
      errors++;
   }

	if(errors > 0) {
		fprintf(stderr, "There were errors in configuration, terminating test early");
		exit(1);
	}

//---------------------------------------------------------------------------
// Run the simulation by stepping 100 cycles through each core until both
// cores have run the test program to completion 
//---------------------------------------------------------------------------

   HEXAPI_CoreState status1, status2;
   HEX_4u_t cycles = 100;
   HEX_4u_t cyclesExecuted1, cyclesExecuted2, result;

   // cyclesExecuted and result are required args for the Step() API but
   // we don't necessarily care about the values in this context so they
   // aren't checked
   do {
      status1 = q6Core1->Step(cycles, &cyclesExecuted1, &result);
      status2 = q6Core2->Step(cycles, &cyclesExecuted2, &result);
   } while (status1 != HEX_CORE_FINISHED || status2 != HEX_CORE_FINISHED);

//---------------------------------------------------------------------------
// Print statistics and exit
//---------------------------------------------------------------------------  
   
   HEX_8u_t simTimeSeconds, simTimeMicroseconds;
   char statsBuff[1024];

   // Printing out perf stats requires specifying the time range for the stats.
   // Therefore it is necessary to first query the simulator to find out how
   // long the simulation ran. That value can then be used to print stats for
   // the entire simulation run
   if (q6Core1->GetElapsedSimulationTime(&simTimeMicroseconds, HEX_MICROSEC) != HEX_STAT_SUCCESS){
       fprintf(stderr, "Error on GetElapsedSimulationTime()\n");
       errors++;
   }       
   simTimeSeconds = simTimeMicroseconds / 1000;

   if (q6Core1->EmitPerfStatistics(0, 0, simTimeSeconds, simTimeMicroseconds, statsBuff, 1024) != HEX_STAT_SUCCESS) {
       fprintf(stderr, "Error on EmitPerfStatistics()\n");
       errors++;
   }
   fprintf(stdout, "Core 1 Stats:\n");
   fprintf(stdout, "------------------------");
   fprintf(stdout, "%s\n", statsBuff);

   if (q6Core2->GetElapsedSimulationTime(&simTimeMicroseconds, HEX_MICROSEC) != HEX_STAT_SUCCESS){
       fprintf(stderr, "Error on GetElapsedSimulationTime()\n");
       errors++;
   }
   simTimeSeconds = simTimeMicroseconds / 1000;

   if (q6Core2->EmitPerfStatistics(0, 0, simTimeSeconds, simTimeMicroseconds, statsBuff, 1024) != HEX_STAT_SUCCESS) {
       fprintf(stderr, "Error on EmitPerfStatistics()\n");
       errors++;
   }
   fprintf(stdout, "Core 2 Stats:\n");
   fprintf(stdout, "------------------------");
   fprintf(stdout, "%s\n", statsBuff);

   delete(q6Core1);
   delete(q6Core2);

   fprintf(stderr, "API Errors: %d\n", errors);
	 
   return 0;
}

