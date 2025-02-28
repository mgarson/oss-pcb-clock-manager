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

void readClock()
{
	int sec = shm_ptr[0];
	int ns = shm_ptr[1];

	printf("Clock time: %dsec and %dns\n", sec, ns);
}

int main(int argc, char* argv[])
{
	shareMem();
	readClock();

	shmdt(shm_ptr);
	return 0;
}
