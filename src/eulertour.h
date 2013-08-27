#ifndef EULER_TOUR_H
#define EULER_TOUR_H
#include <stdio.h>
#include "splay.h"

extern __u32 DX;





/* Return the node corresponding to the Euler Tour linearization of edge 
   (@u,@v)
*/
static inline s_node * getE_SN(const __u32 u,const __u32 v,  s_node *edges) {
//Note: 0:up, 1: right, 2:down, 3:left
        if((u - v == DX )|| (v - u == DX*(DX-1))) {
            return &edges[u*4]; //u to v up
        }
        if((v - u == DX )|| (u - v == DX*(DX - 1))){
            return &edges[u*4 + 2]; //u to v down
        }
        if((v - u + 1== DX) || (u - v == 1)) {
            return &edges[u*4 + 3]; //s's left edge
        }
        if((u- v + 1== DX) || (v - u == 1)) {
            return &edges[u*4 + 1]; //s's right edge
        }
        //printf("NULL\n")           ; 
        return NULL; //means not a valid edge
}

/**
 * link_et: Given the indices of two vertices @u and @v 
 *      insert the edge (u,v) (undirected) between them
 *      and return the pointer to tree and delete one tree
 **/

s_tree * link_et(__u32, __u32, struct EulerTour *); 

/**
 * cut_et: Given the indices of two vertices @u and @v 
 *      delete the edge (u,v) (undirected) between them
 *      and return the pointer to the new s_tree which holds the 
 *      the new tree
 **/
s_tree * cut_et(__u32 , __u32, struct EulerTour *);

/* Delete Euler Tour representation Splay tree given by
 head @t */
static inline s_tree * deleteTour(s_tree *t) {
    deleteTree(t);
    free(t);
    return NULL;
}
/* Reset splay node @n */
static inline void reset_sn(s_node *n) {
    if(n) {
        n->parent = n->sn_left = n->sn_right = NULL;
        n->pt1 = n->pt2 = NULL;
        n->msk &= 0b00111000; //reset E,T and root bit
        if(vertexType(n))
                n->n = 1;
        else
            n->n = 0;
    }

}

struct EulerTour * init_et(__u32, __u8);
struct EulerTour * destroy_et(struct EulerTour *);

 __u8 connected_et(__u32 , __u32 , struct EulerTour *); 


#endif /*EULER_TOUR_H*/
