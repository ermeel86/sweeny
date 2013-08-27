CC=clang
GCC_OPT = -O2 -Wall -Wextra
all:  sweeny_dc
BP=./bin/
OP=./obj/
SP=./src/

sweeny_dc: objs  $(SP)/sweeny.c 
	$(CC)   $(GCC_OPT) $(SP)sweeny.c \
	-DLEVEL_HEURISTIC $(OP)fileio_hdf5.o $(OP)dyncon.o $(OP)eulertour.o\
       	$(OP)splay.o $(OP)dllist.o  -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sweeny_dc

objs: fileio_hdf5.o dllist.o dyncon.o splay.o eulertour.o

queue.o: $(SP)queue.c $(SP)queue.h
	gcc $(GCC_OPT) -c $(SP)queue.c -o $(OP)queue.o

dyncon.o: $(SP)dyncon.h $(SP)dyncon.c $(SP)mytypes.h
	gcc   $(GCC_OPT) -DLEVEL_HEURISTIC -c $(SP)dyncon.c\
	   -o $(OP)dyncon.o 

splay.o: $(SP)splay.c $(SP)splay.h $(SP)mytypes.h
	gcc   $(GCC_OPT) -c $(SP)splay.c -o $(OP)splay.o

eulertour.o: $(SP)eulertour.c $(SP)eulertour.h $(SP)mytypes.h
	gcc $(GCC_OPT) -c $(SP)eulertour.c -o $(OP)eulertour.o

dllist.o: $(SP)dllist.c $(SP)dllist.h $(SP)mytypes.h
	gcc $(GCC_OPT) -c $(SP)dllist.c -o $(OP)dllist.o

fileio_hdf5.o:  $(SP)fileio_hdf5.h $(SP)fileio_hdf5.c
	gcc $(GCC_OPT) -c $(SP)fileio_hdf5.c -o $(OP)fileio_hdf5.o

clean:
	rm $(OP)*.o
