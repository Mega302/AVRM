#!/bin/bash
rm -f *.bin *.o *.out *.*~ *~
make -f Makebin
chmod 777 ascrmlog.bin


