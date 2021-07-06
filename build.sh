#!/usr/bin/sh

if ! test -e bin;then 
    mkdir bin
fi

if [ "$1" == "debug" ] || [ "$1" == "debug-run" ]; then
    gcc -g -fsanitize=address lib/pcsh.c main/pcsh.c -o bin/pcsh_debug

    if [ "$1" == "debug-run" ]; then
        ./bin/pcsh_debug
    fi
    
    exit
fi


# compile to objects
gcc -c -fPIC main/pcsh.c            -o bin/pcsh.o
gcc -c -fPIC lib/pcsh.c             -o bin/lib.o

cd bin

# compile library
gcc -shared lib.o -o libpcsh.so

# link
gcc  pcsh.o -L./ -lpcsh -o pcsh

rm *.o

cd ..

if [ "$1" == "run" ]; then
    LD_LIBRARY_PATH=~/Documents/dev/pcsh/bin/ ./bin/pcsh
fi