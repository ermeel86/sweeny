#include <stdio.h>
#include "eulertour.h"
#include "dllist.h"
static __u32 DX;
static s_tree tmp;
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
/**************************************************************************
 **************************************************************************
 */
/* Link spanning trees/euler Tours of vertices @u and @v which aren't connected 
 * in Spanning Forest @et*/
s_tree * link_et(__u32 u, __u32 v, struct EulerTour *et ) {
    s_node *un = &et->vertices[u], *vn = &et->vertices[v];
    s_tree *ut = getTree(un,1), *vt = getTree(vn,1), *ut1, *vt1;
    s_node *euv = getE_SN(u,v,et->edges),*evu = getE_SN(v,u,et->edges);
    if(ut == vt)
        return ut;
    //Merge the two euler tours 
    ut1 = split_temp(un,ut,&tmp,1); 
    ut = join(ut,ut1,1);
    insLast(ut,euv);
    vt1 = split_temp(vn,vt,&tmp,1);
    vt = join(vt,vt1,1);
    ut = join(ut,vt,1);
    delete_tree_from_dll(et,vt); //delete tree from list of trees
    free(vt); 
    //Add a last occurrence
    insLast(ut,evu);
    return ut; 


}
/* Cut spanning tree caused by deletion of edge (@u,@v) into two
 * separate spanning trees. Return a pointer to the new one*/
s_tree * cut_et(__u32 u , __u32 v, struct EulerTour *et ) {
    s_node *e1 = getE_SN(u,v,et->edges),*e2 = getE_SN(v,u,et->edges),*tmp_n;
    s_tree *t = getTree(e1,1), *t1, *t2 = getTree(e2,1);
    
    /*smaller with respect to the order in the Euler Tour*/
    if(!smaller(e1,e2)) {
        tmp_n = e1;
        e1 = e2;
        e2 = tmp_n;
        
    }
    t1 = split_temp(e1,t,&tmp,0); //split before e1
    t2 = split(e2,t,1); //split after e2
    t = join(t1,t,0); 
    t2 = deleteNode(e1, t2,0); 
    reset_sn(e1);
    
    t2 = deleteNode(e2,t2,0);
    reset_sn(e2);
    insert_tree_to_dll(et,t2);
    return t2;
    







}
/**************************************************************************
 **************************************************************************
 */
/*Initialize EulerTour datastructure for a two dimensional square lattice 
 * graph with periodic boundary conditions and one dimensional length @dlength 
 * with level @level 
 */
struct EulerTour * init_et(__u32 dlength, __u8 level) {
    DX = dlength;
    struct EulerTour *et = malloc(sizeof(*et));
    if(!et)
        return NULL;
    et->head = et->tail = NULL; //empty double linked list
    et->nccomp = 0; 
    et->nedges = 0; // initially no active edges in this level
    et->vertices = malloc(dlength*dlength*sizeof(s_node));
    if(!et->vertices)
        return NULL;
    et->level = level;
    et->edges = malloc(dlength*dlength*4*sizeof(s_node));
    if(!et->edges) {
        free(et->vertices);
        return NULL;
    }
    __u32 i=0;

    for(;i<dlength*dlength;i++) {
        et->vertices[i].key = i;
        et->vertices[i].msk = 0b00100000; //Bit for Vertex
        reset_sn(&et->vertices[i]);
        
        et->vertices[i].parent = CAST_N malloc(sizeof(s_tree));
        SET_R_BIT((&et->vertices[i]));
        (CAST_T(et->vertices[i].parent))->root = &et->vertices[i];
        insert_tree_to_dll(et,CAST_T (et->vertices[i].parent));
    }
    for(i=0;i<dlength*dlength;i++) {
        
        et->edges[4*i].key = i;
        et->edges[4*i].msk = 0;
        reset_sn(&et->edges[4*i]);

        SET_UP(et->edges[4*i]);

        et->edges[4*i + 1].key = i;
        et->edges[4*i + 1].msk = 0;
        reset_sn(&et->edges[4*i + 1]);
        SET_RIGHT(et->edges[4*i + 1]);

        et->edges[4*i + 2].key = i;
        et->edges[4*i + 2].msk = 0;
        reset_sn(&et->edges[4*i + 2]);
        SET_DOWN(et->edges[4*i + 2]);

        et->edges[4*i + 3].key = i;
        et->edges[4*i + 3].msk = 0;
        reset_sn(&et->edges[4*i + 3]);
        SET_LEFT(et->edges[4*i + 3]);




    }


    return et;

}
/**************************************************************************
 **************************************************************************
 */
/* Destroy i.e. free all memory of EulerTour @et. Return NULL*/
struct EulerTour * destroy_et(struct EulerTour *et) {
    if(!et)
        return NULL;
    struct s_tree *ctree = et->head;
    __u32 cnt=0;
    while(ctree) {
        if(ctree->prev) { 
            free(ctree->prev);
            cnt++;
        }
        ctree = ctree->next;
    }
    
    free(et->tail);
    free(et->vertices);
    free(et->edges);
    free(et);
    return NULL;
}
/* Check if Vertices @u and @v are connected in EulerTour @et*/
 __u8 connected_et(__u32 u, __u32 v, struct EulerTour *et) {
     return getTree(&et->vertices[u],0) == getTree(&et->vertices[v],0);
 }
