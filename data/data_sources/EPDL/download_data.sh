#!/bin/bash

for i in $(seq -f "%03g" 1 100)
do
    wget -O "$i".txt https://www-nds.iaea.org/epics/ENDF2017/EPDL.ELEMENTS/ZA"$i"000
done
