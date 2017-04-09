SRC = ../app/src/
INC = ../app/inc/
TARGET_INC = -I/usr/local/mipgs/include/json
HOST_INC = -I/usr/local/include/json 
INCS = -I../app/inc ${HOST_INC}
OBJS = 	MiddleServerMain.o \
		UartApi.o \
		Common.o \
		Socket.o \
		RemoteDataServer.o \
		AsyncCmd.o \
		UartDataServer.o \
		RGPProtocol.o \
		DataQueue.o

TARGET_CC = mipsel-openwrt-linux-gcc
HOST_CC = gcc
CC = ${HOST_CC}

APP_NAME = MiddleServerApp

${APP_NAME}:${OBJS}
	${CC} -o ../out/${APP_NAME} $^ -lpthread -ldl -ljson 

%.o:${SRC}%.c
	${CC} -c ${INCS} $< -o $@ 

.PHONY : clean
clean :
	-rm -f ${OBJS} ../out/${APP_NAME}
