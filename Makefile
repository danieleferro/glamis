LIBRARIES := EEPROM Time SoftwareSerial RTClib Wire MemoryFree
BOARD := nano328
TARGET := glamis
SERIALDEV := /dev/ttyUSB1
LOG_FILE := log.log
include ./arduino.mk
