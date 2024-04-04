# MagField-MT Documentation

## Overview
MagField-MT is a low-cost magnetic field mapping platform that can be easily assembled using  commercially avaliable components. The plaform is constructed using a Ender 3 (or similar) 3d printer and a Metrolab MagVector MV2 Three-Axis Hall Effect Sensor, and controlled by a computer via a LabView interface. 

## Hardware
All necessary hardware may be printed using the requisite Ender 3 3D printer. [Hardware files](Sensor%20Mounting%20Hardware) are provided in an .stl format for use with any slicing program and .sldprt format to enable modification. M3-25mm bolts and M3 nuts are required to attach the sensor to the mount. 

## Downloading the Magnetic Field Mapper LabVIEW Program

## Using Magnetic Field Mapper LabVIEW Program
To establish a connection between the Arduino, 3D printer, and the LabVIEW program, select the proper COM port for each device and input it into the COM port selection for each device. Select initialize to establish a COM port connection. Ensure the proper COM ports are selected in the event of an error and initialize again.

Custom commands can be sent to the printer, in addition to movements in the movement pane. The distance travelled per movement can be configured in the “Distance” selection and the movement speed of all movements of the printer can be configured in the “Speed” selection. The pathing file can be selected in the “Path File Location” dropdown. Ensure that the file selected is a .csv file with comma delimiters. A MATLAB program that generates pathing files is included in GitHub, but custom paths can easily be generated by the user. The mapping path can be displayed for the user in the mapping path pane.

Before running the mapping scan, ensure that a text file that is empty is selected as previous data will be overwritten. To run the mapping program, press “RUN SCAN”, at which point the program will commence. To exit the mapping program, select “EXIT SCAN”. The same button can also be selected to exit the mapping program not during a scan.

## MATLAB Path Generator
Generates a csv file with magnetic field mapper gird. Inputting gridGenerator(x,y,z,xres,yres,zres) will output a path with resolution being specified for every cardinal direction. Inputting gridGenerator(x, y, z, xres, yres, zres, xCenter, yCenter, zCenter will output a grid with with shifted coordinates around a specified center. This is useful when a scan needs to surround a specific sample. Running the function with no variables will prompt the function to ask user to input xyz scan size and resolution.

## Proper Printing MID
This program uses instrument drivers developed by Jón Schone of Proper Printing. The Marlin Instrument Drivers can be found at https://github.com/properprinting/PPC. The LabVIEW folder contains the drivers, but they can be optionally installed into the LabVIEW program files for use in other programs.

## Arduino Code
The latest version of the program uses [MV2_analog_Mean_Var.ino](Arduino%20Code/MV2%20Analog%20Mean%20Var/MV2_analog_Mean_Var). It operates by calculating the mean and variance of 30 readings after receiving an “M” command over the serial port. The number of readings can be changed in the code by changing the variable “nReads” if the user desires. Additional programs are in the GitHub that automatically send magnetic field data while on, which can be useful during debugging of the sensor. Additional documentation of the sensor is available on https://www.metrolab.com/products/magvector-mv2-magnetometer/.
