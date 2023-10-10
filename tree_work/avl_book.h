/*avl_book.h
数据结构书中的方法
*/
#include "public_avl.h"
/*
单次左旋转,比书本有所改变,返回值由函数返回值返回
RR类型单次旋转适用
本旋转函数不调整balance factor
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
单次右旋转,比书本有所改变,返回值由函数返回值返回
LL类型单次旋转适用
本旋转函数不调整balance factor
*/
AVLTree *right_rotate_mod_book(AVLTree *root)
{
	AVLTree *new_root=root->lchild;
	AVLTree *temp_node=new_root->rchild;
	root->lchild=temp_node;
	new_root->rchild=root;
	return new_root;
}

//左平衡处理,包含LL和LR
AVLTree *left_balance(AVLTree *T)
{
	AVLTree *lc=T->lchild;//T的左孩子结点
	switch(lc->balance_factor)//插入结点后且更新的平衡因子
	{
		case 1://LL
			printf("%zu(prev:%d), Engage LL.\n",T->data,T->balance_factor);
			T=right_rotate(T);
			T->balance_factor=0;
			T->lchild->balance_factor=0;
			break;
		case -1://LR
			AVLTree * rd=lc->rchild;//T的左孩子的右孩子结点
			//先调整平衡,再进行旋转
			printf("\nEnter LR.%zu(%d)\n",rd->data,rd->balance_factor);
			switch(rd->balance_factor)
			{	
				case 1://在CL插入
					printf("%zu(prev:%d), Engage LR(Cl).\n",T->data,T->balance_factor);
					T->balance_factor=-1;
					lc->balance_factor=0;
					break;
				case -1://在Cr插入
					printf("%zu(prev:%d), Engage LR(Cr).\n",T->data,T->balance_factor);
					T->balance_factor=0;
					lc->balance_factor=1;
					break;					
				case 0://rd是插入的结点.LR建立C结点时.
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

//右平衡处理,包含RR和RL
AVLTree *right_balance(AVLTree *T)
{
	AVLTree *rc=T->rchild;//T的右孩子结点
	switch(rc->balance_factor)//插入结点后且更新的平衡因子
	{
		case -1://RR
			printf("%zu(prev:%d), Engage RR.\n",T->data,T->balance_factor);
			T=left_rotate(T);
			T->balance_factor=0;
			rc->balance_factor=0;
			break;
		case 1://RL
			AVLTree *ld=rc->lchild;////T的右孩子的左孩子结点
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
				case 0://RL旋转中中建立C结点时
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

/*教科书上使用递归插入AVL树,并调整结点.
返回值:
AVLTree指针,一个结构体
参数:
taller长高标志,1,长高;0,未长高.由参数返回.
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
		{//向左子树插入
			T->lchild=insert_avl_node_recu_book(T->lchild,key,taller);//先插入
			if(T->lchild==NULL) 
				return NULL;
			//再调整平衡
			if(taller)//it does insert a new node in left substree
			{
				switch(T->balance_factor)
				{
					case 1://插入前,左子树高;插入后,需要左平衡处理,树不再长高
						T=left_balance(T);//具体是LL or LR
						taller=false;//平衡之后,树不再长高
						break;
					case 0://插入前,左右等高;插入后,左子树长高
						T->balance_factor=1;
						taller=true;
						break;
					case -1://插入前,右子树高;插入后,左右等高
						T->balance_factor=0;
						taller=false;
						break;
				}
			}
		}//end of go left
		else if(key>T->data)//go right
		{//向右子树插入
			T->rchild=insert_avl_node_recu_book(T->rchild,key,taller);//先插入
			if(T->rchild==NULL)
				return NULL;
			//再调整平衡
			if(taller)//it does insert a new node in right substee
			{
				switch(T->balance_factor)
				{
					case 1://插入前,左子树高;插入后,左右等高
						T->balance_factor=0;
						taller=false;
						break;
					case 0://插入前,左右等高;插入后,右子树高,树长高
						T->balance_factor=-1;
						taller=true;
						break;
					case -1://插入前,右子树高;插入后,需要右平衡处理,树不再长高
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