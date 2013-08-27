#ifndef DLLIST_H
#define DLLIST_H

#include "dyncon.h"
/*Datatypes and routines for a double linked list*/
void insert_tree_to_dll(struct EulerTour *,struct s_tree *);
void delete_tree_from_dll(struct EulerTour *,struct s_tree *);

void insert_treeEdge_to_dlls(struct s_node *,struct s_node *, Edge *);
void delete_treeEdge_from_dlls(struct s_node *, struct s_node *, Edge *);
void insert_N_treeEdge_to_dlls(struct s_node *,struct s_node*, Edge *);
void delete_N_treeEdge_from_dlls(struct s_node *,struct s_node *, Edge *);



#endif /*DLLIST_H*/
