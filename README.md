# ThermaSense

This is a temperature and humidity sensor system consisting of two microcontrollers (Arduino and Raspberry Pi) that communicate via UART and display data on a simple GUI. 
The simple interaction diagram below summarizes the behaviour of the system. 

![Alt text](https://github.com/muradammar/ThermaSense/blob/main/images/Screenshot%202025-07-12%20191009.png)

## FreeRTOS

The main purpose of this project was to learn the fundamentals of FreeRTOS, a low level operating systems for real-time systems.
The arduino was programmed using FreeRTOS. The function of reading from the sensor and sending the data to the Pi were created as 
"tasks". FreeRTOS can give the illusion of concurrent execution of functions even with a single core CPU by switching quickly between tasks.
Furthemore, to safely communicate between tasks, a "queue" was used. This API ensures a task never writes to an overloaded data buffer or a task
never reads from an empty buffer. 

I also learnt that there are many nuances to RTOS. Some parts of tasks cannot be interrupted so you must carefully block interrupts
during critical sections whilst not undermining the function of the kernel. I also learnt that shared resources between tasks bring about
undefined behaviour. I'd like to learn "mutexes" and "semaphores" to safely share these resources

## File Structure




