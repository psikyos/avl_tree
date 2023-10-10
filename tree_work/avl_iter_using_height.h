/*insert avl node iterative,balanced using tree height
使用迭代创建avl树,平衡使用树高
*/
#include <stdlib.h>
#include <stack>
#include "public_avl.h"

//因为类型为LL,RR,LR,RL型,所以最小不平衡子树在父父结点上?
//使用树高决定平衡因子
AVLTree * insert_avl_node_iter_with_height(AVLTree *T,size_t key)
{
	//原bst插入后,返回的是该树的根结点
	std::stack<AVLTree *>node_stack;//record the node which key is passed by
	AVLTree *p=T,*parent=NULL;//parent is the parent of p. p is current node.
	AVLTree *grand_parent=NULL;//grand_parent is the parent of variable parent.
	while(1)
	{
		if(p==NULL)
		{
			p=(AVLTree *)malloc(sizeof(AVLTree));
			p->data=key;//place here
			p->lchild=NULL;
			p->rchild=NULL;
			//确定该结点的父结点
			if(parent==NULL)
				T=p;//p是根结点
			else if(LT(key,parent->data))
				parent->lchild=p;
			else if(EQ(key,parent->data))
			{//should not to do this
				printf("should no to do this line.");
				//return or break have no meaning.
				//return T;//return NULL;
				break;
			}
			else//大于
				parent->rchild=p;
			break;
		}
		else if(EQ(key,p->data))//等于时无需插入
		{
			break;
		}
		else if(LT(key,p->data))//小于时,向左走
		{
			node_stack.push(p);
			parent=p;
			p=p->lchild;
		}
		else//greater, go right
		{
			node_stack.push(p);
			parent=p;
			p=p->rchild;
		}
	}
	
//	理解T,T在前面为树的根。在下面代码中，为插入的结点.是否需要1个3结点的队列或者栈
	//以上为bst action
	//now, node_stack stored the path when key insert passed by.
	while(!node_stack.empty())//check the path, update the balance factor
	{
		parent=node_stack.top();//get the top of the stack
		node_stack.pop();
		
		//calc balance factor by height.
		size_t left_height=height_of_tree_iter(parent->lchild);
		size_t right_height=height_of_tree_iter(parent->rchild);
		parent->balance_factor=left_height-right_height;
		//adjust bf complete
		
		//assuming parent is mubst
		AVLTree *mubst=parent;//min un-banlanced sub tree
		//rotate if necessary
		printf("%zu's balance factor:%d.\n",mubst->data,mubst->balance_factor);
		//adjust unbalance to balance
		if(mubst->balance_factor>1&& mubst->lchild!=NULL &&key<mubst->lchild->data)//LL case,right rotate
		{
			printf("Engaging LL type.\n");
			mubst=right_rotate(mubst);
		}
		else if(mubst->balance_factor<-1&&mubst->rchild!=NULL&&key>mubst->rchild->data)//RR case, left rotate
		{
			printf("Engaging RR type.\n");
			mubst=left_rotate(mubst);
		}
		else if(mubst->balance_factor>1&&mubst->lchild!=NULL&&
			(key<mubst->data&&key>mubst->lchild->data))//LR case.
		{
			printf("Engaging LR type.\n");
			mubst->lchild=left_rotate(mubst->lchild);
			mubst=right_rotate(mubst);
		}
		else if(mubst->balance_factor<-1 && mubst->rchild!=NULL &&
			(key>mubst->data&&key<mubst->rchild->data))	//RL case.
		{
			printf("Engaging RL type.\n");
			mubst->rchild=right_rotate(mubst->rchild);
			mubst=left_rotate(mubst);
		}

		//rotate complete.adjust the grand parent 
		//Update the parent reference after rotations if it has changed
		if(!node_stack.empty())
		{
			grand_parent=node_stack.top();
			if(key<grand_parent->data)//go left
			{
				//if(grand_parent->lchild!=mubst)
					grand_parent->lchild=mubst;
			}
			else// go right
			{
				//if(grand_parent->rchild!=mubst)
					grand_parent->rchild=mubst;
			}
		}
		else//root is mubst
			return mubst;
	}//end of while(!node_stack.empty())

	return T;

}