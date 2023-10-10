/*avl_book.h
���ݽṹ���еķ���
*/
#include "public_avl.h"
/*
��������ת,���鱾�����ı�,����ֵ�ɺ�������ֵ����
RR���͵�����ת����
����ת����������balance factor
*/
AVLTree *left_rotate_mod_book(AVLTree *root)
{
	AVLTree *new_root=root->rchild;
	AVLTree *temp_node=new_root->lchild;
	root->rchild=temp_node;
	new_root->lchild=root;
	return new_root;
} 

/*
��������ת,���鱾�����ı�,����ֵ�ɺ�������ֵ����
LL���͵�����ת����
����ת����������balance factor
*/
AVLTree *right_rotate_mod_book(AVLTree *root)
{
	AVLTree *new_root=root->lchild;
	AVLTree *temp_node=new_root->rchild;
	root->lchild=temp_node;
	new_root->rchild=root;
	return new_root;
}

//��ƽ�⴦��,����LL��LR
AVLTree *left_balance(AVLTree *T)
{
	AVLTree *lc=T->lchild;//T�����ӽ��
	switch(lc->balance_factor)//��������Ҹ��µ�ƽ������
	{
		case 1://LL
			printf("%zu(prev:%d), Engage LL.\n",T->data,T->balance_factor);
			T=right_rotate(T);
			T->balance_factor=0;
			T->lchild->balance_factor=0;
			break;
		case -1://LR
			AVLTree * rd=lc->rchild;//T�����ӵ��Һ��ӽ��
			//�ȵ���ƽ��,�ٽ�����ת
			printf("\nEnter LR.%zu(%d)\n",rd->data,rd->balance_factor);
			switch(rd->balance_factor)
			{	
				case 1://��CL����
					printf("%zu(prev:%d), Engage LR(Cl).\n",T->data,T->balance_factor);
					T->balance_factor=-1;
					lc->balance_factor=0;
					break;
				case -1://��Cr����
					printf("%zu(prev:%d), Engage LR(Cr).\n",T->data,T->balance_factor);
					T->balance_factor=0;
					lc->balance_factor=1;
					break;					
				case 0://rd�ǲ���Ľ��.LR����C���ʱ.
					printf("\n\nLR,building C=%zu(%d)\n\n",T->data,T->balance_factor);
					T->balance_factor=0;
					lc->balance_factor=0;
					break;
			}
			rd->balance_factor=0;
			T->lchild=left_rotate_mod_book(T->lchild);
			T=right_rotate_mod_book(T);
			break;
	}//end of switch lc->balance_factor
	return T;
}

//��ƽ�⴦��,����RR��RL
AVLTree *right_balance(AVLTree *T)
{
	AVLTree *rc=T->rchild;//T���Һ��ӽ��
	switch(rc->balance_factor)//��������Ҹ��µ�ƽ������
	{
		case -1://RR
			printf("%zu(prev:%d), Engage RR.\n",T->data,T->balance_factor);
			T=left_rotate(T);
			T->balance_factor=0;
			rc->balance_factor=0;
			break;
		case 1://RL
			AVLTree *ld=rc->lchild;////T���Һ��ӵ����ӽ��
			switch(ld->balance_factor)
			{
				case 1://CL
					printf("%zu(prev:%d), Engage RL(Cl).\n",T->data,T->balance_factor);
					T->balance_factor=0;
					rc->balance_factor=-1;
					break;
				case -1://CR
					printf("%zu(prev:%d), Engage RL(Cr).\n",T->data,T->balance_factor);
					T->balance_factor=1;
					rc->balance_factor=0;
					break;
				case 0://RL��ת���н���C���ʱ
					printf("\n\nRL,building C=%zu(%d)\n\n",T->data,T->balance_factor);
					T->balance_factor=0;
					rc->balance_factor=0;
					break;
			}
			ld->balance_factor=0;
			T->rchild=right_rotate_mod_book(T->rchild);
			T=left_rotate_mod_book(T);
			break;
	}//end of switch(rc->balance_factor)
	return T;
}

/*�̿�����ʹ�õݹ����AVL��,���������.
����ֵ:
AVLTreeָ��,һ���ṹ��
����:
taller���߱�־,1,����;0,δ����.�ɲ�������.
*/
AVLTree * insert_avl_node_recu_book(AVLTree *T,size_t key,int &taller)
{
	if(T==NULL)//insert a new node
	{
		T=(AVLTree *)malloc(sizeof(AVLTree));
		T->data=key;
		T->lchild=T->rchild=NULL;		T->balance_factor=0;
		taller=true;
		return T;
	}
	else
	{
		if(key<T->data)//go left
		{//������������
			T->lchild=insert_avl_node_recu_book(T->lchild,key,taller);//�Ȳ���
			if(T->lchild==NULL) 
				return NULL;
			//�ٵ���ƽ��
			if(taller)//it does insert a new node in left substree
			{
				switch(T->balance_factor)
				{
					case 1://����ǰ,��������;�����,��Ҫ��ƽ�⴦��,�����ٳ���
						T=left_balance(T);//������LL or LR
						taller=false;//ƽ��֮��,�����ٳ���
						break;
					case 0://����ǰ,���ҵȸ�;�����,����������
						T->balance_factor=1;
						taller=true;
						break;
					case -1://����ǰ,��������;�����,���ҵȸ�
						T->balance_factor=0;
						taller=false;
						break;
				}
			}
		}//end of go left
		else if(key>T->data)//go right
		{//������������
			T->rchild=insert_avl_node_recu_book(T->rchild,key,taller);//�Ȳ���
			if(T->rchild==NULL)
				return NULL;
			//�ٵ���ƽ��
			if(taller)//it does insert a new node in right substee
			{
				switch(T->balance_factor)
				{
					case 1://����ǰ,��������;�����,���ҵȸ�
						T->balance_factor=0;
						taller=false;
						break;
					case 0://����ǰ,���ҵȸ�;�����,��������,������
						T->balance_factor=-1;
						taller=true;
						break;
					case -1://����ǰ,��������;�����,��Ҫ��ƽ�⴦��,�����ٳ���
						T=right_balance(T);//RR or RL
						taller=false;
						break;
				}
			}
		}//end of go right
		else//key equals to T node
		{
			taller=false;
			return T;
		}
	}
	return T;
}