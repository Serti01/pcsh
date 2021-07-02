#!/usr/bin/sh

mkdir bin

# compile to objects
gcc -c -fPIC main/pcsh.c            -o bin/pcsh.o
gcc -c -fPIC lib/pcsh.c             -o bin/lib.o

cd bin

# compile library
gcc -shared lib.o -o libpcsh.so

# link
gcc  pcsh.o -L./ -lpcsh -o pcsh

cd ..

LD_LIBRARY_PATH=~/Documents/dev/pcsh/bin/ ./bin/pcsh