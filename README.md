# Operating Systems Project

This repository contains a comprehensive Operating Systems project implementation featuring CPU scheduling algorithms and memory management systems. The project is divided into two phases, each demonstrating different aspects of operating system concepts.

## Project Structure

```
OS/
├── ph1/                    # Phase 1: CPU Scheduling Algorithms
│   ├── scheduler.c         # Main scheduler implementation
│   ├── process_generator.c # Process generation and simulation
│   ├── process.c          # Individual process implementation
│   ├── clk.c             # Clock management system
│   ├── GUI.c             # Graphical user interface
│   ├── Algorithms.h      # CPU scheduling algorithms
│   ├── DataStructure.h   # Data structures for queues
│   ├── headers.h         # Common headers and utilities
│   ├── Makefile          # Build configuration
│   └── README            # Phase-specific documentation
├── ph2/                    # Phase 2: Memory Management + CPU Scheduling
│   ├── scheduler.c         # Enhanced scheduler with memory management
│   ├── process_generator.c # Process generation with memory requirements
│   ├── process.c          # Individual process implementation
│   ├── clk.c             # Clock management system
│   ├── GUI.c             # Enhanced GUI with memory visualization
│   ├── Algorithms.h      # CPU scheduling algorithms
│   ├── DataStructure.h   # Data structures for queues
│   ├── Memory.h          # Memory management implementation
│   ├── headers.h         # Common headers and utilities
│   ├── Makefile          # Build configuration
│   └── README            # Phase-specific documentation
└── README.md             # This file
```

## Phase 1: CPU Scheduling Algorithms

### Implemented Algorithms

1. **Highest Priority First (HPF)**
   - Non-preemptive priority-based scheduling
   - Processes are scheduled based on their priority level
   - Lower priority numbers indicate higher priority

2. **Shortest Remaining Time Next (SRTN)**
   - Preemptive scheduling algorithm
   - Always schedules the process with the shortest remaining execution time
   - Supports process preemption when a shorter process arrives

3. **Round Robin (RR)**
   - Preemptive scheduling with time quantum
   - Each process gets a fixed time slice (quantum)
   - Fair scheduling that prevents starvation

### Key Features

- **Process States**: arrived, started, resumed, stopped, finished
- **Real-time Process Management**: Uses actual process forking and signal handling
- **Comprehensive Logging**: Detailed logs for process state transitions
- **Performance Metrics**: Tracks wait time, turnaround time, and response time
- **GUI Visualization**: Graphical interface for algorithm selection and execution

### Data Structures

- **Priority Queue**: For HPF and SRTN algorithms
- **Circular Queue**: For Round Robin algorithm
- **Process Data Structure**: Contains process ID, arrival time, running time, priority, and state

## Phase 2: Memory Management + CPU Scheduling

### Memory Management Features

1. **Buddy System Implementation**
   - Binary tree-based memory allocation
   - Efficient memory splitting and merging
   - Configurable threshold for minimum block size

2. **Memory Allocation Strategy**
   - Best-fit allocation algorithm
   - Automatic memory deallocation on process termination
   - Memory compaction through node merging

3. **Enhanced Process Management**
   - Memory requirements for each process
   - Waiting queue for processes that cannot be allocated memory
   - Automatic reallocation when memory becomes available

### Memory Management Components

- **TreeNode Structure**: Represents memory blocks in the buddy system
- **Memory Allocation**: `allocateMemory()` function with best-fit strategy
- **Memory Deallocation**: `deallocateMemory()` with automatic merging
- **Memory Logging**: Detailed logs of memory operations

## Building and Running

### Prerequisites

- GCC compiler
- GTK development libraries (for GUI)
- Cairo graphics library
- Linux/Unix environment (for process management)

### Building Phase 1

```bash
cd ph1
make build
```

### Building Phase 2

```bash
cd ph2
make build
```

### Running the Scheduler

#### Phase 1 Usage

```bash
# Run with HPF algorithm
./process_generator.out 1

# Run with SRTN algorithm  
./process_generator.out 2

# Run with Round Robin algorithm (quantum = 2)
./process_generator.out 3 2
```

#### Phase 2 Usage

```bash
# Same as Phase 1, but with memory management
./process_generator.out 1    # HPF with memory management
./process_generator.out 2    # SRTN with memory management
./process_generator.out 3 2  # RR with memory management (quantum = 2)
```

### GUI Interface

Both phases include a graphical user interface for easy interaction:

```bash
# Compile and run GUI
gcc GUI.c -o GUI.out `pkg-config --cflags --libs gtk+-3.0`
./GUI.out
```

## Output Files

### Phase 1 Outputs
- `scheduler.log`: Detailed process execution log
- `scheduler.pref`: Process preference and scheduling information
- `output_image.png`: Visual representation of scheduling results

### Phase 2 Outputs
- `scheduler.log`: Detailed process execution log
- `scheduler.pref`: Process preference and scheduling information
- `memory.log`: Memory allocation and deallocation log
- `output_image.png`: Visual representation of scheduling and memory results

## Algorithm Details

### HPF (Highest Priority First)
- **Type**: Non-preemptive
- **Queue**: Priority Queue
- **Selection**: Process with highest priority (lowest number)
- **Use Case**: Real-time systems, priority-based applications

### SRTN (Shortest Remaining Time Next)
- **Type**: Preemptive
- **Queue**: Priority Queue (sorted by remaining time)
- **Selection**: Process with shortest remaining execution time
- **Use Case**: Batch processing, minimizing average wait time

### Round Robin
- **Type**: Preemptive
- **Queue**: Circular Queue
- **Selection**: First process in queue (FIFO with time quantum)
- **Use Case**: Time-sharing systems, fair scheduling

## Memory Management Details

### Buddy System
- **Structure**: Binary tree representation
- **Allocation**: Best-fit strategy
- **Splitting**: Automatic when block is too large
- **Merging**: Automatic when adjacent blocks are free
- **Threshold**: Minimum block size (configurable)

### Memory Operations
- **Allocation**: `allocateMemory()` - finds best-fit block and splits if necessary
- **Deallocation**: `deallocateMemory()` - frees block and merges with neighbors
- **Compaction**: Automatic merging of adjacent free blocks

## Performance Metrics

The system tracks and reports:
- **Wait Time**: Time process spends waiting in ready queue
- **Turnaround Time**: Total time from arrival to completion
- **Response Time**: Time from arrival to first CPU burst
- **Average Metrics**: Statistical analysis of performance

## Signal Handling

The implementation uses Unix signals for process management:
- `SIGCONT`: Resume process execution
- `SIGSTOP`: Suspend process execution
- `SIGUSR1`: Process termination notification
- `SIGTSTP`: Initial process suspension

## Error Handling

- Memory allocation failures
- Process creation errors
- File I/O error handling
- Signal handling for graceful termination

## Contributing

This project demonstrates fundamental operating system concepts including:
- Process scheduling algorithms
- Memory management techniques
- Inter-process communication
- Real-time process simulation
- GUI development for system tools

## License

This project is part of an academic Operating Systems course implementation.