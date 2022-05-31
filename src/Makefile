montaje= ../punto_montaje
fichero= $(INC_DIR)/file.img
fuse_flags= -D_FILE_OFFSET_BITS=64 -lfuse -pthread

CFLAGS = -Wall -O0 -I$(INC_DIR)

TARGET = $(BIN_DIR)/fat32
OBJS   = $(OBJ_DIR)/fat32.o
CC     = gcc

INC_DIR  = ../inc#donde se encuentra los .h
OBJ_DIR = ../obj
BIN_DIR = ../bin
SRC_DIR = .

$(TARGET) : $(OBJS) 
	mkdir -p $(BIN_DIR)
	$(CC) -g -o  $@  $^ ${fuse_flags} $(CFLAGS)
	mkdir -p $(montaje) 
	
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) -g -c -o $@  $< ${fuse_flags} $(CFLAGS)

mount: $(TARGET)
	$(TARGET) $(fichero) $(montaje)

debug: $(TARGET)
	$(TARGET) -d $(fichero) $(montaje)

umount:
	fusermount -u $(montaje)
	rm -rf $(montaje)
.PHONY : clean
clean:
	rm -f *.o fat32	
	rm -rf $(montaje) $(OBJ_DIR) $(BIN_DIR)



