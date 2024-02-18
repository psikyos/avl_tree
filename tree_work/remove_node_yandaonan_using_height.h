/*
avl tree remove node.
研道难5.14
https://www.geeksforgeeks.org/deletion-in-an-avl-tree/
https://dl.acm.org/doi/10.1145/800197.806043
https://en.wikipedia.org/wiki/AVL_tree#Comparison_to_other_structures
https://dl.acm.org/doi/pdf/10.1145/355609.362340
https://zhjwpku.com/assets/pdf/AED2-10-avl-paper.pdf
https://www.mathnet.ru/php/archive.phtml?wshow=paper&jrnid=dan&paperid=26964&option_lang=eng
删除后,需要一直溯源到根结点,完成平衡。
remove node which has doule children,update its bf.
*/
#include "public_avl.h"
#include "height_cnt.h"
#include "rotate.h"
#include <cassert>

#ifndef REMOVE_NODE_YANDAONAN_USING_HEIGHT
#define REMOVE_NODE_YANDAONAN_USING_HEIGHT
//the following define the type of AVL tree. to combine 4 case:left left,left right,right right,right left.
#define TYPE_LEFT 1
#define TYPE_RIGHT 2
/*在avl树中搜索,返回找到的结点.递归方法.研道难的思路,先找到需要删除的结点,再进行处理.
同时需要记录下经过的父结点
返回值:
1.找到的结点T.若未找到,返回NULL,函数的返回值;
2.结点T的父结点parent.若T为root,则parent为NULL.
入参:
注意parent是引用.
*/
AVLTree *search_node(AVLTree *T,size_t key,AVLTree *&parent,std::stack<AVLTree *> &node_stack)
{
	if(T!=NULL)
	{
		if(key<T->data)//go left
		{
			parent=T;
			node_stack.push(parent);
			return search_node(T->lchild,key,parent,node_stack);
		}
		else if(key>T->data)//go right
		{
			parent=T;
			node_stack.push(parent);
			return search_node(T->rchild,key,parent,node_stack);
		}
		else//equal,return this node
		{
			return T;
		}
	}
	else
		return NULL;
}

/*find the inorder predecessor of node_p
找到结点p的中序前驱,即左子树的最小值,一定在p的左子树的最右下结点,且值在p的左子树中最小.
假设结点p不为空.此函数只涉及p的左子树.
返回值:
1.node_p的中序前驱,函数返回值.
2.node_p的中序前驱的父结点,从入参返回.
*/
AVLTree* max_value_node(AVLTree *p_lchildtree,AVLTree *&parent_ipd)
{
	AVLTree* foregoer=p_lchildtree;
	while(foregoer->rchild!=NULL)
	{//看前驱是否需要父结点
		parent_ipd=foregoer;
		foregoer=foregoer->rchild;
	}
	return foregoer;
}

/*find the inorder successor of node_p
找到结点p的中序后继,即右子树最小值,一定在p的右子树的最左下的结点,且值在p的右子树中最小.
假设p结点不为空.
返回值:
1.node_p的中序后继,函数返回值.
2.node_p的中序后继的父结点,从入参返回.
*/
AVLTree* min_value_node(AVLTree *p_rchildtree,AVLTree *&parent_ios)
{
	AVLTree* follow_up=p_rchildtree;
	while(follow_up->lchild!=NULL)//here must judge follow_up's lchild in case follow_up get NULL
	{	
		parent_ios=follow_up;
		follow_up=follow_up->lchild;
	}
	return follow_up;
}

/*assuming node_p has 2 children
using inorder successor(中序后继,ios for short)来替代node_p
return value:
1.the root of searched tree, from function return value
*/
AVLTree* process_double_children(AVLTree *root,AVLTree *node_p,std::stack<AVLTree *> &node_stack,int replace_method_inorder)
{
	AVLTree *parent_unified=NULL;//for calc the height
	if(replace_method_inorder==IOS)//中序后继
	{
		AVLTree *parent_ios=node_p;//中序后继的父结点可能是node_p
		AVLTree *ios=min_value_node(node_p->rchild,parent_ios);//node_p的中序后继.中序后继一定没有左子树.ios=inorder successor
		parent_unified=parent_ios;//for calc the height
		if(node_p==parent_ios)
			parent_ios->rchild=ios->rchild;
		else//node_p is not ios_parent
			parent_ios->lchild=ios->rchild;
		node_p->data=ios->data;
		free(ios);
	}
	else if(replace_method_inorder==IPD)//中序前驱
	{
		AVLTree *parent_ipd=node_p;
		AVLTree *ipd=max_value_node(node_p->lchild,parent_ipd);
		parent_unified=parent_ipd;//for calc the height
		if(node_p==parent_ipd)
			parent_ipd->lchild=ipd->lchild;
		else//node_p is not parent_ipd
			parent_ipd->rchild=ipd->lchild;//need to adjust the bf of parent_ipd		
		node_p->data=ipd->data;//node_p 's height will be update during rotation
		free(ipd);
	}
	int lheight=height_of_tree(parent_unified->lchild);
	int rheight=height_of_tree(parent_unified->rchild);
	parent_unified->balance_factor=lheight-rheight;
	node_stack.push(node_p);
	return root;
}

//带父结点的移除法.废弃.deserted function
AVLTree* process_one_child2_desert(AVLTree*root,AVLTree* node_p,AVLTree* parent)
{
	AVLTree *node_p_child=(node_p->lchild!=NULL?node_p->lchild:node_p->rchild);
	if(node_p==root)//alias parent==NULL
		root=node_p_child;
	else
	{
		if(node_p==parent->lchild)
			parent->lchild->data=node_p_child->data;
		else
			parent->rchild->data=node_p_child->data;
	}
	free(node_p_child);
	return root;
}

/*不带父结点的移除法
前置条件:node_p has only one child.
return value:
1.the root node of searched tree,function return value.
2.modified node_p,from function pointer.
3.node_stack,from function reference.
*/
AVLTree* process_one_child(AVLTree*root,AVLTree* node_p,std::stack<AVLTree *> &node_stack)
{
	AVLTree *ps_child=(node_p->lchild!=NULL?node_p->lchild:node_p->rchild);
	node_p->data=ps_child->data;
	node_p->lchild=ps_child->lchild;
	node_p->rchild=ps_child->rchild;
	free(ps_child);
	node_stack.push(node_p);
	return root;
}

/*前置要求:node_p,没有子结点,即叶子结点.使用ASSERT做判断.
删除node_p,并更新node_p的父结点parent的孩子结点指向
返回值:
根结点.若node_p为根结点,则返回为空树.
*/
AVLTree* process_no_child(AVLTree *root,AVLTree *node_p,AVLTree *parent)
{
	assert(node_p->lchild==NULL&&node_p->rchild==NULL);
	if(node_p==root)//alias parent==NULL
	{
		root=NULL;
	}
	else//node_p is not root
	{
		if(node_p==parent->lchild)
			parent->lchild=NULL;
		else
			parent->rchild=NULL;
	}
	free(node_p);
	return root;
}

//更新mubst的平衡因子,并根据bf进行旋转,使用树高
AVLTree* pd_adjust_balance_using_height(AVLTree *mubst)//pd means post deletion
{
	int left_height=height_of_tree_iter(mubst->lchild);
	int right_height=height_of_tree_iter(mubst->rchild);
	int old_balance_factor=mubst->balance_factor;
	mubst->balance_factor=left_height-right_height;//judge the balance factor by height of tree
//	printf("%zu(%d->%d),\n",mubst->data,old_balance_factor,mubst->balance_factor);//输出结点值和平衡因子
	
	//left_height>right_height,should match https://www.cs.usfca.edu/~galles/visualization/AVLtree.html
	AVLTree *first_level_node=left_height>right_height?mubst->lchild:mubst->rchild;//第1级结点是mubst的子树里,树高最大(含相等)的那个
	if(first_level_node==NULL)//in case it was NULL.Maybe has not first_level_node
		return mubst;

	left_height=height_of_tree_iter(first_level_node->lchild);
	right_height=height_of_tree_iter(first_level_node->rchild);
	AVLTree *second_level_node=left_height>right_height?first_level_node->lchild:first_level_node->rchild;//第2级结点是first_level_node的子树里,树高最大的那个
	if(second_level_node==NULL)
		return mubst;

	//decide left or right. futher to decide the type.
	int type_1st=(first_level_node==mubst->lchild)?TYPE_LEFT:TYPE_RIGHT;//first level type
	int type_2nd=(second_level_node==first_level_node->lchild)?TYPE_LEFT:TYPE_RIGHT;//second level type
	//if bf<-1 or bf>1,then has the following step. 
	if(mubst->balance_factor>1)//LL or LR
	{
		if(TYPE_LEFT==type_1st&&TYPE_LEFT==type_2nd)//LL case
		{
			printf("Engaging pd_LL type.\n");
			mubst=right_rotate(mubst);//调整balance factor的函数
		}
		else if(TYPE_LEFT==type_1st&&TYPE_RIGHT==type_2nd)//LR case
		{
			printf("Engaging pd_LR type.\n");
			mubst->lchild=left_rotate(mubst->lchild);
			mubst=right_rotate(mubst);
		}
	}
	else if(mubst->balance_factor<-1)//RR or RL
	{
		if(TYPE_RIGHT==type_1st&&TYPE_RIGHT==type_2nd)//RR case
		{
			printf("Engaging pd_RR type.\n");
			mubst=left_rotate(mubst);
		}
		else if(TYPE_RIGHT==type_1st&&TYPE_LEFT==type_2nd)//RL case
		{
			printf("Engaging pd_RL type.\n");
			mubst->rchild=right_rotate(mubst->rchild);
			mubst=left_rotate(mubst);
		}
	}
	return mubst;
}

AVLTree* pd_junction(AVLTree* mubst,size_t key,std::stack<AVLTree *> &node_stack)
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

/*bst删除完成后的动作
2.遍历父路径,调整结点平衡因子.
3.连接祖父结点和父结点.
入参为删除结点删除路径上的所有父结点,调用一次本函数传入1个父结点进行检查
*/
AVLTree* post_deletion(AVLTree* T,size_t key,std::stack<AVLTree *> &node_stack)
{
	AVLTree *parent=NULL;
	while(!node_stack.empty())
	{
		parent=node_stack.top();
		node_stack.pop();
		//adjust the balance factor of node parent
		printf("%zu,",parent->data);//the passed by parent node
		parent=pd_adjust_balance_using_height(parent);
		//connect the grand-parent and parent. parent become the child of grand-parent
		AVLTree *temp_node=pd_junction(parent,key,node_stack);
		if(temp_node!=NULL)//root is parent
		{
			parent=temp_node;
			return parent;
		}
	}
	return T;
}

/*在给定的avl树中删除一个搜索到的值的结点,并调整平衡.返回处理完成的树的根
1.实现bst删除
2.遍历父路径,调整结点平衡因子.post_deletion
3.连接祖父结点和父结点.post_deletion
*/
AVLTree* remove_avl_tree_node_yandaonan(AVLTree *T,size_t key,int replace_method_inorder)
{
	/*1.找到要删除的结点;
	2.如果该结点是叶子,删除;
	3.如果有单孩子,则删除该结点后,孩子结点顶上;
	4.如果有双孩子,则删除该结点后,结点的中序后继顶上;(中序后继是被删结点右子树上最小的点)
	5.从被删除的结点开始往上查找并更新各结点bf,找到第1个不平衡结点z,根据回溯路径上各级父结点x和y情况,仍然分为4类,LL,RR,LR,RL,旋转使之平衡
	6.从z开始继续回溯,找到路径上不平衡的结点,并调整平衡,直到找到根结点,调整结束。
	*/
	AVLTree *parent=NULL;
	std::stack<AVLTree *>node_stack;//record the node which key is passed by
	AVLTree *p=search_node(T,key,parent,node_stack);
	printf("Want to search:%zu.\n",key);
	if(p!=NULL)
	{	
		printf("Found key:%zu,parent:%zu.\n",p->data,parent!=NULL?parent->data:-1);
		//find inorder succssor and remove and re-balance
		if(p->lchild==NULL&&p->rchild==NULL)//p is leaf
		{
			printf("remove leaf node.\n");
			T=process_no_child(T,p,parent);//处理结点p,返回处理后的根结点.父结点无需入stack,因为前面已经入过stack
		}
		else if(p->lchild!=NULL&&p->rchild!=NULL)//p has 2 children
		{//use inorder successor(ios) to replace. ios should be re-evaluate the balance factor
			printf("remove node which has 2 children.\n");
			T=process_double_children(T,p,node_stack,replace_method_inorder);
		}
		else//p has only one child
		{//child should be re-evaluate the balance factor
			printf("remove node which has only one child.\n");
			T=process_one_child(T,p,node_stack);
		}
		//calc the parent-trail balance factor
		printf("the node path will by(include itself):\n");
		T=post_deletion(T,key,node_stack);
		printf("\n");
	}
	else//not found
	{
		printf("Not found key:%zu.\n",key);
	}	
	return T;
}
#endif