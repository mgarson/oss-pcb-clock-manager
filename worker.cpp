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
	printf("Hello from worker!\n");
	int upper = atoi(argv[1]);
	
	srand(getpid()); 
	int runTime = (rand() % upper) + 1;
	int endSec = shm_ptr[0] + runTime;
	int endNs = shm_ptr[1];
	printf("Run time: %d. Generated at: %dsec and %dns\n", runTime, shm_ptr[0], shm_ptr[1]);

	while(shm_ptr[0] < endSec || (shm_ptr[0] == endSec && shm_ptr[1] <= endNs))
	{
	}
	printf("Stopped loop at %dsec and %dns.\n", shm_ptr[0], shm_ptr[1]);
	shmdt(shm_ptr);
	return 0;
}
