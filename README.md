# IOT irrigation system

In this project the board communicate the data created by the sensors throght the MQTT protocol to a web server where the data is stored in a postgress database and can be seen in a local web page.

## hardware
 - MSP-EXP432P4111 (main board)
 - CC3135 (Wifi Module)
 - DHT11 (humidity and temperature sensor)
 - photoresistor (light sensor)

## Mode state of the system
The system can be on three different State
  - Off or Manual

  No automation is done, you can open and close the water with the web interface or with the button on the board.
  - Scheduled
  Not implemented! The idea is to set the time and the period.
  - Automatic
  A humidity and one hysteresis value must be set and the irrigation system try to keep the humidity in that range opening and closing the water when it is necessary

## What can be seen in the web interface?
#### Values
 - State
   - Water
   - Mode
     - Mode automatic value
     - Mode automatic delta (hysteresis)
 - Sensors data
   - humidity
   - temperature
   - light
 - Calculated value
   - number of openings
   - time it was open
   - water consumption (in liter)
   - price

#### Commands
 - Water: Open/Close
 - set mode: Manual/Sheduled/Automatic
 - config time for scheduled mode
 - config humidity and delta value for the Automatic mode
 - At the bottom is possible to send raw commands

## How it works
### the communication between the board and the web server
This is done with the MQTT protocol where 2 topic are used.

 - infoTopic (board -> webserver)
   Used to communicate the state (eg: water open) and the sensor data to the webserver.
   This communication is synchronous if no command is sent to the board or there are no interaction (eg: button click) because the state and the data is sent every n seconds. But when an action listed above occurs the borad replies immediately, in this way the system is more responsive in the web interface.

 - controlTopic (webserver -> board)
   Used to send commands from the web interface to the board.

### the sensors reading
 - DHT11
 can be see in the file *workspace/project_ok/dht11.h* and *workspace/project_ok/dht11.c*, reads the temperature and humidity

 - photoresistor
 can be see in the file *workspace/project_ok/photoresistor.h* and *workspace/project_ok/photoresistor.c*, reads the light value

### physical interaction and behavior
#### Input
 - Button1
   Used to disconnect from the wifi
 - Button2
   Used to open or close the water, depends on the previus state

#### Output
 - Red Led
   On indicates the water open
