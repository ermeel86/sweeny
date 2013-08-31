CC=clang
GCC_OPT = -O2 -Wall -Wextra
all:  sweeny_dc sweeny_uf sweeny_sbfs sweeny_ibfs python_interface
BP=./bin/
OP=./obj/
SP=./src/
PI=./python_interface/
python_interface: sweeny_mc.so


py_objs_dc: py_sweeny_dc.o py_dllist.o py_dyncon.o py_splay.o py_eulertour.o

py_objs_uf: py_sweeny_uf.o py_queue.o py_uf.o

py_objs_bfs: py_sweeny_bfs.o py_queue_2.o

# Python related
py_sweeny_dc.o: $(PI)sweeny_dc.h $(PI)sweeny_dc.c
	$(CC) $(GCC_OPT) -fPIC -c $(PI)sweeny_dc.c -o $(PI)py_sweeny_dc.o

py_sweeny_uf.o: $(PI)sweeny_uf.h $(PI)sweeny_uf.c
	$(CC) $(GCC_OPT) -fPIC -c  $(PI)sweeny_uf.c -o $(PI)py_sweeny_uf.o

py_sweeny_bfs.o: $(PI)sweeny_bfs.h $(PI)sweeny_bfs.c
	$(CC) $(GCC_OPT) -fPIC -c $(PI)sweeny_bfs.c -o $(PI)py_sweeny_bfs.o

py_queue.o: $(SP)queue.c $(SP)queue.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)queue.c -o $(PI)py_queue.o

py_queue_2.o: $(SP)queue_2.c $(SP)queue_2.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)queue_2.c -o $(PI)py_queue_2.o

py_uf.o: $(SP)uf.c $(SP)uf.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)uf.c -o $(PI)py_uf.o

py_dyncon.o: $(SP)dyncon.h $(SP)dyncon.c $(SP)mytypes.h
	$(CC)   $(GCC_OPT) -fPIC -DLEVEL_HEURISTIC -c $(SP)dyncon.c\
	   -o $(PI)py_dyncon.o 

py_splay.o: $(SP)splay.c $(SP)splay.h $(SP)mytypes.h
	$(CC)   $(GCC_OPT) -fPIC -c $(SP)splay.c -o $(PI)py_splay.o

py_eulertour.o: $(SP)eulertour.c $(SP)eulertour.h $(SP)mytypes.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)eulertour.c -o $(PI)py_eulertour.o

py_dllist.o: $(SP)dllist.c $(SP)dllist.h $(SP)mytypes.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)dllist.c -o $(PI)py_dllist.o

py_sweeny_mc.o: $(PI)sweeny_mc.c $(PI)sweeny_mc.h
	$(CC) $(GCC_OPT) -fPIC -c $(PI)sweeny_mc.c -o $(PI)py_sweeny_mc.o

sweeny_mc.so: py_sweeny_mc.o py_objs_dc py_objs_uf py_objs_bfs
	$(CC) $(GCC_OPT) -shared -o $(PI)sweeny_mc.so $(PI)py_sweeny_mc.o $(PI)py_sweeny_dc.o \
	   $(PI)py_dyncon.o $(PI)py_eulertour.o $(PI)py_splay.o $(PI)py_dllist.o \
	   $(PI)py_sweeny_uf.o $(PI)py_uf.o $(PI)py_queue.o $(PI)py_sweeny_bfs.o \
	   $(PI)py_queue_2.o -lgsl -lgslcblas -lm

# C/command-line version related
objs_dc: fileio_hdf5.o dllist.o dyncon.o splay.o eulertour.o

objs_uf: fileio_hdf5.o queue.o uf.o

objs_bfs: fileio_hdf5.o queue_2.o

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


queue.o: $(SP)queue.c $(SP)queue.h
	$(CC) $(GCC_OPT) -c $(SP)queue.c -o $(OP)queue.o

queue_2.o: $(SP)queue_2.c $(SP)queue_2.h
	$(CC) $(GCC_OPT) -c $(SP)queue_2.c -o $(OP)queue_2.o

uf.o: $(SP)uf.c $(SP)uf.h
	$(CC) $(GCC_OPT) -c $(SP)uf.c -o $(OP)uf.o

dyncon.o: $(SP)dyncon.h $(SP)dyncon.c $(SP)mytypes.h
	$(CC)   $(GCC_OPT) -DLEVEL_HEURISTIC -c $(SP)dyncon.c\
	   -o $(OP)dyncon.o 

splay.o: $(SP)splay.c $(SP)splay.h $(SP)mytypes.h
	$(CC)   $(GCC_OPT) -c $(SP)splay.c -o $(OP)splay.o

eulertour.o: $(SP)eulertour.c $(SP)eulertour.h $(SP)mytypes.h
	$(CC) $(GCC_OPT) -c $(SP)eulertour.c -o $(OP)eulertour.o

dllist.o: $(SP)dllist.c $(SP)dllist.h $(SP)mytypes.h
	$(CC) $(GCC_OPT) -c $(SP)dllist.c -o $(OP)dllist.o

fileio_hdf5.o:  $(SP)fileio_hdf5.h $(SP)fileio_hdf5.c
	$(CC) $(GCC_OPT) -c $(SP)fileio_hdf5.c -o $(OP)fileio_hdf5.o

clean:
#	rm $(OP)*.o
	rm $(PI)*.o
	rm $(PI)*.so
