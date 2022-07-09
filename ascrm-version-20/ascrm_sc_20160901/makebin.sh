#!/bin/bash
./clean.sh
make -f Makefile
rm -f *.o *.out *.so *.*~
