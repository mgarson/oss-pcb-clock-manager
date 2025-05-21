# OSS PCB Clock Manager

*C++, POSIX shared memory & signals*

---

## Overview
A command-line program that forks up to *N* child `worker` processes, enforcing a concurrency limit of *M*, and maintains a simulated system clock in shared memory — printing a Process Control Block table every 0.5 s of system time.

---

## Features

- **Shared-memory clock**  
  Uses `shmget()`/`shmat()` to store a two-word (seconds + nanoseconds), incremented by 1 µs each loop.

- **Process forking**  
  Spawns each `worker` child via `fork()` + `execvp()`, up to *N* total processes (set by `-n N`, default: 1).

- **Concurrency control**  
  Ensures no more than *M* children run simultaneously (set by `-s M`, default: 1).

- **Configurable time limits**  
  Children run for a random duration ≤ *T* seconds (set by `-t T`, default: 1), passed as an argument to `worker`.

- **Fork interval**  
  Only attempts a new fork every *I* ns (set by `-i I`, default: 0) — too-early forks print an error and retry.

- **Real-time safety**  
  Installs `SIGALRM` after 60 s to kill any remaining children and exit cleanly.

- **PCB table printing**  
  Every 0.5 s of simulated time, prints a table of occupied entries, PIDs, and start time.

---

## Build & Run

```bash
# 1. Clone
git clone git@github.com:mgarson/oss-pcb-clock-manager.git
cd oss-pcb-clock-manager

# 2. Build
make

# 3. Usage
./oss [-h] [-n N] [-s M] [-t T] [-i I]

# Options
#   -h         Show help message
#   -n N       Total child processes (default: 1)
#   -s M       Max simultaneous children (default: 1)
#   -t T       Upper bound (s) for child run time (default: 1)
#   -i I       Min interval (ns) between forks (default: 0)

# Examples
./oss -n 10 -s 3 -t 5 -i 500000000  
# Fork up to 10 children, 3 at a time, each runs ≤5 s, forks at ≥0.5 s intervals
```

---

## Technical Highlights

- **Microsecond-precision clock**  
  Uses an incremental nanosecond counter (1,000 ns per loop) with overflow handling to maintain a shared-memory clock.

- **Dynamic PCB management**  
  Allocates `processTable[N]` entries in shared memory, tracking `occupied`, `pid`, and start timestamps for each child.

- **Graceful real-time termination**  
  Installs `SIGALRM` via `alarm(60)` to kill any remaining children after 60 s and exit cleanly

- **Robust CLI parsing**  
  Leverages `getopt()` with digit-by-digit validation to catch missing or non-numeric flag arguments (`-n`, `-s`, `-t`, `-i`).
  


