LIBRARIES := SPI EEPROM Time RTClib Wire MemoryFree RF24
BOARD := nano328
TARGET := glamis
SERIALDEV := /dev/ttyUSB0
LOG_FILE := log.log
include ./arduino.mk
