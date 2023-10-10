/*avl tree iter without height
迭代插入,并完成平衡和父结点连接.
 从下往上更新各结点平衡因子.
依据规则指定balance_factor,不使用树高更新平衡因子.需要taller标志*/
#include "public_avl.h"

/*
单次左旋转,RR情况适用
返回旋转完成后的新结点new_root
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
单次右旋转,LL情况适用
返回旋转完成后的新结点new_root
*/
AVLTree *right_rotate_only(AVLTree *root)
{
	AVLTree *new_root=root->lchild;
	AVLTree *temp_node=new_root->rchild;
	root->lchild=temp_node;
	new_root->rchild=root;
	return new_root;
}

/*调整平衡.post_insertion action 1.
入参:mubst,最小不平衡子树的根.
key,插入或者删除的key.
返回值:
mubst,调整平衡的最小不平衡子树.函数返回.
*/
AVLTree* adjust_balance(AVLTree *mubst,size_t key)
{
	//adjust unbalance to balance
	if(mubst->balance_factor>1&& mubst->lchild!=NULL &&key<mubst->lchild->data)//LL case,right rotate
	{
		printf("Engaging LL type.\n");
		//先调整bf,再做旋转
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
		if(key==rd->data)//第一次创建rd结点,比如5678,50,60#
		{
			mubst->balance_factor=0;
			mubst->lchild->balance_factor=0;
		}
		else if(key<rd->data)//插入到Cl
		{
			mubst->balance_factor=-1;
			mubst->lchild->balance_factor=0;
		}
		else if(key>rd->data)//插入到Cr
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
		AVLTree *ld=mubst->rchild->lchild;//不平衡子树的右子树的左孩子
		if(key==ld->data)//首次创建ld结点,比如50,100,90#
		{
			mubst->balance_factor=0;
			mubst->rchild->balance_factor=0;
		}
		else if(key<ld->data)//插入到Cl
		{
			mubst->balance_factor=0;
			mubst->rchild->balance_factor=-1;
		}
		else if(key>ld->data)//插入到Cr
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

//将调整完的最小不平衡子树的根,接到祖父结点上.post insertion action 2.
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
返回值:
1.parent, from function return value.
2.taller, from function reference.
3.need_balance,from function reference.
*/
AVLTree* update_bf_based_on_rule(AVLTree* parent,size_t key,int &taller,int &need_balance)
{
	if(taller==1)//长高才可能触发旋转
	{//这里判断有几种情况应该设置taller=0			
		if(key<parent->data)//go left
		{
			switch(parent->balance_factor)
			{
				case 1://插入前,左边比右边高,需要平衡.LR or LR
//						parent->balance_factor++;//允许bf==2的存在
					//mubst=adjust_balance(mubst,key,taller);
					need_balance=1;//需要adjust_balance函数
					taller=0;
					break;
				case -1://插入前,右边比左边高;插入后,等高.无需调整.
//						parent->balance_factor++;//为0
					taller=0;
					break;
			}
			parent->balance_factor++;
		}
		else if(key>parent->data)//go right
		{
			switch(parent->balance_factor)
			{
				case 1://插入前,左边比右边高,插入后,等高.无需调整.
//						parent->balance_factor--;//为0
					taller=0;
					break;					
				case -1://插入前,右边比左边高,需要平衡.RR or RL
//						parent->balance_factor--;//允许bf==-2的存在
					//mubst=adjust_balance(mubst,key,taller);
					need_balance=1;//需要adjust_balance函数
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
	int need_balance=0;//0无需调整;1需要adjust_balance函数
	while(!node_stack.empty())//根据插入路径,遍历父结点
	{
		mubst=node_stack.top();//取栈顶而不退出.//假设parent是mubst.assuming parent is mubst.min un-banlanced sub tree.
		node_stack.pop();
		mubst=update_bf_based_on_rule(mubst,key,taller,need_balance);//更新父结点的bf	
		if(need_balance)//rotate if necessary.根据更新后的父结点bf决定是否旋转,旋转后需要根据规则指定以parent为根的各结点bf.
		{
//			printf("%zu's balance factor:%d.\n",mubst->data,mubst->balance_factor);//如果没有问题,此句不参与调试
			mubst=adjust_balance(mubst,key);//bf超出1或者-1的才会进行旋转.处理后taller=0,不再长高.
			need_balance=0;
		}
		//rotate complete.adjust the grand parent .Update the parent reference after rotations if it has changed
		AVLTree *temp_node=junction(mubst,key,node_stack);//if root is mubst,it has no grand-parent,returns
		if(temp_node!=NULL)//root is mubst
		{
			mubst=temp_node;
			return mubst;//绝大多数时候都在这里返回
		}
	}//end of while(!node_stack.empty())
	return T;//当只有1个或2个结点时,会执行到此
}

/*使用基于规则的方法插入avl node,迭代
返回是插入后的整个树的根.
函数内的变量:taller,0,树未长高;1树长高,可能需要的进一步平衡处理,根据平衡因子确定.在其他函数内为引用变量
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
			taller=1;//长高
			//确定该结点的父结点
			if(parent==NULL)
				T=p;//p是根结点
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
	}//end of while

	AVLTree *mubst=post_insertion(T,key,node_stack,taller);//插入之后的事务,调整平衡,连接祖父结点和父结点
	return mubst;
}
