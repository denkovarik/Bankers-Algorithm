/**
 * bankers.c
 *
 * Tests and runs the implementation of the Banker's Algorithm.
 *
 */

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include "bank.h"
#include "customer.h"

// Function Prototypes
void create_customers();
void *customer_loop(void *param);
void display_usage();
void initialize(int argc, char *argv[]);
void print_state();
void print_values(int vals[]);
int safety_test();
int validate_command_args(int argc, char *argv[]);



/**
 This is the main program which serves as the start of the program. It tests
 the Bankers algortihm with multiple threads randomly requesting and releasing
 resources.
**/
int main(int argc, char *argv[])
{
	// declare local variables
   	int i, error;

	// Check command line arguments
	if(validate_command_args(argc, argv) != 0)
	{
		display_usage();
		return 1;
	}

	srand(time(NULL));

	// Initialization
	initialize(argc, argv);

   	/* create the threads */
	create_customers();

   	/* join the threads */
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
    	pthread_join(customers[i], NULL);
	}

	// Destroy the mutex lock
    pthread_mutex_destroy(&mutex_lock);
	
   	/* test is finished */

   	return 0;
}



/**
* Creates and initializes all customer threads in the program
**/
void create_customers()
{
	// variable declarations
	int i, error;

	// Create n customers
   	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) 
	{
		// Allocate and initialize argument to pass the student id into 
		// customerLoop function
		int *arg = malloc(sizeof(*arg));
		*arg = i;

		// Create customer thread and pass in customer_id as argument to the 
		// customer_loop function
        error = pthread_create(&(customers[i]), NULL, &customer_loop, (void *)arg);
        if (error != 0)								// Indicate if error occured
            printf("\nCustomer %d can't be created\n", i);
   	}
}



/**
* Simulates the customer. Also is the function used for creating the customer 
* thread
**/
void *customer_loop(void *param)
{
   	/* varaibles */
	int i, needy, amount, held, release_amount;
	int id = *((int *)param);		// The student id passed into the function
	int request[NUMBER_OF_RESOURCES];

	/* Determines in thread still needs resources */ 	
	needy = 0;
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		needy += need[id][i];
	}

	/* Run while thread still needs resources */
   	while (needy > 0 || held > 0) 
	{
		// Generate a random request
		for(i = 0; i < NUMBER_OF_RESOURCES; i++)
		{
			if(need[id][i] > 0)
				request[i] = rand() % (need[id][i] + 1);
			else
				request[i] = 0;
		}

		// Make sure something is being requested otherwise skip this process
		amount = 0;
		for(i = 0; i < NUMBER_OF_RESOURCES; i++)
			amount += request[i];
	
		// Testing Request P1 <1, 0, 2>
		if(amount > 0 && request_resources(id, request) == 0)
   			sleep((rand() % 4));
		else
			sleep(1);


		// See if thread has resources allocated to it
		held = 0;
		for(i = 0; i < NUMBER_OF_RESOURCES; i++)
		{
			held += allocation[id][i];
		}
		
		if(held > 0)
		{
			release_amount = 0;
			// Generate a random release
			for(i = 0; i < NUMBER_OF_RESOURCES; i++)
			{
				if(allocation[id][i] > 0)
					request[i] = rand() % (allocation[id][i] + 1);
				else
					request[i] = 0;
				release_amount += request[i];
			}

			// Make sure amount of resources to release are > than 0
			if(release_amount > 0)
				release_resources(id, request);
		}
	
		// Calculate remaining need
		needy = 0;
		for(i = 0; i < NUMBER_OF_RESOURCES; i++)
		{
			needy += need[id][i];
		}
   	}
}



/**
 Prints the program usage statement to the screen.
**/
void display_usage()
{
	printf("This program requires %d command line aguments. ", NUMBER_OF_RESOURCES);
	printf("Each argument is of type int and represents then number of ");
	printf("available resources of each resource type in order. \n");
	printf("\n\t./bankers numOfResource1 numOfResource2 numOfResource3 ... \n");
}



/**
 Initializes the allocation, need, and available containers. Creates the mutex
 lock used for synchronization among the different threads.
**/
void initialize(int argc, char *argv[])
{
	int i, j, sum;



	/* init available amount of each resource  */
	for(i = 1; i < argc; i++)
	{
		sscanf(argv[i], "%d", &available[i - 1]);
	}

	/* init customer max and allocation */
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		for(j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			maximum[i][j] = rand() % available[j];
			allocation[i][j] = 0;
		}
	}	

	/* Init Need  */
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		for(j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			need[i][j] = maximum[i][j] - allocation[i][j];
		}
	}	

	// Create the general Mutex Lock used for synchronization 
   	if (pthread_mutex_init(&mutex_lock, NULL) != 0)
      	printf("%s\n",strerror(errno));
}



/**
 Prints the current state of the system, which includes the resources 
 available along with the resources allocated to and  needed by each process. 
**/
void print_state()
{
	//char resource;
	int i, j;


	/* Print header info  */
	printf("\t  Allocation\t\t      Need\t\t   Available\n\t");
	for(j = 0; j < 3; j++)
	{
		for(i = 0; i < NUMBER_OF_RESOURCES; i++)
		{
			char resource = (65 + i);
			printf("  %c  ", resource);
		}
		printf("\t\t");
	}
	printf("\n");

	/* Print the allocated, needed, and available resources */
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("P%d\t", i);

		// Print Allocation
		print_values(allocation[i]);
		printf("\t\t");
		// Print Need
		print_values(need[i]);
		printf("\t\t");

		/* Only print available once */
		if(i == 0)
		{
			// Print available
			print_values(available);
		}
		printf("\n");
	}
}



/**
 Formats and prints to the screen the values held in 'vals[]'. 
**/
void print_values(int vals[])
{
	int j;

	/* Print contents */
	for(j = 0; j < NUMBER_OF_RESOURCES; j++)
	{
		/* Formating based on how big number is */
		if(vals[j] < 10)
		{
			printf("  %d  ", vals[j]);
		}
		else if(vals[j] < 100)
		{				
			printf(" %d  ", vals[j]);
		}
		else if(vals[j] < 1000)
		{				
			printf(" %d ", vals[j]);
		}
		else
		{
			printf(" %d", vals[j]);
		}
	}
}



/**
 Releases the allocated resources in the 'request[]' container. Returns 0 on
 success.
**/
int release_resources(int customer_num, int request[])
{
	int i;

	/* acquire the mutex lock */
	if (pthread_mutex_lock(&mutex_lock) != 0)
		printf("P%d %s\n", customer_num, strerror(errno));
	
	// Print before state
	printf("-------------------------------------------------------------------");
	printf("-----\n\n");
	print_state();
	printf("\nRelease P%d <%d, %d, %d> \n\n", customer_num, request[0], 
		request[1], request[2]);

	/* Release the resources */
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		allocation[customer_num][i] -= request[i];
		available[i] += request[i];
	}

	// Print after state
	print_state();
	printf("\n");

	// Release the mutex lock so someone else can use it
    if (pthread_mutex_unlock(&mutex_lock))
    	printf("P%d %s\n", customer_num, strerror(errno));

	return 0;
}



/**
 Handles the requests from the processes. This function will check if it
 is safe to the requested resources. If safe, then this function will allocate
 the requested resources. Returns 0 if resources can be allocated and -1 
 otherwise.
**/
int request_resources(int customer_num, int request[])
{
	int i, safe = 1;

	/* acquire the mutex lock */
	if (pthread_mutex_lock(&mutex_lock) != 0)
		printf("P%d %s\n", customer_num, strerror(errno));

	// Print before state
	printf("-------------------------------------------------------------------");
	printf("-----\n\n");
	print_state();

	printf("\nRequest P%d <%d, %d, %d> \n", customer_num, request[0], 
		request[1], request[2]);

	// Check that request <= need
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		if(request[i] > need[customer_num][i])
		{			
			printf("Unsafe, request denied. \n\n");

			// Print after state
			print_state();
			printf("\n");

			// Release the mutex lock so someone else can use it
    		if (pthread_mutex_unlock(&mutex_lock))
    			printf("P%d %s\n", customer_num, strerror(errno));

			return -1;
		}
	}

	//pretend to have allocated the requested resources
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		allocation[customer_num][i] += request[i];
		available[i] -= request[i];
		need[customer_num][i] -= request[i];

	}

	// Safety Check
	if(safety_test() < 0)
	{
		for(i = 0; i < NUMBER_OF_RESOURCES; i++)
		{
			allocation[customer_num][i] -= request[i];
			available[i] += request[i];
			need[customer_num][i] += request[i];
		}

		printf("Unsafe, request denied. \n\n");

		// Print after state
		print_state();
		printf("\n");

		// Release the mutex lock so someone else can use it
   		if (pthread_mutex_unlock(&mutex_lock))
   			printf("P%d %s\n", customer_num, strerror(errno));

		return -1;

	}

	printf("Safe, request granted. \n\n");

	// Print after state
	print_state();
	printf("\n");
	
	// Release the mutex lock so someone else can use it
    if (pthread_mutex_unlock(&mutex_lock))
    	printf("P%d %s\n", customer_num, strerror(errno));

	return 0;
}



/**
 This function runs the banker's algorithm to determine if the system is will
 be in a safe state after the requested resources are allocated. Returns 0 if
 safe and -1 otherwise.
**/
int safety_test()
{
	int i, j, work[NUMBER_OF_RESOURCES], finish[NUMBER_OF_CUSTOMERS];
	int success_flag = 0, good;

	/* Initialize work and finish containers */
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
		work[i] = available[i];

	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++)
		finish[i] = 0;

	//bankers algorithm
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		// Find a process that is not finished
		if(finish[i] == 0)
		{
			success_flag = -1;
			// Find an unfinished process that has need <= work
			good = 1;
			for(j = 0; j < NUMBER_OF_RESOURCES; j++)
			{
				if(need[i][j] > work[j])
				{
					good = 0;
				}
			}

			/* Found unfinished process with need <= work */
			if(good == 1)
			{
				// Free up allocated resources
				for(j = 0; j < NUMBER_OF_RESOURCES; j++)
				{
					work[j] += allocation[i][j];
				}

				/* Make finished */
				finish[i] = 1;
				i = -1;
				success_flag = 0;
			}
		}
	}

	return success_flag;
}



/**
 Checks for correct command line arguments. Returns 0 if command line 
 arguments are correct and -1 otherwise.
**/
int validate_command_args(int argc, char *argv[])
{
	int i, j;

	// Check for correct number of command line arguments
	if(argc != NUMBER_OF_RESOURCES + 1)
	{
		printf("Invalid number of command line arguments \n");
		return -1;
	}

	// Validate argument data type
	for(i = 1; i < argc; i++)
	{
		for(j = 0; j < strlen(argv[i]); j++)
		{
			if(!isdigit(argv[i][j]))
			{
				printf("Invalid data type for command line arguments. \n");
				return -1;
			}
		}
	}

	return 0;
}
