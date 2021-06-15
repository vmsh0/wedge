/* vim: set ts=4 sw=4 tw=100 et : */

/*
 * MIT License
 * 
 * Copyright (c) 2021 Riccardo Paolo Bestetti
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/uinput.h>

#define die(str) { \
    perror(str); \
    exit(EXIT_FAILURE); \
};

#if 0
    #define dbg(...) fprintf(stderr, __VA_ARGS__)
#else
    #define dbg(...)
#endif

int key_map[] = {
    0, 0, 0, 0, 0, 0, 0, 0,          /* 0-7 */
    0, 0, KEY_ENTER, 0, 0, 0, 0, 0,  /* 8-15 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 16-23 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 24-31 */
    KEY_SPACE, 0, 0, 0, 0, 0, 0, 0,  /* 32-39 */
    0, 0, 0, 0,                      /* 40-43 */
    KEY_COMMA, 0, KEY_DOT, 0,        /* 44-47 */
    KEY_0, KEY_1, KEY_2, KEY_3,      /* 48-51 */
    KEY_4, KEY_5, KEY_6, KEY_7,      /* 52-55 */
    KEY_8, KEY_9, 0, 0, 0, 0, 0, 0,  /* 56-63 */
    0, KEY_A, KEY_B, KEY_C,          /* 64-67 */
    KEY_D, KEY_E, KEY_F, KEY_G,      /* 68-71 */
    KEY_H, KEY_I, KEY_J, KEY_K,      /* 72-75 */
    KEY_L, KEY_M, KEY_N, KEY_O,      /* 76-79 */
    KEY_P, KEY_Q, KEY_R, KEY_S,      /* 80-83 */
    KEY_T, KEY_U, KEY_V, KEY_W,      /* 84-87 */
    KEY_X, KEY_Y, KEY_Z, 0,          /* 88-91 */
    0, 0, 0, 0,                      /* 92-95 */
    0, KEY_A, KEY_B, KEY_C,          /* 96-99 */
    KEY_D, KEY_E, KEY_F, KEY_G,      /* 100-103 */
    KEY_H, KEY_I, KEY_J, KEY_K,      /* 104-107 */
    KEY_L, KEY_M, KEY_N, KEY_O,      /* 108-111 */
    KEY_P, KEY_Q, KEY_R, KEY_S,      /* 112-115 */
    KEY_T, KEY_U, KEY_V, KEY_W,      /* 116-119 */
    KEY_X, KEY_Y, KEY_Z, 0,          /* 120-123 */
    0, 0, 0, 0                       /* 124-127 */
};

int mod_map[] = {
    0, 0, 0, 0, 0, 0, 0, 0,          /* 0-7 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 8-15 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 16-23 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 24-31 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 32-39 */
    0, 0, 0, 0,                      /* 40-43 */
    0, 0, 0, 0,                      /* 44-47 */
    0, 0, 0, 0,                      /* 48-51 */
    0, 0, 0, 0,                      /* 52-55 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 56-63 */
    0, 1, 1, 1,                      /* 64-67 */
    1, 1, 1, 1,                      /* 68-71 */
    1, 1, 1, 1,                      /* 72-75 */
    1, 1, 1, 1,                      /* 76-79 */
    1, 1, 1, 1,                      /* 80-83 */
    1, 1, 1, 1,                      /* 84-87 */
    1, 1, 1, 0,                      /* 88-91 */
    0, 0, 0, 0,                      /* 92-95 */
    0, 0, 0, 0,                      /* 96-99 */
    0, 0, 0, 0,                      /* 100-103 */
    0, 0, 0, 0,                      /* 104-107 */
    0, 0, 0, 0,                      /* 108-111 */
    0, 0, 0, 0,                      /* 112-115 */
    0, 0, 0, 0,                      /* 116-119 */
    0, 0, 0, 0,                      /* 120-123 */
    0, 0, 0, 0,                      /* 124-127 */
    KEY_LEFTSHIFT                    /* the mod key */
};


int setup_uinput()
{
    struct uinput_setup usetup;
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    if (fd < 0)
        die("error: open");

    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0) 
        die("error: ioctl1");

    for (size_t i = 0; i < sizeof(key_map) / sizeof (key_map[0]); i++)
        if (ioctl(fd, UI_SET_KEYBIT, key_map[i]) < 0)
            die("error: ioctl2a");

    if (ioctl(fd, UI_SET_KEYBIT, mod_map[128]) < 0)
        die("error: ioctl2b");

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_VIRTUAL;
    usetup.id.vendor = 0x5697;
    usetup.id.product = 0x0001;
    strcpy(usetup.name, "bestowedge");

    if (ioctl(fd, UI_DEV_SETUP, &usetup) < 0)
        die("error: ioctl3");

    if (ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl4");

    return fd;
}

void emit_key(int fd, int code, int val)
{
   struct input_event ie;

   ie.type = EV_KEY;
   ie.code = code;
   ie.value = val;
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

void emit_syn(int fd)
{
   struct input_event ie;

   ie.type = EV_SYN;
   ie.code = SYN_REPORT;
   ie.value = 0;
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

void emit_key_event(int fd, int code, int press)
{
    dbg("ev code %d (p? %d)", code, press);
    emit_key(fd, code, press);
    emit_syn(fd);
}

int main()
{
    int fd = setup_uinput();

    int ch;
    while ((ch = getchar()) != EOF)
    {
        dbg("proc '%c' (%d)\n", ch, ch);

        if (mod_map[ch])
            emit_key_event(fd, mod_map[128], 1);

        emit_key_event(fd, key_map[ch], 1);
        emit_key_event(fd, key_map[ch], 0);

        if (mod_map[ch])
            emit_key_event(fd, mod_map[128], 0);
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}

