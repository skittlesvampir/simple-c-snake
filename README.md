# Bens awesome plain c snake implementation

This is a little snake game that I've written using only standard C libraries and escape sequences. So this should work on any operating system where a C compiler and make are installed. The whole source code is less than 300 lines.

The `enableRawMode` and `disableRawMode` functions from `pasted-code.h` are from [snaptoken](https://viewsourcecode.org/snaptoken/kilo/). Check it out, it's really helpful if you want to know how to process key presses in C. You might also want to have a look [this GitHub Gist about escape sequences](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797).

## How to use it

You can move the snake using either the arrow keys, WASD or the vim keys.
