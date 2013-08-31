#include "timeseries.h"
#include "fileio_hdf5.h"
#include <linux/types.h>
#include <stdlib.h>
#include <stdio.h>
extern __u32 *num_bonds,*num_cluster, *size_giant;
extern __u64 *sec_cs_moment;
extern char fn[];

extern __u32 DX;
extern __u32 seed;
extern __u32 cutoff;
extern __u32 steps;
extern double beta;
extern double coupling;
extern double q;
extern double rnd_num;
extern char verbose;
/******************************************************************************
 *****************************************************************************/
char init_observables(void) {
    num_bonds = malloc(sizeof(__u32)*steps);
    if(!num_bonds) return 0;
    num_cluster = malloc(sizeof(__u32)*steps);
    if(!num_cluster) return 0;
    size_giant = malloc(sizeof(__u32)*steps);
    if(!size_giant) return 0;
    sec_cs_moment = malloc(sizeof(__u64)*steps);
    if(!sec_cs_moment) return 0;
    
    return 1;
}
/******************************************************************************
 *****************************************************************************/
void destroy_observables(void) {
    free(num_bonds);
    free(num_cluster);
    free(size_giant);
    free(sec_cs_moment);
}
/******************************************************************************
 *****************************************************************************/
char save_timeseries(void) {
    if(verbose)
        printf("Time-series file %s\n",fn);
    hid_t f_id = openFile_w(fn);
    write_u32_1d(f_id,"num_bonds",num_bonds,steps);
    write_u32_1d(f_id,"num_cluster",num_cluster,steps);
    write_u32_1d(f_id,"size_giant",size_giant,steps);
    write_u64_1d(f_id,"sec_cs_moment",sec_cs_moment,steps);

    writeAttribute_u32(f_id,"num_bonds", "length_1d",&DX);
    writeAttribute_u32(f_id,"num_bonds", "seed",&seed);
    writeAttribute_u32(f_id,"num_bonds", "steps",&steps);
    writeAttribute_u32(f_id,"num_bonds", "cutoff",&cutoff);
    writeAttribute_double(f_id,"num_bonds", "beta",&beta);
    writeAttribute_double(f_id,"num_bonds", "coupling",&coupling);
    writeAttribute_double(f_id,"num_bonds", "q",&q);

    closeFile(f_id);
    return 1;

} 

/*********************************************************************
**********************************************************************/

