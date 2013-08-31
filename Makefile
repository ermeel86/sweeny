CC=clang
GCC_OPT = -O2 -Wall -Wextra
all: python_interface bins
BP=bin/
OP=obj/
SP=src/
python_interface: lib_sweeny_mc.so
bins: sy_dc_mc.bin sy_uf_mc.bin sy_ibfs_mc.bin sy_sbfs_mc.bin
utils: fileio_hdf5.o extract_args.o timeseries.o 
objs_dc: sweeny_dc.o dllist.o dyncon.o splay.o eulertour.o

objs_uf:  sweeny_uf.o queue.o uf.o

objs_bfs: sweeny_bfs.o queue_2.o

sweeny_dc.o: $(SP)sweeny_dc.h $(SP)sweeny_dc.c
	$(CC) $(GCC_OPT) -fPIC -c $(SP)sweeny_dc.c -o $(OP)sweeny_dc.o

sweeny_uf.o: $(SP)sweeny_uf.h $(SP)sweeny_uf.c
	$(CC) $(GCC_OPT) -fPIC -c  $(SP)sweeny_uf.c -o $(OP)sweeny_uf.o

sweeny_bfs.o: $(SP)sweeny_bfs.h $(SP)sweeny_bfs.c
	$(CC) $(GCC_OPT) -fPIC -c $(SP)sweeny_bfs.c -o $(OP)sweeny_bfs.o

queue.o: $(SP)queue.c $(SP)queue.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)queue.c -o $(OP)queue.o

queue_2.o: $(SP)queue_2.c $(SP)queue_2.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)queue_2.c -o $(OP)queue_2.o

uf.o: $(SP)uf.c $(SP)uf.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)uf.c -o $(OP)uf.o

dyncon.o: $(SP)dyncon.h $(SP)dyncon.c $(SP)mytypes.h
	$(CC)   $(GCC_OPT) -fPIC -DLEVEL_HEURISTIC -c $(SP)dyncon.c\
	   -o $(OP)dyncon.o 

splay.o: $(SP)splay.c $(SP)splay.h $(SP)mytypes.h
	$(CC)   $(GCC_OPT) -fPIC -c $(SP)splay.c -o $(OP)splay.o

eulertour.o: $(SP)eulertour.c $(SP)eulertour.h $(SP)mytypes.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)eulertour.c -o $(OP)eulertour.o

dllist.o: $(SP)dllist.c $(SP)dllist.h $(SP)mytypes.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)dllist.c -o $(OP)dllist.o

sweeny_mc.o: $(SP)sweeny_mc.c $(SP)sweeny_mc.h
	$(CC) $(GCC_OPT) -fPIC -c $(SP)sweeny_mc.c -o $(OP)sweeny_mc.o

lib_sweeny_mc.so: sweeny_mc.o objs_dc objs_uf objs_bfs
	$(CC) $(GCC_OPT) -shared -o $(OP)lib_sweeny_mc.so $(OP)sweeny_mc.o $(OP)sweeny_dc.o \
	   $(OP)dyncon.o $(OP)eulertour.o $(OP)splay.o $(OP)dllist.o \
	   $(OP)sweeny_uf.o $(OP)uf.o $(OP)queue.o $(OP)sweeny_bfs.o \
	   $(OP)queue_2.o -lgsl -lgslcblas -lm


sy_dc_mc.bin: objs_dc sweeny_dc.o $(SP)/sweeny_dc.c utils 
	$(CC)   $(GCC_OPT) $(SP)sy_dc_mc.c \
	-DLEVEL_HEURISTIC $(OP)sweeny_dc.o  $(OP)dyncon.o $(OP)eulertour.o\
       	$(OP)splay.o $(OP)dllist.o $(OP)timeseries.o $(OP)fileio_hdf5.o\
	   	$(OP)extract_args.o -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sy_dc_mc.bin

sy_uf_mc.bin: sweeny_uf.o objs_uf  $(SP)/sy_uf_mc.c utils
	$(CC)   $(GCC_OPT) $(SP)sy_uf_mc.c \
       	$(OP)sweeny_uf.o $(OP)queue.o $(OP)timeseries.o $(OP)fileio_hdf5.o \
	   	$(OP)uf.o $(OP)extract_args.o -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sy_uf_mc.bin

sy_sbfs_mc.bin: sweeny_bfs.o objs_bfs  $(SP)/sy_bfs_mc.c utils
	$(CC)   $(GCC_OPT) -DSEQUENTIAL $(SP)sy_bfs_mc.c \
       	$(OP)sweeny_bfs.o $(OP)queue_2.o $(OP)timeseries.o $(OP)fileio_hdf5.o\
	   	$(OP)extract_args.o -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sy_sbfs_mc.bin

sy_ibfs_mc.bin: sweeny_bfs.o objs_bfs  $(SP)/sy_bfs_mc.c utils
	$(CC)   $(GCC_OPT) $(SP)sy_bfs_mc.c \
       	$(OP)sweeny_bfs.o $(OP)queue_2.o $(OP)timeseries.o $(OP)fileio_hdf5.o\
	   	$(OP)extract_args.o -lgsl -lgslcblas\
      	-lhdf5 -lm -o $(BP)sy_ibfs_mc.bin

fileio_hdf5.o:  $(SP)fileio_hdf5.h $(SP)fileio_hdf5.c
	$(CC) $(GCC_OPT) -c $(SP)fileio_hdf5.c -o $(OP)fileio_hdf5.o

extract_args.o: $(SP)extract_args.h $(SP)extract_args.c
	$(CC) $(GCC_OPT) -c $(SP)extract_args.c -o $(OP)extract_args.o

timeseries.o: $(SP)timeseries.h $(SP)timeseries.c
	$(CC) $(GCC_OPT) -c $(SP)timeseries.c -o $(OP)timeseries.o
clean:
	rm $(OP)*.o
	rm $(OP)*.so
