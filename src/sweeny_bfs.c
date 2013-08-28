#include <stdio.h>
#include "queue_2.h"
#include <ctype.h>
#include <strings.h>
#include <unistd.h>
#include "fileio_hdf5.h"
#include <math.h>
#include <gsl/gsl_rng.h> 

// Define Macro SEQUENTIAL to use sequential BFS; Default is interleaved BFS
#ifdef SEQUENTIAL
#define TS_FILE_D "../time_series/sbfs/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"
#define INFO_STRING "*Sequential BFS implementation of Sweeny's algorithm*\n"
#else
#define TS_FILE_D "../time_series/ibfs/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"
#define INFO_STRING "*Interleaved BFS implementation of Sweeny's algorithm*\n"
#endif
#define BUF_LEN 512
char fn[BUF_LEN];

#define MIN(a,b) a<=b ? a: b
#define MAX(a,b) a>=b? a:b
#define SEED_D 123456
#define DX_D 128
#define BETA_D 0.44
#define COUPLING_D 2.
#define Q_D 2.
#define CUTOFF_D 1000
#define STEPS_D  65536

char verbose=0;
double rcWeights[4]; // array with precalculated mc weights
double p_min_del,p_max_del, p_min_ins,p_max_ins;
__s8 dN,dB;
char equilibration=1;
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
__u32 edge[2];
__s32 adj1[4];
__s32 adj2[4];
#ifndef SEQUENTIAL
__u32 offset_1=1;
__u32 offset_2 = 2;
__u32 cs2;
#else
__u32 offset_1 = 0;
#endif /*SEQUENTIAL*/
char *filename;
struct queue_node *todo_pool;
__u32 *visited;
__u32 cs1;
__u32 activeEdges=0;
__u32 cutoff;
__u32 *num_bonds , *num_cluster, *size_giant;
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
void initPRNG(void)
{
r = gsl_rng_alloc (gsl_rng_mt19937);
gsl_rng_set(r,seed);
}
/******************************************************************************
 *****************************************************************************/
void stopPRNG(void)
{
gsl_rng_free (r);
}
/******************************************************************************
 *****************************************************************************/
void init(void)
{
    K=coupling*beta;
    v = exp(K) - 1.0;
    N = DX*DX;
    // Deletion accept. ratios
    rcWeights[0] = pow(v,-1); //db==-1,dN==0
    rcWeights[1] = rcWeights[0]*q; //db==-1,dN=1
    // Insertion accept. ratios
    rcWeights[2] = v; //db==1,dN==0
    rcWeights[3] = v*pow(q,-1); //db==1,dN==-1
  
    p_min_del = MIN(rcWeights[0],rcWeights[1]); 
    p_max_del = MAX(rcWeights[0],rcWeights[1]);
    p_min_ins = MIN(rcWeights[2],rcWeights[3]);
    p_max_ins = MAX(rcWeights[2],rcWeights[3]);      


    initPRNG();
    __u32 i;
    bonds = malloc(sizeof(*bonds)*2*N);
    if(bonds == NULL) {
        fprintf(stderr,"Error in init(): Could not allocate memory!\n");
        stopPRNG();
        exit(1);
    }
    for(i=0;i<2*N;i++) bonds[i] = -1;
    visited = calloc(N,sizeof(*visited));
    if(visited == NULL) {
        fprintf(stderr,"Error in init(): Could not allocate memory!\n");
        free(bonds);
        stopPRNG();
        exit(1);
    }
    todo_pool = malloc(N*sizeof(struct queue_node));
    if(!todo_pool) {
        free(visited);
        free(bonds);
        stopPRNG();
        exit(1);
    }
    if(!init_observables())
        exit(1);
    

}
/******************************************************************************
 *****************************************************************************/
void destroy(void)
{
    free(todo_pool);
    free(bonds);
    stopPRNG();
    free(visited);
}
/******************************************************************************
 *****************************************************************************/
static inline __u32 ltcXnext(__u32 idx)
{
    if((idx+1) % DX)
        return idx+1;
    else
        return idx +1- DX ;
}
/******************************************************************************
 *****************************************************************************/
static inline __u32 ltcXprev(__u32 idx)
{
    if((idx%DX))
        return idx-1;
    else
        return idx + DX  -1;
}
/******************************************************************************
 *****************************************************************************/
 static inline __u32 ltcYnext(__u32 idx)
{

    if(idx <= N -1 && idx >= N - DX)
        return idx +DX- N ;
    else
        return idx+DX;
}
/******************************************************************************
 *****************************************************************************/
static inline __u32 ltcYprev(__u32 idx)
{
    if(idx <= DX - 1) 
        return idx + N - DX;
    else
        return idx - DX;

}
/******************************************************************************
 *****************************************************************************/
void Adjacent(__u32 bidx)
{

  if(bidx%2) { 
    bidx = (bidx -1)/2;
    edge[0] = bidx;
    edge[1] = ltcXnext(bidx);
  }
  else {
    bidx = bidx/2;
    edge[0] = bidx;
    edge[1] = ltcYnext(bidx);
  }


}
/******************************************************************************
 *****************************************************************************/
void neighbours(__u32 idx,__u8 a) {
  if(a == 1)
  {
    if(bonds[2*idx] == 1)  adj1[0] = ltcYnext(idx);
    else  adj1[0] = -1;
    if(bonds[(2*idx)+1] == 1)  adj1[1] = ltcXnext(idx);
    else adj1[1] = -1;
    if(bonds[2*ltcYprev(idx)] == 1)	adj1[2] = ltcYprev(idx);
    else adj1[2] = -1;
    if(bonds[(2*ltcXprev(idx))+1] ==1)adj1[3] = ltcXprev(idx);
    else adj1[3] = -1;
  }
  else {
    if(bonds[2*idx] == 1)adj2[0] = ltcYnext(idx);
    else adj2[0] = -1;
    if(bonds[(2*idx)+1] == 1)adj2[1] = ltcXnext(idx);
    else adj2[1] = -1;
    if(bonds[2*ltcYprev(idx)] == 1)	adj2[2] = ltcYprev(idx);
    else adj2[2] = -1;
    if(bonds[(2*ltcXprev(idx))+1] == 1)adj2[3] = ltcXprev(idx);
    else adj2[3] = -1;
  }

}
/******************************************************************************
 *****************************************************************************/
__u8 breadthFirstSearch_s(__u32 start, __u32 goal)
{
  static struct queue todo1;
  __u32 i=0,activeP1=0;
  init_queue(&todo1); //todo_pool);
  cs1=0;
  enqueue(&todo1,start); 
  cs1++; // Increase cluster size of bfs 1
  visited[start] = offset_1; // mark starting point as visited
    while(!queue_empty_p(&todo1)) { 
      dequeue(&todo1,&activeP1);
      neighbours(activeP1,1);
      for(i=0;i<4;i++) {
        if(adj1[i] != -1) {
            if(visited[adj1[i]] == offset_1) continue;
            if((__u32)adj1[i] == goal) {
                    while(!queue_empty_p(&todo1)){ dequeue(&todo1,&activeP1);}
                    return 1;
                }
            enqueue(&todo1,adj1[i]);
            visited[adj1[i]] = offset_1;
            cs1++;
        }
      }
  }
  return 0;
}
#ifndef SEQUENTIAL
/******************************************************************************
 *****************************************************************************/
__u8 breadthFirstSearch(__u32 start, __u32 goal)
{
  static struct queue todo1,todo2;
  __u32 i=0,activeP1=0,activeP2=0;
  init_queue(&todo1);
  init_queue(&todo2);
  cs1=0;
  cs2=0;
  enqueue(&todo1,start); // Put starting point onto queue 1
  cs1++; // Increase cluster size of bfs 1
  visited[start] = offset_1; // mark starting point as visited
  enqueue(&todo2,goal); // Put goal point onto queue 2
  visited[goal] = offset_2; // mark goal point as visited
  cs2++; // increase cluster size of bfs 2
    while(!queue_empty_p(&todo1) && !queue_empty_p(&todo2)) { 
      dequeue(&todo2,&activeP2); // get next vertex of bfs 2
      neighbours(activeP2,2);	 // get all adjacent vertices of current vertex
      for(i=0;i<4;i++) {

        if(adj2[i] != -1) { // if accessable (edge exists)
	  if(visited[adj2[i]] == offset_2) continue; // already visited
	  if((__u32)adj2[i] == start || visited[adj2[i]] == offset_1) { // reconnected

            while(!queue_empty_p(&todo1)){ dequeue(&todo1,&activeP1);}
            while(!queue_empty_p(&todo2)){ dequeue(&todo2,&activeP2);}
	    return 1; // 1 indicates success
	  }
	  enqueue(&todo2,adj2[i]);
	  visited[adj2[i]] = offset_2; // mark as visited
	  cs2++; // increase cluster size
	  }
      }
      dequeue(&todo1,&activeP1);
      neighbours(activeP1,1);
      for(i=0;i<4;i++) {
	if(adj1[i] != -1) {
	  if(visited[adj1[i]] == offset_1) continue;
	  if((__u32)adj1[i] == goal || visited[adj1[i]] == offset_2) {
            while(!queue_empty_p(&todo1)){ dequeue(&todo1,&activeP1);}
            while(!queue_empty_p(&todo2)){ dequeue(&todo2,&activeP2);}
			return 1;
	  }
	  enqueue(&todo1,adj1[i]);
	  visited[adj1[i]] = offset_1;
	  cs1++;
	}
      }
  }
  while(!queue_empty_p(&todo1)){dequeue(&todo1,&activeP1);}
  while(!queue_empty_p(&todo2)){dequeue(&todo2,&activeP2);}
  return 0; 
}
/******************************************************************************
 *****************************************************************************/
static inline __u8 connected(__u32 start, __u32 goal) {
    offset_1+=2;
    offset_2+=2;
    return breadthFirstSearch(start,goal);
}
#else
/******************************************************************************
 *****************************************************************************/
__u8 static inline connected(__u32 start, __u32 goal) {
    ++offset_1;
    return breadthFirstSearch_s(start,goal);
}
#endif
/******************************************************************************
 *****************************************************************************/
__u8 acceptChange(__s8 d_numCl, __s8 d_numAB, double randV)
{
  static double dw=0;
  dw = pow(q,d_numCl);
  dw *= pow(v,d_numAB); 
  return (dw>=1 || randV < dw);
}
/******************************************************************************
 *****************************************************************************/
static inline void   sweep(void)
{
    static __u32 bidx;
    static double rnd_num;
    __u32 i=0;
    for(;i<2*N ;++i) {
        bidx = gsl_rng_uniform_int(r,2*N);
        rnd_num  = gsl_rng_uniform(r);
        Adjacent(bidx);
        if(bonds[bidx] == 1) { //edge is active hence delete it
            dB = -1;
            if(rnd_num < p_min_del) { // accept bond deletion
                bonds[bidx]*=-1;
                activeEdges--;
            }
            else {
                if(rnd_num < p_max_del) {
                    bonds[bidx]*=-1;
                    dN = (connected(edge[0],edge[1])? 0 : 1);
                    if(rnd_num >= rcWeights[dB == -1? dN : 2-dN]) bonds[bidx]*=-1; // undo bond deletion
                    else activeEdges--; // accept bond deletion
                }

            }

        }
        else {	
            // Insert cedge. If adjacent vertices are already connected, the new 
            // edge will be a non-tree edge, i.e. dN = 0
            dB = 1;
            if(rnd_num < p_min_ins) { // accept bond insertion
                bonds[bidx] *=-1;
                activeEdges++;
            }
            else {
                if(rnd_num < p_max_ins) {
                    dN = ( connected(edge[0],edge[1]) ?0 : -1);
                    if(rnd_num < rcWeights[dB == -1? dN : 2-dN]) {
                        bonds[bidx]*=-1;
                        activeEdges++;
                    }
                }

            }
        }
    }
}
/******************************************************************************
 *****************************************************************************/
void extract_observables(void) {
    static __u32 i=0,j=0;
    static __u32 clust_cnt=0;
    static __u64 sum=0;
    static __u32 maxc=0,chksum=0;
    num_bonds[i] = activeEdges;
    offset_1=0;
    for(j=0;j<N;++j) visited[j] = offset_1;
    offset_1=1;
    for(j=0;j<N&&chksum<=N;++j) {
        if(!visited[j]){
            breadthFirstSearch_s(j,N);
            clust_cnt++;
            if(cs1 > maxc) maxc = cs1;
            sum+=cs1*cs1;
            chksum+=cs1;
        }

    }
    num_cluster[i] = clust_cnt;
    size_giant[i] = maxc;
    sec_cs_moment[i] = sum;
    maxc=clust_cnt=sum=chksum=0;
    offset_1=3;
    ++i;
}
/******************************************************************************
 *****************************************************************************/
void generateTimeSeries(void)
{ 
  __u32 i;
  for(i=0;i<cutoff;++i)sweep();
  equilibration=0;
  if(verbose)
      printf("Equilibration done!\n");
  for(i=0;i<steps;++i) { 
      sweep();
      extract_observables();
  }
      
}
/******************************************************************************
 *****************************************************************************/
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
      printf(INFO_STRING);
      printf("SEED = %u\n",seed);
      printf("DX = %u\n",DX);
      printf("Q = %f\n",q);
      printf("beta = %f\n",beta);
      printf("steps = %u\n",steps);
      printf("coupling = %f\n",coupling);
      printf("cutoff = %u\n",cutoff);
      printf("time-series file: %s\n",fn);
    }
    return 1;
}
/******************************************************************************
 *****************************************************************************/
int main(int argc, char *argv[])
{

    if(!extractArgs(argc, argv))
      return EXIT_SUCCESS;
    init();
    generateTimeSeries();
    save_timeseries();
    destroy();
    exit(0);
}
/******************************************************************************
 *****************************************************************************/
