#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include "dyncon.h"
#include "fileio_hdf5.h"
#include <ctype.h>
#include <unistd.h>




#define BUF_LEN 512
#define  MIN(a,b) a<=b ? a: b
#define MAX(a,b) a>=b? a:b
char fn[BUF_LEN];
char verbose = 0;
char equilibration=1;

/*Default Simulation parameter*/
#define SEED_D 123456
#define DX_D 128
#define BETA_D 0.44
#define COUPLING_D 2.
#define Q_D 2.
#define CUTOFF_D 1000
#define STEPS_D  65536
#define TS_FILE_D "../time_series/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"

__u32 DX;
__u32 seed;
__u32 cutoff;
__u32 steps;
double beta;
double coupling;
double q;
double rnd_num;

double p;
double v;
__u32 j=0;
__s8 dB = 0,dN=0;
gsl_rng *r;
Edge *cEdge;
__u32 te=0; //Current number of tree edges
__u32 nte=0; // Current number of non tree edges
double rcWeights[4]; // array with precalculated mc weights
double p_min_del,p_max_del, p_min_ins,p_max_ins;

__u32 *num_bonds, *num_cluster, *size_giant;
__u64 *sec_cs_moment;
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
/******************************************************************************
 *****************************************************************************/
void extract_observables(void) {
    static __u32 i=0;
    static __u32 clust_cnt=0;
    static __u64 sum=0;
    static __u32 maxc=0;
    static s_tree *c;
    num_bonds[i] = nte + te;
    c = levels[0]->head;
    while(c) {
        clust_cnt++;
        sum += pow(c->root->n,2);//*c->root->n;
        if(c->root->n > maxc)
            maxc = c->root->n;
        c = c->next;
    }
    num_cluster[i] = clust_cnt;
    size_giant[i] = maxc;
    sec_cs_moment[i] = sum;
    sum=0;
    maxc = 0;
    clust_cnt=0;
    ++i;
}

/******************************************************************************
 *****************************************************************************/

static inline void mcStep(void) {
    // get a randomly selected edge 
    cEdge = &adjMatrix[gsl_rng_uniform_int(r,2*DX*DX)];

	rnd_num = gsl_rng_uniform(r);
 	if(ACTIVE_EDGE(cEdge)) { //edge is active hence delete it
	  dB = -1;
	  if(rnd_num < p_min_del) {
	    delete_dc(cEdge);
	  }
	  else {
	      if(rnd_num < p_max_del) {
		dN = delete_dc(cEdge);
		if(rnd_num >= rcWeights[dB == -1? dN : 2-dN]) 
		  insert_dc(cEdge);
	      }
	
	  }

	}
      else {	
        // Insert cedge. If adjacent vertices are already connected, the new 
        // edge will be a non-tree edge, i.e. dN = 0
        dB = 1;
        if(rnd_num < p_min_ins) {
	  insert_dc(cEdge);
	}
	else {
	  if(rnd_num < p_max_ins) {
	    dN = connected_dc_e(cEdge,1) ? 0: -1;
	    dB = 1;
	    if(rnd_num < rcWeights[dB == -1? dN : 2-dN])
		insert_dc(cEdge);
	  }

	}
      }





}
/******************************************************************************
 *****************************************************************************/
static inline void sweep() {
    for(j=0;j<2*DX*DX;j++)mcStep();
}

/*********************************************************************
**********************************************************************/

int extractArgs(int argc, char **argv) {
    seed = SEED_D;
    DX = DX_D;
    beta = BETA_D;
    steps = STEPS_D;
    q = Q_D;
    coupling = COUPLING_D;
    cutoff = CUTOFF_D;
    int c;
    opterr = 0;
    while((c = getopt(argc,argv,"s:l:q:b:m:j:c:vh")) != -1) {
      switch(c) {
	case 's':
	  seed = atoi(optarg);
	  break;
	case 'l':
	  DX = atoi(optarg);
	  break;
	case 'q':
	  q = atof(optarg);
	  break;
	case 'b':
	  beta = atof(optarg);
	  break;
	case 'm':
	  steps = atoi(optarg);
	  break;
	case 'j':
	  coupling = atof(optarg);
	  break;
	case 'c':
	  cutoff = atoi(optarg);
	  break;
	case 'v':
	  verbose = 1;
	  break;
	case 'h':
	    printf("-s\t Pseudo random number generator seed, default value: %d\n",SEED_D);
	    printf("-l\t Number of vertices per dimension, default value: %d\n",DX_D);
	    printf("-q\t Number of states (also non integer allowed), default value: %f\n",Q_D);
	    printf("-b\t Inverse temperature (In natural units), default value: %f\n",BETA_D);
	    printf("-m\t Maximal (physical) simulation time, default value: %d\n",STEPS_D);
	    printf("-j\t Coupling constant, default value %f\n",COUPLING_D);
  	    printf("-c\t Cutoff for equilibration (only in runtime mode), default value %d\n",CUTOFF_D);
	    printf("-v\t Print summary of all parameters, flag\n");
	    printf("-h\t Show this help, flag\n");
	    return 0;
        case '?':
	default:
	  fprintf(stderr,"Invalid argument(s)\n");
	  abort();
      }
    }
    snprintf(fn,BUF_LEN,TS_FILE_D,DX,q,beta,coupling,seed);
    if(verbose) {
      printf("SEED = %u\n",seed);
      printf("DX = %u\n",DX);
      printf("Q = %f\n",q);
      printf("beta = %f\n",beta);
      printf("steps = %u\n",steps);
      printf("coupling = %f\n",coupling);
      printf("cutoff = %u\n",cutoff);
      printf("time-series file: %s\n",fn);
      printf("*Info: Using level heuristic!*\n");
    }
    return 1;
}

/*********************************************************************
**********************************************************************/
int main(int argc, char **argv) {


    if(!extractArgs(argc, argv))
      return EXIT_SUCCESS;
    r = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(r,seed);
    if(!init_dc(DX)) {
        fprintf(stderr,"*Error: Could not initialize Dynamic Connectivity algorithm!*\n");
        goto freeMem;

    }
    if(!init_observables()) {
        fprintf(stderr,"*Error: Could not allocate memory for observables!*\n");
        goto freeMem;

    }

    __u32 i=0;
    v = exp(coupling*beta) - 1.; // >0
    rcWeights[0] = pow(v,-1); //db==-1,dN==0
    rcWeights[1] = rcWeights[0]*q; //db==-1,dN=1
    rcWeights[2] = v; //db==1,dN==0
    rcWeights[3] = v*pow(q,-1); //db==1,dN==-1
    p_min_del = MIN(rcWeights[0],rcWeights[1]); 
    p_max_del = MAX(rcWeights[0],rcWeights[1]);
    p_min_ins = MIN(rcWeights[2],rcWeights[3]);
    p_max_ins = MAX(rcWeights[2],rcWeights[3]);      


    for(i=0;i<cutoff;i++)sweep();
    printf("Equilibration done!\n");
    equilibration=0;
    i=0;
    for(i=0;i<steps;i++){
        sweep();
        extract_observables();
    }
    
    destroy_dc();
    save_timeseries();
    


    
       

    freeMem:
    if(r)
        gsl_rng_free(r);
    destroy_observables();
    return EXIT_SUCCESS;
 
}



