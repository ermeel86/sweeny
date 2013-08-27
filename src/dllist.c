#include "dllist.h"
#include <stdio.h>

/**************************************************************************
 **************************************************************************
 */
/* In vertices pt1 is head of tree edges dll, pt2 head of non tree edges dll
 **/
void insert_treeEdge_to_dlls(struct s_node *fromv,struct s_node *tov,Edge *e) {
    
    if(!fromv || !tov || !e)
        return;
    SET_TE(e);
    
    if(!fromv->pt1) {
        fromv->pt1 = e;
        e->fnext = e->fprev = NULL;
    }
    else {
        e->fnext = fromv->pt1; //place e before fromv's head
        if(fromv->pt1->from == fromv->key)
            fromv->pt1->fprev = e; //set fromv's prev pointer to e
        else
            fromv->pt1->tprev = e;
        e->fprev = NULL; //e's prev is null, because first
        fromv->pt1 = e; //new head is e


    }
    updateTBit_Ins(fromv);
    /*To Vertex part*/
    if(!tov->pt1) {
        tov->pt1 = e;
        e->tnext = e->tprev = NULL;
    }
    else {
        e->tnext = tov->pt1; //place e before tov's head
        if(tov->pt1->from == tov->key) 
            tov->pt1->fprev = e;
        else
            tov->pt1->tprev = e; //set tov's prev pointer to e
        e->tprev = NULL; //e's prev is null, because first
        tov->pt1 = e; //new head is e
        

    }
    updateTBit_Ins(tov);
}
/**************************************************************************
 **************************************************************************
 */
void delete_treeEdge_from_dlls(struct s_node *fromv, struct s_node *tov, Edge *e) {
    /* From Vertex Part
     * fromv is the vertex which has edge e=(fromv->key, tov->key) in its list,
     * hence use e's fnext,fprev pointer*/
    if(!fromv || !tov  || !e)
        return;

    if(!e->fprev) {
        fromv->pt1 = e->fnext;
        if(fromv->pt1) {
            if(fromv->pt1->from == fromv->key)
                fromv->pt1->fprev = NULL;
            else
                fromv->pt1->tprev = NULL;
        }
    }
    else {
        if(e->fprev->from == fromv->key)
            e->fprev->fnext = e->fnext; //e's predecessors next points to e's successor
        else
            e->fprev->tnext = e->fnext;
        if(e->fnext) { //if successor exists
            if(e->fnext->from == fromv->key)
                e->fnext->fprev = e->fprev; //let successors prev point to e's predecessor
            else
                e->fnext->tprev = e->fprev;
        }
    }
    updateTBit_Del(fromv);

    /* To Vertex Part
     * tov is the vertex which has edge e=(fromv->key, tov->key) in its list,
     * hence use e's tnext,tprev pointer*/
    if(!e->tprev) {
        tov->pt1 = e->tnext;
        if(tov->pt1) {
            if(tov->pt1->from == tov->key)
                tov->pt1->fprev = NULL;
            else
                tov->pt1->tprev = NULL;
        }
    }
    else {
        if(e->tprev->from == tov->key)
            e->tprev->fnext = e->tnext; //e's predecessors next points to e's successor
        else
            e->tprev->tnext = e->tnext;
        if(e->tnext) { //if successor exists
            if(e->tnext->from == tov->key)
                e->tnext->fprev = e->tprev; //let successors prev point to e's predecessor
            else
                e->tnext->tprev = e->tprev;
        }
    }
    updateTBit_Del(tov);
    RESET_EDGE(e);
    DEACTIVATE_E(e);
}
/**************************************************************************
 **************************************************************************
 */
void insert_N_treeEdge_to_dlls(struct s_node *fromv,struct s_node *tov,Edge *e) {
    
    if(!fromv || !tov || !e)
        return;
    SET_NTE(e);
     
    if(!fromv->pt2) {
        fromv->pt2 = e;
        e->fnext = e->fprev = NULL;
    }
    else {

        e->fnext = fromv->pt2; //place e before fromv's head
        if(fromv->pt2->from == fromv->key)
            fromv->pt2->fprev = e; //set fromv's prev pointer to e
        else
            fromv->pt2->tprev = e;
        e->fprev = NULL; //e's prev is null, because first
        fromv->pt2 = e; //new head is e


    }
    updateEBit_Ins(fromv);
    /*To Vertex part*/
    if(!tov->pt2) {
        tov->pt2 = e;
        e->tnext = e->tprev = NULL;
    }
    else {
        e->tnext = tov->pt2; //place e before tov's head
       if(tov->pt2->from == tov->key) 
            tov->pt2->fprev = e;
        else
            tov->pt2->tprev = e; //set tov's prev pointer to e
        e->tprev = NULL; //e's prev is null, because first
        tov->pt2 = e; //new head is e


    }
    updateEBit_Ins(tov);
}
/**************************************************************************
 **************************************************************************
 */
void delete_N_treeEdge_from_dlls(struct s_node *fromv, struct s_node *tov, Edge *e) {
    /* From Vertex Part
     * fromv is the vertex which has edge e=(fromv->key, tov->key) in its list,
     * hence use e's fnext,fprev pointer*/
    if(!fromv || !tov  || !e)
        return;
    if(!e->fprev) {
        //printf("Deleting first in fnode!\n");
        fromv->pt2 = e->fnext;
        if(fromv->pt2) {
            if(fromv->pt2->from == fromv->key)
                fromv->pt2->fprev = NULL;
            else
                fromv->pt2->tprev = NULL;
        }
    }
    else {
        if(e->fprev->from == fromv->key)
            e->fprev->fnext = e->fnext; //e's predecessors next points to e's successor
        else
            e->fprev->tnext = e->fnext;
        if(e->fnext) { //if successor exists 
            if(e->fnext->from == fromv->key)
                e->fnext->fprev = e->fprev; //let successors prev point to e's predecessor
            else
                e->fnext->tprev = e->fprev;
        }
    }
    updateEBit_Del(fromv);
    /* To Vertex Part
     * tov is the vertex which has edge e=(fromv->key, tov->key) in its list,
     * hence use e's tnext,tprev pointer*/
    if(!e->tprev) {
        //printf("Deleting firt in tnode!\n");
        tov->pt2 = e->tnext;
        if(tov->pt2) {
            if(tov->pt2->from == tov->key)
                tov->pt2->fprev = NULL;
            else
                tov->pt2->tprev = NULL;
        }
    }
    else {
        if(e->tprev->from == tov->key)
            e->tprev->fnext = e->tnext; //e's predecessors next points to e's successor
        else
            e->tprev->tnext = e->tnext;
        if(e->tnext) {//if successor exists
            if(e->tnext->from == tov->key)
                e->tnext->fprev = e->tprev; //let successors prev point to e's predecessor
            else
                e->tnext->tprev = e->tprev;
        }
    }
    updateEBit_Del(tov);
    RESET_EDGE(e);
    DEACTIVATE_E(e);

}
/**************************************************************************
 **************************************************************************
 */
void insert_tree_to_dll(struct EulerTour *et,struct s_tree *n) {
    if(!n || !et)
        return;
    if(!et->head) { /*Empty list*/
       et->head = n;
       n->prev = n->next = NULL;
    }
    else {
        n->next = et->head;
        n->prev = NULL;
        et->head->prev = n;
        et->head = n;
    }
    et->nccomp++;
}
/**************************************************************************
 **************************************************************************
 */
void delete_tree_from_dll(struct EulerTour *et,struct s_tree *n) {
    if(!n | !et)
        return;
    if(!n->prev) { //is head 
        et->head = n->next;
        if(et->head)
            et->head->prev = NULL;
    }
    else {
        n->prev->next = n->next;
        if(n->next)
            n->next->prev = n->prev;

    }
    et->nccomp--;


}

