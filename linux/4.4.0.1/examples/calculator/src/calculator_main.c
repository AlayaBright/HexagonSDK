#include "calculator_test.h"
#include "rpcmem.h"
#include "remote.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "os_defines.h"

static void print_usage()
{
  printf( "Usage:\n"
    "    calculator [-d domain] [-U unsigned_PD] [-r run_locally] -n array_size\n\n"
    "Options:\n"
    "-d domain: Run on a specific domain.\n"
    "    0: Run the example on ADSP\n"
    "    3: Run the example on CDSP\n"
    "    1: Run the example on MDSP\n"
    "    2: Run the example on SDSP\n"
    "        Default Value: 3(CDSP) for targets having CDSP and 0(ADSP) for targets not having CDSP like Agatti.\n"
    "-U unsigned_PD: Run on signed or unsigned PD.\n"
    "    0: Run on signed PD.\n"
    "    1: Run on unsigned PD.\n"
    "        Default Value: 1\n"
    "-r run_locally:\n"
    "    1: Run locally on APPS.\n"
    "    0: Run on DSP.\n"
    "        Default Value: 1\n"
    "-n array_size: Natural number up to which sum is calculated from 0 to (n-1)\n"
    "        Default Value: 1\n"
    );
}

int main(int argc, char* argv[])
{
  int nErr = 0;
  int runLocal = 0;
  int num = 0;
  int domain = -1;
  int unsignedPDFlag = 1;
  bool isUnsignedPD_Enabled = false;
  int option = 0;

  while ((option = getopt(argc, argv,"d:U:r:n:")) != -1) {
    switch (option) {
      case 'd' : domain = atoi(optarg);
        break;
      case 'U' : unsignedPDFlag = atoi(optarg);
        break;
      case 'r' : runLocal = atoi(optarg);
        break;
      case 'n' : num = atoi(optarg);
        break;
      default:
        print_usage();
      return -1;
    }
  }

  if (domain == -1) {
    printf("\nDSP domain is not provided. Retrieving DSP information using Remote APIs.\n");
    nErr = get_dsp_support(&domain);
    if (nErr != AEE_SUCCESS) {
      printf("ERROR in get_dsp_support: 0x%x, defaulting to CDSP domain\n", nErr);
    }
  }

  if (domain < ADSP_DOMAIN_ID || domain > CDSP_DOMAIN_ID) {
    nErr = AEE_EBADPARM;
    printf("\nERROR 0x%x: Invalid domain %d\n", nErr, domain);
    print_usage();
    goto bail;
  }

  if (unsignedPDFlag < 0 || unsignedPDFlag > 1) {
    nErr = AEE_EBADPARM;
    printf("\nERROR 0x%x: Invalid unsigned PD flag %d\n", nErr, unsignedPDFlag);
    print_usage();
    goto bail;
  }
  if(unsignedPDFlag == 1) {
    if (domain == CDSP_DOMAIN_ID) {
      isUnsignedPD_Enabled = get_unsignedpd_support();
    }
    else {
      printf("Overriding user request for unsigned PD. Only signed offload is allowed on domain %d.\n", domain);
      unsignedPDFlag = 0;
    }
  }

  if(runLocal < 0 || runLocal > 1) {
    printf("Please provide a valid run local value value.\n");
    nErr = -1;
    print_usage();
    goto bail;
  }

  if(num <= 0) {
    printf("Please provide a valid array size value.\n");
    nErr = -1;
    print_usage();
    goto bail;
  }

  printf("\nStarting calculator test\n");
  printf("Attempting to run on %s PD on domain %d\n", unsignedPDFlag==1?"unsigned":"signed", domain);
  nErr = calculator_test(runLocal, domain, num, isUnsignedPD_Enabled);
  if (nErr) {
    printf("ERROR 0x%x: Calculator test failed\n\n", nErr);
  }

bail:
  if (nErr) {
    printf("ERROR 0x%x: Calculator example failed\n\n", nErr);
  } else {
    printf("Success\n\n");
  }

  return nErr;
}
