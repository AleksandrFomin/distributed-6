#!/bin/bash

make

if [ $? -eq 0 ]
then

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/fomin/Documents/itmo/master/parallel/pa6/";

LD_PRELOAD=/home/fomin/Documents/itmo/master/parallel/pa6/libruntime.so ./a.out --mutexl -p 3 10 20 30
fi
