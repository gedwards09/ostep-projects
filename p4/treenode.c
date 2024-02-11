#include <stdio.h>
#include "list.h"
#include "treenode.h"
#include "utilities.h"

/*
 * @fn tree_node_t* init_tree_node(char* pKey, char* pValue)
 * @brief Allocate a new tree node containing pointers to the specified data.
 * @param char* pKey   [in] The key stored in this node
 * @param char* pValue [in] The value to store
 * @returns The allocated tree node
 */
tree_node_t* init_tree_node(char* pKey, char* pValue)
{
	tree_node_t* pTreeNode;
	pTreeNode = Malloc(sizeof(tree_node_t));
	pTreeNode->key = CopyString(pKey);
	pTreeNode->values = init_list();
	pTreeNode->values = list_add(pTreeNode->values, pValue);
	pTreeNode->left = NULL;
	pTreeNode->right = NULL;
	pTreeNode->color = Red;
	return pTreeNode;
}

/* 
 * @fn char* get_key(tree_node_t* pTreeNode)
 * @brief get the key associated with this tree node
 * @param tree_node_t* pTreeNode [in] The node to return key
 * @returns the key for this node
 */
char* get_key(tree_node_t* pTreeNode)
{
	return pTreeNode->key;
}

/*
 * @fn tree_node_t* get_left(tree_node_t* pTreeNode)
 * @brief Gets the right child of this node
 * @param tree_node_t* pTreeNode [in] The node to return left child
 * @returns the left child of this node
 */
tree_node_t* get_left(tree_node_t* pTreeNode)
{
	return pTreeNode->left;
}

/*
 * @fn tree_node_t* get_right(tree_node_t* pTreeNode)
 * @brief Gets the right child of this node
 * @param tree_node_t* pTreeNode [in] The node to return right child
 * @returns the right child of this node
 */
tree_node_t* get_right(tree_node_t* pTreeNode)
{
	return pTreeNode->right;
}

/*
 * @fn enum Color get_color(tree_node_t* pTreeNode)
 * @brief Gets the color of the tree node
 * @param tree_node_t* pTreeNode [in] The tree node
 * @returns Color of the tree node: 0 = Red, 1 = Black.
 */
enum Color get_color(tree_node_t* pTreeNode)
{
	if (pTreeNode != NULL)
	{
		return pTreeNode->color;
	}
	return None;
}

/*
 * @fn void set_left(tree_node_t* pTreeNode, tree_node_t* pChild)
 * @brief sets the left child of this node to the child node
 * @param tree_node_t* pTreeNode [in,out] The node to set the child to
 * @param tree_node_t* pChild    [in]     The child node
 */
void set_left(tree_node_t* pTreeNode, tree_node_t* pChild)
{
	pTreeNode->left = pChild;
}

/*
 * @fn void set_right(tree_node_t* pTreeNode, tree_node_t* pChild)
 * @brief sets the right child of this node to the child node
 * @param tree_node_t* pTreeNode [in,out] The node to set the child to
 * @param tree_node_t* pChild    [in]     The child node
 */
void set_right(tree_node_t* pTreeNode, tree_node_t* pChild)
{
	pTreeNode->right = pChild;
}

/*
 * @fn set_color(tree_node_t* pTreeNode, enum Color color)
 * @brief set the node to the give color
 * @param tree_node_t*     pTreeNode [in,out] The node to set color to
 * @param enum Color color color     [in]     Color to set node to
 */
void set_color(tree_node_t* pTreeNode, enum Color color)
{
	pTreeNode->color =  color;
}

/*
 * @fn void add_value(tree_node_t* pTreeNode, char* pValue)
 * @brief adds the value to this node's value list
 * @param tree_node_t* pTreeNode [in,out] The node to add value to
 * @param char*        pValue    [in]     The value to add to this node's list
 */
void add_value(tree_node_t* pTreeNode, char* pValue)
{
	list_add(pTreeNode->values, pValue);
}

/*
 * @fn void destroy_tree_node(tree_node_t* pTreeNode)
 * @brief Frees the current node and all chidren recursively
 * @param tree_node_t* pTreeNode [in,out] The node to free
 */
void destroy_tree_node(tree_node_t* pTreeNode)
{
	if (pTreeNode == NULL)
	{
		return;
	}
	
	destroy_list(pTreeNode->values);
	destroy_tree_node(pTreeNode->left);
	destroy_tree_node(pTreeNode->right);
	free(pTreeNode->key);
	free(pTreeNode);
}

/*
 * @fn print_tree_node_key_values(tree_node_t* pTreeNode)
 * @brief prints the key and list of values for the tree node
 * @param tree_node_t* pTreeNode [in] The tree node
 */
void print_tree_node(tree_node_t* pTreeNode)
{
	printf("%s: %s\n", 
		pTreeNode->key, 
		pTreeNode->color == Red ? "Red" : "Black");
}
