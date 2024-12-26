# CPU-Scheduling-Using-Multi-Feedback-Queue

This project is a simulation of a CPU scheduling system implemented in C++ using pthreads, semaphores, and a multi-level queue structure. It demonstrates how processes are moved between different priority queues based on their burst times and scheduling rules.

---

## Algorithm Overview

The simulation involves three queues (`q1`, `q2`, and `q3`) of varying sizes and time quanta:

- **Queue 1 (`q1`)**: 
  - Size: 10
  - Quantum: 8
  - Handles newly arrived processes.

- **Queue 2 (`q2`)**: 
  - Size: 20
  - Quantum: 16
  - Processes moved from `q1` if not fully executed.

- **Queue 3 (`q3`)**:
  - Size: 30
  - No quantum, processes execute until completion.

### Scheduling Flow
1. **Long-Term Scheduler**:
   - Generates processes with random burst times.
   - Adds processes to `q1` if space is available.

2. **Short-Term Scheduler**:
   - Selects processes from `q1`, `q2`, or `q3` based on random selection probabilities.
   - Processes from `q1` and `q2` are partially executed if their burst times exceed the queue's quantum.
   - Moves partially executed processes to the next appropriate queue or terminates them if queues are full.

3. **Deadlock Handling**:
   - If a process cannot be moved due to full queues, it is terminated.

4. **Execution Time**:
   - The `TIME` variable tracks the total time of the simulation.

---

## Build and Run Instructions

### Prerequisites
- A C++ compiler that supports pthreads (e.g., `g++`).
- Make utility for building the project.

### Steps
1. Clone or download the repository containing the code and Makefile.
2. Open a terminal in the project directory.

#### Build the Project
Run the following command:
```bash
make build
