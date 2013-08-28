#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include "uf.h"
#include "queue.h"
#include <math.h>
#include <gsl/gsl_rng.h> //Verwendung von Mersenne-Twister Pseudozufallszahlengenerator
#include "fileio_hdf5.h"
__s8 dN;

#define MIN(a,b) a <= b ? a: b
#define MAX(a,b) a >= b? a: b

#define SEED_D 123456
#define DX_D 128
#define BETA_D 0.44
#define COUPLING_D 2.
#define Q_D 2.
#define CUTOFF_D 1000
#define STEPS_D  65536
#define TS_FILE_D "../time_series/uf/simulation_l%u_q%.4f_b%.4f_c%.4f_s%u.hdf5"
#define BUF_LEN 512
char fn[BUF_LEN];

char verbose=0;
double rcWeights[4]; // array with precalculated mc weights
double p_min_del,p_max_del, p_min_ins,p_max_ins;
struct queue collect1,collect2;
struct queue_node *todo_pool, *collect_pool;
__u32 offset_1=1,offset_2=2;
__u64 *sec_cs_moment ;
__u32 *size_giant;
__u32 *num_bonds;
__u32 *num_cluster;
//natural units! k_b=1
__u32 DX;
__u32 N;
__u32 seed=123456;
double q=2; 
double coupling;
double beta;
double v;
double K;
__u32 steps;
__s8 *bonds;
//short int *ltc;
gsl_rng * r; 
__u32 edge[2];
__s32 adj1[4];
__s32 adj2[4];
__s32 *uf1;
__u32 cs1,cs2;
__u32 activeEdges=0;
__u32 cutoff;
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
  rcWeights[0] = MIN(pow(v,-1),1); //db==-1,dN==0
  rcWeights[1] = MIN(pow(v,-1)*q,1); //db==-1,dN=1
  rcWeights[2] = MIN(v,1); //db==1,dN==0
  rcWeights[3] = MIN(v*pow(q,-1),1); //db==1,dN==-1
  p_min_del = MIN(rcWeights[0],rcWeights[1]); 
  p_max_del = MAX(rcWeights[0],rcWeights[1]);
  p_min_ins = MIN(rcWeights[2],rcWeights[3]);
  p_max_ins = MAX(rcWeights[2],rcWeights[3]);      
  if(verbose) {
	  printf("p_min_del = %f\n",p_min_del);
	  printf("p_max_del = %f\n",p_max_del);
	  printf("p_min_ins = %f\n",p_min_ins);
	  printf("p_max_ins = %f\n",p_max_ins);


  }
  initPRNG();
  __u32 i;
  uf1 = initUF(N);
  todo_pool = malloc(N*sizeof(struct queue_node));
  if(!todo_pool) {
      exit(1);
  }
  collect_pool = malloc(N*sizeof(struct queue_node));
  if(!collect_pool) {
      exit(1);
  }
  for(i=0;i<N;++i) {

      collect_pool[i].next = todo_pool[i].next = NULL;
      collect_pool[i].data = todo_pool[i].data= i;
      collect_pool[i].visited = todo_pool[i].visited = 0;
    }

  sec_cs_moment = malloc(sizeof(*sec_cs_moment)*steps);
  if(!sec_cs_moment) exit(1);
  size_giant = malloc(sizeof(__u32)*steps);
  if(!size_giant) {
    exit(1);
    free(sec_cs_moment);
  }
  num_bonds = malloc(sizeof(__u32)*steps);
  if(!num_bonds) {
    exit(1);
    free(sec_cs_moment);
    free(size_giant);
  }
  if(!num_bonds) {
    free(size_giant);
    free(sec_cs_moment);
    free(num_bonds);
    exit(1);
  }
  bonds = malloc(sizeof(*bonds)*2*N);
  if(bonds == NULL) {
    fprintf(stderr,"Error in init(): Could not allocate memory!\n");
    destroyUF(uf1);
    stopPRNG();
    free(sec_cs_moment);
    free(size_giant);
    free(num_bonds);
    exit(1);
  }
  for(i=0;i<2*N;i++) bonds[i] = -1;
  num_cluster = malloc(sizeof(__u32)*steps);
  if(!num_cluster) {
    fprintf(stderr,"Error in init(): Could not allocate memory!\n");
    destroyUF(uf1);
    free(bonds);
    stopPRNG();
    free(size_giant);
    free(sec_cs_moment);
    free(num_bonds);
    exit(1);  
  }
}
/******************************************************************************
 *****************************************************************************/
void destroy(void)
{
  destroyUF(uf1);
  free(bonds);
  stopPRNG();
  free(num_bonds);
  free(size_giant);
  free(sec_cs_moment);
  free(num_cluster);
  free(collect_pool);
  free(todo_pool);
  //free(visited);
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
void Adjacent2(__u32 idx, __u8 a) {
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
__u8 breadthFirstSearch(__u32 start, __u32 goal,__u8 accept_split)
{

    static struct queue todo1,todo2;
  __u32 i=0,activeP1=0,activeP2=0;
  init_queue(&todo1,todo_pool);
  init_queue(&todo2,todo_pool);
  init_queue(&collect1,collect_pool);
  init_queue(&collect2,collect_pool);
  cs1=0;
  cs2=0;
  enqueue(&todo1,start); // Put starting point onto queue 1
  enqueue(&collect1,start);
  cs1++; // Increase cluster size of bfs 1
  collect_pool[start].visited = offset_1;
  enqueue(&todo2,goal); // Put goal point onto queue 2
  enqueue(&collect2,goal);
  collect_pool[goal].visited = offset_2;
  cs2++; // increase cluster size of bfs 2
    while(!queue_empty_p(&todo1) && !queue_empty_p(&todo2)) {
        dequeue(&todo2,&activeP2); // get next vertex of bfs 2
        Adjacent2(activeP2,2);	 // get all adjacent vertices of current vertex
        for(i=0;i<4;i++) {
            if(adj2[i] != -1) { // if accessable (edge exists)
                if(collect_pool[adj2[i]].visited == offset_2) continue; // already visited
                if((__u32)adj2[i] == start || collect_pool[adj2[i]].visited == offset_1) { // reconnected
                    while(!queue_empty_p(&todo1)) dequeue(&todo1,&activeP1); // empty queue 1
                    while(!queue_empty_p(&todo2)) dequeue(&todo2,&activeP2); // empty queue 2
                    while(!queue_empty_p(&collect1)) dequeue(&collect1,&activeP1);
                    while(!queue_empty_p(&collect2)) dequeue(&collect2,&activeP2);
                    offset_1+=2;
                    offset_2+=2;
                    return 1; // 1 indicates success
                }
                enqueue(&todo2,adj2[i]);
                enqueue(&collect2,adj2[i]);
                collect_pool[adj2[i]].visited = offset_2; // mark as visited
                cs2++; // increase cluster size
            }
        }
        dequeue(&todo1,&activeP1);
        Adjacent2(activeP1,1);
        for(i=0;i<4;i++) {
            if(adj1[i] != -1) {
                if(collect_pool[adj1[i]].visited == offset_1) continue;
                if((__u32)adj1[i] == goal || collect_pool[adj1[i]].visited == offset_2) {
                    while(!queue_empty_p(&todo1)) dequeue(&todo1,&activeP1);
                    while(!queue_empty_p(&todo2)) dequeue(&todo2,&activeP2);
                    while(!queue_empty_p(&collect1)) dequeue(&collect1,&activeP1);
                    while(!queue_empty_p(&collect2)) dequeue(&collect2,&activeP2);
                    offset_1+=2;
                    offset_2+=2;
                    return 1;
                }
                enqueue(&todo1,adj1[i]);
                enqueue(&collect1,adj1[i]);
                collect_pool[adj1[i]].visited = offset_1;
                cs1++;
            }
        }
    }
    /* Only if a deletion of a cutting edge has to be accepted, 
        * completely traverse the unfinished cluster to 
        * be able to update the UF datastructure afterwards based on
        * the collect1 and collect2 queues.
    */
    if(accept_split) { 
        while(!queue_empty_p(&todo1)) {
            dequeue(&todo1,&activeP1);
            Adjacent2(activeP1,1);
            for(i=0;i<4;i++) {
                if(adj1[i] != -1) {
                    if(collect_pool[adj1[i]].visited == offset_1) continue;
                    enqueue(&todo1,adj1[i]);
                    enqueue(&collect1,adj1[i]);
                    collect_pool[adj1[i]].visited = offset_1;
                    cs1++;
                }
            }

        } 
        while(!queue_empty_p(&todo2)) {
            dequeue(&todo2,&activeP2);
            Adjacent2(activeP2,2);
            for(i=0;i<4;i++) {
                if(adj2[i] != -1) {
                    if(collect_pool[adj2[i]].visited == offset_2) continue;
                    enqueue(&todo2,adj2[i]);
                    enqueue(&collect2,adj2[i]);
                    collect_pool[adj2[i]].visited = offset_2;
                    cs2++;
                }
            }
        }
    }
    else {
        while(!queue_empty_p(&todo1)) dequeue(&todo1,&activeP1);
        while(!queue_empty_p(&todo2)) dequeue(&todo2,&activeP2);
        while(!queue_empty_p(&collect1)) dequeue(&collect1,&activeP1);
        while(!queue_empty_p(&collect2)) dequeue(&collect2,&activeP2);
    }
    offset_1+=2;
    offset_2+=2;

    return 0;
}
/******************************************************************************
 *****************************************************************************/
__u8 removeBond(__u32 a, __u32 b,__u8 accept_split,__u8 accept_non_split) {
    __u32 vertex;
    if(breadthFirstSearch(a,b,accept_split)) //replacement edge found
        return accept_non_split;
    else {  // no replacement edge available and hence cluster will be splitted if move accepted
        if(accept_split) {  // accept cluster splitting case? if yes rebuild uf
            while(!queue_empty_p(&collect1)) {
                dequeue(&collect1,&vertex);
                uf1[vertex] = a;
            }
            uf1[a] = -1*cs1;
            while(!queue_empty_p(&collect2)) {
                dequeue(&collect2,&vertex);
                uf1[vertex] = b;
            }
            uf1[b] = -1*cs2;
            return 1;
        } // not accepted
        else {
            return 0;
        }
    }
  
}	
/******************************************************************************
 *****************************************************************************/
static inline __u8 acceptChange(__s8 d_numCl, __s8 d_numAB, double randV)
{
  return d_numAB == -1 ? (randV < rcWeights[d_numCl]) : (randV < rcWeights[2 - d_numCl]);
 /* static double dw=0;
  dw = pow(q,d_numCl);
  dw *= pow(v,d_numAB); 
  return (dw>=1 || randV < dw); */
}
/******************************************************************************
 *****************************************************************************/
static inline void   mcStep(void)
{

  static __u32 bidx;
  static double rnd_num;
  bidx = gsl_rng_uniform_int(r,2*N);
  rnd_num  = gsl_rng_uniform(r);
  Adjacent(bidx);
  if(bonds[bidx] == 1) { //edge is active hence delete it
    if(rnd_num < p_min_del) {
   	  bonds[bidx] = -1;
      if(removeBond(edge[0],edge[1],1,1))
        activeEdges--;
      else {
          fprintf(stderr,"ERROR!\n");
          exit(-1);
      }
    }
    else {
	if(rnd_num < p_max_del) {
	  bonds[bidx] = -1;
	  if(removeBond(edge[0],edge[1], acceptChange(1,-1,rnd_num),acceptChange(0,-1,rnd_num)))
          activeEdges--;
      else
          bonds[bidx] = 1;
    }

    }

  }
  else {	
    if(rnd_num < p_min_ins) {
        bonds[bidx]  = 1;
        activeEdges++;
        unite(edge[0],edge[1],uf1);
      }
    else {
    if(rnd_num < p_max_ins) {
      dN = connected(edge[0],edge[1],uf1) ? 0: -1;
      if(acceptChange(dN,1,rnd_num)) {
        bonds[bidx]=1;
        activeEdges++;
        if(dN == -1)
            unite(edge[0],edge[1],uf1);
      }
    }

  }
  }
}
/******************************************************************************
 *****************************************************************************/
void saveObs(__u32 cnt)
{
  __u32 i=0,tn=0,nclust=0;
  size_giant[cnt] = 0;
  sec_cs_moment[cnt] = 0;
  for(i=0;i<N && tn <= N;i++) {
    if(uf1[i] < 0) {
      tn -= uf1[i];
      nclust+=1;
      sec_cs_moment[cnt]+= (__u64)pow(uf1[i],2);
      if((__u32)(-uf1[i]) > size_giant[cnt]) size_giant[cnt] = (__u32)(-uf1[i]);
    }
  }
  num_bonds[cnt] = activeEdges;
  num_cluster[cnt] = nclust;
}
/******************************************************************************
 *****************************************************************************/
void generateTimeSeries(void)
{ 
  __u32 i=0,j=0;
  for(i=0;i<cutoff;i++){
      for(j=0;j<2*N;j++)mcStep();
      // reset visited array
      // otherwise it will overflow at some point
      // could also be done less frequent
      for(j=0;j<N;j++)collect_pool[j].visited=0;
  }
  for(i=0;i<steps;i++) {
    for(j=0;j<2*N;j++)
      mcStep();
      saveObs(i);
      for(j=0;j<N;j++)collect_pool[j].visited=0;
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
      printf("*Union-Find/BFS implementation of Sweeny's algorithm*\n");
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
