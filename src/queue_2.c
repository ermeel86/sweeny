#include <stdio.h>
#include "queue_2.h"
#include <stdlib.h>

/********************************************************************************************/
__u8 enqueue(struct queue *q, const __u32 value) {

struct queue_node *node = malloc(sizeof(struct queue_node));
if (node == NULL) {
	fprintf(stderr,"Error in enqueue(): Could not allocate memory!\n");
	exit(1);
}
node->data = value;
if(q->first == NULL) {
	q->first = q->last = node;
}
else {
	q->last->next = node;
	q->last = node;	

}
node->next = NULL;
return 0;

}
/********************************************************************************************/
__u8 dequeue(struct queue *q, __u32 *value)
{
	if(!q->first) {
		*value = 0;
		return 1;
	}
	*value = q->first->data;
	struct queue_node *tmp = q->first;
	if(q->first == q->last) {
		q->first = q->last = NULL;
	}
	else {
		q->first = q->first->next;
	}
	free(tmp);
	return 0;

}
/********************************************************************************************/
void init_queue(struct queue *q)
{
q->first = q->last = NULL;
}
int queue_empty_p(const struct queue *q)
{
	return q->first == NULL;
}
/********************************************************************************************/
#
