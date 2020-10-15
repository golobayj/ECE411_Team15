# ECE411 Team 15 Practicum Project

### Idea 1: 3D "Simon" Says Game

This project is a game that is similar to "Simon Says".  The game works by displaying a seqeunce of 
lights and tones that correlate to a specific button on the device.  The game player than attempts to 
input the same sequence as the device displayed.  The sequence starts at a length of 1 and increases by 1 
with each successful matched sequence input by the player.  When the player inputs an incorrect sequence 
the game ends and reports the length of the final correct sequence. This project will build on this 
concept by changing the typical plane of 4 buttons and expand it to a 3D shape such as a cube with 6 
buttons or a octahedron with 8 sides.  The device will use tactile buttons as sensors. The device will 
actuate a speaker and LEDs.  This project can be implemented using an ATMega328P microcontroller.

Sensor: One button for every side of the object

Actuator: One LED for every button and a speaker for audible tones

Controller: ATMega328P

### Idea 2: Cup With a Temperature Indicator

This project's main purpose is to display the temperature of a cup and/or the liquid its holding so
that the user can drink the liquid at its optimal/preferred temperature. The basic design of the 
project consists of a cup with a temperature sensor inside. The lower part of the cup would be where
the microcontroller is stored. When the temperature of the inside of up shifts by a significant amount,
the microcontroller would then save the temperature, and display it on a flexible LCD display on the outside
of the cup for the user to read.

Sensor: Temperature Sensors 

Controller: ATMega328P

Actuator: LCD (16x2)

### Idea 3: Digital Clock
Sensor: DS1307/1038 serial real-time clock (RTC), LM335 Precision Temperature Sensors 

Actuator: LCD (16x2)

Controller: ATMega328P

This project is aimed to design an automated system of digital clock and temperature thermometer, which 
assists people’s need on a daily basis to keep track of time. The problem with a regular digital clock 
is that we have to set the time manually every time the battery runs out or electric outages and also 
have to look elsewhere/other devices to know the exact time to set to. Therefore, this clock will come 
in advantage of helping us set the real time automatically with more accuracy. The basic concept of how
this clock works is that it takes in measurements from LM335 and time information stored in DS1307, 
processes in ATMMega32 microcontroller and then displays these values via LCD 16x2 screen. The DS1307 
is chosen because it has a built-in feature that can automatically switch to backup supply to keep 
track of time when power source outage, which assists our purpose of the project. 

Expanding on idea: 
1. Small automatic misting system (low pressures).

Sensor: Temperature Sensors 

Actuator: LCD (16x2), pump & mist line system

Controller: ATMega328P

2. Elevator control system

Sensor: Button 

Actuator: 7-seg display, motors to move cabin

Controller: ATMega328P

