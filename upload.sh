#!/bin/bash
python.exe updateTimers.py
python.exe updateIO.py
echo "COMPILING"
arduino-cli compile -b arduino:avr:nano ~/Documents/software/ShuttleLocomotion
echo "UPLOADING"
arduino-cli upload -b arduino:avr:nano:cpu=atmega328old -p COM3 -i ~/Documents/software/ShuttleLocomotion/ShuttleLocomotion.arduino.avr.nano.hex
rm *.hex *.elf
exit