#include <linux/types.h>
#include "sweeny_bfs.h"
#include <stdio.h>
#include "timeseries.h"
#include "extract_args.h"
#include <math.h>
#include <gsl/gsl_rng.h> 


#ifdef SEQUENTIAL
char impl_title[] = "Sequential BFS implementation";
#define TS_FILE_D "../time_series/sbfs/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"
#define INFO_STRING "*Sequential BFS implementation of Sweeny's algorithm*\n"
#define INIT init_sweeny_sbfs
#define SIMULATE  simulate_sweeny_sbfs
#define DESTROY destroy_sweeny_sbfs
#else
char impl_title[] = "Interleaved BFS implementation";
#define TS_FILE_D "../time_series/ibfs/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"
#define INFO_STRING "*Interleaved BFS implementation of Sweeny's algorithm*\n"
#define INIT init_sweeny_ibfs
#define SIMULATE  simulate_sweeny_ibfs
#define DESTROY destroy_sweeny_ibfs
#endif
#define BUF_LEN 512
char fn[BUF_LEN];


char verbose=0;
__u32 DX;
__u32 N;
__u32 seed;
double q; 
double coupling;
double beta;
double v;
double K;
__u32 steps;
__s8 *bonds;
gsl_rng * r; 
char *filename;
__u32 cutoff;
__u32 *num_bonds , *num_cluster, *size_giant;
__u64 *sec_cs_moment,*four_cs_moment;
int main(int argc, char *argv[])
{

    if(!extractArgs(argc, argv,impl_title))
      return EXIT_FAILURE;
    snprintf(fn,BUF_LEN,TS_FILE_D,DX,q,beta,coupling,seed);

    if(!init_observables())
        return EXIT_FAILURE;
    if(!INIT(q,DX,beta,coupling,cutoff,steps,seed,num_bonds,num_cluster,size_giant,sec_cs_moment,four_cs_moment))
        return EXIT_FAILURE;
    if(!SIMULATE())
       return EXIT_FAILURE;
    DESTROY(); 
    save_timeseries();
    destroy_observables();
    return EXIT_SUCCESS;
}
/******************************************************************************
 *****************************************************************************/
