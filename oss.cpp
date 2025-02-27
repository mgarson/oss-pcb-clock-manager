#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

//Structure to hold values for command line arguments
typedef struct
{
        int proc;       // Number of processes; default 1
        int simul;      // Number of simulatenous processes; default 1
        int timelim;    
	int interval;

} options_t;


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
        fprintf (stdout, "usage: %s [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInMsToLaunchChildren\n",\
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
	 // Structure to hold values for options in command line argument
        options_t options;

        //Set default values
        options.proc = 1;
        options.simul = 1;
        options.timelim = 1;

	 // Values to keep track of child iterations
        int total = 0; // Total amount of processes
        int running = 0; // Processes currently running

        const char optstr[] = "hn:s:t:i:"; // options h, n, s, t, i
        char opt;

	   // Parse command line arguments with getopt
        while ( ( opt = getopt (argc, argv, optstr ) ) != -1 )
        {
                switch ( opt )
                {
                case 'h': // Help
                        //  Prints usage
                        print_usage( argv[0] );
                        return ( EXIT_SUCCESS );
                case 'n': // Total amount of processes
                        // Check if n's argument starts with '-'
                        if (optarg[0] == '-')
                        {
                                // Check if next character starts with other option, meaning no argument given for n and another option given
                                if(optarg[1] == 's' || optarg[1] == 't' || optarg[1] == 'h')
                                {
                                        // Print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! Option n requires an argument.\n");
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                                // Means argument is not another option, but is invalid input
                                else
                                {
                                        // Print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! Invalid input.\n");
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                        }
                        // Loop to ensure all characters in n's argument are digits
                        for(int i = 0; optarg[i] != '\0'; i++)
                        {
                                if (!isdigit(optarg[i]))
                                {
                                        // If non digit is found, print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! %s is not a valid number.\n", optarg);
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                        }

                        // Sets proc to optarg and breaks
                        options.proc = atoi(optarg);
                        break;

                case 's': // Total amount of processes that can run simultaneously
                        // Checks if s's argument starts with '-'
                        if (optarg[0] == '-')
                        {
                                // Checks if next character is character of other option, meaning no argument given for s and another option given
                                if (optarg[1] == 'n' || optarg[1] == 't' || optarg[1] == 'h')
                                {
                                        // Print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! Option s requires an argument.\n");
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                                // Means argument is not another option, but is invalid input
                                else
                                {
                                        // Print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! Invalid input.\n");
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                        }
                        // Loop to ensure all characters in s's argument are digits
                        for(int i = 0; optarg[i] != '\0'; i++)
                        {
                                if (!isdigit(optarg[i]))
                                {
                                        // If non digit is found, print error statement, print usage and exit program
                                        fprintf(stderr, "Error! %s is not a valid number.\n", optarg);
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                        }

                        // Sets simul to optarg and breaks
                        options.simul = atoi(optarg);
                        break;

                case 't': // Total amount of
                        // Checks if t's argument starts with '-'
                        if (optarg[0] == '-')
                        {
                                // Checks if next character is characterof other option, meaning no argument given for t and another option given
                                if (optarg[1] == 'n' || optarg[1] == 's' || optarg[1] == 'h')
                                {
                                        // Print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! Option t requires an argument.\n");
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                                // Means argument is not another option, but is invalid input
                                else
                                {
                                        // Print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! Invalid input.\n");
                                        print_usage(argv[0]);
                                        return EXIT_FAILURE;
                                }
                        }
                        // Loop to ensure all characters in t's argument are digits
                        for(int i = 0; optarg[i] != '\0'; i++)
                        {
                                if (!isdigit(optarg[i]))
                                {
                                        // If non digit is found, print error statement, print usage, and exit program
                                        fprintf(stderr, "Error! %s is not a valid number.\n", optarg);
                                        return EXIT_FAILURE;
                                }
                        }

                        // Sets iter to optarg and breaks
                        options.timelim = atoi(optarg);
                        break;
                default:
                        // Prints message that option given is invalid, prints usage, and exits program
                        printf("Invalid option %c\n", optopt);
                        print_usage(argv[0]);
                        return EXIT_FAILURE;
                }
	}

	shareMem();
	 string str = to_string(options.timelim);
        // Creates new char array to hold value to be passed into child program
        char* arg = new char[str.length()+1];
        // Copies str to arg so it is able to be passed into child program
        strcpy(arg, str.c_str());

        // Loop that will continue until specified amount of child processes is reached or until running processes is 0
        // Ensures only the specified amount of processes are able to be run, and that no processes are still running when loop ends
        while (total < options.proc || running > 0)
        {

                // Loop that will continue until both total amount of processes is greater than/ equal to specified amount
                // and current processes running is greater than/ equal to specified amount
                while (total < options.proc && running < options.simul)
                {
                        // Fork a new child process 
                        pid_t childPid = fork();

                        if (childPid == 0) // Child process
                        {
                                // Create array of arguments to pass to exec. "./user" is the program to execute, arg is the command line argument
                                // to be passed to "./user", and NULL shows it is the end of the argument list
                                char* args[] = {"./worker", arg, NULL};

                                // Replace current process with "./user" process and pass iteration amount as parameter
                                execvp(args[0], args);
                                // If this prints, means exec failed. 
                                // Prints error message and exits
                                fprintf(stderr, "Exec failed, terminating!\n");
                                exit(1);
                        }
                        else // Parent process
                        {
                                // Increment total created processes and running processes
                                total++;
                                running++;
                        }
                }

			int status;
                        // Wait for any child process to finish and set its pid to finishedChild
                        pid_t finishedChild = waitpid(-1, &status, 0);
                        // Ensures a valid pid was returned, meaning child process successfully ended
                        if (finishedChild > 0)
                        {
                                // Decrement amount of processes running 
                                running--;
                        }
        }

	return 0;
}
