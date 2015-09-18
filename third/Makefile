PROGRAMS = glamis_server
SOURCES = ${PROGRAMS:=.cpp}

all: ${PROGRAMS}

${PROGRAMS}: ${SOURCES}
	g++ ${CCFLAGS} -Wall -I../ -lrf24-bcm $@.cpp -o $@

clean:
	rm -rf $(PROGRAMS)

permission:
# in order to grant access to /dev/mem
	sudo chown 0:0 ${PROGRAMS}
	sudo chmod u+s ${PROGRAMS}
