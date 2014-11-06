/*   Author           :boris shlafman
     date last modifie:1.8.10
     creation date    :25.7.10
     brief            :Associative container that allow  multiple keys,
     with equal values,on Inserting a single new element to the container
     all container elements sorted following the criterion specified by treeIsLess
     func supplyed by client Each element is inserted in its respective position
     following this ordering.
*/
#ifndef __BTREE__
#define __BTREE__
/***************************** Tree *******************************************/
/*Is a fd for the container*/
typedef struct   BTree_s Tree;

/***************************** TreeIter ***************************************/
/*Iterator is any object that, pointing to some element in a range of elements*/
typedef struct   TreesIter_t*  TreeIter;

/*************************  treeIsLess compare function ***********************/
/* Client must supply compare func that return 1 if first element key is less
 * then second for example :
 * int cmpfunc(void* _first,void* _second)
 * {
 *     return !((person*)_first->age=>(person*)_second->age)
 * }
 */
typedef int (*treeIsLess)(void*,void*);

/*  --------------------------- Public Interface ---------------------------  */

/*****************************    TreeNew    **********************************/
/* on success return fd for the container,On faiure return Null pointer
 */
Tree*       TreeNew(treeIsLess);

/*****************************    TreeDel    **********************************/
/* remove all elements from the container and free the container.
 * operates at O(n).
 */

void        TreeDel(Tree* _tree);

/*****************************    TreeInsert    *******************************/
/* Return iterator to the newly inserted element.
 * On failure iterator to container end wil be return ,
 * The element past the biggest key  of the container that is illigal.
 * operates at O(n)
 */
TreeIter    TreeInsert(Tree*,void* _data);

/*****************************    TreeRemove    *******************************/
/* Removes element from the container and return iterator to next element
 * Notice that after remove operation all iterator are invalid except returned
 * One
 */
TreeIter    TreeRemove(Tree* _tree,TreeIter _it);

/*****************************    TreeFind    *********************************/
/*
 * Return iterator to element wich key first found ,
 * On failure return End iterator ,operates at O(log(n)).
 */
TreeIter    TreeFind(Tree*,void* _key);

/*****************************    TreeBegin    ********************************/
/* Returns an iterator referring to the element with the lowest key value
 * in the container.
 */
TreeIter    TreeBegin(Tree*);

/*****************************    TreeEnd      ********************************/
/*Returns an iterator to the element past the end of the container,
 *The element past the biggest element key  of the container,
 *It is illigal iterator.
*/
TreeIter    TreeEnd(Tree*);

/*****************************    TreePrev     ********************************/
/* Returns an iterator referring to the element with less key value .
 *  On failure return  end iterator, operates at O(1).
 */
TreeIter    TreePrev(Tree*  _tree,TreeIter  _it);

/*****************************    TreeNext     ********************************/
/* Returns an iterator referring to the element with bigger key value .
 * Operates at O(1).
 */
TreeIter    TreeNext(Tree*  _tree,TreeIter  _it);

/*****************************    TreeGetData  ********************************/
/* Returns an element,with no iterator validation checkout
*/
void*       TreeGetData(TreeIter);

/*****************************    TreeIsEmpty  ********************************/
int         TreeIsEmpty(Tree*);
/*Returns 1 if the  container is empty,else 0
*/

/*****************************    TreeIsEqual  ********************************/
int         TreeIsEqual(Tree*,TreeIter  _first,TreeIter  _second);
/*Returns 1 if input iterators are equal,else 0
*/

#endif /*__BTREE__*/
