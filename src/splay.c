#include <stdio.h>
#include <stdlib.h>

#include "splay.h"
static s_tree tmp_tree = INIT_TREE;
/**************************************************************************
 **************************************************************************
 */
  /*Perform a left-rotation at note @y if @y is not the root*/
void rotate_left(struct s_node * y ) {
    struct s_node *x = RIGHT(y), *z  = y->parent;
    if(!(TEST_R_BIT(y))) { //y is not root
        if(LEFT(z) == y)
            z->sn_left = x;
        else
            z->sn_right = x;
    }
    else {
        UNSET_R_BIT(y);
       (CAST_T(y->parent))->root = x;
        SET_R_BIT(x);
        
    }
    y->sn_right = x->sn_left;
    /*Keep augmentation up-to-date*/
    if(TEST_E_BIT(y))
        SET_E_BIT(x);
    else
        UNSET_E_BIT(x);

    if(TEST_T_BIT(y))
        SET_T_BIT(x);
    else
        UNSET_T_BIT(x);

    
    if(y->pt1 || (y->sn_right && TEST_T_BIT(y->sn_right)) || (y->sn_left && TEST_T_BIT(y->sn_left))) //
            SET_T_BIT(y);
    else
        UNSET_T_BIT(y);

    if(y->pt2 || (y->sn_right && TEST_E_BIT(y->sn_right)) || (y->sn_left && TEST_E_BIT(y->sn_left))) //
            SET_E_BIT(y);
    else
        UNSET_E_BIT(y);
     /*End of augmentation update*/   

    if(y->sn_right) 
        y->sn_right->parent = y;

    x->sn_left = y;
    x->parent = z;
    y->parent = x;
    y->n = CALC_N(y);
    x->n = CALC_N(x);




}
/**************************************************************************
 **************************************************************************
 */
 /*Perform a right-rotation at note @y if @y is not the root*/
void rotate_right(struct s_node * y ) {
    struct s_node *x = LEFT(y), *z  = y->parent;
    if(!(TEST_R_BIT(y))) { //y is not root
        if(LEFT(z) == y)
            z->sn_left = x;
        else
            z->sn_right = x;
    }
    else {
        UNSET_R_BIT(y);
       (CAST_T(y->parent))->root = x;
        SET_R_BIT(x);
        
    }
    y->sn_left = x->sn_right;
    /*Keep augmentation up-to-date*/
    if(TEST_E_BIT(y))
        SET_E_BIT(x);
    else
        UNSET_E_BIT(x);

    if(TEST_T_BIT(y))
        SET_T_BIT(x);
    else
        UNSET_T_BIT(x);

    
    if(y->pt1 || (y->sn_right && TEST_T_BIT(y->sn_right)) || (y->sn_left && TEST_T_BIT(y->sn_left))) //
            SET_T_BIT(y);
    else
        UNSET_T_BIT(y);

    if(y->pt2 || (y->sn_right && TEST_E_BIT(y->sn_right)) || (y->sn_left && TEST_E_BIT(y->sn_left))) //
            SET_E_BIT(y);
    else
        UNSET_E_BIT(y);
     /*End of augmentation update*/   

    if(y->sn_left) 
        y->sn_left->parent = y;

    x->sn_right = y;
    x->parent = z;
    y->parent = x;
    y->n = CALC_N(y);
    x->n = CALC_N(x);




}
/**************************************************************************
 **************************************************************************
 */
 /*Perform the splay-operation starting at node @x. Note at the end of this 
  *operation @x is the root of its tree and x->parent points to the head of this
  *Splay Tree
  */
void splay(struct s_node *x) {
    
	while(!(TEST_R_BIT(x))) {
        if(x == x->parent->sn_left) {
            if(TEST_R_BIT(x->parent)) //case 1, parent of x is tree root
                rotate_right(x->parent);
            else if (x->parent == x->parent->parent->sn_left) { //case 2
                    rotate_right(x->parent->parent);
                    rotate_right(x->parent);
            }
            else { // PARENT(x) == RIGHT(GPARENT(x)), case 3
                rotate_right(x->parent);
                rotate_left(x->parent);
            }


        }
        else if (x == x->parent->sn_right) {
            if(TEST_R_BIT(x->parent))
                rotate_left(x->parent);
            else if (x->parent == x->parent->parent->sn_right) {
                    rotate_left(x->parent);
                    rotate_left(x->parent);
            }
            else {
                rotate_left(x->parent);
                rotate_right(x->parent);
            }




        }


    }

}

/**************************************************************************
 **************************************************************************
 */
 /* Split Splay Tree given by head @tr at node @n and use @tl as head of the 
  * additional new tree caused by the split. 
  * @tr will hold the right part, i.e. all elements larger then n
  * @tl will hold the left part, i.e. all elements smaller then n
  * @insLeft specifies either @n will be inserted into tr (0) or tl (1)
  */
struct s_tree * split_temp(struct s_node *n, struct s_tree *tr, 
struct s_tree *tl, __u8 insLeft) {
        
    if(!EMPTY_TREE(tr)) {
        if(!tl)
            return NULL;

        splay(n);
        if(insLeft) {

            tr->root = n->sn_right;
            if(tr->root)   {
                SET_R_BIT(tr->root);
                tr->root->parent = CAST_N tr;
            }

            if(n->sn_right)
                n->n -= n->sn_right->n;
            n->sn_right = NULL;
            tl->root = n;
            n->parent = CAST_N tl;

            /*Keep augmentation up-to-date*/
    
            if(n->pt2 || (n->sn_left && TEST_E_BIT(n->sn_left))) //
                SET_E_BIT(n);
            else
                UNSET_E_BIT(n);

            if(n->pt1 || (n->sn_left && TEST_T_BIT(n->sn_left))) //
                SET_T_BIT(n);
            else
                UNSET_T_BIT(n);
            /*End of augmentation update*/   

        }
        else {
            tl->root = n->sn_left;
            if(tl->root) {
                tl->root->parent = CAST_N tl;
                SET_R_BIT(tl->root);
            }
            if(n->sn_left)
                n->n -= n->sn_left->n;
            n->sn_left = NULL;
            /*Keep augmentation up-to-date*/
            
            if(n->pt2 || (n->sn_right && TEST_E_BIT(n->sn_right))) //
                SET_E_BIT(n);
            else
                UNSET_E_BIT(n);

            if(n->pt1 || (n->sn_right && TEST_T_BIT(n->sn_right))) //
                SET_T_BIT(n);
            else
                UNSET_T_BIT(n);
            /*End of augmentation update*/   
        }
    
    return tl;
    }

    return NULL; 



}
/**************************************************************************
 **************************************************************************
 */
 /* Join the Splay trees given by head @tleft and @tright so that 
 *  all elements in @tleft's tree are followed (<=) any element of @tright's 
 *  tree.
 * @useLeft indidicates which Splay tree head is used for the new joined 
 * splay tree and hence returned by the function. 
 * 0 results in usage of tright
 * 1 results in usage of tleft
 */ 
struct s_tree * join(struct s_tree *tleft, struct s_tree *tright, __u8 useLeft){
        if(useLeft) {
            if(EMPTY_TREE(tleft)) {
                if(!EMPTY_TREE(tright))
                    tleft->root= tright->root;
                    tleft->root->parent = CAST_N tleft;

            }
            else {
                struct s_node *mid;
                mid = s_last(tleft->root); 
                splay(mid);
                mid->sn_right = tright->root;
                mid->n = CALC_N(mid);
                if(tright->root) {
                    tright->root->parent = mid;
                    UNSET_R_BIT(tright->root);
                }
                /*Keep augmentation up-to-date*/
                if(!(TEST_E_BIT(mid)))    
                    if(mid->sn_right && TEST_E_BIT(mid->sn_right)) //
                        SET_E_BIT(mid);
                if(!(TEST_T_BIT(mid)))    
                    if(mid->sn_right && TEST_T_BIT(mid->sn_right)) //
                        SET_T_BIT(mid);
            /*End of augmentation update*/   
            
            }
            
            return tleft;
        }
        else{
            if(EMPTY_TREE(tright)) {
                if(!EMPTY_TREE(tleft))
                    tright->root= tleft->root;
                    tright->root->parent = CAST_N tright;
            }
            else {
                struct s_node *mid;
                mid = s_first(tright->root); 
                splay(mid);
                mid->sn_left = tleft->root;
                mid->n = CALC_N(mid);
                if(tleft->root) {
                    tleft->root->parent = mid;
                    UNSET_R_BIT(tleft->root);
                }
                /*Keep augmentation up-to-date*/
                if(!(TEST_E_BIT(mid)))    
                    if(mid->sn_left && TEST_E_BIT(mid->sn_left)) //
                        SET_E_BIT(mid);
                if(!(TEST_T_BIT(mid)))    
                    if(mid->sn_left && TEST_T_BIT(mid->sn_left)) //
                        SET_T_BIT(mid);
            /*End of augmentation update*/   
        
            }
        
        return tright;
        }


}
/**************************************************************************
 **************************************************************************
 */
/* Insert the node @n into the Splay tree given by head t as the first (min) 
*  element and afterwards (important for the amortized runtime) splay this 
* node @n
*/
void insFirst(struct s_tree *t, struct s_node *n) {
    struct s_node *c = s_first(t->root);
    if(!c) {
        t->root = n;
        n->parent = CAST_N t;
        SET_R_BIT(n);
        n->n = CALC_N(n);
        if(n->pt1)
            SET_T_BIT(n);
        if(n->pt2)
            SET_E_BIT(n);
    }
    else {
        c->sn_left = n;
        n->parent = c;
        c->n = CALC_N(c);
        /*Keep augmentation up-to-date*/
        if(!TEST_E_BIT(c))    
            if(TEST_E_BIT(n)) //
                SET_E_BIT(c);
        if(!(TEST_T_BIT(c)))    
            if(TEST_T_BIT(n)) //
                SET_T_BIT(c);
        /*End of augmentation update*/   

        splay(n);


    }



}
/**************************************************************************
 **************************************************************************
 */
/* Insert the node @n into the Splay tree given by head t as the last (max) 
*  element and afterwards (important for the amortized runtime) splay this 
* node @n
*/
void insLast(struct s_tree *t, struct s_node *n) {
    struct s_node *c = s_last(t->root);
    if(!c) {
        t->root = n;
        n->parent = CAST_N t;
        SET_R_BIT(n);
        n->n = CALC_N(n);
        if(n->pt1)
            SET_T_BIT(n);
        if(n->pt2)
            SET_E_BIT(n);
    }
    else {
        c->sn_right = n;
        n->parent = c;
        c->n = CALC_N(c);
        /*Keep augmentation up-to-date*/
        if(!(TEST_E_BIT(c)))    
            if(TEST_E_BIT(n)) { //
                //printf("SET c's E bit!\n");
                SET_E_BIT(c);

            }
        if(!(TEST_T_BIT(c)))   
            if(TEST_T_BIT(n)) { // {
                //printf("SET c's T bit!\n");
                SET_T_BIT(c);
            }
        /*End of augmentation update*/   


    }
    splay(n);

}
/**************************************************************************
 **************************************************************************
 */
/* Create a new node at the correct position in the Splay tree given 
*  by head @t with key @key and two pointers to data @p1, @p2 . After insertion
* this newly created node will be the root of its tree.
* The new node is returned
*/
struct s_node * insert(__u32 key, void *p1, void *p2,struct s_tree *t ) {
    struct s_node *c = t->root, *p = NULL,*n;
 
        while(c) {
            if(key == c->key) { //insert only new keys
                splay(c);
                    return NULL;
            }
            else {
                p = c;
                c = (c->key < key ? c->sn_right: c->sn_left);
            }
        }
        n = malloc(sizeof(*n));
        INIT_NODE(n,key,p1,p2);
        if(p) {
            if(p->key >= key) {
                p->sn_left = n;
                //update augmentation
            }
            else {
                p->sn_right = n;
                //update augmentation


            }
                n->parent = p;
                p->n = CALC_N(p);
                splay(n);

        }
        else { 
            t->root = n;
            SET_R_BIT(n);
            n->parent = CAST_N t;
            if(n->pt1)
                SET_T_BIT(n);
            if(n->pt2)
                SET_E_BIT(n);

        }
 
    return n;


}
/**************************************************************************
 **************************************************************************
 */
 /* Remove node @d from tree given by head @. If del is 1 the node will also 
 * be free'd. The head of the resulting tree is returned
 */
struct s_tree *  deleteNode(struct s_node *d, struct s_tree *t , __u8 del) {
    tmp_tree.root =NULL;
    tmp_tree.next = tmp_tree.prev = NULL;
    if(EMPTY_TREE(t))
        return t;
    splay(d);
    t->root = d->sn_left;

    if(d->sn_left) {
        d->sn_left->parent = CAST_N t;
        SET_R_BIT(t->root);
    }
    tmp_tree.root = d->sn_right;
    if(d->sn_right) {
        d->sn_right->parent = CAST_N &tmp_tree;
        SET_R_BIT(tmp_tree.root);
    }
    t = join(t,&tmp_tree,1);
    if(del)
        free(d);
 
   
   return t;






}
/**************************************************************************
 **************************************************************************
 */
 /* Delete all nodes from tree given by head @t.*/
void deleteTree(struct s_tree *t) {
    
    while(!EMPTY_TREE(t)) {
         t = deleteNode(t->root,t,1);
    }

}
/**************************************************************************
 **************************************************************************
 */
/*Returns 0 if @a >=@b and 1 if @a<@b*/
__u8 smaller(struct s_node *a, struct s_node *b) {
    splay(a);
    splay(b);
    if(a == b->sn_left)
        return 1;
    if(a == b->sn_right)
        return 0;
    if(a == (b->sn_left ? b->sn_left->sn_left : NULL))
        return 1;
    if (a ==(b->sn_right ? b->sn_right->sn_right : NULL))
         return 0;
    return 2;
}
/**************************************************************************
 **************************************************************************
 */
/* Return the head of tree in which node @c resides. If withSplay is 1 then
*  @c is splayed to the root also. 
*/
struct s_tree * getTree(struct s_node *c,__u8 withSplay) {
    if(!c)
        return NULL;
    if(withSplay) {
        splay(c);
        return CAST_T c->parent;
    }
    else {
            
        s_node *n;
        n = c;
        while(n) {
            if(TEST_R_BIT(n)) {
                return CAST_T n->parent;
            }
            else
                n = n->parent;
        }
    }
  return NULL;
}
/*******************************************************************************
 *******************************************************************************
 */
/* Starting from node @n which was modified hence its T bit could have changed,
* go up to the root and update T bit if necessary
*/
void updateTBit(s_node *n) {

    if(!n)
        return;
    if(TEST_T_BIT(n)) {
        if(!n->pt1 && (!n->sn_left || (n->sn_left && !(TEST_T_BIT(n->sn_left)))) 
        &&   (!n->sn_right || (n->sn_right && !(TEST_T_BIT(n->sn_right))))  ) {
            UNSET_T_BIT(n);
            if(TEST_R_BIT(n))
                return;
            else
                updateTBit(n->parent);
        }
    }
    else {
        if( n->pt1 || (n->sn_left && TEST_T_BIT(n->sn_left)) 
        ||(n->sn_right && TEST_T_BIT(n->sn_right)) ) {
            SET_T_BIT(n);
            if(TEST_R_BIT(n))
                return;
            else
                updateTBit(n->parent);
        }
    }


}
/*******************************************************************************
 *******************************************************************************
 */
void updateTBit_Ins(s_node *n) {

    if(!n)
        return;
    else if(!(TEST_T_BIT(n))) {
        if(n->pt1 || (n->sn_left && TEST_T_BIT(n->sn_left)) 
        ||(n->sn_right && TEST_T_BIT(n->sn_right)) ) {
            SET_T_BIT(n);
            if(TEST_R_BIT(n))
                return;
            if(!(TEST_T_BIT(n->parent)))
                    updateTBit_Ins(n->parent);
        
        }
   }


}
/*******************************************************************************
 *******************************************************************************
 */
void updateEBit_Ins(s_node *n) {

    if(!n)
        return;
    else if(!(TEST_E_BIT(n))) {
        if(n->pt2 || (n->sn_left && TEST_E_BIT(n->sn_left)) 
        ||(n->sn_right && TEST_E_BIT(n->sn_right)) ) {
            SET_E_BIT(n);
            if(TEST_R_BIT(n))
                return;
            if(!(TEST_E_BIT(n->parent)))
                    updateEBit_Ins(n->parent);
        
        }
    }


}
/*******************************************************************************
 *******************************************************************************
 */
void updateEBit_Del(s_node *n) {

    if(!n)
        return;
    else if(TEST_E_BIT(n)) {
        if(!n->pt2 && (!n->sn_left || (n->sn_left && !(TEST_E_BIT(n->sn_left))))
         &&   (!n->sn_right || (n->sn_right && !(TEST_E_BIT(n->sn_right))))  ) {
            UNSET_E_BIT(n);
            if(TEST_R_BIT(n))
                return;
            if(TEST_E_BIT(n->parent))
                    updateEBit_Del(n->parent);
        
        }
   }


}
/*******************************************************************************
 *******************************************************************************
 */
void updateTBit_Del(s_node *n) {

    if(!n)
        return;
    else if(TEST_T_BIT(n)) {
        if(!n->pt1 && (!n->sn_left || (n->sn_left && !(TEST_T_BIT(n->sn_left))))
         &&   (!n->sn_right || (n->sn_right && !(TEST_T_BIT(n->sn_right))))  ) {
            UNSET_T_BIT(n);
            if(TEST_R_BIT(n))
                return;
            if(TEST_T_BIT(n->parent))
                    updateTBit_Del(n->parent);
        
        }   
    }


}
/**************************************************************************
 **************************************************************************
 */
/* Starting from node @n which was modified hence its E bit could have changed,
* go up to the root and update E bit if necessary
*/
void updateEBit(s_node *n) {

    if(!n)
        return;
    if(TEST_E_BIT(n)) {
        if(!n->pt2 && (!n->sn_left || (n->sn_left && !(TEST_E_BIT(n->sn_left))))
         &&   (!n->sn_right || (n->sn_right && !(TEST_E_BIT(n->sn_right))))  ) {
            UNSET_E_BIT(n);
            if(TEST_R_BIT(n))
                return;
            else
                updateEBit(n->parent);
        }
    }
    else {
        if( n->pt2 || (n->sn_left && TEST_E_BIT(n->sn_left))
         ||(n->sn_right && TEST_E_BIT(n->sn_right)) ) {
            SET_E_BIT(n);
            if(TEST_R_BIT(n))
                return;
            else
                updateEBit(n->parent);
        }
    }


}

