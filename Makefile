ARDUINO_DIR = /usr/share/arduino
ARDUINO_PORT = /dev/ttyACM1
ARDUINO_LIBS = USB_Host_Shield Servo 

USER_LIB_PATH = /home/pi/sketchbook/libraries
# BOARD_TAG = uno
 BOARD_TAG = mega-2560
# BOARD_TAG = nano328
BOARD_SUB = atmega328	
# BOARD_TAG = leonardo

include /usr/share/arduino/Arduino.mk
