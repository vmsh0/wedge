# wedge

`wedge` is a simple Linux program that works as a softwedge, i.e. a program which translates a
stream of characters (e.g. from a serial device) to a sequence of keyboard events.

The source code is small and simple. I specifically avoided reinventing the wheel. The program
doesn't know about serial devices - instead, it relies on external tools for tty configuration, and
on I/O redirection for input. It uses the `uinput` interface to feed the events to the kernel.

## Features

- consumes data from stdin
- translates it to Linux input subsystem events
- submits those events through the uinput interface

Substantially, the program translates its `stdin` to Linux keyboard events.

## Example usage (serial)

Two support files are provided:
- `99-wedge.rules`: udev rules file which symlinks a serial device to `/dev/tty_wedge` and adds a
  requirement to the wedge.service systemd unit
- `wedge.service`: systemd unit which configures `/dev/tty_wedge` using `stty` and starts the wedge
  program

Adapt `99-wedge.rules` to your application and copy it to /etc/udev/rules.d. You should change the
file to match your serial device. In the provided example file, I specified the `ID_VENDOR_ID` and
`ID_MODEL_ID` properties. You can query a connected device for all its properties with `udevadm info
/dev/tty{device}`.

After copying the file, you should run `udevadm control --reload-rules && udevadm trigger` to pick
up the new rule. If the device is connected to your computer, you should see a new symlink called
`/dev/tty_wedge`.

Adapt `wedge.service` to your application and copy it to /etc/systemd/system. You should mostly have
to change the baud rate passed to the `stty` program.

After copying the file, you should run `systemctl daemon-reload`. Disconnecting and reconnecting the
device, or running `udevadm trigger`, should start the `wedge` program.

Finally, you should add `uinput` to your /etc/modules (and either reboot or manually load the module
with `modprobe uinput`).

## Translation tables

The program uses two tables to translate from ASCII characters to keyboard events. Those tables have
each 128 entries, corresponding to the 128 standard ASCII characters.
- `key_map`: translates between the nth character of the ASCII standard and a Linux keyboard event
- `key_mod`: represents whether the nth character should be sent as an event with the modifier key
- `key_mod[128]`: extra element representing the Linux keyboard event for the modifier key

The modifier key mechanism is intended for sending capital letters.

The program supports a subset of the ASCII character set. In particular, it handles all alphanumeric
characters, plus SPACE, DOT, COMMA, LINE FEED, and CARRIAGE RETURN. It doesn't support anything else
out of the box as most characters other than the ones mentioned depend on the keyboard layout
configured in your terminal/desktop environment.

There are probably a few more ASCII characters that don't change between keyboard layouts (e.g.
TAB). If these are needed for your application, please update the table and submit a pull request.

It might be a good idea to make keyboard and mod maps loadable at runtime.

## Compiling

The `Makefile` is intended for static compilation, as this is a small utility intended to be
deployed on a multitude of different systems without a compiler. It uses the `musl-gcc` wrapper to
link against MUSL. It enables aggressive optimization, and binary stripping.

On some distributions, you may need to install a copy on the kernel headers specifically sanified
for MUSL. (E.g. on Arch Linux you can use the `kernel-headers-musl` package.)

Should you want to use more standard tooling to compile the program, you can edit the `Makefile` to
use `gcc` as your C compiler, and everything should work just fine.

## Contributing

Feel free to send anything you may see fit as a pull request. If you want to make sure it's going to
be accepted, open an issue first. Please follow the existing code style: format everything as you
see in `wedge.c`, and keep the code stupid-simple.

Things that need to be worked on:

- Support loading keyboard layouts (maybe we can parse xkb files? or kbd files might be easier but
  less accurate)
- Command line parameter to open files other than stdin

## License

This project is released under the MIT license. You can find a copy in the `LICENSE` file.

