/*
avl tree remove node.
�е���5.14
https://www.geeksforgeeks.org/deletion-in-an-avl-tree/
https://dl.acm.org/doi/10.1145/800197.806043
https://en.wikipedia.org/wiki/AVL_tree#Comparison_to_other_structures
https://dl.acm.org/doi/pdf/10.1145/355609.362340
https://zhjwpku.com/assets/pdf/AED2-10-avl-paper.pdf
https://www.mathnet.ru/php/archive.phtml?wshow=paper&jrnid=dan&paperid=26964&option_lang=eng
ɾ����,��Ҫһֱ��Դ�������,���ƽ�⡣
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
/*��avl��������,�����ҵ��Ľ��.�ݹ鷽��.�е��ѵ�˼·,���ҵ���Ҫɾ���Ľ��,�ٽ��д���.
ͬʱ��Ҫ��¼�¾����ĸ����
����ֵ:
1.�ҵ��Ľ��T.��δ�ҵ�,����NULL,�����ķ���ֵ;
2.���T�ĸ����parent.��TΪroot,��parentΪNULL.
���:
ע��parent������.
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
�ҵ����p������ǰ��,������������Сֵ,һ����p���������������½��,��ֵ��p������������С.
������p��Ϊ��.�˺���ֻ�漰p��������.
����ֵ:
1.node_p������ǰ��,��������ֵ.
2.node_p������ǰ���ĸ����,����η���.
*/
AVLTree* max_value_node(AVLTree *p_lchildtree,AVLTree *&parent_ipd)
{
	AVLTree* foregoer=p_lchildtree;
	while(foregoer->rchild!=NULL)
	{//��ǰ���Ƿ���Ҫ�����
		parent_ipd=foregoer;
		foregoer=foregoer->rchild;
	}
	return foregoer;
}

/*find the inorder successor of node_p
�ҵ����p��������,����������Сֵ,һ����p���������������µĽ��,��ֵ��p������������С.
����p��㲻Ϊ��.
����ֵ:
1.node_p��������,��������ֵ.
2.node_p�������̵ĸ����,����η���.
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
using inorder successor(������,ios for short)�����node_p
return value:
1.the root of searched tree, from function return value
*/
AVLTree* process_double_children(AVLTree *root,AVLTree *node_p,std::stack<AVLTree *> &node_stack,int replace_method_inorder)
{
	AVLTree *parent_unified=NULL;//for calc the height
	if(replace_method_inorder==IOS)//������
	{
		AVLTree *parent_ios=node_p;//�����̵ĸ���������node_p
		AVLTree *ios=min_value_node(node_p->rchild,parent_ios);//node_p��������.������һ��û��������.ios=inorder successor
		parent_unified=parent_ios;//for calc the height
		if(node_p==parent_ios)
			parent_ios->rchild=ios->rchild;
		else//node_p is not ios_parent
			parent_ios->lchild=ios->rchild;
		node_p->data=ios->data;
		free(ios);
	}
	else if(replace_method_inorder==IPD)//����ǰ��
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

//���������Ƴ���.����.deserted function
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

/*�����������Ƴ���
ǰ������:node_p has only one child.
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

/*ǰ��Ҫ��:node_p,û���ӽ��,��Ҷ�ӽ��.ʹ��ASSERT���ж�.
ɾ��node_p,������node_p�ĸ����parent�ĺ��ӽ��ָ��
����ֵ:
�����.��node_pΪ�����,�򷵻�Ϊ����.
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

//����mubst��ƽ������,������bf������ת,ʹ������
AVLTree* pd_adjust_balance_using_height(AVLTree *mubst)//pd means post deletion
{
	int left_height=height_of_tree_iter(mubst->lchild);
	int right_height=height_of_tree_iter(mubst->rchild);
	int old_balance_factor=mubst->balance_factor;
	mubst->balance_factor=left_height-right_height;//judge the balance factor by height of tree
//	printf("%zu(%d->%d),\n",mubst->data,old_balance_factor,mubst->balance_factor);//������ֵ��ƽ������
	
	//left_height>right_height,should match https://www.cs.usfca.edu/~galles/visualization/AVLtree.html
	AVLTree *first_level_node=left_height>right_height?mubst->lchild:mubst->rchild;//��1�������mubst��������,�������(�����)���Ǹ�
	if(first_level_node==NULL)//in case it was NULL.Maybe has not first_level_node
		return mubst;

	left_height=height_of_tree_iter(first_level_node->lchild);
	right_height=height_of_tree_iter(first_level_node->rchild);
	AVLTree *second_level_node=left_height>right_height?first_level_node->lchild:first_level_node->rchild;//��2�������first_level_node��������,���������Ǹ�
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
			mubst=right_rotate(mubst);//����balance factor�ĺ���
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

/*bstɾ����ɺ�Ķ���
2.������·��,�������ƽ������.
3.�����游���͸����.
���Ϊɾ�����ɾ��·���ϵ����и����,����һ�α���������1���������м��
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

/*�ڸ�����avl����ɾ��һ����������ֵ�Ľ��,������ƽ��.���ش�����ɵ����ĸ�
1.ʵ��bstɾ��
2.������·��,�������ƽ������.post_deletion
3.�����游���͸����.post_deletion
*/
AVLTree* remove_avl_tree_node_yandaonan(AVLTree *T,size_t key,int replace_method_inorder)
{
	/*1.�ҵ�Ҫɾ���Ľ��;
	2.����ý����Ҷ��,ɾ��;
	3.����е�����,��ɾ���ý���,���ӽ�㶥��;
	4.�����˫����,��ɾ���ý���,���������̶���;(�������Ǳ�ɾ�������������С�ĵ�)
	5.�ӱ�ɾ���Ľ�㿪ʼ���ϲ��Ҳ����¸����bf,�ҵ���1����ƽ����z,���ݻ���·���ϸ��������x��y���,��Ȼ��Ϊ4��,LL,RR,LR,RL,��תʹ֮ƽ��
	6.��z��ʼ��������,�ҵ�·���ϲ�ƽ��Ľ��,������ƽ��,ֱ���ҵ������,����������
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
			T=process_no_child(T,p,parent);//������p,���ش����ĸ����.�����������stack,��Ϊǰ���Ѿ����stack
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