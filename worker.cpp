//Author: Maija Garson                                                                                              
//Date: 03/03/2025
//Description: A program that will print it's pid, parent's pid, the system clock time in seconds and nanoseconds, and the time this program should terminate
//in seconds and nanoseconds. It will continue running at a randomly generated value that is at most the specified amount of time given in the command line

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>

int *shm_ptr;

// Function to attach to shared memory
void shareMem()
{
	// Generate key
	const int sh_key = ftok("main.c",0);
	// Access shared memory
	int shm_id = shmget(sh_key, sizeof(int) * 2, 0666);

	// Determines if shared memory was not successful
	if (shm_id == -1) 
	{
		// IF true, print error and exit
		fprintf(stderr, "Child: Shared memory get failed\n");
		exit(1);
	}
	
	// Attach shared memory
	shm_ptr = (int *)shmat(shm_id, 0, 0);
	// Determines if insuccessful
	if (shm_ptr == (int *)-1) 
	{
		// IF true, print error message and exit
		fprintf(stderr, "Child: Shared memory attach failed\n");
		exit(1);
	}


}

int main(int argc, char* argv[])
{

	// Attach to shared memory to access system clock
	shareMem();

	// Tracks time started 
	int startSec = shm_ptr[0];
	int lastKSec = startSec;
	// Represents timme to end
	int endSec;
	int endNs;

	if(argv[2] != nullptr ) // Determines if second argument was passed, meaning value was given for termination for both sec (arg1) and nanosec (arg2)
	{
		// Determine when program should end based on passed values plus current time in system
		endSec = shm_ptr[0] + atoi(argv[1]);
		endNs = shm_ptr[1] + atoi(argv[2]);
	}
	else if(argv[1] != nullptr)// Means no second argument given. Only value for seconds
	{	
		// Convert first argument passed to int that represents upper value for random number to be found
		int upper = atoi(argv[1]);
		// Generate random run time using pid as seed
		srand(getpid()); 
		int runTimeSec = (rand() % upper) + 1;
		long long int runTimeNs = (rand() % 999999999);
		// Calculate time to end program based
		endSec = shm_ptr[0] + runTimeSec;
		endNs = shm_ptr[1] + runTimeNs;
	}
	else if(argv[1] == nullptr)// Invalid amount of arguments passed
	{
		fprintf(stderr, "Error! No value given for termination time. Program will now end.\n");
		return EXIT_FAILURE;
	}	

	// Print starting message
	printf("Worker PID:%d PPID:%d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n", getpid(), getppid(), shm_ptr[0], shm_ptr[1], endSec, endNs);
	printf("--Just starting\n"); 

		// Loop that loops until determined end time is reach
		while(shm_ptr[0] < endSec || (shm_ptr[0] == endSec && shm_ptr[1] <= endNs))
		{
			// Determine if a full second has passed since last check
			if (shm_ptr[0] > lastKSec)
			{	// If true update values to reflect second has passed and print message
				int secPassed = shm_ptr[0] - startSec;
				printf("Worker PID:%d PPID:%d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n", getpid(), getppid(), shm_ptr[0], shm_ptr[1], endSec, endNs);
				printf("--%d seconds have passed since starting\n", secPassed);
				lastKSec = shm_ptr[0];
			}
		}
		
		// Print termination message
		printf("WORKER PID:%d, PPID:%d, SysClockS: % d SysClockNano: %d TermTimeS: %d, TermTimeNano: %d/n", getpid(), getppid(), shm_ptr[0], shm_ptr[1], endSec, endNs);
		printf("--Terminating\n");

		// Detach from shared memory
		shmdt(shm_ptr);
		return 0;
	
}
