LIBRARIES := EEPROM Time SoftwareSerial RTClib Wire
BOARD := nano328
TARGET := glamis
SERIALDEV := /dev/ttyUSB0
LOG_FILE := log.log
include ./arduino.mk
