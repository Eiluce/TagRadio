CC = gcc
CFLAGS = -Wall -pipe -O2 -Wall -W -D_REENTRANT -std=c99
EXEC_NAME = main
INCLUDES =
#LIBS = -lzmq
LIBS = -lm
OBJ_FILES = matrice.o main.o
INSTALL_DIR = /usr/bin

all : $(EXEC_NAME)

clean :
	rm $(EXEC_NAME) $(OBJ_FILES)

$(EXEC_NAME) : $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

#%.o: %.cpp	
#	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

#%.o: %.cc
#	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<