#ifndef QUEUE_H
#define QUEUE_H
#include <linux/types.h>
struct queue_node
{
	struct queue_node *next;
	__u32 data;
    __u32 visited;
};

struct queue 
{
	__u32 no_elems;
    struct queue_node *first;
	struct queue_node *last;
    struct queue_node *pool;
};
/********************************************************************************************/
 __u8 enqueue(struct queue *, const __u32);
 __u8 dequeue(struct queue *, __u32 *);
__u8 init_queue(struct queue *, struct queue_node *);
int queue_empty_p(const struct queue *);
/********************************************************************************************/
#endif /*QUEUE_H*/
