#include <stdio.h>
#include "listnode.h"
#include "list.h"
#include "utilities.h"

/*
 * @fn list_t* init_list()
 * @brief Construct new linked list
 * @returns pointer to list
 */
list_t* init_list()
{
	list_t* pList;
	pList = Malloc(sizeof(list_t));
	pList->head = NULL;
	pList->cursor = NULL;
	pList->size = 0;
	return pList;
}

/*
 * @fn list_t* list_add(list_t* pList, char* pData)
 * @brief Add data to front of list
 * @param list_t* pList [in,out] list to add data to
 * @param char*   pData  [in]     data to add to list
 * @returns pointer to the list passed in
 */
list_t* list_add(list_t* pList, char* pData)
{
	list_node_t* pNode;
	pNode = init_list_node(pData, pList->head);
	pList->head = pNode;
	pList->size++;
	return pList;
}

/*
 * @fn list_node_t* get_head(list_t* pList)
 * @brief Get head of the linked list
 * @param list_t* pList [in] The list to return head pointer
 * @returns Head of the linked list
 */
list_node_t* get_head(list_t* pList)
{
	return pList->head;
}

/*
 * @fn unsigned int get_size(list_t* pList)
 * @brief Get size of the linked list
 * @param list_t* pList [in] The list to get size of
 * @returns Size of the linked list
 */
unsigned int get_size(list_t* pList)
{
	return pList->size;
}

/*
 * @fn char* list_get_next(list_t* pList)
 * @brief iterator for data elements in the list
 * @param list_t* pList [in,out] The list to iterate. Increments cursor positon.
 * @returns the next data element;
 */
char* list_get_next(list_t* pList)
{
	// list empty nothing to return
	if (pList->head == NULL)
	{
		pList->cursor = NULL;
		return NULL;
	}

	// initialize cursor
	if (pList->cursor == NULL)
	{
		pList->cursor = pList->head;
	}
	// otherwise increment cursor
	else
	{
		pList->cursor = pList->cursor->next;
	}

	if (pList->cursor == NULL)
	{
		// signal end of list
		return NULL;
	}

	return pList->cursor->data;
}

/*
 * @fn void destroy_list(list_t* pList)
 * @brief Frees current list and all of its nodes
 * @param list_t* pList [in,out] The list to destroy
 */
void destroy_list(list_t* pList)
{
	if (pList == NULL)
	{
		return;
	}

	destroy_list_node(pList->head);
	free(pList);
}

void print_list(list_t* pList)
{
	r_print_list_node(pList->head);
	printf("NULL\n");
	printf("(%u)\n", pList->size);
}
