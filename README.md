# Concurrent Factory Simulation System

## Overview
This system simulates a factory production line with multiple conveyor belts, producers, and consumers working concurrently. The implementation uses POSIX threads (pthreads) to manage parallel operations and synchronization mechanisms to ensure thread-safe operations.

### Components
- Factory Manager (factory_manager.c)
    - The main controller that reads configuration from an input file
    - Creates and manages all process manager threads
    - Coordinates the simultaneous start of all production lines
    - Handles command-line arguments and file parsing

- Process Manager (process_manager.c)
    - Manages individual production lines (belts)
    - Creates producer and consumer threads for each belt
    - Handles the lifecycle of production items
    - Tracks and reports production status

- Queue Implementation (queue.c, queue.h)
    - Thread-safe circular buffer implementation
    - Synchronization using mutexes and condition variables
    - Handles producer-consumer coordination
    - Manages belt capacity and item tracking

### Input File Format
The input file should contain:
```bash
First line: Total number of belts (N)
```
Subsequent lines: Belt configurations in the format:
```bash
<belt_id> <belt_size> <items_to_produce>
```
Example:
```bash
3
1 5 10
2 3 7
3 8 15
```
### Build and Run
#### Compile the program:

```bash
gcc -o factory factory_manager.c process_manager.c queue.c -pthread
```

#### Run the program with an input file:

```bash
./factory input.txt
```
#### Output
The system provides detailed logging with the following format:
```bash
[OK] for successful operations
[ERROR] for any issues encountered
Each message is tagged with its component ([factory_manager], [process_manager], [queue])
```
### Synchronization Details
- Start Coordination: All belts start simultaneously using a semaphore

- Queue Operations:
    - Producers wait when the belt is full
    - Consumers wait when the belt is empty
    - Mutexes protect shared queue state
    - Condition variables signal state changes

### Error Handling
- The system handles various error conditions including:
- Invalid input files
- Memory allocation failures
- Thread creation errors
- Synchronization primitive failures

### Cleanup
All resources (memory, threads, synchronization primitives) are properly cleaned up on exit.

### Notes
- The system is designed to handle multiple production lines concurrently
- Each belt operates independently with its own queue
- The implementation ensures no deadlocks or race conditions in queue operations
