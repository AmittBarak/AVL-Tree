#include "avl.h"
#include <stdio.h>
#include <stdlib.h>
#define NOT_EXISTS -1
#define TRUE 1
#define FALSE 0
#define findMax(x,y) (x>y ? x:y)
#define absolute(x) (x<0 ? -x:x)
#define RIGHT 1
#define LEFT 0
#define PREVIOUS 3
#define SUCCESSOR 4

AVLNodePtr balance(AVLNodePtr iBalanceNode);
AVLNodePtr balance_tree(AVLNodePtr newNodeptr, AVLNodePtr root);
AVLNodePtr rotation(AVLNodePtr iBalanceNode, int first);
void update_max_y(AVLNodePtr root);
int balance_height(AVLNodePtr root);
int max_in_range(AVLNodePtr lca, AVLNodePtr x, int direction, int max, int lca_key);
int find_max_y(AVLNodePtr first, AVLNodePtr secoud, AVLNodePtr root);
AVLNodePtr find_min(AVLNodePtr root);
int find_depth(AVLNodePtr x, AVLNodePtr root);
AVLNodePtr balance_after_delete(AVLNodePtr newRoot, AVLNodePtr root);
AVLNodePtr avl_search_by_x(AVLNodePtr root, int x, char elseFind);


int _max(int x, int y) {
	return (x < y) ? y : x;
}

int _abs(int x) {
	return (x < 0) ? -x : x;
}

// AVL search
AVLNodePtr avl_search(AVLNodePtr root, int x, int y) {
	AVLNodePtr tempRoot = root;

	while (tempRoot != NULL) {
		if (tempRoot->key < x) {
			tempRoot = tempRoot->child[RIGHT];
		} else if (tempRoot->key > x) {
			tempRoot = tempRoot->child[LEFT];
		} else if (tempRoot->y == y) {
			return tempRoot; // return node
		} else {
			return NULL;
		}
	}

	return NULL;
}

// Insert new node to avl tree
AVLNodePtr avl_insert(AVLNodePtr root, int x, int y) {
	char notFound = TRUE;
	AVLNodePtr tempRoot = root;// save root
	AVLNodePtr ourRoot = root;
	// if tree empty
	if (root == NULL) {
		tempRoot = new_avl_node(x, y);
		return tempRoot;
	}
	// find his right location and insert him
	while (notFound) {
		if (tempRoot->key < x) {
			if (tempRoot->child[RIGHT] == NULL) {
				tempRoot->child[RIGHT] = new_avl_node(x, y);
				tempRoot->child[RIGHT]->parent = tempRoot;
				notFound = FALSE;
				tempRoot = tempRoot->child[RIGHT];
				// return balance tree after inset
				return balance_tree(tempRoot, root);
			}
			tempRoot = tempRoot->child[RIGHT];
		}
		else if (tempRoot->key > x) {
			if (tempRoot->child[LEFT] == NULL) {
				tempRoot->child[LEFT] = new_avl_node(x, y);
				tempRoot->child[LEFT]->parent = tempRoot;
				notFound = FALSE;
				tempRoot = tempRoot->child[LEFT];
				// return balance tree after inset
				return balance_tree(tempRoot, root);
			}
			tempRoot = tempRoot->child[LEFT];
		}
		// if node already in the tree
		else {
			return ourRoot;
		}
	}
	// return balance tree after inset
	return balance_tree(tempRoot, root);
}

// balncing tree
AVLNodePtr balance_tree(AVLNodePtr newNodeptr, AVLNodePtr root) {
	// save two ibalance index in the route
	int left = 0;
	int right = 0;
	int diff = 0;
	// else - find the first ibalance node in the route
	update_max_y(newNodeptr);
	if ((newNodeptr->parent) != NULL) {
		newNodeptr = newNodeptr->parent;
		while (newNodeptr->key != root->key) {
			// updates height's
			left = newNodeptr->child[LEFT] != NULL ? newNodeptr->child[LEFT]->height : NOT_EXISTS;
			right = newNodeptr->child[RIGHT] != NULL ? newNodeptr->child[RIGHT]->height : NOT_EXISTS;
			newNodeptr->height = findMax(left, right) + 1;
			diff = (left - right);
			if (absolute(diff) > 1) {
				// if the node ibalance balnce him
				newNodeptr = balance(newNodeptr);
			}
			update_max_y(newNodeptr);
			newNodeptr = newNodeptr->parent;
		}
	}
	// update root
	left = newNodeptr->child[LEFT] != NULL ? newNodeptr->child[LEFT]->height : NOT_EXISTS;
	right = newNodeptr->child[RIGHT] != NULL ? newNodeptr->child[RIGHT]->height : NOT_EXISTS;
	newNodeptr->height = findMax(left, right) + 1;
	diff = (left - right);
	// if root is ibalance - balance him
	if (absolute(diff) > 1) {
		newNodeptr = balance(newNodeptr);
	}
	update_max_y(newNodeptr);
	// return root of our balance tree
	return newNodeptr;
}

// balance heights and return balance index (if > 1 the node ibalance)
int balance_height(AVLNodePtr root) {
	int left = root->child[LEFT] != NULL ? root->child[LEFT]->height : NOT_EXISTS;
	int right = root->child[RIGHT] != NULL ? root->child[RIGHT]->height : NOT_EXISTS;
	root->height = findMax(left, right) + 1;
	int diff = (left - right);
	return absolute(diff);
}

// Finds the route with the maximum height - and return his direction (if not exsist return -1)
int max_height(AVLNodePtr root) {

	if (root->child[LEFT] != NULL && root->child[RIGHT] != NULL) {
		return root->child[LEFT]->height > root->child[RIGHT]->height ? LEFT : RIGHT;
	}
	if (root->child[LEFT] != NULL) {
		return LEFT;
	}
	if (root->child[RIGHT] != NULL) {
		return RIGHT;
	}
	return NOT_EXISTS;
}

// balance the first ibalance node in route
AVLNodePtr balance(AVLNodePtr iBalanceNode) {
	// save the first and second in route after ibalance node
	int firstDirection = max_height(iBalanceNode);
	int secondDirection = max_height(iBalanceNode->child[firstDirection]);
	// if we need to do 2 rotations - do the first
	if (firstDirection != secondDirection) {
		AVLNodePtr first = NULL, second = NULL;
		// save the first and second in route
		first = iBalanceNode->child[firstDirection];
		second = first->child[!firstDirection];
		int right = 0;
		int left = 0;
		// update - the second becomes the father of the first
		first->child[!firstDirection] = second->child[firstDirection];
		if (second->child[firstDirection] != NULL) {
			second->child[firstDirection]->parent = first;
		}
		// update height - first
		left = first->child[LEFT] != NULL ? first->child[LEFT]->height : -1;
		right = first->child[RIGHT] != NULL ? first->child[RIGHT]->height : -1;
		first->height = findMax(left, right) + 1;
		update_max_y(first);
		second->child[firstDirection] = first;
		first->parent = second;
		// update height -seocnd
		left = second->child[LEFT] != NULL ? second->child[LEFT]->height : NOT_EXISTS;
		right = second->child[RIGHT] != NULL ? second->child[RIGHT]->height : NOT_EXISTS;
		first->height = findMax(left, right) + 1;
		update_max_y(second);
		iBalanceNode->child[firstDirection] = second;
		second->parent = iBalanceNode;
	}
	// return balance tree after rotation
	return rotation(iBalanceNode, firstDirection);
}

// rotate left-left or right-right
AVLNodePtr rotation(AVLNodePtr iBalanceNode, int first) {
	AVLNodePtr newRoot = iBalanceNode->child[first];
	// Updating pointer's - the father becomes the child of his first child in route
	iBalanceNode->child[first] = newRoot->child[!first];
	int left = iBalanceNode->child[LEFT] != NULL ? iBalanceNode->child[LEFT]->height : NOT_EXISTS;
	int right = iBalanceNode->child[RIGHT] != NULL ? iBalanceNode->child[RIGHT]->height : NOT_EXISTS;
	iBalanceNode->height = findMax(left, right) + 1;
	update_max_y(iBalanceNode);
	if (newRoot->child[!first] != NULL) {
		newRoot->child[!first]->parent = iBalanceNode;
	}
	AVLNodePtr ibalanceParent = iBalanceNode->parent;
	// if ibalance node isent the root - update his parent child
	if (ibalanceParent != NULL) {
		(iBalanceNode->key == ibalanceParent->child[RIGHT]->key) ? (ibalanceParent->child[RIGHT] = newRoot) : (ibalanceParent->child[LEFT] = newRoot);
	}

	iBalanceNode->parent = newRoot;
	newRoot->parent = ibalanceParent;
	newRoot->child[!first] = iBalanceNode;
	left = newRoot->child[LEFT] != NULL ? newRoot->child[LEFT]->height : NOT_EXISTS;
	right = newRoot->child[RIGHT] != NULL ? newRoot->child[RIGHT]->height : NOT_EXISTS;
	newRoot->height = findMax(left, right) + 1;
	// return the right node in route after rotation
	return newRoot;
}

// delete node from tree
AVLNodePtr avl_delete(AVLNodePtr root, int x, int y) {
	AVLNodePtr deleteNode = avl_search(root, x, y);
	// if the node not i the tree - return root
	if (deleteNode == NULL) {
		return root;
	}
	int isRoot = FALSE;
	AVLNodePtr temp = deleteNode;
	AVLNodePtr deletedFather = deleteNode->parent;;
	int childIndex = NOT_EXISTS;
	// Indicates if it is the root
	if (root->key == x) {
		isRoot = TRUE;
		deletedFather = NULL;
	}
	// if the node isn't the root -> find i he is right or left child
	if (deletedFather != NULL) {
		childIndex = deletedFather->child[RIGHT] == deleteNode ? RIGHT : LEFT;
	}
	// Checking his position in the tree and updating pointers
	// If he has two children - find successor and update tree
	if (deleteNode->child[LEFT] != NULL && deleteNode->child[RIGHT] != NULL)
	{
		// find min in right subTree - successor and updates pointres
		temp = find_min(deleteNode->child[RIGHT]);
		AVLNodePtr successorFather = NULL;
		if (temp->parent != deleteNode) {
			successorFather = temp->parent;
			deleteNode->child[RIGHT]->parent = temp;
			temp->child[RIGHT] = deleteNode->child[RIGHT];
			successorFather->child[LEFT] = temp->child[RIGHT];
			if (temp->child[RIGHT] != NULL) {
				temp->child[RIGHT]->parent = successorFather;
			}
		}
		temp->child[LEFT] = deleteNode->child[LEFT];
		deleteNode->child[LEFT]->parent = temp;
		temp->parent = deletedFather;
		if (isRoot) {
			root = temp;
			temp->parent = NULL;
		}
		if (childIndex != -1) {
			deletedFather->child[childIndex] = temp;
		}
		if (successorFather != NULL) {
			if (successorFather->child[RIGHT] != NULL) {
				temp = successorFather->child[RIGHT];
			}
			else {
				temp = successorFather;
			}
		}
		free(deleteNode);
		return balance_after_delete(temp, root);
	}
	// if he a leaf
	else if (deleteNode->child[LEFT] == NULL && deleteNode->child[RIGHT] == NULL) {
		if (childIndex != NOT_EXISTS) {
			deletedFather->child[childIndex] = NULL;
			temp = deletedFather;
			free(deleteNode);
			if (isRoot) {
				root = temp;
			}
			return balance_after_delete(temp, root);
		}
		// if after delete the tree empty return null
		else {
			free(deleteNode);
			return NULL;
		}
	}
	// if he have only left child
	else if (deleteNode->child[LEFT] != NULL && deleteNode->child[RIGHT] == NULL) {
		deleteNode->child[LEFT]->parent = deletedFather;
		temp = deleteNode->child[LEFT];
		if (isRoot) {
			root = temp;
		}
		if (childIndex != NOT_EXISTS) {
			deletedFather->child[childIndex] = deleteNode->child[LEFT];
		}
		free(deleteNode);
		return balance_after_delete(temp, root);
	}

	// if he have only right child
	else {
		deleteNode->child[RIGHT]->parent = deletedFather;
		temp = deleteNode->child[RIGHT];
		if (isRoot) {
			root = temp;
		}
		if (childIndex != NOT_EXISTS) {
			deletedFather->child[childIndex] = deleteNode->child[RIGHT];
		}
		free(deleteNode);
		return balance_after_delete(temp, root);
	}
}

AVLNodePtr balance_after_delete(AVLNodePtr newRoot, AVLNodePtr root) {
	// return balanced tree
	if (newRoot != NULL) {
		if (newRoot->child[LEFT] != NULL) {
			newRoot = newRoot->child[LEFT];
		}
		else if (newRoot->child[RIGHT] != NULL) {
			newRoot = newRoot->child[RIGHT];
		}
	}
	return balance_tree(newRoot, root);
}
// update max_y in route
void update_max_y(AVLNodePtr root) {
	int tempMax = root->y;
	// if two child exsist
	if (root->child[LEFT] != NULL && root->child[RIGHT] != NULL) {
		tempMax = root->child[LEFT]->max_y > root->child[RIGHT]->max_y ?
			root->child[LEFT]->max_y : root->child[RIGHT]->max_y;
		root->max_y = findMax(tempMax, root->y);
		return;
	}
	// if only right exsist
	if (root->child[RIGHT]) {
		tempMax = root->child[RIGHT]->max_y;
		root->max_y = findMax(tempMax, root->y);
		return;
	}
	// if only left exsist
	else if (root->child[LEFT]) {
		tempMax = root->child[LEFT]->max_y;
		root->max_y = findMax(tempMax, root->y);
		return;
	}
	// if is a leaf update and return
	root->max_y = root->y;
	return;
}

// create new node
AVLNodePtr new_avl_node(int x, int y) {
	AVLNodePtr newNode = (AVLNodePtr)malloc(sizeof(AVLNode));
	if (newNode == NULL) {
		return NULL;
	}
	newNode->child[LEFT] = NULL;
	newNode->child[RIGHT] = NULL;
	newNode->parent = NULL;
	newNode->key = x;
	newNode->y = y;
	newNode->max_y = y;
	newNode->height = 0;
	return newNode;
}

// delete tree
void delete_avl_tree(AVLNodePtr root) {
	if (root == NULL) {
		return;
	}
	delete_avl_tree(root->child[LEFT]);
	delete_avl_tree(root->child[RIGHT]);
	free(root);
}

// return max y in range
int avl_range_query(AVLNodePtr root, int x1, int x2) {
	AVLNodePtr first, second;
	// search x1 - if not exists find successor
	first = avl_search_by_x(root, x1, SUCCESSOR);
	// search x2 - if not exists find previous
	second = avl_search_by_x(root, x2, PREVIOUS);
	// If there is no such range in the tree return NULL
	if ((first == NULL || second == NULL) || first->key > second->key) {
		return -1;
	}
	// return max y in range
	return find_max_y(first, second, root);
}

// find max y in range
int find_max_y(AVLNodePtr first, AVLNodePtr second, AVLNodePtr root) {
	int lower;
	int higher;
	AVLNodePtr lower_node;
	AVLNodePtr higher_node;

	// find depth
	int first_depth = find_depth(first, root);
	int second_depth = find_depth(second, root);

	if (first_depth > second_depth) {
		lower = first_depth;
		lower_node = first;

		higher = second_depth;
		higher_node = second;
	}
	else {
		lower = second_depth;
		lower_node = second;

		higher = first_depth;
		higher_node = first;
	}

	// Until the heights are not equal advance the low
	while (lower != higher)
	{
		lower_node = lower_node->parent;
		lower--;
	}
	// find common ancestor
	while (lower_node != higher_node)
	{
		lower_node = lower_node->parent;
		higher_node = higher_node->parent;
	}
	// Find the maximum value in range between LCA to x1 and x2
	int max1 = max_in_range(higher_node, first, LEFT, 0, higher_node->key);
	int max2 = max_in_range(higher_node, second, RIGHT, 0, higher_node->key);
	// return max value
	return findMax(max1, max2);
}

// return max value in range
int max_in_range(AVLNodePtr lca, AVLNodePtr x, int direction, int max, int lca_key) {
	// if we reach x, return his y value
	int sub_max;
	if (lca->key == x->key) {
		if (direction == LEFT && x->child[RIGHT] != NULL) {
			return x->y > x->child[RIGHT]->max_y ? x->key : x->child[RIGHT]->max_y;
		}
		if (direction == RIGHT && x->child[LEFT] != NULL) {
			return x->y > x->child[LEFT]->max_y ? x->key : x->child[LEFT]->max_y;
		}
		return x->y;
	}

	if (lca->key < x->key) {
		// if this is the right dirction
		if (direction == RIGHT) {
			max = lca->y;
			// if we need to inclued this subtree max y
			if (lca->child[LEFT] != NULL && lca->key != lca_key) {
				max = lca->child[LEFT]->max_y > max ? lca->child[LEFT]->max_y : max;
			}
		}
		// return max in range
		sub_max = max_in_range(lca->child[RIGHT], x, direction, max, lca_key);
	}

	else {
		// if this is the right dirction
		if (direction == LEFT) {
			max = lca->y;
			// if we need to inclued this subtree max y
			if (lca->child[RIGHT] != NULL && lca->key != lca_key) {
				max = lca->child[RIGHT]->max_y > max ? lca->child[RIGHT]->max_y : max;
			}
		}
		// return max in range
		sub_max = max_in_range(lca->child[LEFT], x, direction, max, lca_key);
	}

	return findMax(sub_max, max);
}

// find min in sub tree
AVLNodePtr find_min(AVLNodePtr root) {
	while (root->child[LEFT] != NULL) {
		root = root->child[LEFT];
	}
	return root;
}

// find depth
int find_depth(AVLNodePtr x, AVLNodePtr root) {
	int depth = 0;
	AVLNodePtr tmp = x;
	while (tmp != root) {
		tmp = tmp->parent;
		depth++;
	}
	return depth;
}

AVLNodePtr avl_search_by_x(AVLNodePtr root, int x, char elseFind) {
	AVLNodePtr tempRoot = root;
	AVLNodePtr lastNode = NULL;

	// if the tree empty - return null
	while (tempRoot != NULL) {
		// search
		if (tempRoot->key < x) {
			lastNode = tempRoot;
			tempRoot = tempRoot->child[RIGHT];
		}
		else if (tempRoot->key > x) {
			lastNode = tempRoot;
			tempRoot = tempRoot->child[LEFT];
		}
		// if we find - return node
		else {
			return tempRoot;
		}
	}
	// if we didn't found - search succ/previous

	if (elseFind == PREVIOUS) {
		while (lastNode != NULL) {
			if (lastNode->key < x) {
				return lastNode;
			}
			lastNode = lastNode->parent;
		}
	}
	if (elseFind == SUCCESSOR) {
		while (lastNode != NULL) {
			if (lastNode->key > x) {
				return lastNode;
			}
			lastNode = lastNode->parent;
		}
	}
	// if we didnt found - return NULL
	return NULL;
}
