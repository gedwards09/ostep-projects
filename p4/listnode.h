#ifndef __list_node_h__
#define __list_node_h__

typedef struct __list_node_t list_node_t;
struct __list_node_t
{
	/* data element for this node of the list */
	char* data;
	/* pointer to next node in the list */
	list_node_t* next;
};

list_node_t* init_list_node(char* pData, list_node_t* pNext);
char* get_data(list_node_t* pListNode);
list_node_t* get_next_node(list_node_t* pListNode);
void set_next(list_node_t* pListNode, list_node_t* pNext);
void destroy_list_node(list_node_t* pListNode);
void r_print_list_node(list_node_t* pListNode);

#endif // __list_node_h__
