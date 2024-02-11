#ifndef __treenode_h__
#define __treenode_h__

#include "list.h"

enum Color { Red, Black, None };

typedef struct __tree_node_t tree_node_t;
struct __tree_node_t
{
	/* the key stored in this node */
	char* key;
	/* linked list of values associated with this key */
	list_t* values;
	/* left child of the tree node */
	tree_node_t* left;
	/* right child of the tree node */
	tree_node_t* right;
	/* node color for red-black implementation */
	enum Color color;
};

tree_node_t* init_tree_node(char* pKey, char* pValue);
char* get_key(tree_node_t* pTreeNode);
tree_node_t* get_left(tree_node_t* pTreeNode);
tree_node_t* get_right(tree_node_t* pTreeNode);
enum Color get_color(tree_node_t* pTreeNode);
void set_left(tree_node_t* pTreeNode, tree_node_t* pChild);
void set_right(tree_node_t* pTreeNode, tree_node_t* pChild);
void set_color(tree_node_t* pTreeNode, enum Color color);
void add_value(tree_node_t* pTreeNode, char* pValue);
void destroy_tree_node(tree_node_t* pTreeNode);
void print_tree_node(tree_node_t* pTreeNode);

#endif // __treenode_h__
