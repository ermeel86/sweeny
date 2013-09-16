#include <linux/types.h>
#include <linux/types.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <math.h>

#include "sweeny_dc.h"
#include "timeseries.h"
#include "extract_args.h"


#define BUF_LEN 512
char fn[BUF_LEN];
char verbose = 0;

#define TS_FILE_D  "../time_series/dc/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"
char impl_title[] = "Dynamic Connectivity implementation";
__u32 DX;
__u32 seed;
__u32 cutoff;
__u32 steps;
double beta;
double coupling;
double q;
double rnd_num;


__u32 *num_bonds, *num_cluster, *size_giant;
__u64 *sec_cs_moment,*four_cs_moment;

int main(int argc, char **argv) {

    
    if(!extractArgs(argc, argv,impl_title))
      return EXIT_FAILURE;
    snprintf(fn,BUF_LEN,TS_FILE_D,DX,q,beta,coupling,seed);
    if(!init_observables())
        return EXIT_FAILURE;
    if(!init_sweeny_dc(q,DX,beta,coupling,cutoff,steps,seed,num_bonds,num_cluster,size_giant,
                sec_cs_moment,four_cs_moment))
        return EXIT_FAILURE;
    if(!simulate_sweeny_dc())
        return EXIT_FAILURE;
    destroy_sweeny_dc();
    save_timeseries();
    destroy_observables();
    return EXIT_SUCCESS;
 
}



