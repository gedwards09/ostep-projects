#include <stdio.h>
#include "listnode.h"
#include "utilities.h"

/*
 * @fn list_node_t* init_list_node(char* pData, list_node_t* pNext)
 * @brief Constructs a new node containing the specified data pointing to the
 *        next node in the list
 * @param char*        pData [in] This node's data
 * @param list_node_t* pNext [in] The node following this node
 * @returns The creates list node
 */
list_node_t* init_list_node(char* pData, list_node_t* pNext)
{
	list_node_t* pListNode;
	pListNode = Malloc(sizeof(list_node_t));
	pListNode->data = CopyString(pData);
	pListNode->next = pNext;
	return pListNode;
}

/*
 * @fn char* get_data(list_node_t* pListNode)
 * @brief gets the data element for the specified node
 * @param list_node_t* pListNode [in] The node whose data to get
 * @returns The data from the specified node
 */
char* get_data(list_node_t* pListNode)
{
	return pListNode->data;
}

/*
 * @fn list_node_t* get_next_node(list_node_t* pListNode)
 * @brief Gets the node following this one in the list
 * @param list_node_t* pListNode [in] The current node
 * @returns pointer to node following this one
 */
list_node_t* get_next_node(list_node_t* pListNode)
{
	return pListNode->next;
}

/*
 * @fn void set_next(list_node_t* pListNode, list_node_t* pNext)
 * @brief Sets the given node to follow the current node in the list
 * @param list_node_t* pListNode [in,out] The current node
 * @param list_node_t* pNext     [in] The node to follow the current node
 */
void set_next(list_node_t* pListNode, list_node_t* pNext)
{
	pListNode->next = pNext;
}

/*
 * @fn void destroy_list-node(list_node_t* pListNode)
 * @brief Frees the current node and all nodes following it, recursively
 * @param list_node_t* pListNode [in,out] The node to destroy
 */
void destroy_list_node(list_node_t* pListNode)
{
	if (pListNode == NULL)
	{
		return;
	}

	destroy_list_node(pListNode->next);
	free(pListNode->data);
	free(pListNode);
}

void r_print_list_node(list_node_t* pListNode)
{
	if (pListNode == NULL)
	{
		return;
	}
	printf("%s -> ", pListNode->data);
	r_print_list_node(pListNode->next);
}
