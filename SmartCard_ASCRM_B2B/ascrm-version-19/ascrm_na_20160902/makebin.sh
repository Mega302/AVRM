#!/bin/bash
rm -f *.bin *.o *.out *.so *.*~ *~
make -f Makefile
rm -f  *.o *.out *.so *.*~ *~
