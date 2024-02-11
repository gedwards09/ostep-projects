#ifndef __treemap_h__
#define __treemap_h__

#include "list.h"
#include "treenode.h"

typedef struct __treemap_t
{
	/* root node of the tree */
	tree_node_t* root;
	/* cursor position of the current key for iterating keys in the tree */
	char* cursor;
} treemap_t;

void init_treemap(treemap_t* pTreeMap);
void treemap_add(treemap_t* pTreeMap, char* pKey, char* pValue);
tree_node_t* r_treemap_add(tree_node_t* pNode, char* pKey, char*pValue);
tree_node_t* rotate_left(tree_node_t* pTreeNode);
tree_node_t* rotate_right(tree_node_t* pTreeNode);
char* treemap_get_next_key(treemap_t* pTreeMap, char* pKey);
char* r_treemap_get_next_key(tree_node_t* pNode, char* pKey);
char* treemap_get_next_value(treemap_t* pTreeMap, char* pKey);
char* r_treemap_get_next_value(tree_node_t* pTreeNode, char* pKey);
void destroy_treemap(treemap_t* pTreeMap);
void print_treemap(treemap_t* pTreeMap);
void r_print_tree_node(tree_node_t* pTreeNode);

#endif // __treemap_h__
