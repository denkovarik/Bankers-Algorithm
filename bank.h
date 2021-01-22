/**
 * bank.h
 *
 * Header file containing data structures used by the banker's algorithm. 
 *
 */

/* tese with 5 customes and 3 resources */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the available amount of each resource  */
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer  */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
