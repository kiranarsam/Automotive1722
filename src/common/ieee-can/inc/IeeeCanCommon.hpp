
#pragma once

/* System files */
#include <linux/can.h>

/* CAN CC/FD frame union */
/* This is needed because the data structures for CAN and CAN-FD in Linux
    are slightly different. However, in Zephyr same data structure is used for both.
*/
typedef union {
    struct can_frame cc;
    struct canfd_frame fd;
} frame_t;
