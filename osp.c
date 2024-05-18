// Including necessary built-in libraries:
#include <pthread.h>    // Header for POSIX threads (thread creation and synchronization)
#include <semaphore.h>  // Header for semaphores (synchronization primitives)
#include <stdlib.h>     // Standard library for memory allocation and other utilities
#include <stdio.h>      // Standard I/O library for input and output functions

#define BufferSize 100   // Defining Size of shared memory which is 100.

// Declaring Necessary items:
pthread_mutex_t mutex;  // Mutex lock for thread synchronization
sem_t empty;            // Semaphore to track empty slots in the buffer
sem_t full;             // Semaphore to track filled slots in the buffer

int *arr1dDynamic[5];   // This dynamic array is shared among Producer and Consumer. 
int arr2d[5][5];        // 2D array representing shared memory (5x5 matrix)
int totalEven = 0;      // Counter to keep track of total even numbers found
int out = 0;            // Index for the consumer to retrieve data from arr1dDynamic
int in = 0;             // Index for the producer to insert data into arr1dDynamic

// Thread from Producer:
void *producer(void *pno){
    int i;
    for(i = 0; i < 5; i++) {  // Loop over 5 rows (producers produce row addresses)
        sem_wait(&empty);     // Wait if buffer is empty (decrement empty)
        pthread_mutex_lock(&mutex);  // Acquire mutex lock for critical section
        
        // Initializing Address of first item of each row of 2d array to dynamic 1d array:
        arr1dDynamic[in] = &arr2d[i][0];  // Store the address of the current row in arr1dDynamic
        printf("Producer %d: Insert Address of Value: %d, From Row: %d\n\n", *((int *)pno), *arr1dDynamic[in], in);
        
        in = (in + 1) % BufferSize;  // Update producer's index in circular buffer
        pthread_mutex_unlock(&mutex);  // Release mutex lock
        sem_post(&full);       // Signal that buffer has a new item (increment full)
    }
}

// Thread from Consumer:
void *consumer(void *cno){ 
    int i; 
    for(i = 0; i < 5; i++) {   // Loop over 5 rows (consumers consume row addresses)
        sem_wait(&full);       // Wait if buffer is full (decrement full)
        pthread_mutex_lock(&mutex);  // Acquire mutex lock for critical section
        
        // Counter for even numbers in current row
        int totalEvenCon = 0;
        
        // Iterating through each element in the row
        int j;
        for(j = 0; j < 5; j++){
            int n = *((arr1dDynamic[out]) + j);  // Get the value at the current address
            
            if (n % 2 == 0) {  // Check if the number is even
                printf("%d ", n);  // Print even numbers found
                totalEvenCon++;  // Increment even count
                totalEven++;
            }
        }
        
        printf("\nConsumer %d: Finds Even Numbers from Row: %d, Total Even Numbers Found in a Row: %d\n\n", *((int *)cno), out, totalEvenCon);
        out = (out + 1) % BufferSize;  // Update consumer's index in circular buffer
        pthread_mutex_unlock(&mutex);  // Release mutex lock
        sem_post(&empty);     // Signal that buffer has an empty slot (increment empty)
    }
}

int main(){  
    // Initializing each row of 2d array with values from 0 to 4:    
    int i,j;
    for(i = 0; i < 5; i++){
        for(j = 0; j < 5; j++)
            arr2d[i][j] = j;  // Initializing each element with the column index (0 to 4)
    }
    
    pthread_t pro[5], con[5];  // Arrays of producer and consumer threads
    pthread_mutex_init(&mutex, NULL);  // Initialize mutex lock
    sem_init(&empty, 0, BufferSize);   // Initialize empty semaphore with BufferSize/(maximum number of empty slots)
    sem_init(&full, 0, 0);             // Initialize full semaphore with 0 (buffer initially empty)

    // Array containing IDs for producers and consumers
    int proConNum[2] = {1, 2};
    
    // Creating 2 producer threads and 2 consumer threads
    //int i;
    for(i = 0; i < 2; i++) 
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&proConNum[i]);
        
    for(i = 0; i < 2; i++) 
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&proConNum[i]);
        
    // Waiting for producer threads to complete
    for(i = 0; i < 2; i++)
        pthread_join(pro[i], NULL);
    // Waiting for consumer threads to complete
    for( i = 0; i < 2; i++) 
        pthread_join(con[i], NULL);
    
    // Printing total number of Even numbers found
    printf("Total Even Number found in 2d-Array: %d\n\n", totalEven);

    // Destroying mutex lock and semaphores
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);   
    return 0;
}
