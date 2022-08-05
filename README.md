# freertos-riscvspike
Exploration of FreeRTOS on a RISC-V Architecture
- The FreeRTOS port for the SPIKE Simulator - 64-bit RISC-V core is explored.
- The paper written (ICT4SD.pdf) was published in International Conference on ICT for Sustainable Development, 2022. It consists of installation steps for SPIKE Simulator, FreeRTOS port as well as documentation for the rest of the work.
- Given below are the details of the application layer programs written:
  - UsingQueues.c: Inter-process communication using Queues.
  - UsingMutex.c: Task synchronization using mutex.
  - PerformanceAnalysis.c: Time taken for performance parameters such as boot time, task creation, task deletion, mutex locking, mutex unlocking and context switching.
