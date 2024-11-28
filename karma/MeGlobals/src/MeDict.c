/*
 * Dictionary Abstract Data Type
 * Copyright (C) 1997 Kaz Kylheku <kaz@ashi.footprints.net>
 *
 * Free Software License:
 *
 * All rights are reserved by the author, with the following exceptions:
 * Permission is granted to freely reproduce and distribute this software,
 * possibly in exchange for a fee, provided that this copyright notice appears
 * intact. Permission is also granted to adapt this software to produce
 * derivative works, as long as the modified versions carry this copyright
 * notice and additional notices stating that the work has been modified.
 * This source code may be translated into executable form and incorporated
 * into proprietary software; there is no requirement for such software to
 * contain a copyright notice related to this source.
 *
 * $Id: MeDict.c,v 1.8 2001/12/10 19:39:33 dilips Exp $
 * $Name: t-stevet-RWSpre-030110 $
 */

/* 
 * NOTE: This work is derived by MathEngine from Kazlib 
 * Pretty much only function names have been modified from the original
*/

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#if _DEBUG
#   include <MeMessage.h>
#endif

#define DICT_IMPLEMENTATION
#include <MeDict.h>
#include <MeMemory.h>

#ifdef KAZLIB_RCSID
static const char rcsid[] = "$Id: MeDict.c,v 1.8 2001/12/10 19:39:33 dilips Exp $";
#endif

#if _DEBUG
#   define _DEBUG_INSERT    0
#   define _DEBUG_DELETE    0
#endif

/*
 * These macros provide short convenient names for structure members,
 * which are embellished with dict_ prefixes so that they are
 * properly confined to the documented namespace. It's legal for a 
 * program which uses dict to define, for instance, a macro called ``parent''.
 * Such a macro would interfere with the MeDictNode struct definition.
 * In general, highly portable and reusable C modules which expose their
 * structures need to confine structure member names to well-defined spaces.
 * The resulting identifiers aren't necessarily convenient to use, nor
 * readable, in the implementation, however!
 */


#define dict_root(D) ((D)->nilnode.left)
#define dict_nil(D) (&(D)->nilnode)
#define DICT_DEPTH_MAX 64

static MeDictNode *dnode_alloc(void *context);
static void dnode_free(MeDictNode *node, void *context);

/*
 * Perform a ``left rotation'' adjustment on the tree.  The given node P and
 * its right child C are rearranged so that the P instead becomes the left
 * child of C.   The left subtree of C is inherited as the new right subtree
 * for P.  The ordering of the keys within the tree is thus preserved.
 */

static void rotate_left(MeDictNode *upper)
{
    MeDictNode *lower, *lowleft, *upparent;

    lower = upper->right;
    upper->right = lowleft = lower->left;
    lowleft->parent = upper;

    lower->parent = upparent = upper->parent;

    /* don't need to check for root node here because root->parent is
       the sentinel nil node, and root->parent->left points back to root */

    if (upper == upparent->left) {
	upparent->left = lower;
    } else {
	MEASSERT(upper == upparent->right);
	upparent->right = lower;
    }

    lower->left = upper;
    upper->parent = lower;
}

/*
 * This operation is the ``mirror'' image of rotate_left. It is
 * the same procedure, but with left and right interchanged.
 */

static void rotate_right(MeDictNode *upper)
{
    MeDictNode *lower, *lowright, *upparent;

    lower = upper->left;
    upper->left = lowright = lower->right;
    lowright->parent = upper;

    lower->parent = upparent = upper->parent;

    if (upper == upparent->right) {
	upparent->right = lower;
    } else {
	MEASSERT(upper == upparent->left);
	upparent->left = lower;
    }

    lower->right = upper;
    upper->parent = lower;
}

/*
 * Do a postorder traversal of the tree rooted at the specified
 * node and free everything under it.  Used by MeDictFree().
 */

static void free_nodes(MeDict *dict, MeDictNode *node, MeDictNode *nil)
{
    if (node == nil)
	return;
    free_nodes(dict, node->left, nil);
    free_nodes(dict, node->right, nil);
    dict->freenode(node, dict->context);
}

/*
 * This procedure performs a verification that the given subtree is a binary
 * search tree. It performs an inorder traversal of the tree using the
 * MeDictNext() successor function, verifying that the key of each node is
 * strictly lower than that of its successor, if duplicates are not allowed,
 * or lower or equal if duplicates are allowed.  This function is used for
 * debugging purposes. 
 */

static int verify_bintree(MeDict *dict)
{
    MeDictNode *first, *next;

    first = MeDictFirst(dict);

    if (dict->dupes) {
	while (first && (next = MeDictNext(dict, first))) {
	    if (dict->compare(first->key, next->key) > 0)
		return 0;
	    first = next;
	}
    } else {
	while (first && (next = MeDictNext(dict, first))) {
	    if (dict->compare(first->key, next->key) >= 0)
		return 0;
	    first = next;
	}
    }
    return 1;
}


/*
 * This function recursively verifies that the given binary subtree satisfies
 * three of the red black properties. It checks that every red node has only
 * black children. It makes sure that each node is either red or black. And it
 * checks that every path has the same count of black nodes from root to leaf.
 * It returns the blackheight of the given subtree; this allows blackheights to
 * be computed recursively and compared for left and right siblings for
 * mismatches. It does not check for every nil node being black, because there
 * is only one sentinel nil node. The return value of this function is the
 * black height of the subtree rooted at the node ``root'', or zero if the
 * subtree is not red-black.
 */

static unsigned int verify_redblack(MeDictNode *nil, MeDictNode *root)
{
    unsigned height_left, height_right;

    if (root != nil) {
	height_left = verify_redblack(nil, root->left);
	height_right = verify_redblack(nil, root->right);
	if (height_left == 0 || height_right == 0)
	    return 0;
	if (height_left != height_right)
	    return 0;
	if (root->color == kMeDictColorRed) {
	    if (root->left->color != kMeDictColorBlack)
		return 0;
	    if (root->right->color != kMeDictColorBlack)
		return 0;
	    return height_left;
	}
	if (root->color != kMeDictColorBlack)
	    return 0;
	return height_left + 1;
    } 
    return 1;
}

/*
 * Compute the actual count of nodes by traversing the tree and
 * return it. This could be compared against the stored count to
 * detect a mismatch.
 */

static unsigned long verify_node_count(MeDictNode *nil, MeDictNode *root)
{
    if (root == nil)
	return 0;
    else
	return 1 + verify_node_count(nil, root->left)
	    + verify_node_count(nil, root->right);
}

/*
 * Verify that the tree contains the given node. This is done by
 * traversing all of the nodes and comparing their pointers to the
 * given pointer. Returns 1 if the node is found, otherwise
 * returns zero. It is intended for debugging purposes.
 */

static int verify_dict_has_node(MeDictNode *nil, MeDictNode *root, MeDictNode *node)
{
    if (root != nil) {
	return root == node
		|| verify_dict_has_node(nil, root->left, node)
		|| verify_dict_has_node(nil, root->right, node);
    }
    return 0;
}


/*
 * Dynamically allocate and initialize a dictionary object.
 */

MeDict *MeDictCreate(unsigned long maxcount, MeDictCompareFn comp)
{
    MeDict *new = MeMemoryAPI.create(sizeof *new);

    if (new) {
	new->compare = comp;
	new->allocnode = dnode_alloc;
	new->freenode = dnode_free;
	new->context = NULL;
	new->nodecount = 0;
	new->maxcount = maxcount;
	new->nilnode.left = &new->nilnode;
	new->nilnode.right = &new->nilnode;
	new->nilnode.parent = &new->nilnode;
	new->nilnode.color = kMeDictColorBlack;
	new->dupes = 0;
    }
    return new;
}

/*
 * Select a different set of node allocator routines.
 */

void MeDictSetAllocator(MeDict *dict, MeDictAllocFn al,
	MeDictFreeFn fr, void *context)
{
    MEASSERT(MeDictCount(dict) == 0);
    MEASSERT((al == NULL && fr == NULL) || (al != NULL && fr != NULL));

    dict->allocnode = al ? al : dnode_alloc;
    dict->freenode = fr ? fr : dnode_free;
    dict->context = context;
}

/*
 * Free a dynamically allocated dictionary object. Removing the nodes
 * from the tree before deleting it is required.
 */

void MeDictDestroy(MeDict *dict)
{
    MEASSERT(MeDictIsEmpty(dict));
    MeMemoryAPI.destroy(dict);
}

/*
 * Free all the nodes in the dictionary by using the dictionary's
 * installed free routine. The dictionary is emptied.
 */

void MeDictFreeNodes(MeDict *dict)
{
    MeDictNode *nil = dict_nil(dict), *root = dict_root(dict);
    free_nodes(dict, root, nil);
    dict->nodecount = 0;
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
}

/*
 * Obsolescent function, equivalent to MeDictFreeNodes
 */

void MeDictFree(MeDict *dict)
{
#ifdef KAZLIB_OBSOLESCENT_DEBUG
    MEASSERT("call to obsolescent function MeDictFree()" && 0);
#endif
    MeDictFreeNodes(dict);
}

/*
 * Initialize a user-supplied dictionary object.
 */

MeDict *MeDictInit(MeDict *dict, unsigned long maxcount, MeDictCompareFn comp)
{
    dict->compare = comp;
    dict->allocnode = dnode_alloc;
    dict->freenode = dnode_free;
    dict->context = NULL;
    dict->nodecount = 0;
    dict->maxcount = maxcount;
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
    dict->nilnode.parent = &dict->nilnode;
    dict->nilnode.color = kMeDictColorBlack;
    dict->dupes = 0;
    return dict;
}

/* 
 * Initialize a dictionary in the likeness of another dictionary
 */

void MeDictInitLike(MeDict *dict, const MeDict *template)
{
    dict->compare = template->compare;
    dict->allocnode = template->allocnode;
    dict->freenode = template->freenode;
    dict->context = template->context;
    dict->nodecount = 0;
    dict->maxcount = template->maxcount;
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
    dict->nilnode.parent = &dict->nilnode;
    dict->nilnode.color = kMeDictColorBlack;
    dict->dupes = template->dupes;

    MEASSERT(MeDictSimilar(dict, template));
}

/*
 * Remove all nodes from the dictionary (without freeing them in any way).
 */

static void MeDictClear(MeDict *dict)
{
    dict->nodecount = 0;
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
    dict->nilnode.parent = &dict->nilnode;
    MEASSERT(dict->nilnode.color == kMeDictColorBlack);
}


/*
 * Verify the integrity of the dictionary structure.  This is provided for
 * debugging purposes, and should be placed in assert statements.   Just because
 * this function succeeds doesn't mean that the tree is not corrupt. Certain
 * corruptions in the tree may simply cause undefined behavior.
 */ 

int MeDictVerify(MeDict *dict)
{
    MeDictNode *nil = dict_nil(dict), *root = dict_root(dict);

    /* check that the sentinel node and root node are black */
    if (root->color != kMeDictColorBlack)
	return 0;
    if (nil->color != kMeDictColorBlack)
	return 0;
    if (nil->right != nil)
	return 0;
    /* nil->left is the root node; check that its parent pointer is nil */
    if (nil->left->parent != nil)
	return 0;
    /* perform a weak test that the tree is a binary search tree */
    if (!verify_bintree(dict))
	return 0;
    /* verify that the tree is a red-black tree */
    if (!verify_redblack(nil, root))
	return 0;
    if (verify_node_count(nil, root) != MeDictCount(dict))
	return 0;
    return 1;
}

/*
 * Determine whether two dictionaries are similar: have the same comparison and
 * allocator functions, and same status as to whether duplicates are allowed.
 */

int MeDictSimilar(const MeDict *left, const MeDict *right)
{
    if (left->compare != right->compare)
	return 0;

    if (left->allocnode != right->allocnode)
	return 0;

    if (left->freenode != right->freenode)
	return 0;

    if (left->context != right->context)
	return 0;

    if (left->dupes != right->dupes)
	return 0;

    return 1;
}

/*
 * Locate a node in the dictionary having the given key.
 * If the node is not found, a null a pointer is returned (rather than 
 * a pointer that dictionary's nil sentinel node), otherwise a pointer to the
 * located node is returned.
 */

MeDictNode *MeDictLookup(MeDict *dict, const void *key)
{
    MeDictNode *root = dict_root(dict);
    MeDictNode *nil = dict_nil(dict);
    MeDictNode *saved;
    int result;

    /* simple binary search adapted for trees that contain duplicate keys */

    while (root != nil) {
	result = dict->compare(key, root->key);
	if (result < 0)
	    root = root->left;
	else if (result > 0)
	    root = root->right;
	else {
	    if (!dict->dupes) {	/* no duplicates, return match		*/
		return root;
	    } else {		/* could be dupes, find leftmost one	*/
		do {
		    saved = root;
		    root = root->left;
		    while (root != nil && dict->compare(key, root->key))
			root = root->right;
		} while (root != nil);
		return saved;
	    }
	}
    }

    return NULL;
}

/*
 * Look for the node corresponding to the lowest key that is equal to or
 * greater than the given key.  If there is no such node, return null.
 */

MeDictNode *MeDictLowerBound(MeDict *dict, const void *key)
{
    MeDictNode *root = dict_root(dict);
    MeDictNode *nil = dict_nil(dict);
    MeDictNode *tentative = 0;

    while (root != nil) {
	int result = dict->compare(key, root->key);

	if (result > 0) {
	    root = root->right;
	} else if (result < 0) {
	    tentative = root;
	    root = root->left;
	} else {
	    if (!dict->dupes) {
	    	return root;
	    } else {
		tentative = root;
		root = root->left;
	    }
	} 
    }
    
    return tentative;
}

/*
 * Look for the node corresponding to the greatest key that is equal to or
 * lower than the given key.  If there is no such node, return null.
 */

MeDictNode *MeDictUpperBound(MeDict *dict, const void *key)
{
    MeDictNode *root = dict_root(dict);
    MeDictNode *nil = dict_nil(dict);
    MeDictNode *tentative = 0;

    while (root != nil) {
	int result = dict->compare(key, root->key);

	if (result < 0) {
	    root = root->left;
	} else if (result > 0) {
	    tentative = root;
	    root = root->right;
	} else {
	    if (!dict->dupes) {
	    	return root;
	    } else {
		tentative = root;
		root = root->right;
	    }
	} 
    }
    
    return tentative;
}

/*
 * Insert a node into the dictionary. The node should have been
 * initialized with a data field. All other fields are ignored.
 * The behavior is undefined if the user attempts to insert into
 * a dictionary that is already full (for which the MeDictIsFull()
 * function returns true).
 */

void MeDictInsert(MeDict *dict, MeDictNode *node, const void *key)
{
    MeDictNode *where = dict_root(dict), *nil = dict_nil(dict);
    MeDictNode *parent = nil, *uncle, *grandpa;
    int result = -1;

#if _DEBUG_INSERT
    MeDebug(2,"MeDictInsert: dict 0x%08x, node 0x%08x, key 0x%08x\n",
      (long) dict, (long) node, (long) key);
#endif

    node->key = key;

    MEASSERT(!MeDictIsFull(dict));
    MEASSERT(!MeDictContains(dict, node));
    MEASSERT(!MeDictNodeIsInADict(node));

    /* basic binary tree insert */

    while (where != nil) {
	parent = where;
	result = dict->compare(key, where->key);
	/* trap attempts at duplicate key insertion unless it's explicitly allowed */
	MEASSERT(dict->dupes || result != 0);
	if (result < 0)
	    where = where->left;
	else
	    where = where->right;
    }

    MEASSERT(where == nil);

    if (result < 0)
	parent->left = node;
    else
	parent->right = node;

    node->parent = parent;
    node->left = nil;
    node->right = nil;

    dict->nodecount++;

    /* red black adjustments */

    node->color = kMeDictColorRed;

    while (parent->color == kMeDictColorRed) {
	grandpa = parent->parent;
	if (parent == grandpa->left) {
	    uncle = grandpa->right;
	    if (uncle->color == kMeDictColorRed) {	/* red parent, red uncle */
		parent->color = kMeDictColorBlack;
		uncle->color = kMeDictColorBlack;
		grandpa->color = kMeDictColorRed;
		node = grandpa;
		parent = grandpa->parent;
	    } else {				/* red parent, black uncle */
	    	if (node == parent->right) {
		    rotate_left(parent);
		    parent = node;
		    MEASSERT(grandpa == parent->parent);
		    /* rotation between parent and child preserves grandpa */
		}
		parent->color = kMeDictColorBlack;
		grandpa->color = kMeDictColorRed;
		rotate_right(grandpa);
		break;
	    }
	} else { 	/* symmetric cases: parent == parent->parent->right */
	    uncle = grandpa->left;
	    if (uncle->color == kMeDictColorRed) {
		parent->color = kMeDictColorBlack;
		uncle->color = kMeDictColorBlack;
		grandpa->color = kMeDictColorRed;
		node = grandpa;
		parent = grandpa->parent;
	    } else {
	    	if (node == parent->left) {
		    rotate_right(parent);
		    parent = node;
		    MEASSERT(grandpa == parent->parent);
		}
		parent->color = kMeDictColorBlack;
		grandpa->color = kMeDictColorRed;
		rotate_left(grandpa);
		break;
	    }
	}
    }

    dict_root(dict)->color = kMeDictColorBlack;

    MEASSERT(MeDictVerify(dict));
}

/*
 * Delete the given node from the dictionary. If the given node does not belong
 * to the given dictionary, undefined behavior results.  A pointer to the
 * deleted node is returned.
 */

MeDictNode *MeDictDelete(MeDict *dict, MeDictNode *delete)
{
    MeDictNode *nil = dict_nil(dict), *child, *delparent = delete->parent;

#if _DEBUG_DELETE
    MeDebug(2,"MeDictDelete: dict 0x%08x, delete 0x%08x\n",
      (long) dict, (long) delete);
#endif

    /* basic deletion */

    MEASSERT(!MeDictIsEmpty(dict));
    MEASSERT(MeDictContains(dict, delete));

    /*
     * If the node being deleted has two children, then we replace it with its
     * successor (i.e. the leftmost node in the right subtree.) By doing this,
     * we avoid the traditional algorithm under which the successor's key and
     * value *only* move to the deleted node and the successor is spliced out
     * from the tree. We cannot use this approach because the user may hold
     * pointers to the successor, or nodes may be inextricably tied to some
     * other structures by way of embedding, etc. So we must splice out the
     * node we are given, not some other node, and must not move contents from
     * one node to another behind the user's back.
     */

    if (delete->left != nil && delete->right != nil) {
	MeDictNode *next = MeDictNext(dict, delete);
	MeDictNode *nextparent = next->parent;
	MeDictColor nextcolor = next->color;

	MEASSERT(next != nil);
	MEASSERT(next->parent != nil);
	MEASSERT(next->left == nil);

	/*
	 * First, splice out the successor from the tree completely, by
	 * moving up its right child into its place.
	 */

	child = next->right;
	child->parent = nextparent;

	if (nextparent->left == next) {
	    nextparent->left = child;
	} else {
	    MEASSERT(nextparent->right == next);
	    nextparent->right = child;
	}

	/*
	 * Now that the successor has been extricated from the tree, install it
	 * in place of the node that we want deleted.
	 */

	next->parent = delparent;
	next->left = delete->left;
	next->right = delete->right;
	next->left->parent = next;
	next->right->parent = next;
	next->color = delete->color;
	delete->color = nextcolor;

	if (delparent->left == delete) {
	    delparent->left = next;
	} else {
	    MEASSERT(delparent->right == delete);
	    delparent->right = next;
	}

    } else {
	MEASSERT(delete != nil);
	MEASSERT(delete->left == nil || delete->right == nil);

	child = (delete->left != nil) ? delete->left : delete->right;

	child->parent = delparent = delete->parent;	    

	if (delete == delparent->left) {
	    delparent->left = child;    
	} else {
	    MEASSERT(delete == delparent->right);
	    delparent->right = child;
	}
    }

    delete->parent = NULL;
    delete->right = NULL;
    delete->left = NULL;

    dict->nodecount--;

    MEASSERT(verify_bintree(dict));

    /* red-black adjustments */

    if (delete->color == kMeDictColorBlack) {
	MeDictNode *parent, *sister;

	dict_root(dict)->color = kMeDictColorRed;

	while (child->color == kMeDictColorBlack) {
	    parent = child->parent;
	    if (child == parent->left) {
		sister = parent->right;
		MEASSERT(sister != nil);
		if (sister->color == kMeDictColorRed) {
		    sister->color = kMeDictColorBlack;
		    parent->color = kMeDictColorRed;
		    rotate_left(parent);
		    sister = parent->right;
		    MEASSERT(sister != nil);
		}
		if (sister->left->color == kMeDictColorBlack
			&& sister->right->color == kMeDictColorBlack) {
		    sister->color = kMeDictColorRed;
		    child = parent;
		} else {
		    if (sister->right->color == kMeDictColorBlack) {
			MEASSERT(sister->left->color == kMeDictColorRed);
			sister->left->color = kMeDictColorBlack;
			sister->color = kMeDictColorRed;
			rotate_right(sister);
			sister = parent->right;
			MEASSERT(sister != nil);
		    }
		    sister->color = parent->color;
		    sister->right->color = kMeDictColorBlack;
		    parent->color = kMeDictColorBlack;
		    rotate_left(parent);
		    break;
		}
	    } else {	/* symmetric case: child == child->parent->right */
		MEASSERT(child == parent->right);
		sister = parent->left;
		MEASSERT(sister != nil);
		if (sister->color == kMeDictColorRed) {
		    sister->color = kMeDictColorBlack;
		    parent->color = kMeDictColorRed;
		    rotate_right(parent);
		    sister = parent->left;
		    MEASSERT(sister != nil);
		}
		if (sister->right->color == kMeDictColorBlack
			&& sister->left->color == kMeDictColorBlack) {
		    sister->color = kMeDictColorRed;
		    child = parent;
		} else {
		    if (sister->left->color == kMeDictColorBlack) {
			MEASSERT(sister->right->color == kMeDictColorRed);
			sister->right->color = kMeDictColorBlack;
			sister->color = kMeDictColorRed;
			rotate_left(sister);
			sister = parent->left;
			MEASSERT(sister != nil);
		    }
		    sister->color = parent->color;
		    sister->left->color = kMeDictColorBlack;
		    parent->color = kMeDictColorBlack;
		    rotate_right(parent);
		    break;
		}
	    }
	}

	child->color = kMeDictColorBlack;
	dict_root(dict)->color = kMeDictColorBlack;
    }

    MEASSERT(MeDictVerify(dict));

    return delete;
}

/*
 * Allocate a node using the dictionary's allocator routine, give it
 * the data item.
 */

int MeDictAllocInsert(MeDict *dict, const void *key, void *data)
{
    MeDictNode *node = dict->allocnode(dict->context);

    if (node) {
	MeDictNodeInit(node, data);
	MeDictInsert(dict, node, key);
	return 1;
    }
    return 0;
}

void MeDictDeleteFree(MeDict *dict, MeDictNode *node)
{
    MeDictDelete(dict, node);
    dict->freenode(node, dict->context);
}

/*
 * Return the node with the lowest (leftmost) key. If the dictionary is empty
 * (that is, MeDictIsEmpty(dict) returns 1) a null pointer is returned.
 */

MeDictNode *MeDictFirst(MeDict *dict)
{
    MeDictNode *nil = dict_nil(dict), *root = dict_root(dict), *left;

    if (root != nil)
	while ((left = root->left) != nil)
	    root = left;

    return (root == nil) ? NULL : root;
}

/*
 * Return the node with the highest (rightmost) key. If the dictionary is empty
 * (that is, MeDictIsEmpty(dict) returns 1) a null pointer is returned.
 */

MeDictNode *MeDictLast(MeDict *dict)
{
    MeDictNode *nil = dict_nil(dict), *root = dict_root(dict), *right;

    if (root != nil)
	while ((right = root->right) != nil)
	    root = right;

    return (root == nil) ? NULL : root;
}

/*
 * Return the given node's successor node---the node which has the
 * next key in the the left to right ordering. If the node has
 * no successor, a null pointer is returned rather than a pointer to
 * the nil node.
 */

MeDictNode *MeDictNext(MeDict *dict, MeDictNode *curr)
{
    MeDictNode *nil = dict_nil(dict), *parent, *left;

    if (curr->right != nil) {
	curr = curr->right;
	while ((left = curr->left) != nil)
	    curr = left;
	return curr;
    }

    parent = curr->parent;

    while (parent != nil && curr == parent->right) {
	curr = parent;
	parent = curr->parent;
    }

    return (parent == nil) ? NULL : parent;
}

/*
 * Return the given node's predecessor, in the key order.
 * The nil sentinel node is returned if there is no predecessor.
 */

MeDictNode *MeDictPrev(MeDict *dict, MeDictNode *curr)
{
    MeDictNode *nil = dict_nil(dict), *parent, *right;

    if (curr->left != nil) {
	curr = curr->left;
	while ((right = curr->right) != nil)
	    curr = right;
	return curr;
    }

    parent = curr->parent;

    while (parent != nil && curr == parent->left) {
	curr = parent;
	parent = curr->parent;
    }

    return (parent == nil) ? NULL : parent;
}

void MeDictAllowDupes(MeDict *dict)
{
    dict->dupes = 1;
}

#undef MeDictCount
#undef MeDictIsEmpty
#undef MeDictIsFull
#undef MeDictNodeGet
#undef MeDictNodePut
#undef MeDictNodeGetkey

unsigned long MeDictCount(MeDict *dict)
{
    return dict->nodecount;
}

int MeDictIsEmpty(MeDict *dict)
{
    return dict->nodecount == 0;
}

int MeDictIsFull(MeDict *dict)
{
    return dict->nodecount == dict->maxcount;
}

int MeDictContains(MeDict *dict, MeDictNode *node)
{
    return verify_dict_has_node(dict_nil(dict), dict_root(dict), node);
}

static MeDictNode *dnode_alloc(void *context)
{
    return MeMemoryAPI.create(sizeof *dnode_alloc(NULL));
}

static void dnode_free(MeDictNode *node, void *context)
{
    MeMemoryAPI.destroy(node);
}

MeDictNode *MeDictNodeCreate(void *data)
{
    MeDictNode *new = MeMemoryAPI.create(sizeof *new);
    if (new) {
	new->data = data;
	new->parent = NULL;
	new->left = NULL;
	new->right = NULL;
    }
    return new;
}

MeDictNode *MeDictNodeInit(MeDictNode *dnode, void *data)
{
    dnode->data = data;
    dnode->parent = NULL;
    dnode->left = NULL;
    dnode->right = NULL;
    return dnode;
}

void MeDictNodeDestroy(MeDictNode *dnode)
{
    MEASSERT(!MeDictNodeIsInADict(dnode));
    MeMemoryAPI.destroy(dnode);
}

void *MeDictNodeGet(MeDictNode *dnode)
{
    return dnode->data;
}

const void *MeDictNodeGetkey(MeDictNode *dnode)
{
    return dnode->key;
}

void MeDictNodePut(MeDictNode *dnode, void *data)
{
    dnode->data = data;
}

int MeDictNodeIsInADict(MeDictNode *dnode)
{
    return (dnode->parent && dnode->left && dnode->right);
}

void MeDictProcess(MeDict *dict, void *context, MeDictProcessFn function)
{
    MeDictNode *node = MeDictFirst(dict), *next;

    while (node != NULL) {
	/* check for callback function deleting	*/
	/* the next node from under us		*/
	MEASSERT(MeDictContains(dict, node));
	next = MeDictNext(dict, node);
	function(dict, node, context);
	node = next;
    }
}

static void load_begin_internal(MeDictLoad *load, MeDict *dict)
{
    load->dictptr = dict;
    load->nilnode.left = &load->nilnode;
    load->nilnode.right = &load->nilnode;
}

void MeDictLoadBegin(MeDictLoad *load, MeDict *dict)
{
    MEASSERT(MeDictIsEmpty(dict));
    load_begin_internal(load, dict);
}

void MeDictLoadNext(MeDictLoad *load, MeDictNode *newnode, const void *key)
{
    MeDict *dict = load->dictptr;
    MeDictNode *nil = &load->nilnode;
   
    MEASSERT(!MeDictNodeIsInADict(newnode));
    MEASSERT(dict->nodecount < dict->maxcount);

    #ifndef NDEBUG
    if (dict->nodecount > 0) {
	    if (dict->dupes)
        {
	        MEASSERT(dict->compare(nil->left->key, key) <= 0);
        }
	    else
        {
	        MEASSERT(dict->compare(nil->left->key, key) < 0);
        }
    }
    #endif

    newnode->key = key;
    nil->right->left = newnode;
    nil->right = newnode;
    newnode->left = nil;
    dict->nodecount++;
}

void MeDictLoadEnd(MeDictLoad *load)
{
    MeDict *dict = load->dictptr;
    MeDictNode *tree[DICT_DEPTH_MAX] = { 0 };
    MeDictNode *curr, *dictnil = dict_nil(dict), *loadnil = &load->nilnode, *next;
    MeDictNode *complete = 0;
    unsigned long fullcount = DICTCOUNT_T_MAX, nodecount = dict->nodecount;
    unsigned long botrowcount;
    unsigned baselevel = 0, level = 0, i;

    MEASSERT(kMeDictColorRed == 0 && kMeDictColorBlack == 1);

    while (fullcount >= nodecount && fullcount)
	fullcount >>= 1;

    botrowcount = nodecount - fullcount;

    for (curr = loadnil->left; curr != loadnil; curr = next) {
	next = curr->left;

	if (complete == NULL && botrowcount-- == 0) {
	    MEASSERT(baselevel == 0);
	    MEASSERT(level == 0);
	    baselevel = level = 1;
	    complete = tree[0];

	    if (complete != 0) {
		tree[0] = 0;
		complete->right = dictnil;
		while (tree[level] != 0) {
		    tree[level]->right = complete;
		    complete->parent = tree[level];
		    complete = tree[level];
		    tree[level++] = 0;
		}
	    }
	}

	if (complete == NULL) {
	    curr->left = dictnil;
	    curr->right = dictnil;
	    curr->color = level % 2;
	    complete = curr;

	    MEASSERT(level == baselevel);
	    while (tree[level] != 0) {
		tree[level]->right = complete;
		complete->parent = tree[level];
		complete = tree[level];
		tree[level++] = 0;
	    }
	} else {
	    curr->left = complete;
	    curr->color = (level + 1) % 2;
	    complete->parent = curr;
	    tree[level] = curr;
	    complete = 0;
	    level = baselevel;
	}
    }

    if (complete == NULL)
	complete = dictnil;

    for (i = 0; i < DICT_DEPTH_MAX; i++) {
	if (tree[i] != 0) {
	    tree[i]->right = complete;
	    complete->parent = tree[i];
	    complete = tree[i];
	}
    }

    dictnil->color = kMeDictColorBlack;
    dictnil->right = dictnil;
    complete->parent = dictnil;
    complete->color = kMeDictColorBlack;
    dict_root(dict) = complete;

    MEASSERT(MeDictVerify(dict));
}

void MeDictMerge(MeDict *dest, MeDict *source)
{
    MeDictLoad load;
    MeDictNode *leftnode = MeDictFirst(dest), *rightnode = MeDictFirst(source);

    MEASSERT(MeDictSimilar(dest, source));	

    if (source == dest)
	return;

    dest->nodecount = 0;
    load_begin_internal(&load, dest);

    for (;;) {
	if (leftnode != NULL && rightnode != NULL) {
	    if (dest->compare(leftnode->key, rightnode->key) < 0)
		goto copyleft;
	    else
		goto copyright;
	} else if (leftnode != NULL) {
	    goto copyleft;
	} else if (rightnode != NULL) {
	    goto copyright;
	} else {
	    MEASSERT(leftnode == NULL && rightnode == NULL);
	    break;
	}

    copyleft:
	{
	    MeDictNode *next = MeDictNext(dest, leftnode);
	    #ifndef NDEBUG
	    leftnode->left = NULL;	/* suppress assertion in MeDictLoadNext */
	    #endif
	    MeDictLoadNext(&load, leftnode, leftnode->key);
	    leftnode = next;
	    continue;
	}
	
    copyright:
	{
	    MeDictNode *next = MeDictNext(source, rightnode);
	    #ifndef NDEBUG
	    rightnode->left = NULL;
	    #endif
	    MeDictLoadNext(&load, rightnode, rightnode->key);
	    rightnode = next;
	    continue;
	}
    }

    MeDictClear(source);
    MeDictLoadEnd(&load);
}

#ifdef KAZLIB_TEST_MAIN

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

typedef char input_t[256];

static int tokenize(char *string, ...)
{
    char **tokptr; 
    va_list arglist;
    int tokcount = 0;

    va_start(arglist, string);
    tokptr = va_arg(arglist, char **);
    while (tokptr) {
	while (*string && isspace((unsigned char) *string))
	    string++;
	if (!*string)
	    break;
	*tokptr = string;
	while (*string && !isspace((unsigned char) *string))
	    string++;
	tokptr = va_arg(arglist, char **);
	tokcount++;
	if (!*string)
	    break;
	*string++ = 0;
    }
    va_end(arglist);

    return tokcount;
}

static int comparef(const void *key1, const void *key2)
{
    return strcmp(key1, key2);
}

static char *dupstring(char *str)
{
    int sz = strlen(str) + 1;
    char *new = MeMemoryAPI.create(sz);
    if (new)
	memcpy(new, str, sz);
    return new;
}

static MeDictNode *new_node(void *c)
{
    static MeDictNode few[5];
    static int count;

    if (count < 5)
	return few + count++;

    return NULL;
}

static void del_node(MeDictNode *n, void *c)
{
}

static int prompt = 0;

static void construct(MeDict *d)
{
    input_t in;
    int done = 0;
    MeDictLoad dl;
    MeDictNode *dn;
    char *tok1, *tok2, *val;
    const char *key;
    char *help = 
	"p                      turn prompt on\n"
	"q                      finish construction\n"
	"a <key> <val>          add new entry\n";

    if (!MeDictIsEmpty(d))
	puts("warning: dictionary not empty!");

    MeDictLoadBegin(&dl, d);

    while (!done) {
	if (prompt)
	    putchar('>');
	fflush(stdout);

	if (!fgets(in, sizeof(input_t), stdin))
	    break;

	switch (in[0]) {
	    case '?':
		puts(help);
		break;
	    case 'p':
		prompt = 1;
		break;
	    case 'q':
		done = 1;
		break;
	    case 'a':
		if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
		    puts("what?");
		    break;
		}
		key = dupstring(tok1);
		val = dupstring(tok2);
		dn = MeDictNodeCreate(val);

		if (!key || !val || !dn) {
		    puts("out of memory");
		    MeMemoryAPI.destroy((void *) key);
		    MeMemoryAPI.destroy(val);
		    if (dn)
			MeDictNodeDestroy(dn);
		}

		MeDictLoadNext(&dl, dn, key);
		break;
	    default:
		putchar('?');
		putchar('\n');
		break;
	}
    }

    MeDictLoadEnd(&dl);
}

int main(void)
{
    input_t in;
    MeDict darray[10];
    MeDict *d = &darray[0];
    MeDictNode *dn;
    int i;
    char *tok1, *tok2, *val;
    const char *key;

    char *help =
	"a <key> <val>          add value to dictionary\n"
	"d <key>                delete value from dictionary\n"
	"l <key>                lookup value in dictionary\n"
	"( <key>                lookup lower bound\n"
	") <key>                lookup upper bound\n"
	"# <num>                switch to alternate dictionary (0-9)\n"
	"j <num> <num>          merge two dictionaries\n"
	"f                      free the whole dictionary\n"
	"k                      allow duplicate keys\n"
	"c                      show number of entries\n"
	"t                      dump whole dictionary in sort order\n"
	"m                      make dictionary out of sorted items\n"
	"p                      turn prompt on\n"
	"s                      switch to non-functioning allocator\n"
	"q                      quit";

    for (i = 0; i < sizeof darray / sizeof *darray; i++)
	MeDictInit(&darray[i], DICTCOUNT_T_MAX, comparef);

    for (;;) {
	if (prompt)
	    putchar('>');
	fflush(stdout);

	if (!fgets(in, sizeof(input_t), stdin))
	    break;

	switch(in[0]) {
	    case '?':
		puts(help);
		break;
	    case 'a':
		if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
		    puts("what?");
		    break;
		}
		key = dupstring(tok1);
		val = dupstring(tok2);

		if (!key || !val) {
		    puts("out of memory");
		    MeMemoryAPI.destroy((void *) key);
		    MeMemoryAPI.destroy(val);
		}

		if (!MeDictAllocInsert(d, key, val)) {
		    puts("MeDictAllocInsert failed");
		    MeMemoryAPI.destroy((void *) key);
		    MeMemoryAPI.destroy(val);
		    break;
		}
		break;
	    case 'd':
		if (tokenize(in+1, &tok1, (char **) 0) != 1) {
		    puts("what?");
		    break;
		}
		dn = MeDictLookup(d, tok1);
		if (!dn) {
		    puts("MeDictLookup failed");
		    break;
		}
		val = MeDictNodeGet(dn);
		key = MeDictNodeGetkey(dn);
		MeDictDeleteFree(d, dn);

		MeMemoryAPI.destroy(val);
		MeMemoryAPI.destroy((void *) key);
		break;
	    case 'f':
		MeDictFree(d);
		break;
	    case 'l':
	    case '(':
	    case ')':
		if (tokenize(in+1, &tok1, (char **) 0) != 1) {
		    puts("what?");
		    break;
		}
		dn = 0;
		switch (in[0]) {
		case 'l':
		    dn = MeDictLookup(d, tok1);
		    break;
		case '(':
		    dn = MeDictLowerBound(d, tok1);
		    break;
		case ')':
		    dn = MeDictUpperBound(d, tok1);
		    break;
		}
		if (!dn) {
		    puts("lookup failed");
		    break;
		}
		val = MeDictNodeGet(dn);
		puts(val);
		break;
	    case 'm':
		construct(d);
		break;
	    case 'k':
		MeDictAllowDupes(d);
		break;
	    case 'c':
		printf("%lu\n", (unsigned long) MeDictCount(d));
		break;
	    case 't':
		for (dn = MeDictFirst(d); dn; dn = MeDictNext(d, dn)) {
		    printf("%s\t%s\n", (char *) MeDictNodeGetkey(dn),
			    (char *) MeDictNodeGet(dn));
		}
		break;
	    case 'q':
		exit(0);
		break;
	    case '\0':
		break;
	    case 'p':
		prompt = 1;
		break;
	    case 's':
		MeDictSetAllocator(d, new_node, del_node, NULL);
		break;
	    case '#':
		if (tokenize(in+1, &tok1, (char **) 0) != 1) {
		    puts("what?");
		    break;
		} else {
		    int dictnum = atoi(tok1);
		    if (dictnum < 0 || dictnum > 9) {
			puts("invalid number");
			break;
		    }
		    d = &darray[dictnum];
		}
		break;
	    case 'j':
		if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
		    puts("what?");
		    break;
		} else {
		    int dict1 = atoi(tok1), dict2 = atoi(tok2);
		    if (dict1 < 0 || dict1 > 9 || dict2 < 0 || dict2 > 9) {
			puts("invalid number");
			break;
		    }
		    MeDictMerge(&darray[dict1], &darray[dict2]);
		}
		break;
	    default:
		putchar('?');
		putchar('\n');
		break;
	}
    }

    return 0;
}

#endif
