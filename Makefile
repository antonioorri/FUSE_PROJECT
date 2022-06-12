#IMGFS_CFLAGS=-DLOG

imgfs : anal.o log.o
	gcc -g -o imgfs anal.o log.o `pkg-config fuse --libs`

anal.o : anal.c log.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c anal.c ${IMGFS_CFLAGS}

log.o : log.c log.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c log.c ${IMGFS_CFLAGS}

clean:
	rm -f imgfs *.o

dist:
	rm -rf imgfs/
	mkdir imgfs/
	cp ../*.html imgfs/
	mkdir imgfs/example/
	mkdir imgfs/example/mountdir/
	mkdir imgfs/example/rootdir/
	echo "a bogus file" > imgfs/example/rootdir/bogus.txt
	mkdir imgfs/src
	cp Makefile COPYING COPYING.LIB *.c *.h imgfs/src/
	tar cvzf ../../imgfs.tgz imgfs/
	rm -rf imgfs/
