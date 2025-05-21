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
