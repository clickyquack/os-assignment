# ABOUT
This program is a simple implementation of the producer-consumer problem using a buffer table in C. A more thorough description of how it works is given in the file "[Description.md](Description.md)".

# HOW TO USE
Note that the program has editable parameters starting at line 12 of producer.c, and starting at line 13 of consumer.c, which may be changed from their defaults before compiling for different behavior. A description of each parameter is given in the program on its respective line as a comment in producer.c (also shown in [parameters.png](screenshots/parameters.png)).

In a terminal in a Linux or Unix environment with gcc installed, the program can be run by running:
```
./run_demo.sh
```
in a directory containing the [run_demo.sh](run_demo.sh) shell file and all of the program files, those being [producer.c](producer.c) and [consumer.c](consumer.c). Alternatively, the program can be run by manually running each of the shell file's commands in sequence. Those being:
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

# EXAMPLE SCREENSHOTS
Terminal output when ran with default parameters:

![Default](/screenshots/default.png)

Terminal output when ran with DELAY = 0 and otherwise default parameters:

![No delay](/screenshots/nodelay.png)

Terminal output when ran with INFINITE_LOOP = 1 and otherwise default parameters:

![Infinite loop](/screenshots/infiniteloop.png)

Terminal output when program compiled and ran manually with default parameters:

![Manual run](/screenshots/manual.png)

Parameters as listed in producer.c:

![Parameters](/screenshots/parameters.png)
