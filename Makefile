LIBRARIES := EEPROM Time TimeAlarms SoftwareSerial
BOARD := nano328
TARGET := glamis
SERIALDEV := /dev/ttyUSB0
LOG_FILE := log.log
include ./arduino.mk
