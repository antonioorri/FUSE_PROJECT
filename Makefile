montaje= punto_montaje
fichero= es.txt

fuse_flags= -D_FILE_OFFSET_BITS=64 -lfuse -pthread

basicFUSE : basicFUSE.o basicFUSE_lib.o
	gcc -g -o $@  $^ ${fuse_flags}
	mkdir -p $(montaje)
	
basicFUSE.o : basicFUSE2.c basicFUSE_lib.h
	gcc -g -c -o $@  $< ${fuse_flags}

basicFUSE_lib.o : basicFUSE_lib.c basicFUSE_lib.h
	gcc -g -c -o $@  $<


mount: basicFUSE
	./basicFUSE  $(montaje)
debug: basicFUSE
	./basicFUSE -d $(fichero) $(montaje)

umount:
	fusermount -u $(montaje)
