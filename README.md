# Software timer for general embedded systems

[![License](https://img.shields.io/github/license/LincolnUehara/EmbeddedSoftwareTimer)](https://github.com/LincolnUehara/EmbeddedSoftwareTimer/blob/master/LICENSE)
![Maintenance](https://img.shields.io/maintenance/no/2018)

### Purpose

This code was intended to study how to implement a software timer abstraction layer for embedded systems. Since I did not like the idea of using "tick", I have tried to use timers and interruptions to manage it.
I have tested on Tiva TM4C123 board with LEDs and it worked, and I believe that will work on other boards too.

### How to use it

The first thing needed to do is to edit `hmcu_timer.c` archive. It makes the interface between the logic layer and the hardware layer of chosen MCU. The instructions to do it is written inside the archive.

Once you've done with it, include the software timer library (`<soft_timer.h>`) in the code you will work with.

The way of usage is given bellow:
```c
soft_timer timer_1; /* Create a software timer object. */

soft_timer_init(); /* Initialize the software timer. */

soft_timer_create(&timer_1); /* Create an instance of software timer. */

/* Set the instance, its callback function, timeout in miliseconds and if 
 * it will repeat. */
soft_timer_set(&timer_1, blinky, 1000, true);

soft_timer_start(&timer_1); /* Start the software timer instance. */
```

### Author

Lincoln Uehara
