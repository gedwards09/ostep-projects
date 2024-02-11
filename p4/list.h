#ifndef __list_h__
#define __list_h__

typedef struct __list_node_t list_node_t;
typedef struct __list_t
{
	/* pointer to head of the linked list */
	list_node_t* head;
	/* pointer to current cursor position for iterating list*/
	list_node_t* cursor;
	/* size of the list */
	unsigned int size;
} list_t;

list_t* init_list();
list_t* list_add(list_t* pList, char* pData);
list_node_t* get_head(list_t* pList);
unsigned int get_size(list_t* pList);
char* list_get_next(list_t* pList);
void destroy_list(list_t* pList);
void print_list(list_t* pList);

#endif // __list_h__
