#ifndef QUEUE_2_H
#define QUEUE_2_H
#include <linux/types.h>
struct queue_2_node
{
	struct queue_2_node *next;
	__u32 data;
};

struct queue_2 
{
	struct queue_2_node *first;
	struct queue_2_node *last;
};
/********************************************************************************************/
 __u8 enqueue_2(struct queue_2 *, const __u32);
 __u8 dequeue_2(struct queue_2 *, __u32 *);
void init_queue_2(struct queue_2 *);
int queue_2_empty_p(const struct queue_2 *q);

/********************************************************************************************/
#endif /*QUEUE_2_H*/
