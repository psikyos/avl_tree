/*insert avl node iterative,balanced using tree height
ʹ�õ�������avl��,ƽ��ʹ������
*/
#include <stdlib.h>
#include <stack>
#include "public_avl.h"

//��Ϊ����ΪLL,RR,LR,RL��,������С��ƽ�������ڸ��������?
//ʹ�����߾���ƽ������
AVLTree * insert_avl_node_iter_with_height(AVLTree *T,size_t key)
{
	//ԭbst�����,���ص��Ǹ����ĸ����
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
			//ȷ���ý��ĸ����
			if(parent==NULL)
				T=p;//p�Ǹ����
			else if(LT(key,parent->data))
				parent->lchild=p;
			else if(EQ(key,parent->data))
			{//should not to do this
				printf("should no to do this line.");
				//return or break have no meaning.
				//return T;//return NULL;
				break;
			}
			else//����
				parent->rchild=p;
			break;
		}
		else if(EQ(key,p->data))//����ʱ�������
		{
			break;
		}
		else if(LT(key,p->data))//С��ʱ,������
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
	
//	���T,T��ǰ��Ϊ���ĸ�������������У�Ϊ����Ľ��.�Ƿ���Ҫ1��3���Ķ��л���ջ
	//����Ϊbst action
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