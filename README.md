# DOCUMENTATION
This program is a simple implementation of the producer-consumer problem using a buffer table in C. That is, a producer process (the code for which is in producer.c) creates items until a buffer table is full, and a consumer process (the code for which is in consumer.c) consumes items until said table is empty. It uses the shared memory and semaphore functionality defined in the C Standard & POSIX library to achieve this. 

The implementation is accomplished by creating and storing a table in a shared memory object using shm_open from the <sys/mman.h> header of the POSIX library. The table is an instance of an explicitly defined struct table_t, containing an array of size TABLE_SIZE (set to 2 by default) of items, and an integer to count the number of items currently in the table. The program also defines semaphores from the <semaphore.h> header for the number of slots full, number of slots empty, and to manage mutual exclusion between threads. Both processes (the code for which is written in producer.c and consumer.c) will either create these shared memory resources, or open them if already created, in their respective main() functions. Then, from the <pthread.h> header, threads are concurrently ran for the producer and consumer.

The thread functions are defined to end their loop after max_runtime seconds (set to 5 by default), although the loop can be made infinite by commenting out the if statement that checks if the time limit has been reached (which is noted in the comments of the code).

The producer thread will wait until both the mutual exclusion semaphore indicates its turn, and the semaphore tracking empty slots shows that the table is not full, before running its critical section. Its critical section adds an item (an integer representing the total number of items produced thus far) to the table, and increments the table position (count) and item number before signaling via the semaphores that its critical section is over. A line giving information about its production is also printed during its critical section. At the end of the loop, an optional delay of 1 second is added by default to make output more easily readable (this may be commented out if desired).

The consumer thread will wait until both the mutual exclusion semaphore indicates its turn, and the semaphore tracking full slots shows that the table is not empty, before running its critical section. Its critical section consumes an item from the table by setting its item variable to the value of the current table position, and decrements the table position (count) to indicate that item has been consumed before signaling via the semaphores that its critical section is over. A line giving information about its consumption is also printed during its critical section. At the end of the loop, an optional delay of 1 second is added by default to make output more easily readable (this may be commented out if desired).

Cleanup functions for both files are also defined to unlink and close the shared resources, and these functions are ran at the beginning and end of execution, and if any setup operation in the main() function fails. If this is not done, the program may reuse data from shared memory from previous instances of running the program, which is unwanted behavior.

# HOW TO RUN
In a terminal in a Linux or Unix environment with gcc installed, the program can be run by running:
```
./run_demo.sh
```
in a directory containing all of the program files. Alternatively, it can be run by manually running each of its commands in sequence. Those being:
```
gcc producer.c -pthread -lrt -o producer
gcc consumer.c -pthread -lrt -o consumer
./producer & ./consumer &
```
Upon doing so, the output should look something like:
```
Producer: Cleanup done
Consumer: Start
Producer: Start
Producer: Produced item 1. Table has 1 items.
Consumer: Consumed item 1. Table has 0 items.
Producer: Produced item 2. Table has 1 items.
Consumer: Consumed item 2. Table has 0 items.
Producer: Produced item 3. Table has 1 items.
Consumer: Consumed item 3. Table has 0 items.
Producer: Produced item 4. Table has 1 items.
Consumer: Consumed item 4. Table has 0 items.
Producer: Produced item 5. Table has 1 items.
Consumer: Consumed item 5. Table has 0 items.
Producer: Produced item 6. Table has 1 items.
Consumer: Consumed item 6. Table has 0 items.
Consumer: Time limit reached
Producer: Time limit reached
Producer: Cleanup done
Consumer: Cleanup done
```
