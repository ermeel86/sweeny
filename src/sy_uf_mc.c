#include <linux/types.h>
#include "sweeny_uf.h"
#include "extract_args.h"
#include "timeseries.h"
#include <stdio.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h> //Verwendung von Mersenne-Twister Pseudozufallszahlengenerator

#define TS_FILE_D "../time_series/uf/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"
#define BUF_LEN 512
char fn[BUF_LEN];
char impl_title[] = "Union-Find implementation";

char verbose=0;
__u64 *sec_cs_moment ;
__u32 *size_giant;
__u32 *num_bonds;
__u32 *num_cluster;
__u32 DX;
__u32 N;
__u32 seed=123456;
double q=2; 
double coupling;
double beta;
double v;
double K;
__u32 steps;
__u32 cutoff;
int main(int argc, char *argv[])
{

    if(!extractArgs(argc, argv,impl_title))
      return EXIT_FAILURE;
    snprintf(fn,BUF_LEN,TS_FILE_D,DX,q,beta,coupling,seed);
    if(!init_observables())
        return EXIT_FAILURE;
    if(!init_sweeny_uf( q,DX,beta,coupling,cutoff,steps,seed,num_bonds,num_cluster,size_giant,sec_cs_moment))
        return EXIT_FAILURE;
    if(!simulate_sweeny_uf())
        return EXIT_FAILURE;
    destroy_sweeny_uf();
    save_timeseries();
    destroy_observables();
    return EXIT_SUCCESS;
}
/******************************************************************************
 *****************************************************************************/
