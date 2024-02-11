#include <stdio.h>
#include <string.h>
#include "treenode.h"
#include "treemap.h"
#include "utilities.h"

/*
 * @fn treemap_t* init_treemap()
 * @brief Allocates and initializes a new treemap
 * @returns The initialized treemap
 */
void init_treemap(treemap_t* pTreeMap)
{
	pTreeMap->root = NULL;
}

/*
 * @fn void treemap_add(treemap_t* pTreeMap, char* pKey, char* pValue)
 * @brief Adds value to list on the node with the given key
 * @param treemap_t* pTreeMap [in,out] The map to add key value pair
 * @param char*      pKey     [in]     The key to add value to
 * @param char*      pValue   [in]     The value to add to list
 */
void treemap_add(treemap_t* pTreeMap, char* pKey, char* pValue)
{
	pTreeMap->root = r_treemap_add(pTreeMap->root, pKey, pValue);
	set_color(pTreeMap->root, Black);
}

/*
 * @fn tree_node_t* r_treemap_add(tree_node_t* pNode, char* pKey, char* pValue)
 * @brief Add key value pair to descendent of the given node. 
 *        Creates node for the key if one doesn't exists and adds it to tree.
 *        Adds the value to list of values associated with the key.
 * @param tree_node_t* pNode  [in] The node to search for key in descendents
 * @param char*        pKey   [in] The key to add
 * @param char*        pValue [in] The value to add
 * @returns Pointer to the node which was passed in.
 */
tree_node_t* r_treemap_add(tree_node_t* pNode, char* pKey, char* pValue)
{
	if (pNode == NULL)
	{
		return init_tree_node(pKey, pValue);
	}

	int compare;
	tree_node_t* pLeft;
	tree_node_t* pRight;

	compare = strcmp(pKey, pNode->key);
	if (compare == 0)
	{
		//found!
		add_value(pNode, pValue);
		return pNode;
	}

	pLeft = get_left(pNode);
	pRight = get_right(pNode);
	if (pLeft != NULL && pRight != NULL 
		&& get_color(pLeft) == Red && get_color(pRight) == Red)
	{
		set_color(pLeft, Black);
		set_color(pRight, Black);
		set_color(pNode, Red);
	}

	if (compare < 0)
	{
		pLeft = r_treemap_add(pLeft, pKey, pValue);
		set_left(pNode, pLeft);

		if (get_color(pLeft) == Red)
		{
			if (get_color(get_left(pLeft)) == Red)
			{
				set_color(pLeft, Black);
				set_color(pNode, Red);
				pNode = rotate_right(pNode);
			}
			else if (get_color(get_right(pLeft)) == Red)
			{
				pLeft = rotate_left(pLeft);
				set_left(pNode, pLeft);
				set_color(pLeft, Black);
				set_color(pNode, Red);
				pNode = rotate_right(pNode);
			}
		}
	}
	else if (compare > 0)
	{
		pRight = r_treemap_add(pRight, pKey, pValue);
		set_right(pNode, pRight);

		if (get_color(pRight) == Red)
		{
			if (get_color(get_right(pRight)) == Red)
			{
				set_color(pRight, Black);
				set_color(pNode, Red);
				pNode = rotate_left(pNode);
			}
			else if (get_color(get_left(pRight)) == Red)
			{
				pRight = rotate_right(pRight);
				set_right(pNode, pRight);
				set_color(pRight, Black);
				set_color(pNode, Red);
				pNode = rotate_left(pNode);
			}
		}
	}

	return pNode;
}

/*
 * @fn tree_node_t* rotate_right(tree_node_t* pTreeNode)
 * @brief Perform right tree rotation at node
 * @param tree_node_t* pTreeNode [in,out] the node to rotate right
 * @returns the new local root after left rotation
 */
tree_node_t* rotate_right(tree_node_t* pTreeNode)
{
	tree_node_t* pLeft = get_left(pTreeNode);
	tree_node_t* pChild = pLeft != NULL ? get_right(pLeft) : NULL;
	set_left(pTreeNode, pChild);
	set_right(pLeft, pTreeNode);
	return pLeft;
}

/*
 * @fn tree_node_t* rotate_left(tree_node_t* pTreeNode)
 * @brief Perform left tree rotation at node
 * @param tree_node_t* pTreeNode [in,out] the node to rotate left
 * @returns the new local root after left rotation
 */
tree_node_t* rotate_left(tree_node_t* pTreeNode)
{
	tree_node_t* pRight = get_right(pTreeNode);
	tree_node_t* pChild = pRight != NULL ? get_left(pRight) : NULL;
	set_right(pTreeNode, pChild);
	set_left(pRight, pTreeNode);
	return pRight;
}

/*
 * @fn char* treemap_get_next_key(treemap_t* pTreeMap, char* pKey)
 * @brief gets the next keys in the map.
 * @param treemap_t* pTreeMap [in] The tree from which to get next key.
 * @param char*      pKey     [in] The current key
 * @returns The next key in the map in sorted order. 
*           NULL if no key follows this key.
 */
char* treemap_get_next_key(treemap_t* pTreeMap, char* pKey)
{
	if (pTreeMap->root == NULL)
	{
		return NULL;
	}

	return r_treemap_get_next_key(pTreeMap->root, pKey);
}

/*
 * @fn r_treemap_get_next_key(tree_node_t* pNode, char* pKey)
 * @brief Searches decendents of the node for the next key in sorted order
 * @param tree_node_t* pNode [in] The current node.
 * @param char*        pKey  [in] The previous key.
 * @returns The next key, if found as a descendent of the current node.
 *          NULL if not found as a descendent of the current node.
 *          Returns first key if pKey is passes NULL.
 */
char* r_treemap_get_next_key(tree_node_t* pNode, char* pKey)
{
	char* pNext;
	int compare;

	if (pNode == NULL)
	{
		return NULL;
	}
	else if (pKey == NULL)
	{
		tree_node_t* pChild;
		pChild = get_left(pNode);
		if (pChild != NULL)
		{
			return r_treemap_get_next_key(pChild, pKey);
		}
		
		return pNode->key;
	}

	compare = strcmp(pKey, pNode->key);
	if (compare < 0)
	{
		pNext = r_treemap_get_next_key(get_left(pNode), pKey);
		if (pNext == NULL)
		{
			pNext = pNode->key;
		}
	}
	else
	{
		pNext = r_treemap_get_next_key(get_right(pNode), pKey);
	}

	return pNext;
}

/*
 * @fn char* treemap_get_next_value(treemap_t* pTreeMap, char* pKey)
 * @brief Gets the next values from the node with the given key
 * @param treemap_t* pTreeMap [in] Pointer to tree map
 * @param char*      pKey     [in] The given key
 * @returns the next value from the node with the given key
 */
char* treemap_get_next_value(treemap_t* pTreeMap, char* pKey)
{
	return r_treemap_get_next_value(pTreeMap->root, pKey);
}

/*
 * @fn char* r_treemap_get_next_value(tree_node_t* pTreeNode, char* pKey)
 * @brief Recursively searches for the node with the given key and 
 *        returns the next value from that node.
 * @param tree_node_t pTreeNode [in] The current node
 * @param char*      pKey       [in] The give key
 * @returns The next value from the given node if found, NULL otherwise.
 */
char* r_treemap_get_next_value(tree_node_t* pTreeNode, char* pKey)
{
	if (pTreeNode == NULL)
	{
		return NULL;
	}
	if (pKey == NULL)
	{
		return NULL;
	}

	int compare = strcmp(pKey, pTreeNode->key);
	if (compare == 0)
	{
		return list_get_next(pTreeNode->values);
	}
	else if (compare < 0)
	{
		return r_treemap_get_next_value(pTreeNode->left, pKey);
	}
	else
	{
		return r_treemap_get_next_value(pTreeNode->right, pKey);
	}
}

/*
 * @fn void destroy_treemap(treemap_t* pTreeMap)
 * @brief destroy all nodes in tree
 * @param treemap_t* pTreeMap [in,out] treemap to destroy
 */
void destroy_treemap(treemap_t* pTreeMap)
{
	destroy_tree_node(pTreeMap->root);
	pTreeMap->root = NULL;
}

/*
 * @fn void print_treemap(treemap_t* pTreeMap)
 * @brief Print out the contents of the tree map by key and list of values.
 * @param treemap_t* pTreeMap [in] Thre treemap
 */
void print_treemap(treemap_t* pTreeMap)
{
	if (pTreeMap->root == NULL)
	{
		printf("NULL");
		return;
	}
	r_print_tree_node(pTreeMap->root);
}

/*
 * @fn void r_print_tree_node(tree_node_t* pTreeNode)
 * @brief Recursively prints the key and linked list of values for each child
 *        node in key order
 * @param tree_node_t* pTreeNode [in] The curent tree node
 */
void r_print_tree_node(tree_node_t* pTreeNode)
{
	tree_node_t* pChild;
	pChild = get_left(pTreeNode);
	if (pChild != NULL)
	{
		r_print_tree_node(pChild);
	}
	print_tree_node(pTreeNode);
	pChild = get_right(pTreeNode);
	if (pChild != NULL)
	{
		r_print_tree_node(pChild);
	}
}
