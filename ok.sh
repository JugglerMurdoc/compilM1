#!/bin/bash

clear && make clean && make && ./matC -f test.c && spim -file matC.s
