#include <stdio.h> // For printf, perror
#include <stdlib.h> // For exit
#include <unistd.h> // For sleep
#include <pthread.h> // For pthread functions (threads)
#include <semaphore.h> // For semaphore functions (synchronization)
#include <sys/mman.h> // For mmap, munmap (shared memory mapping)
#include <sys/stat.h> // For struct stat and fstat
#include <fcntl.h> // For file control options (O_CREAT, O_RDWR)
#include <time.h> // For time function

// Configuration constants
#define DELAY 1 // Delay in seconds for thread loop iterations, can be 0
#define INFINITE_LOOP 0 // 0 for time-bounded loop, 1 for infinite
#define MAX_RUNTIME 5 // Seconds to run thread loop for (if above set to 0)
#define TABLE_SIZE 2 // Maximum # of items the table can hold

#define SHARED_MEMORY "/shm_table" // Name for shared memory object
#define SEM_EMPTY "/sem_empty" // Name for empty slots semaphore
#define SEM_FULL "/sem_full" // Name for full slots semaphore
#define SEM_MUTEX "/sem_mutex" // Name for mutual exclusion semaphore

// Define buffer structure
typedef struct {
    int items[TABLE_SIZE]; // Array to store items of size TABLE_SIZE
    int count; // Current number of items in the table
} table_t;

// Global variables for shared resources
table_t *table; // Pointer to shared memory table
sem_t *empty_sem; // Semaphore counting empty slots
sem_t *full_sem; // Semaphore counting full slots
sem_t *mutex_sem; // Binary semaphore for mutual exclusion
int shm_fd; // File descriptor for shared memory

// Cleanup function to release shared resources
void cleanup() {
    // Unmap shared memory from process address space
    if (table) munmap(table, sizeof(table_t));
    
    // Close and unlink shared memory object
    if (shm_fd != -1) {
        close(shm_fd); // Close file descriptor
        shm_unlink(SHARED_MEMORY); // Remove shared memory object from system
    }
    
    // Close and unlink semaphores
    if (empty_sem) {
        sem_close(empty_sem); // Close semaphore
        sem_unlink(SEM_EMPTY); // Remove semaphore from system
    }
    if (full_sem) {
        sem_close(full_sem);
        sem_unlink(SEM_FULL);
    }
    if (mutex_sem) {
        sem_close(mutex_sem);
        sem_unlink(SEM_MUTEX);
    }
    
    printf("Producer: Cleanup done\n");
}

void* producer_thread(void* arg) {
    int item = 1; //Integer variable for producing numbered items
    time_t start_time = time(NULL);
    
    printf("Producer: Start\n");
    // Loop until break
    while (1) {
        // Check if run loop has run long enough
        if (INFINITE_LOOP) ;
        else if (time(NULL) - start_time > MAX_RUNTIME) {
            printf("Producer: Time limit reached\n");
            break;
        }

        sem_wait(empty_sem); // Wait for table to not be full
        sem_wait(mutex_sem); // Wait for exclusive access to table
        
        // Critical section
        // Add item to table
        table->items[table->count] = item; // Add item at current position
        table->count++; // Increment item count
        printf("Producer: Produced item %d. Table has %d items.\n", item, table->count);
        item++; // Prepare next item number
        
        sem_post(mutex_sem); // Release exclusive access to table
        sem_post(full_sem); // Signal that a slot is now full
        sleep(DELAY); // Delay next iteration for DELAY seconds
    }
    return NULL;
}

int main() {
    cleanup(); // Cleanup shared memory from past runs in case they failed
    sleep(1); // Start at same time as consumer

    // Create (or open if already exists) shared memory object
    shm_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) { // Print error and exit if fails
        perror("shm_open");
        exit(1);
    }
    
    // Set the size of the shared memory object if new
    struct stat st;
    if (fstat(shm_fd, &st) == 0 && st.st_size == 0) {
        if (ftruncate(shm_fd, sizeof(table_t)) == -1) { // Print error and exit if fails
            perror("ftruncate"); 
            close(shm_fd);
            exit(1);
        }
    }
    
    // Map shared memory into process address space
    table = mmap(NULL, sizeof(table_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table == MAP_FAILED) { // Print error and exit if fails
        perror("mmap");
        cleanup();
        exit(1);
    }
    
    // Initialize the shared table as empty
    table->count = 0;
    // Create (or open if already exists) semaphore for empty slots, starts at TABLE_SIZE
    empty_sem = sem_open(SEM_EMPTY, O_CREAT, 0666, TABLE_SIZE);
    // Semaphore for full slots, starts at 0
    full_sem = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    // Semaphore for mutual exclusion, starts at 1
    mutex_sem = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    // Print error and exit if any semaphore fails
    if (empty_sem == SEM_FAILED || full_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        perror("sem_open"); 
        cleanup(); 
        exit(1); 
    }
    
    // Create producer thread
    pthread_t thread;
    int result = pthread_create(&thread, NULL, producer_thread, NULL);
    if (result != 0) { // Print error and exit if fails
        perror("pthread_create");
        cleanup();
        exit(1);
    }
    
    // Wait for producer thread to complete
    pthread_join(thread, NULL);
    // Clean up resources before exiting
    cleanup();
    return 0;
}
