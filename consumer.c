// Same initial setup as producer.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define DELAY 1
#define INFINITE_LOOP 0
#define MAX_RUNTIME 5
#define TABLE_SIZE 2

#define SHARED_MEMORY "/shm_table"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define SEM_MUTEX "/sem_mutex"

typedef struct {
    int items[TABLE_SIZE];
    int count;
} table_t;

table_t *table;
sem_t *empty_sem;
sem_t *full_sem;
sem_t *mutex_sem;
int shm_fd;

// Cleanup function for consumer program
void cleanup() {
    // Unmap table 
    if (table) munmap(table, sizeof(table_t));
    
    // Close shared memory object
    if (shm_fd != -1) {
        close(shm_fd);
        shm_unlink(SHARED_MEMORY);
    }
    
    // Close semaphores
    if (empty_sem) {
        sem_close(empty_sem);
        sem_unlink(SEM_EMPTY);
    }
    if (full_sem) {
        sem_close(full_sem);
        sem_unlink(SEM_FULL);
    }
    if (mutex_sem) {
        sem_close(mutex_sem);
        sem_unlink(SEM_MUTEX);
    }

    printf("Consumer: Cleanup done\n");
}

void* consumer_thread(void* arg) {
    int item; // Integer variable to store consumed items
    time_t start_time = time(NULL);
    
    printf("Consumer: Start\n");
    // Loop until break
    while (1) {
        // Check if loop has run long enough
        if (INFINITE_LOOP) ;
        else if (time(NULL) - start_time > MAX_RUNTIME) {
            printf("Consumer: Time limit reached\n");
            break;
        }

        sem_wait(full_sem); // Wait for table to not be empty
        sem_wait(mutex_sem); // Wait for exclusive access to the table

         // Critical section
         // Remove item from table
        table->count--; // Decrement item count
        item = table->items[table->count]; // Get the item at the new position
        printf("Consumer: Consumed item %d. Table has %d items.\n", item, table->count);
        
        sem_post(mutex_sem); // Release exclusive access to table
        sem_post(empty_sem); // Signal that a slot is now empty
        sleep(DELAY);
    }
    return NULL;
}

int main() {
    sleep(1); // Wait for initial cleanup

    // Create (or open if already exists) shared memory object
    shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    // Set the size of the shared memory object if new
    struct stat st;
    if (fstat(shm_fd, &st) == 0 && st.st_size == 0) {
        if (ftruncate(shm_fd, sizeof(table_t)) == -1) {
            perror("ftruncate");
            close(shm_fd);
            exit(1);
        }
    }
    
    // Map shared memory into process address space
    table = mmap(NULL, sizeof(table_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table == MAP_FAILED) {
        perror("mmap"); 
        cleanup();
        exit(1); 
    }
    
    // Open semaphores, create if they don't exist
    empty_sem = sem_open(SEM_EMPTY, O_CREAT, 0666, TABLE_SIZE);
    full_sem = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    mutex_sem = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    // Print error and exit if any semaphore fails
    if (empty_sem == SEM_FAILED || full_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        perror("sem_open"); 
        cleanup(); 
        exit(1); 
    }
    
    // Create consumer thread
    pthread_t thread;
    int result = pthread_create(&thread, NULL, consumer_thread, NULL);
    if (result != 0) { // Print error and exit if fails
        perror("pthread_create");
        cleanup();
        exit(1);
    }
    
    // Wait for consumer thread to complete
    pthread_join(thread, NULL);
    // Clean up resources before exiting
    cleanup();
    return 0;
}