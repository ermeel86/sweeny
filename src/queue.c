#include <stdio.h>
#include "queue.h"
#include <stdlib.h>
/********************************************************************************************/
__u8 enqueue(struct queue *q, const __u32 value) {

    static struct queue_node *node;
    if(!q->pool) {
        node = malloc(sizeof(struct queue_node));
        if (node == NULL) {
            fprintf(stderr,"Error in enqueue(): Could not allocate memory!\n");
            exit(1);
        }
        node->data = value;
    }
    else
        node= q->pool +  value;
    if(q->first == NULL) {
            q->first = q->last = node;
    }
    else {
            q->last->next = node;
            q->last = node;	

    }
    node->next = NULL;
    q->no_elems++;
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
	static struct queue_node *tmp;
    tmp = q->first;
	if(q->first == q->last) {
		q->first = q->last = NULL;
	}
	else {
		q->first = q->first->next;
	}
	tmp->next = NULL;
//    tmp->visited = 0;
    if(!q->pool)
        free(tmp);
    q->no_elems--;
    //printf("Currently %u elements in queue!\n",q->no_elems);
	return 1;

}
/********************************************************************************************/
__u8 init_queue(struct queue *q, struct queue_node *pool)
{
    q->first = q->last = NULL;
    q->pool = pool;
    q->no_elems = 0;
    return 1;
}
int queue_empty_p(const struct queue *q)
{
	return q->first == NULL;
}

/********************************************************************************************/

