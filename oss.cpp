#include <cstdio>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>

struct PCB {
int occupied; // either true or false
pid_t pid; // process id of this child
int startSeconds; // time when it was forked
int startNano; // time when it was forked
};

int *shm_ptr;
int sec = 0;
int nanoSec = 0;

// Method to print information on command line arguments
void print_usage(const char * app)
{
        fprintf (stdout, "usage: %s [-h] [-n proc] [-s simul] [-t ]\n",\
                        app);
        fprintf (stdout, "      proc is the number of total children to launch\n");
        fprintf (stdout, "      simul indicates how many children are to be allowed to run simultaneously\n");
        fprintf (stdout, "      iter is the number to pass to the user process\n");
}

void incrementClock()
{
	nanoSec += 100000000;
	if (nanoSec >= 1000000000) {
		nanoSec -= 1000000000;
		sec++;
	}
	shm_ptr[0] = sec;
	shm_ptr[1] = nanoSec;
}

void shareMem()
{
	const int sh_key = ftok("main.c",0);
	int shm_id = shmget( sh_key , sizeof(int) * 2 , IPC_CREAT | 0666  );
	if (shm_id <= 0) 
	{
   		fprintf(stderr,"Shared memory get failed\n");
   		exit(1);
	}

	shm_ptr = (int*)shmat(shm_id, 0, 0);
	if (shm_ptr <= 0 )
	{
		fprintf(stderr, "Shared memory attach failed\n");
		exit(1);
	}

	shm_ptr[0] = 0;
	shm_ptr[1] = 0;
	
	//shmdt(clock);

}

int main(int argc, char* argv[])
{
	shareMem();
	return 0;
}
