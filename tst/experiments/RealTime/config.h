#ifndef __CONFIG_H_782FF04FF7FDD7__
#define __CONFIG_H_782FF04FF7FDD7__

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------

#define __ETHERNET__

#define RT_LOGGING
//#define RT_LOGGING_TO_FILE

#define CLOCK_ACCURACY_OUTPUT
//#define DISPATCHER_OUTPUT

// utput verbosity: 0 - no output, 1 - only status changes, 2 - all activities
#define RTSEC_OUTPUT 1
#define RTPEC_OUTPUT 1

// Output by evaluation code
#define RT_TEST_OUTPUT_PER_PERIOD
//#define RT_TEST_STATISTICS

#define CLOCK_ACC_US 10

//#define HIGH_NRT_LOAD

// Communication or end-to-end latency test
#define RT_TEST_COM_LAT

// Define to perform accuracy test for about 300 sec and exit
//#define CLOCK_ACCURACY_TEST

//-----------------------------------------------------------------------------

#endif // __CONFIG_H_782FF04FF7FDD7__


