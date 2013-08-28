#ifndef QUEUE_H
#define QUEUE_H
#include <linux/types.h>
struct queue_node
{
	struct queue_node *next;
	__u32 data;
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

/********************************************************************************************/
#endif /*QUEUE_H*/
