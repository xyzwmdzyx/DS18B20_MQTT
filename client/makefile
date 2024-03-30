CFLAGS += -I ./include -I ../common/include
LDFLAGS = -Llib -lclimodule -lsqlite3

PREFIX ?= ./bin
LIB = ./lib
DATA = ./data
LOG = ./log

PROGRAM_NAME = client
LIB_NAME = climodule

EXCLUDE = ./src/client.c
SRC = $(filter-out $(EXCLUDE), $(wildcard ./src/*.c)) $(wildcard ../common/src/*.c)

all: shared_lib clean
	@gcc ${CFLAGS} ./src/client.c -o ${PROGRAM_NAME} ${LDFLAGS}

shared_lib:
	@gcc ${CFLAGS} -fPIC -shared -o lib${LIB_NAME}.so $(SRC)
	@mkdir -p ${LIB}
	@mv lib${LIB_NAME}.so ${LIB}
	
install:
	@mkdir -p ${LOG}
	@mkdir -p ${DATA}
	@mkdir -p ${PREFIX}
	@mv ${PROGRAM_NAME} ${PREFIX}

clean:	
	@rm -rf ./src/*.o
	
distclean:
	@rm -rf ${DATA} ${LOG}
	
uninstall:
	@rm -rf ${PREFIX} ${LIB} ${DATA} ${LOG} 
