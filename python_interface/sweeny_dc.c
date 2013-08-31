#include <stdio.h>
#include "sweeny_dc.h"
#include <gsl/gsl_rng.h>
#include <math.h>
#include "../src/dyncon.h"
#include <ctype.h>


#define  MIN(a,b) a<=b ? a: b
#define MAX(a,b) a>=b? a:b
static char verbose = 0;
static char equilibration=1;
static char setup=0;

static __u32 DX;
static __u32 seed;
static __u32 cutoff;
static __u32 steps;
static double beta;
static double coupling;
static double q;
static double rnd_num;

static double v;
static __u32 j=0;
static __s8 dB = 0,dN=0;
static gsl_rng *r;
static Edge *cEdge;
static double rcWeights[4]; // array with precalculated mc weights
static double p_min_del,p_max_del, p_min_ins,p_max_ins;

static __u32 *num_bonds, *num_cluster, *size_giant;
static __u64 *sec_cs_moment;

/******************************************************************************
 *****************************************************************************/
static void extract_observables(__u32 i) {
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
/******************************************************************************
 *****************************************************************************/
char init_sweeny_dc(double _q,unsigned int _l,double _beta,double _coupl,
        unsigned int _cutoff,unsigned _tslength,unsigned int rng_seed,
        void *ts_0,void *ts_1,void * ts_2,void *ts_3) {

    q = _q;
    DX = _l;
    beta = _beta;
    coupling = _coupl;
    cutoff = _cutoff;
    steps = _tslength;
    seed = rng_seed;
    num_bonds = (__u32 *)ts_0;
    num_cluster = (__u32 *)ts_1;
    size_giant = (__u32 *)ts_2;
    sec_cs_moment = (__u64 *)ts_3;
    
    r = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(r,seed);
    v = exp(coupling*beta) - 1.; // >0
    rcWeights[0] = pow(v,-1); //db==-1,dN==0
    rcWeights[1] = rcWeights[0]*q; //db==-1,dN=1
    rcWeights[2] = v; //db==1,dN==0
    rcWeights[3] = v*pow(q,-1); //db==1,dN==-1
    p_min_del = MIN(rcWeights[0],rcWeights[1]); 
    p_max_del = MAX(rcWeights[0],rcWeights[1]);
    p_min_ins = MIN(rcWeights[2],rcWeights[3]);
    p_max_ins = MAX(rcWeights[2],rcWeights[3]);      
    return setup=(r && init_dc(DX));

}
    
    
/******************************************************************************
 *****************************************************************************/
void destroy_sweeny_dc(void) {
    if(setup) {
        gsl_rng_free(r);
        destroy_dc();
    }
    setup=0;
}
/******************************************************************************
 *****************************************************************************/
char simulate_sweeny_dc(void) {
    if(!setup) return 0;
    __u32 i;
    for(i=0;i<cutoff;i++)sweep();
    if(verbose)
        printf("Equilibration done!\n");
    equilibration=0;
    for(i=0;i<steps;i++){
        sweep();
        extract_observables(i);
    }
    return 1;
}
/******************************************************************************
 *****************************************************************************/
