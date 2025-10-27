# ABOUT
This program is a simple implementation of the producer-consumer problem using a buffer table in C. A more thorough description of how it works is given in the file titled "[Description.md](https://github.com/clickyquack/os-assignment/blob/main/Description.md)".

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

# Example screenshots
Terminal output when ran with default parameters: 
![default](/screenshots/default.png)
