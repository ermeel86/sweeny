#ifndef MYTYPES_H
#define MYTYPES_H


#include <linux/types.h>


#define PARENT(node) (node ?  node->parent : NULL )
#define LEFT(node) (node ? node->sn_left : NULL)
#define RIGHT(node) (node ?  node->sn_right : NULL)
#define GPARENT(node) (PARENT(PARENT(node)))
#define EMPTY_TREE(tree) (!tree->root)
#define INIT_NODE(n,k,p1, p2) do {n->parent = n->sn_left = n->sn_right = NULL; n->key = k; n->msk = 0; setDat(n,p1,p2); n->n = 1;}while(0)

#define INIT_TREE (struct s_tree )  {NULL,NULL,NULL}

#define EMASK 0b00000001 
#define TMASK 0b00000010 
#define RMASK 0b00000100

 
#define TEST_E_BIT(a) (a->msk)&EMASK
#define TEST_T_BIT(a) (a->msk)&TMASK
#define TEST_R_BIT(a) (a->msk)&RMASK
#define SET_E_BIT(a) (a->msk |=EMASK)
#define SET_T_BIT(a) (a->msk |= TMASK)
#define SET_R_BIT(a) (a->msk |=RMASK)
#define UNSET_R_BIT(a) (a->msk = (a->msk)&(~(RMASK)))
#define UNSET_E_BIT(a) (a->msk = (a->msk)&(~(EMASK)))
#define UNSET_T_BIT(a) (a->msk = (a->msk)&(~(TMASK)))
#define FLIP_R_BIT(a) (a->msk = (a->msk)^RMASK)
#define FLIP_E_BIT(a) (a->msk = (a->msk)^EMASK)
#define FLIP_T_BIT(a) (a->msk = (a->msk)^TMASK)
#define CAST_N (struct s_node *)
#define CAST_T (struct s_tree *)
#define N(node) (node ? (node->n) : 0)
#define CALC_N(node) ( (vertexType(node)? 1: 0 ) + N(node->sn_left) + N(node->sn_right))

#ifdef DEBUG
#define EM(msg) do {fprintf(stderr,"%s in line %d:",__FILE__,__LINE__); fprintf(stderr,msg);}while(0)
#define EM_P(msg,par) do {fprintf(stderr,"%s in line %d:",__FILE__,__LINE__); fprintf(stderr,msg,par);}while(0)


#endif




#define RESET_EDGE(e) do{e->fnext = e->fprev = e->tnext = e->tprev = NULL;}while(0)
#define NXT_PTR(vertex,edge) (vertex->key == edge->from ? edge->fnext: edge->tnext)
#define PREV_PTR(vertex,edge) (vertex->key == edge->from ? edge->fprev: edge->tprev)

#define IS_TE(edge) (edge->type == 1)
#define IS_NTE(edge) (edge->type == 2)
#define ACTIVE_EDGE(edge) (edge->type)

#define SET_TE(edge) (edge->type = 1)
#define SET_NTE(edge) (edge->type = 2)
#define DEACTIVATE_E(edge) (edge->type = 0)



#define DIRECTION_P(a) ((a->msk &0b00011000) >> 3)
#define DIRECTION(a) ((a.msk &0b00011000) >> 3)
#define SET_UP(a) ((a.msk) |=    0b00000000)
#define SET_RIGHT(a) ((a.msk) |= 0b00001000)
#define SET_DOWN(a) ((a.msk) |=  0b00010000)
#define SET_LEFT(a) ((a.msk) |=  0b00011000)
typedef struct Edge {
    __u32 from,to;
    __u8 lvl;
    __u8 type;
    struct Edge *fnext,*fprev;
    struct Edge *tnext,*tprev;

} Edge;
typedef struct s_node {
    __u32 key;

    struct s_node *parent;
    struct s_node *sn_left;
    struct s_node *sn_right;
    __u8 msk;
    __u32 n;
    /* Two pointer which could be used for linked list etc*/
    Edge *pt1;
    Edge *pt2; 

} s_node;

typedef struct s_tree {
    struct s_node * root;
    struct s_tree * next; //Pointer for double linked list of trees
    struct s_tree * prev; //Pointer for double linked list for trees
} s_tree;

typedef struct EulerTour {
     __u8 level; //Level of this eulerTour

    __u32 nedges; //Number of (active) edges in this spanning forest/ET
    s_node * edges; //List of pointer to all nodes of all edges
    s_node * vertices; //List of pointer to all vertices
    s_tree *head; //Head of double linked list of trees
    s_tree *tail; //Tail of double linked list of trees
    __u32 nccomp; //number of trees in double linked list of number of connected components in this forest
} EulerTour;

static inline __u8 activeEdge(const s_node *c) {
    return c->msk & 0b01000000;
}

static inline __u8 vertexType(const s_node *c) {
    if(!c)
        return 0;
    else
        return c->msk & 0b00100000;
}


#endif /*MYTYPES_H*/
