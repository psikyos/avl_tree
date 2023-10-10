/*avl tree iter without height
��������,�����ƽ��͸��������.
 �������ϸ��¸����ƽ������.
���ݹ���ָ��balance_factor,��ʹ�����߸���ƽ������.��Ҫtaller��־*/
#include "public_avl.h"

/*
��������ת,RR�������
������ת��ɺ���½��new_root
*/
AVLTree *left_rotate_only(AVLTree *root)
{
	AVLTree *new_root=root->rchild;
	AVLTree *temp_node=new_root->lchild;
	root->rchild=temp_node;
	new_root->lchild=root;
	return new_root;
}

/*
��������ת,LL�������
������ת��ɺ���½��new_root
*/
AVLTree *right_rotate_only(AVLTree *root)
{
	AVLTree *new_root=root->lchild;
	AVLTree *temp_node=new_root->rchild;
	root->lchild=temp_node;
	new_root->rchild=root;
	return new_root;
}

/*����ƽ��.post_insertion action 1.
���:mubst,��С��ƽ�������ĸ�.
key,�������ɾ����key.
����ֵ:
mubst,����ƽ�����С��ƽ������.��������.
*/
AVLTree* adjust_balance(AVLTree *mubst,size_t key)
{
	//adjust unbalance to balance
	if(mubst->balance_factor>1&& mubst->lchild!=NULL &&key<mubst->lchild->data)//LL case,right rotate
	{
		printf("Engaging LL type.\n");
		//�ȵ���bf,������ת
		mubst->balance_factor=0;
		mubst->lchild->balance_factor=0;
		mubst=right_rotate_only(mubst);
	}
	else if(mubst->balance_factor<-1&&mubst->rchild!=NULL&&key>mubst->rchild->data)//RR case, left rotate
	{
		printf("Engaging RR type.\n");//key=%zu,mubst->rchild's data=%zu.\n",key,mubst->rchild->data);
		mubst->balance_factor=0;
		mubst->rchild->balance_factor=0;
		mubst=left_rotate_only(mubst);
	}
	else if(mubst->balance_factor>1&&mubst->lchild!=NULL&&
		(key<mubst->data&&key>mubst->lchild->data))//LR case.
	{
		printf("Engaging LR type.\n");
		AVLTree *rd=mubst->lchild->rchild;
		if(key==rd->data)//��һ�δ���rd���,����5678,50,60#
		{
			mubst->balance_factor=0;
			mubst->lchild->balance_factor=0;
		}
		else if(key<rd->data)//���뵽Cl
		{
			mubst->balance_factor=-1;
			mubst->lchild->balance_factor=0;
		}
		else if(key>rd->data)//���뵽Cr
		{
			mubst->balance_factor=0;
			mubst->lchild->balance_factor=1;
		}
		rd->balance_factor=0;
		mubst->lchild=left_rotate_only(mubst->lchild);
		mubst=right_rotate_only(mubst);
	}
	else if(mubst->balance_factor<-1 && mubst->rchild!=NULL &&
		(key>mubst->data&&key<mubst->rchild->data))	//RL case.
	{
		printf("Engaging RL type.\n");
		AVLTree *ld=mubst->rchild->lchild;//��ƽ��������������������
		if(key==ld->data)//�״δ���ld���,����50,100,90#
		{
			mubst->balance_factor=0;
			mubst->rchild->balance_factor=0;
		}
		else if(key<ld->data)//���뵽Cl
		{
			mubst->balance_factor=0;
			mubst->rchild->balance_factor=-1;
		}
		else if(key>ld->data)//���뵽Cr
		{
			mubst->balance_factor=1;
			mubst->rchild->balance_factor=0;
		}
		ld->balance_factor=0;
//		printf("surprise equal RL:%zu:%zu.\n",key,ld->data);
		mubst->rchild=right_rotate_only(mubst->rchild);
		mubst=left_rotate_only(mubst);
	}
	return mubst;
}

//�����������С��ƽ�������ĸ�,�ӵ��游�����.post insertion action 2.
AVLTree* junction(AVLTree *mubst,size_t key,std::stack<AVLTree *> &node_stack)
{
	if(!node_stack.empty())
	{
		AVLTree *grand_parent=node_stack.top();
		if(key<grand_parent->data)//go left
				grand_parent->lchild=mubst;
		else// go right
				grand_parent->rchild=mubst;
		return NULL;
	}
	else	//root is mubst
		return mubst;
}
/*udpate parent node's balance factor
����ֵ:
1.parent, from function return value.
2.taller, from function reference.
3.need_balance,from function reference.
*/
AVLTree* update_bf_based_on_rule(AVLTree* parent,size_t key,int &taller,int &need_balance)
{
	if(taller==1)//���߲ſ��ܴ�����ת
	{//�����ж��м������Ӧ������taller=0			
		if(key<parent->data)//go left
		{
			switch(parent->balance_factor)
			{
				case 1://����ǰ,��߱��ұ߸�,��Ҫƽ��.LR or LR
//						parent->balance_factor++;//����bf==2�Ĵ���
					//mubst=adjust_balance(mubst,key,taller);
					need_balance=1;//��Ҫadjust_balance����
					taller=0;
					break;
				case -1://����ǰ,�ұ߱���߸�;�����,�ȸ�.�������.
//						parent->balance_factor++;//Ϊ0
					taller=0;
					break;
			}
			parent->balance_factor++;
		}
		else if(key>parent->data)//go right
		{
			switch(parent->balance_factor)
			{
				case 1://����ǰ,��߱��ұ߸�,�����,�ȸ�.�������.
//						parent->balance_factor--;//Ϊ0
					taller=0;
					break;					
				case -1://����ǰ,�ұ߱���߸�,��Ҫƽ��.RR or RL
//						parent->balance_factor--;//����bf==-2�Ĵ���
					//mubst=adjust_balance(mubst,key,taller);
					need_balance=1;//��Ҫadjust_balance����
					taller=0;						
					break;
			}
			parent->balance_factor--;
		}
	}//end of if(taller==1)
	return parent;
}

AVLTree* post_insertion(AVLTree *T,size_t key,std::stack<AVLTree *> &node_stack,int &taller)
{
	AVLTree *mubst=T;//assuming parent is mubst
	int need_balance=0;//0�������;1��Ҫadjust_balance����
	while(!node_stack.empty())//���ݲ���·��,���������
	{
		mubst=node_stack.top();//ȡջ�������˳�.//����parent��mubst.assuming parent is mubst.min un-banlanced sub tree.
		node_stack.pop();
		mubst=update_bf_based_on_rule(mubst,key,taller,need_balance);//���¸�����bf	
		if(need_balance)//rotate if necessary.���ݸ��º�ĸ����bf�����Ƿ���ת,��ת����Ҫ���ݹ���ָ����parentΪ���ĸ����bf.
		{
//			printf("%zu's balance factor:%d.\n",mubst->data,mubst->balance_factor);//���û������,�˾䲻�������
			mubst=adjust_balance(mubst,key);//bf����1����-1�ĲŻ������ת.�����taller=0,���ٳ���.
			need_balance=0;
		}
		//rotate complete.adjust the grand parent .Update the parent reference after rotations if it has changed
		AVLTree *temp_node=junction(mubst,key,node_stack);//if root is mubst,it has no grand-parent,returns
		if(temp_node!=NULL)//root is mubst
		{
			mubst=temp_node;
			return mubst;//�������ʱ�������ﷵ��
		}
	}//end of while(!node_stack.empty())
	return T;//��ֻ��1����2�����ʱ,��ִ�е���
}

/*ʹ�û��ڹ���ķ�������avl node,����
�����ǲ������������ĸ�.
�����ڵı���:taller,0,��δ����;1������,������Ҫ�Ľ�һ��ƽ�⴦��,����ƽ������ȷ��.������������Ϊ���ñ���
*/
AVLTree *insert_avl_node_with_rule(AVLTree *T,size_t key)
{
	int taller=0;
	std::stack<AVLTree *>node_stack;//record the node which key is passed by
	AVLTree *p=T,*parent=NULL;//parent is the parent of p. p is current node.
	while(1)
	{
		if(p==NULL)
		{
			p=(AVLTree *)malloc(sizeof(AVLTree));
			if(NULL==p)
				return NULL;
			p->data=key;//place here
			p->lchild=p->rchild=NULL;
			p->balance_factor=0;
			taller=1;//����
			//ȷ���ý��ĸ����
			if(parent==NULL)
				T=p;//p�Ǹ����
			else if(key < parent->data)//key is less than parent->data
				parent->lchild=p;
			else if(key > parent->data)//key is greater than praent->data
				parent->rchild=p;
			else//(EQ(key,parent->data))
			{//should not to do this
				printf("should no to do this line.");
				break;
			}
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
	}//end of while

	AVLTree *mubst=post_insertion(T,key,node_stack,taller);//����֮�������,����ƽ��,�����游���͸����
	return mubst;
}
