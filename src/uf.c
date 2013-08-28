#include <stdio.h>
#include <stdlib.h>
#include "uf.h"
__u32 findroot(__u32 i, __s32 *ptr)
{
  if(ptr[i] < 0) return i;
  return ptr[i] = findroot(ptr[i],ptr); //path compression!
}
/********************************************************************************************/
void unite(__u32 a,__u32 b, __s32 *ptr)
{	
	__u32 rt_a, rt_b;
	rt_a = findroot(a,ptr);
	rt_b = findroot(b,ptr);
	if(rt_a != rt_b) {
	if(ptr[rt_a] <= ptr[rt_b]) { //balancing
		ptr[rt_a] = ptr[rt_a] + ptr[rt_b];
		ptr[rt_b] = rt_a;
	}
	else {
		ptr[rt_b] = ptr[rt_b] + ptr[rt_a];
		ptr[rt_a] = rt_b;
	}
 }


}
/********************************************************************************************/
__u8 connected(__u32 a, __u32 b, __s32 *ptr)
{
	return findroot(a,ptr) == findroot(b,ptr);

}
/********************************************************************************************/
__s32 * initUF(__u32 length)
{
__u32 i;
__s32 *ptr;

ptr = malloc(sizeof(*ptr)*length);
if(!ptr) {
	fprintf(stderr,"Error in initUF(): Could not allocate memory!\n");
	exit(1);
}
for(i=0;i<length;i++) {
ptr[i] = -1;
}
return ptr;
}
/********************************************************************************************/
void destroyUF(__s32 *ptr)
{
	
		free(ptr);
		ptr = NULL;
}
/********************************************************************************************/

