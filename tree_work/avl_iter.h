/*avl tree insert iter without height
��������,�����ƽ��͸��������.
 �������ϸ��¸����ƽ������.
���ݹ���ָ��balance_factor,��ʹ�����߸���ƽ������.��Ҫtaller��־
20241022: try to use need_balance value 1,2,3,4 to discriminate the rotate type.
1,LL; 2,LR; 3,RR; 4,RL.
*/

#include "public_avl.h"
#include "rotate.h"

/*����ƽ��.post_insertion action 1.
A is mubst. 
B is A'child which is heavier,
C is B's child which is heavier.
���:mubst,��С��ƽ�������ĸ�.
key,�������ɾ����key.
need_balance,��Ҫƽ��.1,LL; 2,LR; 3,RR; 4,RL.
����ֵ:
mubst,����ƽ�����С��ƽ������.��������.
*/
AVLTree* rotate_and_balance(AVLTree *mubst,size_t key,int need_balance)
{
	//adjust unbalance to balance
	//if(mubst->balance_factor>1&& mubst->lchild!=NULL &&key<mubst->lchild->data)//LL case,right rotate
	if(need_balance==1)
	{
		printf("Engaging LL type.\n");
		//�ȵ���bf,������ת
		mubst->balance_factor=0;
		mubst->lchild->balance_factor=0;
		mubst=right_rotate_only(mubst);
	}
	//else if(mubst->balance_factor<-1&&mubst->rchild!=NULL&&key>mubst->rchild->data)//RR case, left rotate
	else if(need_balance==3)
	{
		printf("Engaging RR type.\n");//key=%zu,mubst->rchild's data=%zu.\n",key,mubst->rchild->data);
		mubst->balance_factor=0;
		mubst->rchild->balance_factor=0;
		mubst=left_rotate_only(mubst);
	}
	//else if(mubst->balance_factor>1&&mubst->lchild!=NULL&&	(key<mubst->data&&key>mubst->lchild->data))//LR case.
	else if(need_balance==2)
	{
		printf("Engaging LR type.\n");
		AVLTree *rd=mubst->lchild->rchild;
		if(key==rd->data)//��һ�δ���rd���,����5678,50,60#
		{
			mubst->balance_factor=0;//A
			mubst->lchild->balance_factor=0;//B
		}
		else if(key<rd->data)//���뵽Cl
		{
			mubst->balance_factor=-1;//A
			mubst->lchild->balance_factor=0;//B
		}
		else if(key>rd->data)//���뵽Cr
		{
			mubst->balance_factor=0;//A
			mubst->lchild->balance_factor=1;//B
		}
		rd->balance_factor=0;//C
		mubst->lchild=left_rotate_only(mubst->lchild);
		mubst=right_rotate_only(mubst);
	}
	//else if(mubst->balance_factor<-1 && mubst->rchild!=NULL &&	(key>mubst->data&&key<mubst->rchild->data))	//RL case.
	else if(need_balance==4)
	{
		printf("Engaging RL type.\n");
		AVLTree *ld=mubst->rchild->lchild;//��ƽ��������������������
		if(key==ld->data)//�״δ���ld���,����50,100,90#
		{
			mubst->balance_factor=0;//A
			mubst->rchild->balance_factor=0;//B
		}
		else if(key<ld->data)//���뵽Cl
		{
			mubst->balance_factor=0;//A
			mubst->rchild->balance_factor=-1;//B
		}
		else if(key>ld->data)//���뵽Cr
		{
			mubst->balance_factor=1;//A
			mubst->rchild->balance_factor=0;//B
		}
		ld->balance_factor=0;//C
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

/*udpate node_p node's balance factor
Note: taller will be updated in this function.
return value:
1.node_p, from function return value.
2.taller, from function reference.
3.need_balance,from function reference,default value=0.
*/
AVLTree* update_bf_based_on_rule(AVLTree* node_p,size_t key,int &taller,int &need_balance)
{
	if(taller==1)//���߲ſ��ܴ�����ת
	{
		if(key<node_p->data)//go left
		{
			switch(node_p->balance_factor)
			{
				case 1://����ǰ,��߱��ұ߸�,��Ҫƽ��.LL or LR
//						node_p->balance_factor++;//����bf==2�Ĵ���
					if( node_p->lchild!=NULL &&key<node_p->lchild->data)//LL case,right rotate
					{
						need_balance=1;
					}
					if(node_p->lchild!=NULL&&
						(key<node_p->data&&key>node_p->lchild->data))//LR case.
					{
						need_balance=2;
					}
					//need_balance=1;//need rotate function
					taller=0;
					break;
				case -1://����ǰ,�ұ߱���߸�;�����,�ȸ�.absorbed.�������.
//						node_p->balance_factor++;//Ϊ0
					need_balance=0;
					taller=0;
					break;
				case 0://����ǰ,�������;�����,�󳤸�,��������ת,20241024				
					need_balance=0;
					taller=1;
					break;
			}
			node_p->balance_factor++;
		}
		else if(key>node_p->data)//go right
		{
			switch(node_p->balance_factor)
			{
				case 1://����ǰ,��߱��ұ߸�,�����,�ȸ�.absorbed.�������.
//						node_p->balance_factor--;//Ϊ0
					need_balance=0;
					taller=0;
					break;					
				case -1://����ǰ,�ұ߱���߸�,��Ҫƽ��.RR or RL
//						node_p->balance_factor--;//����bf==-2�Ĵ���
					if(node_p->rchild!=NULL && key>node_p->rchild->data)//RR case, left rotate
					{
						need_balance=3;
					}
					if(  node_p->rchild!=NULL &&
							(key>node_p->data&&key<node_p->rchild->data))	//RL case.
					{
						need_balance=4;
					}
					//need_balance=1;//need rotate function
					taller=0;						
					break;
				case 0://����ǰ,�������;�����,�ҳ���,��������ת.20241024
					need_balance=0;					
					taller=1;
					break;
			}
			node_p->balance_factor--;
		}
	}//end of if(taller==1)
	return node_p;
}

AVLTree* post_insertion(AVLTree *T,size_t key,std::stack<AVLTree *> &node_stack,int &taller)
{
	AVLTree *mubst=T;//assuming parent is mubst
	int need_balance=0;//0�������;1,2,3,4,rotate type.
	while(!node_stack.empty())//���ݲ���·��,���������
	{
		mubst=node_stack.top();//ȡջ�������˳�.//����parent��mubst.assuming parent is mubst.min un-banlanced sub tree.
		node_stack.pop();
		mubst=update_bf_based_on_rule(mubst,key,taller,need_balance);//���¸�����bf	
		if(need_balance>0)//rotate if necessary.���ݸ��º�ĸ����bf�����Ƿ���ת,��ת����Ҫ���ݹ���ָ����parentΪ���ĸ����bf.
		{
//			printf("%zu's balance factor:%d.\n",mubst->data,mubst->balance_factor);//���û������,�˾䲻�������
			mubst=rotate_and_balance(mubst,key,need_balance);//bf����1����-1�ĲŻ������ת.�����taller=0,���ٳ���.
			need_balance=0;	//taller=0;//have been balanced at this step.but update in update_bf_based_on_rule
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
			if(parent!=NULL)
				printf("parent's data:%zu.",parent->data);
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
		else if( key< p->data )//С��ʱ,������
		{
			node_stack.push(p);
			parent=p;
			p=p->lchild;
		}
		else if (key > p->data)//greater, go right
		{
			node_stack.push(p);
			parent=p;
			p=p->rchild;
		}
		else //if( key==p->data )//����ʱ�������
		{
			printf("Equal and DO NOT insert.\n");
			break;
		}
	}//end of while

	AVLTree *mubst=post_insertion(T,key,node_stack,taller);//����֮�������,����ƽ��,�����游���͸����
	return mubst;
}
