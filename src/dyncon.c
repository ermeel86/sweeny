#include "dyncon.h"
#include "dllist.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
__u8 curlvl;
static __u32 DX;
struct s_tree *ut, *vt;
__u32 i,from;
/*********************************************************************
**********************************************************************/
/* Initialize dynamic connectivity data structures for a planar 2d square lattice
 * graph with periodic boundary conditions and one dimensional size @dl
 * Return 1 if successfull or 0 if unsuccessful. 
 * For each level save a pointer to the corresponding Euler-Tour data structures in 
 * the @levels array
 */
__u8 init_dc(__u32 dl) {
    
    DX=dlength = dl; 
    nvertices = dlength*dlength;  // 2d square lattice
    nte=te=0;
    maxLevel = (__u8) floor(log(nvertices)/log(2)); //maybe change to foor(log2(dlength)) + 1 ?
#ifdef LEVEL_HEURISTIC
    maxLevel /= 2;
    printf("Reduced maxLevel = %u\n",maxLevel);
#endif /*LEVEL_HEURISTIC*/
    // allocate pointer to Euler-Tour structs for each level
    levels = malloc(sizeof(*levels)*(maxLevel+1));  
    if(!levels)
        return 0;
    __u32 i=0;
    for(;i<=maxLevel;i++) {
        levels[i] = init_et(dlength,i); // create Euler-Tour for level i
        if(!levels[i]) {
            __u32 i_;
            for(i_=0;i_<=i;++i_)
                levels[i_] = destroy_et(levels[i_]);
            free(levels);
            return 0;
        }
    }
    
    // allocate memory for edg epointer adjacency Matrix 
    adjMatrix = malloc(2*nvertices*sizeof(Edge));
    if(!adjMatrix) {
        for(i=0;i<=maxLevel;i++)
            levels[i] = destroy_et(levels[i]);
        free(levels);
        return 0;
    }
    for(i=0;i<nvertices;i++) {
        adjMatrix[2*i].type = 0; // inactive edge  
        adjMatrix[2*i].from = i; 
        adjMatrix[2*i].to =( i < DX ? i+ DX*(DX -1) : i - DX); //up edge
        adjMatrix[2*i].tnext = adjMatrix[2*i].tprev = NULL; // not part of any adjacency list
        adjMatrix[2*i].fnext = adjMatrix[2*i].fprev = NULL;
        adjMatrix[2*i].lvl = 0; // default level is 0
        
        adjMatrix[2*i + 1].type = 0; 
        adjMatrix[2*i+1].from = i;
        adjMatrix[2*i+1].to = ((i + 1) % (DX) ? i +1: i  - DX +1); //right edge 
        adjMatrix[2*i+1].tnext = adjMatrix[2*i+1].tprev = NULL;
        adjMatrix[2*i+1].fnext = adjMatrix[2*i+1].fprev = NULL;
        adjMatrix[2*i+1].lvl = 0;
       
    }
    return 1;




}
/**************************************************************************
 **************************************************************************
 */
/* Destroy the dynamic connectivity data structure*/
__u8 destroy_dc(void) {
    __u32 i;
    for(i=0;i<=maxLevel;i++) {
        levels[i] = destroy_et(levels[i]);
    }
    free(levels);
    free(adjMatrix);
    return 1;




}
/**************************************************************************
 **************************************************************************
 */
/*
 * Insert Edge @e into the dynamic connectivity datastructure. This function assumes that this edge 
 * is not already part of the graph! No test for that. Could be easily implemented (if desired) by checking
 * edge type with ACTIVE_EDGE macro*/
__u8 insert_dc(Edge *e) {
    levels[0]->nedges++;
    if(connected_dc_e(e,1)) { /*Non-Tree-Edge*/
		nte++;
	// insert edge into the two corresponding adjacency lists of non-tree-edges
        insert_N_treeEdge_to_dlls(&levels[0]->vertices[e->from],&levels[0]->vertices[e->to],e); //also updates flag E
    }
    else { /*Tree Edge*/
        te++;
	// insert edge into the two corresponding adjacency lists of tree-edges
        insert_treeEdge_to_dlls(&levels[0]->vertices[e->from],&levels[0]->vertices[e->to],e); //also updates flag T
        // link the corresponding Euler-Tours two construct one EulerTour, i.e. one spanning tree @ level 0
	link_et(e->from,e->to,levels[0]);
        /*insert into tree edge list of vertex u and v*/

    }
 return 1;
}
/**************************************************************************
 **************************************************************************
 */
/* Delete Edge @e from the dynamic connectivity datastructure. Here it is assumed that the edge is
 * part of the graph. No test for that. If desired use ACTIVE_EDGE Macro.
 */ 
__u8 delete_dc(Edge *e) {
    levels[e->lvl]->nedges--;
    if(IS_TE(e)) {
        te--;
        delete_treeEdge_from_dlls(&levels[e->lvl]->vertices[e->from],&levels[e->lvl]->vertices[e->to],e);
        // cut all corresponding Euler-Tours with level <= e->lvl
        for(i=0;i<=e->lvl;i++) {
            cut_et(e->from,e->to,levels[i]);
        }
        // recursively search for a replacement edge at levels <= e->lvl
        if(replace(e->from,e->to,e->lvl)) {
            e->lvl = 0;
			return 0;
        }
        else {
          e->lvl = 0;
          return 1;
         }
    }
    else {
        nte--;
        delete_N_treeEdge_from_dlls(&levels[e->lvl]->vertices[e->from],&levels[e->lvl]->vertices[e->to],e);
        e->lvl = 0;
        return 0;
    }
}

/**************************************************************************
 **************************************************************************
 */
/* Search for a replacement edge connecting vertices @u and @v in levels 
 * <= @lvl. Return 1 if found and 0 if unsuccessful
 */
__u8 replace(__u32 u, __u32 v, __u8 lvl) {
    ut = getTree(&levels[lvl]->vertices[u],0);
    vt = getTree(&levels[lvl]->vertices[v],0);
    curlvl = lvl;
    // if current level is less then maxLevel move all tree edges of smaller spanning tree one level up
    if(lvl<maxLevel)
        moveTreeEdgesUp(ut->root->n <= vt->root->n ? ut->root:vt->root); 
    // start search for replacement edge in smaller spanning tree
    if(seekReplaceEdge(ut->root->n <= vt->root->n ? ut->root:vt->root))
        return 1;
    // if not successfull at level lvl go one down if possible
    else
    {
         if (lvl > 0) 
            return replace(u,v,lvl - 1);
        else    // no replacement edge found, i.e. components must keep split
            return 0;
    }

}
/**************************************************************************
 **************************************************************************
 */
/* Move all tree edges stored in @n's subtree one level up*/
__u8 moveTreeEdgesUp(struct s_node *n) {

  //check if n is non-empty and has tree edges in its subtree (including n)
    if(!n || !(TEST_T_BIT(n)) ) return 0;
    // move tree edges of left sub tree up
    moveTreeEdgesUp(n->sn_left);
    
    // if current splay-tree node represents a vertex move all tree edges
    // saved in its adjacency list one level up
    if(vertexType(n) && n->pt1) { //Only if this is a vertex AND has edges
        Edge *ce;
        while((ce = n->pt1)) {
            if(n->key == ce->from) {
                delete_treeEdge_from_dlls(n,&levels[ce->lvl]->vertices[ce->to],ce);
            }
            else {
                delete_treeEdge_from_dlls(&levels[ce->lvl]->vertices[ce->from],n,ce);
            }
            levels[ce->lvl]->nedges--; 
            ce->lvl +=1;
            insert_treeEdge_to_dlls(&levels[ce->lvl]->vertices[ce->from],&levels[ce->lvl]->vertices[ce->to],ce);
	    levels[ce->lvl]->nedges++; 

            link_et(ce->from,ce->to,levels[ce->lvl]);
        }
    }
   
   moveTreeEdgesUp(n->sn_right);
   return 0;
}
/**************************************************************************
 **************************************************************************
 */
/* Check all non-tree edges in @n's subtree for replacement edge
 * If replacement edge rejoin euler Tours if not move edge one level up
 */
__u8 seekReplaceEdge(struct s_node *n) {

  // check if n is non-empty and if it has non-tree edges in its subtree (including n)
    if(!n || !(TEST_E_BIT(n))) return 0;

    //search in left subtree
        if(seekReplaceEdge(n->sn_left))
            return 1;
        
    if(vertexType(n) && n->pt2) {
        Edge *ce = n->pt2;
        struct s_node *tmp;
        // search through adjacency list of nontree-edges of current vertex n
	while(ce) {
            //get the splay-tree-node of the other vertex adjacent to ce
            tmp = &levels[ce->lvl]->vertices[(n->key == ce->from? ce->to: ce->from)];
            //if this edge reconnects the previously splitted spanning trees use it as replacement edge 
            if(getTree(tmp,0) == (ut->root->n <= vt->root->n ? vt:ut)) { //Replacement Edge found
                // delete it from the corresponding non-tree edge adj.lists
                if(n->key == ce->from) delete_N_treeEdge_from_dlls(n,tmp,ce);
                else delete_N_treeEdge_from_dlls(tmp,n,ce);
                nte--;
                te++;
                for(i=0;i<=ce->lvl;i++) link_et(ce->from,ce->to,levels[i]);
                // insert edge to corresponding tree-edge adj.lists
                insert_treeEdge_to_dlls(&levels[ce->lvl]->vertices[ce->from],&levels[ce->lvl]->vertices[ce->to],ce);
                return 1;         
            }
            // if this edge is not a repl. edge then move it one level up, because its in more dense
            // region of our graph (if possible)
            else { 
	      if(ce->lvl < maxLevel) { /*If not move non-tree edge one level up (if possible)*/
                if(n->key == ce->from) delete_N_treeEdge_from_dlls(n,tmp,ce);
                else delete_N_treeEdge_from_dlls(tmp,n,ce);
		levels[ce->lvl]->nedges--;
                ce->lvl +=1;
                insert_N_treeEdge_to_dlls(&levels[ce->lvl]->vertices[ce->from],&levels[ce->lvl]->vertices[ce->to],ce);
		levels[ce->lvl]->nedges++;
		ce = n->pt2;
	      }
	      else {
		if(ce->from == n->key)
		  ce = ce->fnext;
		else
		  ce = ce->tnext;
	      }
	    }
            
        }
    }
    // search in right subtree
    if(seekReplaceEdge(n->sn_right))
        return 1;
    return 0;
}


