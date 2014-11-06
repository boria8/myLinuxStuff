#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "BTree.h"

#define DEADBEAF (BTNode*)0xdeadbeef

/******************************************************************************/
/**************************structs declarations********************************/

/*                  struct definition of binary tree  node                    */

typedef struct BTnode
{
	void*   data;
	struct  BTnode *parent;
	struct  BTnode *small;
	struct  BTnode *big;
}BTNode;

/*                  struct definition of binary tree                          */

struct  BTree_s
{
    BTNode       *root;
    BTNode       *begin;
    BTNode       end;/*is stub that stares in parent biggest key */
    treeIsLess   less;/*compare*/
};

/******************************************************************************/
/************************static declarations***********************************/
/*mode for static TreeFindInsertService func*/
typedef enum Look
{
    FIND   = 0,
    INSERT = 1
}LookMod;

/*child return type in TreeGetChildType and TreeConnectChildToParent func*/
typedef enum _ChildType
{
    SMALL  = 0,
    BIG    = 1,
    ORPHAN = 2
}ChildType;

/*************************** TreeNodeCreate ************************************/
/*Creates node and initalize it */
static BTNode*    TreeNodeCreate(void* _data);

/*************************** TreeNodeCreate ************************************/
/*return 1 if first argument is bigger the second,else 0 */
static int        TreeKeyIsBigger(Tree* _tree,void* _dataA,void* _dataB);

/*************************** TreeNodeCreate ************************************/
/*return 1 if keys are equal,else 0*/
static int        TreeKeysEqual(Tree* _tree,void* _dataA,void* _dataB);

/*************************** TreeFindInsertService ****************************/
/* if mod is INSERT func return placment place for node.
 * if mod is FIND   func return node thas is equal to _node .
 */
static BTNode*    TreeFindInsertService(Tree* _tree,BTNode* _root,BTNode* _node,LookMod _mod);

/*************************** TreeFree *****************************************/
/*recursive func that frees all node and initialize them to DEADBEAF*/
static void       TreeFree(BTNode* _root);

/*************************** TreeGetChildNumber *******************************/
/*return node child number 0,1,2*/
static int        TreeGetChildNumber(BTNode* _node);

/*************************** TreeGetChildType *********************************/
/*return what kind of child is node for its parent :BIG,SMALL,ORPHANT*/
static ChildType  TreeGetChildType(BTNode* _node);

/*************************** TreeDestroyNode **********************************/
/*free node and in debug mode itialize it to deadbeaf*/
static void       TreeDestroyNode(BTNode* _node);

/*************************** TreeConnectChildToParent *************************/
/* in childmode SMALL connect child to parent small node.
 * In childmode BIG   connect child to parent big  node.
 * In childmode ORPHAN  switch child with  parent .
 */
static void       TreeConnectChildToParent(Tree* _tree,BTNode* _parent,BTNode* _child,ChildType _childType);

/*************************** TreeConnectChildToParent *************************/
/*return first parent wich child is _child ..small big*/
static TreeIter  TreeReturnChildParent(Tree* _tree,BTNode* _node,ChildType _child);


/******************************************************************************/
/************************public interface implementation***********************/


Tree*       TreeNew(treeIsLess _less)
{
    Tree   *tree = malloc(sizeof(Tree));

    assert(_less);

    if(!tree)
 	{
  	    return 0;
 	}

 	tree->root  = 0;
 	tree->begin = &tree->end;
 	tree->end.parent=0;
 	tree->less=_less;

 	return tree;
}

TreeIter    TreeInsert(Tree* _tree,void* _data)
{
    BTNode* node;

    assert(_tree);

    node = TreeNodeCreate(_data);

    if(!node)
    {
        fprintf(stderr,"cannot allocat node");
        return (TreeIter)(&_tree->end);
    }
    /*insert the node to right place and return its parent*/
    node->parent = TreeFindInsertService(_tree,_tree->root,node,INSERT);

    if (node->parent)
    {
        if (!(*_tree->less)(_tree->begin->data,node->data))/*update begin */
        {
            _tree->begin = node;
        }

        if (TreeKeyIsBigger(_tree,node->data,_tree->end.parent->data))/*update end */
        {
            _tree->end.parent = node;
        }
    }
    else/*we are inserting root*/
    {
        _tree->root=node;
        _tree->begin = node ;
        _tree->end.parent = node ;

    }

    return (TreeIter)node;
}

TreeIter    TreeFind(Tree* _tree,void* _key)
{
    BTNode* keyNode;
    BTNode* node;

    assert(_tree);

    keyNode = TreeNodeCreate(_key);
    /*    return found node*/
    node = TreeFindInsertService(_tree,_tree->root,keyNode,FIND);

    if (TreeKeysEqual(_tree,_key,node->data))
    {
        free(keyNode);

        return (TreeIter)node;
    }

    free(keyNode);

    return (TreeIter)&(_tree->end);

}

void*       TreeGetData(TreeIter _it)
{
    assert(_it);

    return ((BTNode*)_it)->data;
}

TreeIter    TreeEnd(Tree* _tree)
{
   assert(_tree);

   return (TreeIter)&_tree->end;
}

TreeIter    TreeBegin(Tree* _tree)
{
   assert(_tree);

   return (TreeIter)_tree->begin ;
}

TreeIter    TreeNext(Tree*  _tree,TreeIter  _it)
{
    BTNode* node ;

    assert(_tree || _it);

    node = (BTNode*)_it;

    assert( !(TreeIsEmpty(_tree)) ||  !(_it == TreeEnd(_tree)) );

    if (node->big)  /* if there exists big child */
    {
        node = node->big;

        while(node->small)
        {
            node = node->small;
        }

        return  (TreeIter)node;
    }

    return  TreeReturnChildParent(_tree,node,SMALL);
}

TreeIter    TreePrev(Tree*  _tree,TreeIter  _it)
{
    BTNode* node = (BTNode*)_it;

    assert(_tree);
    assert(_it);
    assert( !(TreeIsEmpty(_tree)) ||  !(_it == TreeBegin(_tree)) );

    if (node->small)  /* if there exists small child */
    {
        node = node->small;

        while(node->big)
        {
            node = node->big;
        }

        return  (TreeIter)node;
    }

    return  TreeReturnChildParent(_tree,node,BIG);
}

int  TreeIsEqual(Tree*  _tree,TreeIter  _first,TreeIter  _second)
{
    assert(_tree||_first||_second);

    return (_first==_second);
}

TreeIter    TreeRemove(Tree* _tree,TreeIter _it)
{
    BTNode*  node = (BTNode*)_it;
    BTNode*  temp = 0;
    TreeIter Return = 0;
    ChildType child = 0;

    assert(_tree);
    assert(node);
    assert( !(TreeIsEmpty(_tree)) ||  !(TreeEnd(_tree)==_it) );

    if(_tree->begin == node )
    {
        _tree->begin =(BTNode*)TreeNext(_tree,_it);
    }

    if( _tree->end.parent == node  )
    {
        _tree->end.parent = (BTNode*)TreePrev(_tree,_it);
    }

    Return = TreeNext(_tree,_it);
    child  = TreeGetChildType(node);

    switch (TreeGetChildNumber(node))
    {
        case 0:
        {
            if (child==SMALL)
            {
                node->parent->small = 0;
            }
            else if (child==BIG)
            {
                node->parent->big = 0;
            }
            else
            {
                _tree->root = 0;
            }

            break;
        }

        case 1:
        {
            if(node->small)
            {
                TreeConnectChildToParent(_tree,node->parent,node->small,child);
            }
            else
            {
                TreeConnectChildToParent(_tree,node->parent,node->big  ,child);
            }

            break;
        }

        case   2:
        {
            TreeConnectChildToParent(_tree,node->parent,node->small,child);

            temp=node->small;

            while(temp->big)
            {
                temp=temp->big;
            }

            TreeConnectChildToParent(_tree,temp,node->big,BIG);

            break;
        }
    };

    TreeDestroyNode(node);

    return Return;
}

int TreeIsEmpty(Tree* _tree)
{
    assert(_tree);

    return (TreeBegin(_tree) == TreeEnd(_tree) );
}

void        TreeDel(Tree* _tree)
{
   assert(_tree);

   TreeFree(_tree->root);

   free(_tree);
}


/******************************************************************************/
/**********************static function implementation**************************/

static int      TreeKeyIsBigger(Tree* _tree,void* _dataA,void* _dataB)
{
    if (!(*_tree->less)(_dataA,_dataB) && !TreeKeysEqual(_tree,_dataA,_dataB))
    {
        return 1;
    }

    return 0;
}

static int      TreeKeysEqual(Tree* _tree,void* _dataA,void* _dataB)
{
    if ( !(*_tree->less)(_dataA,_dataB) && !(*_tree->less)(_dataB,_dataA) )
    {
        return 1;
    }

    return 0;
}

static BTNode*  TreeNodeCreate(void* _data)
{
    BTNode        *node=malloc(sizeof(BTNode));

    if (!node)
    {
        return 0;
    }

    node->data=_data;
	node->small = 0;
	node->big = 0;
	node->parent=0;

    return node;
}

static BTNode*  TreeFindInsertService(Tree* _tree,BTNode* _root,BTNode* _node,LookMod _mod)
{
    if (_root)
    {
        if ((*_tree->less)(_node->data,_root->data))
        {
            if(!_root->small)
            {
                if (_mod==INSERT) _root->small = _node;

                return _root;
            }
            else
            {
                _root=TreeFindInsertService(_tree,_root->small,_node,_mod);
            }
        }
        else if(TreeKeyIsBigger( _tree,_node->data,_root->data))
        {
            if(!_root->big)
            {
               if (_mod==INSERT) _root->big = _node;

                return _root;
            }
            else
            {
                _root=TreeFindInsertService(_tree,_root->big,_node,_mod);
            }
        }
        else   if(_mod==FIND)
        {
            return _root;
        }
    }

    return _root;
}

static void    TreeFree(BTNode* _root)
{
    if (_root)
    {
       TreeFree(_root->small);
       TreeFree(_root->big);
       TreeDestroyNode(_root);
    }
}

static int TreeGetChildNumber(BTNode* _node)
{
    if(!_node->small && !_node->big)
    {
        return 0;
    }

    if((_node->small && _node->big))
    {
        return 2;
    }

    return 1;
}

static  ChildType  TreeGetChildType(BTNode* _node)
{
    if(_node->parent)
    {
        if (_node->parent->small==_node)
        {
            return  SMALL;
        }

        return  BIG;
    }

    return  ORPHAN;
}

static  void TreeDestroyNode(BTNode* _node)
{
    free(_node);

    #ifdef _DEBUG
        _node=(BTNode*)0xdeadbeef;
    #else
        _node=0;
    #endif
}

static void TreeConnectChildToParent(Tree* _tree,BTNode* _parent,BTNode* _child,ChildType _childType)
{
    if(_childType==SMALL)
    {
        _parent->small = _child;
        _child->parent = _parent;
        return;
    }

    if(_childType==BIG)
    {
        _parent->big   =  _child;
        _child->parent =  _parent;
        return;
    }

    _tree->root = _child;
    _child->parent = 0;
}

static TreeIter  TreeReturnChildParent(Tree* _tree,BTNode* _node,ChildType _child)
{
    while(TreeGetChildType(_node)!=ORPHAN)   /*  no existens of big  child */
    {
        if (TreeGetChildType(_node) == _child)
        {
            return (TreeIter)_node->parent;
        }

        _node = _node->parent;
    }

    return  TreeEnd(_tree);
}

#ifdef utestTree
#define IS_SUCCESS(err,str) if (err) {printf("\n success with ...%s test \n",str);}\
else{printf("\n fail with...%s  test",str);}
#define Fail_pattern(err,str) if (err) {printf("\n fail pattern is ...%s \n ",str); return 0;}
#include<string.h>
#include<math.h>
#include<time.h>

void  Regresion_test(void);
void  AddInOrder(BTNode* _root,int* _arr,int *_index);
int   TreeNewInsertTest();
int   ChekSortArr(int *_arr,int _elements);
int*  MakeRandomArr(int _digits);
void  RandArray(int  *_arr,int _elements);
int   TreeIterChek(void);
int   TreeRemoveTest(void);
int   IsLess(void* _dataA,void* _dataB);
void  PrintInOrder(BTNode* _root);

int main()
{

   Regresion_test();

   return 0;
}


int    TreeNewInsertTest()
{
    Tree* tree=TreeNew(IsLess);
    int arr[1000];
    int i=0;
    TreeIter it;

    int *array=MakeRandomArr(3);/*2 digits mean 100 elemnts*/

    for (i=0;i<1000;i++)
    {
       it=TreeInsert(tree,(void*)*(array+i));
       Fail_pattern((int)TreeGetData(it)!=*(array+i),"taking iter value");
    }

    i=0;

    AddInOrder(tree->root,arr,&i);

    Fail_pattern(!ChekSortArr(arr,1000),"tree logic");

    for(i=0;i<1000;i++)
    {
        Fail_pattern((int)TreeGetData(TreeFind(tree,(void*)(*(array+i))))!=*(array+i),"tree find");
    }

    free(array);

    TreeDel(tree);

    return 1;
}

int TreeIterChek(void)
{
    Tree* tree=TreeNew(IsLess);
    int i=0;
    TreeIter it;

    int *array=MakeRandomArr(3);/*2 digits mean 100 elemnts*/

    for (i=0;i<1000;i++)
    {
       it=TreeInsert(tree,(void*)*(array+i));
/*       printf("--%d--",*(array+i));*/

       Fail_pattern((int)TreeGetData(it)!=*(array+i),"taking iter value");
    }

    it=TreeBegin(tree);

    for(i=1;i<1000;i++)
    {
        Fail_pattern((int)TreeGetData(it)!=i,"TreeNext operation");
        it=TreeNext(tree,it);
    }


    for(i=1000;i>1;i--)
    {
        Fail_pattern((int)TreeGetData(it)!=i,"TreePrev operation");
        it=TreePrev(tree,it);
    }




    return 1;

}

int  TreeRemoveTest(void)
{
    Tree* tree=TreeNew(IsLess);
    int i=0;
    TreeIter it;

    int *array=MakeRandomArr(3);/*2 digits mean 100 elemnts*/

    for (i=0;i<1000;i++)
    {
        it=TreeInsert(tree,(void*)*(array+i));
    }

    it=TreeBegin(tree);

    while (it!=TreeEnd(tree))
    {

        it=TreeRemove(tree,it);

        if (it==TreeEnd(tree))
        {
            break;
        }
/*         printf("\n");
 *         PrintInOrder(tree->root);
 */
    }

    Fail_pattern(!TreeIsEmpty(tree),"did not remove all nodes");

    return 1;
}

void    Regresion_test(void)
{
    IS_SUCCESS(TreeNewInsertTest(),    "TreeNew TreeInsert TreeGetData TreeFind");
    IS_SUCCESS(TreeIterChek(),  "TreeNext TreePrev TreeBegin TreeEnd ");
    IS_SUCCESS(TreeRemoveTest(),  "TreeRemove TreeIsEmpty  ");
}


int        IsLess(void* _dataA,void* _dataB)
{
    if ((int)_dataA >= (int)_dataB)
    {
        return 0;
    }

    return 1;
}

void   AddInOrder(BTNode* _root,int* _arr,int *_index)
{
   if (_root)
   {
       AddInOrder(_root->small,_arr,_index);
/*       printf("%d \n",(int)_root->data);*/
       *(_arr+*_index)=(int)_root->data;
       ++(*_index);
       AddInOrder(_root->big,_arr,_index);
   }
}

void  PrintInOrder(BTNode* _root)
{

   if (_root)
   {
       PrintInOrder(_root->small);
       printf("--%d--",(int)_root->data);
       PrintInOrder(_root->big);
   }
}

int*  MakeRandomArr(int _digits)
{

    int     elements,*arr;

    elements=pow(10,_digits);
    arr=malloc(sizeof(int)*elements);

    RandArray(arr,elements);

    return arr;
}

int  ChekSortArr(int *_arr,int _elements)
{
    int      i=1;

    for(;i<_elements;i++)
    {
        if(_arr[i]<_arr[i-1])
        {
            return 0;
        }
    }

    return 1;
}

void         RandArray(int  *_arr,int _elements)
{
     int        *arr=_arr,value2,value1,i,temp;

     srand(time(NULL));

     for(i=1;i<_elements+1;i++,arr++)
     {
         *arr=i;
     }

     for(i=0;i<_elements;i++)
     {
         value1=rand()%_elements;
         value2=rand()%_elements;
         if (value1!=value2)
         {
             temp=*(_arr+value1);
             *(_arr+value1)=*(_arr+value2);
             *(_arr+value2)=temp;
         }
     }
}

#endif/*utestTree*/

























