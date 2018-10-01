# Software timer for general embedded systems

Software timer abstraction for general embedded systems.

### How to use it

First thing you need to do is to edit `hmcu_timer.c` archive. It makes the interface between the software timer logic and the hardware layer of chosen MCU. All the instruction to do it is written inside the archive.

Once you've done it, include the software timer library as `#include <soft_timer.h>` or `#include "soft_timer.h"` in the place you will work with.

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

### ToDo list

- [ ] Do deep copy instead of shallow copy of soft_timer_t;
- [ ] Update _st_QUEUE_parserAndSet;
- [ ] Make it 'repeatable';
- [ ] Rename the functions for better understanding.
- [ ] Review the writting/code standards;
- [x] Make a simple tutorial on README;
- [ ] Do a wiki explaining the functioning of this code.

### License

This project is licensed under the MIT License.

### Author

Lincoln Uehara