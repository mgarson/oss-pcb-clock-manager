#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>

int *shm_ptr;

void shareMem()
{
	const int sh_key = ftok("main.c",0);
	int shm_id = shmget(sh_key, sizeof(int) * 2, 0666);
	if (shm_id == -1) 
	{
		fprintf(stderr, "Child: Shared memory get failed\n");
		exit(1);
	}

	shm_ptr = (int *)shmat(shm_id, 0, 0);
	if (shm_ptr == (int *)-1) 
	{
		fprintf(stderr, "Child: Shared memory attach failed\n");
		exit(1);
	}


}

int readClock()
{
	int sec = shm_ptr[0];
	int ns = shm_ptr[1];

	//printf("Clock time: %dsec and %dns\n", sec, ns);
	return sec;
}

int main(int argc, char* argv[])
{

	shareMem();
	int endSec;
	int endNs;
	printf("Hello from worker!\n");
	if(argv[2] != nullptr ) // Determines if first argument is null, meaning no value given for iter
	{
		endSec = shm_ptr[0] + atoi(argv[1]);
		endNs = shm_ptr[1] + atoi(argv[2]);
	
		printf("Run time: %ssec and%sns\n", argv[1], argv[2]);
	}
	else
	{	
		int upper = atoi(argv[1]);
	
		srand(getpid()); 
		int runTimeSec = (rand() % upper) + 1;
		long long int runTimeNs = (rand() % 999999999);
		endSec = shm_ptr[0] + runTimeSec;
		endNs = shm_ptr[1] + runTimeNs;
		printf("Run time: %dsec %dns. Generated at: %dsec and %dns\n", runTimeSec, runTimeNs, shm_ptr[0], shm_ptr[1]);
	}

		while(shm_ptr[0] < endSec || (shm_ptr[0] == endSec && shm_ptr[1] <= endNs))
		{
		}
		printf("Stopped loop at %dsec and %dns.\n", shm_ptr[0], shm_ptr[1]);
		shmdt(shm_ptr);
		return 0;
	
}
