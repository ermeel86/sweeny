#ifndef QUEUE_H
#define DIST
#define QUEUE_H
#include <linux/types.h>
struct queue_node
{
	struct queue_node *next;
	__u32 data;
#ifdef DIST
    __u32 distance;
#endif
};

struct queue 
{
	struct queue_node *first;
	struct queue_node *last;
};
/********************************************************************************************/
 __u8 enqueue(struct queue *, const __u32);
 __u8 dequeue(struct queue *, __u32 *);
void init_queue(struct queue *);
int queue_empty_p(const struct queue *q);

#ifdef DIST
 __u8 enqueue_dist(struct queue *, const __u32,const __u32 dist);

__u8 dequeue_dist(struct queue *q, __u32 *value,__u32 *dist);
#endif
/********************************************************************************************/
#endif /*QUEUE_H*/
