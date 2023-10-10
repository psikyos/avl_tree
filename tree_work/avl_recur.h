/*avl_recursive.h
使用递归创建AVL树
*/
#include "rotate.h"

AVLTree * insert_avl_node(AVLTree *T,size_t key)//插入后,使用树高,且平衡的结点.树高决定平衡因子
{
	if(T==NULL)
	{
		//new a node
		T=(AVLTree *)malloc(sizeof(AVLTree));
		T->data=key;
		T->lchild=NULL;
		T->rchild=NULL;
		T->balance_factor=0;//it should be balanced
		return T;
	}
	else if(key<T->data)//go left
	{
//		printf("key%zu is less than T->data:%zu.\n",key,T->data);
		T->lchild=insert_avl_node(T->lchild,key);
	}
	else if(key>T->data)
	{
//		printf("key%zu is greater than T->data:%zu.\n",key,T->data);
		T->rchild=insert_avl_node(T->rchild,key);
	}
	else // key equals T->data
	{
//		printf("They equal to %zu.\n",key);
		return T;
	}
	//complete insert action.
	//calc balance factor of node T
	size_t left_height=height_of_tree_iter(T->lchild);
	size_t right_height=height_of_tree_iter(T->rchild);
	T->balance_factor=left_height-right_height;

	//adjust unbalance to balance
	if(T->balance_factor>1&& T->lchild!=NULL &&key<T->lchild->data)//LL case,right rotate
		T=right_rotate(T);
	else if(T->balance_factor<-1&&T->rchild!=NULL&&key>T->rchild->data)//RR case, left rotate
		T=left_rotate(T);
	else if(T->balance_factor>1&&T->lchild!=NULL&&
		(key<T->data&&key>T->lchild->data))//LR case.
	{
		T->lchild=left_rotate(T->lchild);
		T=right_rotate(T);
	}
	else if(T->balance_factor<-1 && T->rchild!=NULL &&
		(key>T->data&&key<T->rchild->data))	//RL case.
	{
		T->rchild=right_rotate(T->rchild);
		T=left_rotate(T);
	}
	return T;
}