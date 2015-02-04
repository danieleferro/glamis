INC := ../../inc

#  .bin for gitignore
TARGET  := skeleton.bin
CC      := g++
# SOURCES := $(shell ls src/*.cpp)
SOURCES := main.cpp
OBJECTS := $(basename $(notdir $(SOURCES)))
OBJECTS := $(patsubst %, .obj/%.o, $(OBJECTS))


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.obj/%.o: src/%.cpp
	mkdir -p .obj
	mkdir -p .dep/$(dir $<)
	$(CC) -I$(INC) -o $@ -c $<

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET).elf $(TARGET).hex $(ARDUINOLIB) *~
	rm -rf .lib .dep .obj
