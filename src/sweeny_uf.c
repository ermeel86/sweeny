#include <stdio.h>
#include "sweeny_uf.h"
#include <ctype.h>
#include <strings.h>
#include <stdio.h>
#include "../src/uf.h"
#include "../src/queue.h"
#include <math.h>
#include <gsl/gsl_rng.h> //Verwendung von Mersenne-Twister Pseudozufallszahlengenerator
static __s8 dN;

#define MIN(a,b) a <= b ? a: b
#define MAX(a,b) a >= b? a: b

static char setup=0;
static char verbose=0;
static double rcWeights[4]; // array with precalculated mc weights
static double p_min_del,p_max_del, p_min_ins,p_max_ins;
static struct queue collect1,collect2;
static struct queue_node *todo_pool, *collect_pool;
static __u32 offset_1=1,offset_2=2;
static __u64 *sec_cs_moment ;
static __u32 *size_giant;
static __u32 *num_bonds;
static __u32 *num_cluster;
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
static __s32 *uf1;
static __u32 cs1,cs2;
static __u32 activeEdges=0;
static __u32 cutoff;
/******************************************************************************
 *****************************************************************************/
static char init(void)
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
  r = gsl_rng_alloc(gsl_rng_mt19937);
  if(!r)
      return 0;
  gsl_rng_set(r,seed);
  __u32 i;
  uf1 = initUF(N);
  todo_pool = malloc(N*sizeof(struct queue_node));
  if(!todo_pool) {
      return 0;
  }
  collect_pool = malloc(N*sizeof(struct queue_node));
  if(!collect_pool) {
      return 0;
  }
  for(i=0;i<N;++i) {

      collect_pool[i].next = todo_pool[i].next = NULL;
      collect_pool[i].data = todo_pool[i].data= i;
      collect_pool[i].visited = todo_pool[i].visited = 0;
    }

  bonds = malloc(sizeof(*bonds)*2*N);
  if(!bonds) {
    return 0;
  }
  for(i=0;i<2*N;i++) bonds[i] = -1;
  return 1;
}
/******************************************************************************
 *****************************************************************************/
static void destroy(void)
{
  destroyUF(uf1);
  free(bonds);
  gsl_rng_free(r);
  free(collect_pool);
  free(todo_pool);
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
    edge[1] = ltcYnext(bidx);
  }


}
/******************************************************************************
 *****************************************************************************/
static void Adjacent2(__u32 idx, __u8 a) {
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
static __u8 breadthFirstSearch(__u32 start, __u32 goal,__u8 accept_split)
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
static __u8 removeBond(__u32 a, __u32 b,__u8 accept_split,__u8 accept_non_split) {
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
          if(verbose)fprintf(stderr,"ERROR!\n");
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
static void extract_observables(__u32 cnt)
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
static void generateTimeSeries(void)
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
      extract_observables(i);
      for(j=0;j<N;j++)collect_pool[j].visited=0;
    }
}
/******************************************************************************
 *****************************************************************************/
char init_sweeny_uf(double _q,unsigned int _l,double _beta,double _coupl,
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
    return setup=init();


}
/******************************************************************************
 *****************************************************************************/
void destroy_sweeny_uf(void) {
    if(setup)
        destroy();
    setup=0;
}
/******************************************************************************
 *****************************************************************************/
char simulate_sweeny_uf(void) {
    if(!setup)return 0;
    generateTimeSeries();
    return 1;
}
/******************************************************************************
 *****************************************************************************/
