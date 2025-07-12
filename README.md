# ThermaSense

This is a temperature and humidity sensor system consisting of two microcontrollers (Arduino and Raspberry Pi) that communicate via UART and display data on a simple GUI. The arduino is programmed using RTOS to manage sensor reads and UART communication. The Pi is programmed using a simply python script and the built in Serial and Tkinter libs. 
The simple interaction diagram below summarizes the behaviour of the system. 

![Alt text](https://github.com/muradammar/ThermaSense/blob/main/images/Screenshot%202025-07-12%20191009.png)

## FreeRTOS

The main purpose of this project was to learn the fundamentals of FreeRTOS, a low level operating systems for real-time systems.
The arduino was programmed using FreeRTOS. The function of reading from the sensor and sending the data to the Pi were created as 
"tasks". FreeRTOS can give the illusion of concurrent execution of functions even with a single core CPU by switching quickly between tasks.
Furthemore, to safely communicate between tasks, a "queue" was used. This API ensures a task never writes to an overloaded data buffer or a task
never reads from an empty buffer. Together, these APIs allowed me to separate the tasks of reading from the sensor and writing to the Pi and allowed them to communicate and exist simultaneously.  

I learnt that there are many nuances to RTOS. Some parts of tasks cannot be interrupted so you must carefully block interrupts
during critical sections whilst not undermining the function of the kernel. I also learnt that shared resources between tasks bring about
undefined behaviour. I'd like to learn "mutexes" and "semaphores" to safely share these resources

## File Structure

## Lessons Learnt

This project involved a few deadends and improvisations. Originally, I was going to create a nicer GUI using PyQT6. However, installing that library broke my SD card so I opted for Tkinter instead. Furthermore, the first version of the arduino code uses no sensor library. I read from the sensor using bare metal programming and bit-banging (see temp_sensor_fail.ino). It was immediately clear that my timing was not as precise as it should be and I was reading garbage values from the sensor. The task was also failing to read from the sensor more than half the time:

![Alt text] (https://github.com/muradammar/ThermaSense/blob/main/images/Image%20(6).jpg)

After this, I decided to use an existing library. I started with the Adafruit library which turns out not to work with FreeRTOS. Then I found the "SimpleDHT" library which performed well. 




