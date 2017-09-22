/* CMPT 332 -- Fall 2017
* Assignment 1
* Derek Perrin 		dmp450 11050915
* Dominic McKeith 	dom258 11184543
*/
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#define MAX_LISTS 64    /* we picked these because they're computer numbers */
#define MAX_NODES 1024

/* NODE structure: Used to store data and are used in the linked list. */
typedef struct NODE {
	void* data;
	struct NODE* next;
	struct NODE* prev;
} NODE;

/* LIST structure: 
 * next_free field points to the next free list when the current list is free,
 * otherwise it's NULL.
 */
typedef struct LIST{
	NODE* head;
	NODE* tail;
	NODE* curr; 
	struct LIST* next_free; 
	int size;
} LIST;

/* list memory pool to create new lists */
extern LIST* list_memory;
extern LIST* curr_free_list;

/* node memory pool to create new nodes */
extern NODE* node_memory;
extern NODE* curr_free_node;

LIST* ListCreate();
int ListCount(LIST*);
void* ListFirst(LIST*);
void* ListLast(LIST*);
void* ListNext(LIST*);
void* ListPrev(LIST*);
void* ListCurr(LIST*);
int ListAdd(LIST*, void*);
int ListInsert(LIST*, void*);
int ListPrepend(LIST*, void*);
void* ListRemove(LIST*);
void ListConcat(LIST*, LIST*);
void ListFree(LIST*, void (*)(void*));
void* ListTrim(LIST*);
void* ListSearch(LIST*, int (*)(void*, void*), void*);
#endif /* LIST_H */
