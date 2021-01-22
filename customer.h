/**
 * customers.h
 *
 * Header file containing function prototypes for customers and data structures
 * used to hold the threads/customers.
 *
 */

/* mutexe lock for four shared data structures */
pthread_mutex_t mutex_lock;

/* Array of customers */
pthread_t customers[NUMBER_OF_CUSTOMERS];

/* Function prototypes */
int release_resources(int customer_num, int release[]);
int request_resources(int customer_num, int request[]);
