#ifndef DYNCON_H
#define DYNCON_H
#include "eulertour.h"

/*Set Macro LEVEL_HEURISTIC in dyncon.h to use only half of all levels */
__u32 te;
__u32 nte;
__u8 maxLevel;
__u32 dlength;
__u32 nvertices;

char collect_stat;
/*__u64 del_nte;
__u64 del_te;
__u64 ins_nte;
__u64 ins_te;
__u64 del_red_bonds;
__u64 del_dangle_bonds; */



EulerTour **levels;
Edge *adjMatrix;

__u8 init_dc(__u32); 
__u8 destroy_dc(void);
__u8 delete_dc(Edge *);
__u8 insert_dc(Edge *);
__u8 replace(__u32, __u32 , __u8) ;
static inline __u8 connected_dc(__u32 u, __u32 v, __u8 withSplay) {
    return getTree(&levels[0]->vertices[u],withSplay) == getTree(&levels[0]->vertices[v],\
    withSplay);
}
static inline __u8 connected_dc_e(Edge *e, __u8 withSplay) {
    if(e)
        return connected_dc(e->from,e->to,withSplay);
    else
        return 0;
}
__u8 moveTreeEdgesUp(struct s_node *); 
__u8 seekReplaceEdge(struct s_node *);
#endif /*DYNCON_H*/
