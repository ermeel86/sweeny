#include <stdio.h>
#include "sweeny_bfs.h"
#include "../src/queue_2.h"
#include <ctype.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <gsl/gsl_rng.h> 

// Define Macro SEQUENTIAL to use sequential BFS; Default is interleaved BFS

#define MIN(a,b) a<=b ? a: b
#define MAX(a,b) a>=b? a:b

static char setup=0; // 0 not setup; 1 ibfs; 2 sbfs
static char verbose=0;
static double rcWeights[4]; // array with precalculated mc weights
static double p_min_del,p_max_del, p_min_ins,p_max_ins;
static __s8 dN,dB;
static char equilibration=1;
static __u32 DX;
static __u32 N;
static __u32 seed;
static double q; 
static double coupling;
static double beta;
static double v;
static double K;
static __u32 steps;
static __s8 *bonds;
static gsl_rng * r; 
static __u32 edge[2];
static __s32 adj1[4];
static __s32 adj2[4];
static __u32 offset_1;
static __u32 offset_2;
static __u32 cs2;
static struct queue_2_node *todo_pool;
static __u32 *visited;
static __u32 cs1;
static __u32 activeEdges=0;
static __u32 cutoff;
static __u32 *num_bonds , *num_cluster, *size_giant;
static __u64 *sec_cs_moment, *four_cs_moment;

/******************************************************************************
 *****************************************************************************/
static char init(void)
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

    r = gsl_rng_alloc (gsl_rng_mt19937);
    if(!r)
        return 0;
    gsl_rng_set(r,seed);
    __u32 i;
    bonds = malloc(sizeof(*bonds)*2*N);
    if(!bonds) {
        return 0;
    }
    for(i=0;i<2*N;i++) bonds[i] = -1;
    visited = calloc(N,sizeof(*visited));
    if(!visited) {
        return 0;
    }
    todo_pool = malloc(N*sizeof(struct queue_2_node));
    if(!todo_pool) {
        return 0;
    }
    return 1;    

}
/******************************************************************************
 *****************************************************************************/
static void destroy(void)
{
    if(!setup)return;
    free(todo_pool);
    free(bonds);
    gsl_rng_free(r);
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
static void Adjacent(__u32 bidx)
{

  if(bidx%2) { 
    bidx = (bidx -1)/2;
    edge[0] = bidx;
    edge[1] = ltcXnext(bidx);
  }
  else {
    bidx = bidx/2;
    edge[0] = bidx;
    edge[1] = ltcYprev(bidx);
  }


}
/******************************************************************************
 *****************************************************************************/
static void neighbours(__u32 idx,__u8 a) {
  if(a == 1)
  {
    if(bonds[2*idx] == 1)  adj1[0] = ltcYprev(idx);
    else  adj1[0] = -1;
    if(bonds[(2*idx)+1] == 1)  adj1[1] = ltcXnext(idx);
    else adj1[1] = -1;
    if(bonds[2*ltcYnext(idx)] == 1)	adj1[2] = ltcYnext(idx);
    else adj1[2] = -1;
    if(bonds[(2*ltcXprev(idx))+1] ==1)adj1[3] = ltcXprev(idx);
    else adj1[3] = -1;
  }
  else {
    if(bonds[2*idx] == 1)adj2[0] = ltcYprev(idx);
    else adj2[0] = -1;
    if(bonds[(2*idx)+1] == 1)adj2[1] = ltcXnext(idx);
    else adj2[1] = -1;
    if(bonds[2*ltcYnext(idx)] == 1)	adj2[2] = ltcYnext(idx);
    else adj2[2] = -1;
    if(bonds[(2*ltcXprev(idx))+1] == 1)adj2[3] = ltcXprev(idx);
    else adj2[3] = -1;
  }

}
/******************************************************************************
 *****************************************************************************/
static __u8 breadthFirstSearch_s(__u32 start, __u32 goal)
{
  static struct queue_2 todo1;
  __u32 i=0,activeP1=0;
  init_queue_2(&todo1); //todo_pool);
  cs1=0;
  enqueue_2(&todo1,start); 
  cs1++; // Increase cluster size of bfs 1
  visited[start] = offset_1; // mark starting point as visited
    while(!queue_2_empty_p(&todo1)) { 
      dequeue_2(&todo1,&activeP1);
      neighbours(activeP1,1);
      for(i=0;i<4;i++) {
        if(adj1[i] != -1) {
            if(visited[adj1[i]] == offset_1) continue;
            if((__u32)adj1[i] == goal) {
                    while(!queue_2_empty_p(&todo1)){ dequeue_2(&todo1,&activeP1);}
                    return 1;
                }
            enqueue_2(&todo1,adj1[i]);
            visited[adj1[i]] = offset_1;
            cs1++;
        }
      }
  }
  return 0;
}
/******************************************************************************
 *****************************************************************************/
static __u8 breadthFirstSearch(__u32 start, __u32 goal)
{
  static struct queue_2 todo1,todo2;
  __u32 i=0,activeP1=0,activeP2=0;
  init_queue_2(&todo1);
  init_queue_2(&todo2);
  cs1=0;
  cs2=0;
  enqueue_2(&todo1,start); // Put starting point onto queue_2 1
  cs1++; // Increase cluster size of bfs 1
  visited[start] = offset_1; // mark starting point as visited
  enqueue_2(&todo2,goal); // Put goal point onto queue_2 2
  visited[goal] = offset_2; // mark goal point as visited
  cs2++; // increase cluster size of bfs 2
    while(!queue_2_empty_p(&todo1) && !queue_2_empty_p(&todo2)) { 
      dequeue_2(&todo2,&activeP2); // get next vertex of bfs 2
      neighbours(activeP2,2);	 // get all adjacent vertices of current vertex
      for(i=0;i<4;i++) {

        if(adj2[i] != -1) { // if accessable (edge exists)
	  if(visited[adj2[i]] == offset_2) continue; // already visited
	  if((__u32)adj2[i] == start || visited[adj2[i]] == offset_1) { // reconnected

            while(!queue_2_empty_p(&todo1)){ dequeue_2(&todo1,&activeP1);}
            while(!queue_2_empty_p(&todo2)){ dequeue_2(&todo2,&activeP2);}
	    return 1; // 1 indicates success
	  }
	  enqueue_2(&todo2,adj2[i]);
	  visited[adj2[i]] = offset_2; // mark as visited
	  cs2++; // increase cluster size
	  }
      }
      dequeue_2(&todo1,&activeP1);
      neighbours(activeP1,1);
      for(i=0;i<4;i++) {
	if(adj1[i] != -1) {
	  if(visited[adj1[i]] == offset_1) continue;
	  if((__u32)adj1[i] == goal || visited[adj1[i]] == offset_2) {
            while(!queue_2_empty_p(&todo1)){ dequeue_2(&todo1,&activeP1);}
            while(!queue_2_empty_p(&todo2)){ dequeue_2(&todo2,&activeP2);}
			return 1;
	  }
	  enqueue_2(&todo1,adj1[i]);
	  visited[adj1[i]] = offset_1;
	  cs1++;
	}
      }
  }
  while(!queue_2_empty_p(&todo1)){dequeue_2(&todo1,&activeP1);}
  while(!queue_2_empty_p(&todo2)){dequeue_2(&todo2,&activeP2);}
  return 0; 
}
/******************************************************************************
 *****************************************************************************/
static inline __u8 connected(__u32 start, __u32 goal) {
    offset_1+=2;
    offset_2+=2;
    return breadthFirstSearch(start,goal);
}

/******************************************************************************
 *****************************************************************************/
__u8 static inline connected_s(__u32 start, __u32 goal) {
    ++offset_1;
    return breadthFirstSearch_s(start,goal);
}
/******************************************************************************
 *****************************************************************************/
static inline void   sweep_ibfs(void)
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
static inline void   sweep_sbfs(void)
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
                    dN = (connected_s(edge[0],edge[1])? 0 : 1);
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
                    dN = ( connected_s(edge[0],edge[1]) ?0 : -1);
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
static void extract_observables(__u32 i) {
    static __u32 j=0;
    static __u32 clust_cnt=0;
    static __u64 sum=0,sum_2;
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
            sum_2+=pow(cs1,4);
            chksum+=cs1;
        }

    }
    num_cluster[i] = clust_cnt;
    size_giant[i] = maxc;
    sec_cs_moment[i] = sum;
    four_cs_moment[i] = sum_2;
    maxc=clust_cnt=sum=sum_2=chksum=0;
    offset_1=3;
    offset_2=2;
}
/******************************************************************************
 *****************************************************************************/
static void generateTimeSeries(void)
{ 
  
  __u32 i;
  if(setup==1) {
    for(i=0;i<cutoff;++i)sweep_ibfs();
    equilibration=0;
    if(verbose)
        printf("Equilibration done!\n");
    for(i=0;i<steps;++i) { 
        sweep_ibfs();
        extract_observables(i);
    }
  }
  if(setup==2) {
    for(i=0;i<cutoff;++i)sweep_sbfs();
    equilibration=0;
    if(verbose)
        printf("Equilibration done!\n");
    for(i=0;i<steps;++i) { 
        sweep_sbfs();
        extract_observables(i);
    }


  }
      
}
/******************************************************************************
 *****************************************************************************/
char init_sweeny_ibfs(double _q,unsigned int _l,double _beta,double _coupl,
        unsigned int _cutoff,unsigned _tslength,unsigned int rng_seed,
        void *ts_0,void *ts_1,void * ts_2,void *ts_3,void *ts_4) {
    setup=1;
    offset_1 =1;
    offset_2 = 2;
    activeEdges =0;
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
    four_cs_moment = (__u64 *)ts_4;
    if(!init())
        setup=0;
    return setup;

}
/******************************************************************************
 *****************************************************************************/
char init_sweeny_sbfs(double _q,unsigned int _l,double _beta,double _coupl,
        unsigned int _cutoff,unsigned _tslength,unsigned int rng_seed,
        void *ts_0,void *ts_1,void * ts_2,void *ts_3, void *ts_4) {
    setup=init_sweeny_ibfs(_q,_l,_beta,_coupl,_cutoff,_tslength,rng_seed,
            ts_0,ts_1,ts_2,ts_3,ts_4);
    if(setup)
        return setup=2;
    else
        return setup;
}
/******************************************************************************
 *****************************************************************************/
void destroy_sweeny_ibfs(void) {
    if(setup) {
        destroy();
    }
    setup=0;
}
/******************************************************************************
 *****************************************************************************/
void destroy_sweeny_sbfs(void) {
    destroy_sweeny_ibfs();
}
/******************************************************************************
 *****************************************************************************/
char simulate_sweeny_ibfs(void) {
    if(setup!=1)
        return 0;
    else
        generateTimeSeries();
    return 1;
}
/******************************************************************************
 *****************************************************************************/
char simulate_sweeny_sbfs(void) {
    if(setup!=2)
        return 0;
    else
        generateTimeSeries();
    return 1;
}

