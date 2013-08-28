CC=clang
GCC_OPT = -O2 -Wall -Wextra
all:  sweeny_dc sweeny_uf sweeny_sbfs sweeny_ibfs
BP=./bin/
OP=./obj/
SP=./src/

sweeny_dc: objs_dc  $(SP)/sweeny_dc.c 
	$(CC)   $(GCC_OPT) $(SP)sweeny_dc.c \
	-DLEVEL_HEURISTIC $(OP)fileio_hdf5.o $(OP)dyncon.o $(OP)eulertour.o\
       	$(OP)splay.o $(OP)dllist.o  -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sweeny_dc

sweeny_uf: objs_uf  $(SP)/sweeny_uf.c 
	$(CC)   $(GCC_OPT) $(SP)sweeny_uf.c \
       	$(OP)queue.o $(OP)fileio_hdf5.o  $(OP)uf.o -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sweeny_uf

sweeny_sbfs: objs_bfs  $(SP)/sweeny_bfs.c
	$(CC)   $(GCC_OPT) -DSEQUENTIAL $(SP)sweeny_bfs.c \
       	$(OP)queue_2.o $(OP)fileio_hdf5.o  -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sweeny_sbfs

sweeny_ibfs: objs_bfs  $(SP)/sweeny_bfs.c 
	$(CC)   $(GCC_OPT) $(SP)sweeny_bfs.c \
       	$(OP)queue_2.o $(OP)fileio_hdf5.o  -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sweeny_ibfs

objs_dc: fileio_hdf5.o dllist.o dyncon.o splay.o eulertour.o

objs_uf: fileio_hdf5.o queue.o uf.o

objs_bfs: fileio_hdf5.o queue.o queue_2.o

queue.o: $(SP)queue.c $(SP)queue.h
	gcc $(GCC_OPT) -c $(SP)queue.c -o $(OP)queue.o

queue_2.o: $(SP)queue_2.c $(SP)queue_2.h
	gcc $(GCC_OPT) -c $(SP)queue_2.c -o $(OP)queue_2.o

uf.o: $(SP)uf.c $(SP)uf.h
	gcc $(GCC_OPT) -c $(SP)uf.c -o $(OP)uf.o

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
