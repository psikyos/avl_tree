/*
avl tree remove node.iterative,based on rule.
研道难5.14 method,but use rule not tree height.
https://www.geeksforgeeks.org/deletion-in-an-avl-tree/
https://dl.acm.org/doi/10.1145/800197.806043
https://en.wikipedia.org/wiki/AVL_tree#Comparison_to_other_structures
https://dl.acm.org/doi/pdf/10.1145/355609.362340
https://zhjwpku.com/assets/pdf/AED2-10-avl-paper.pdf
https://www.mathnet.ru/php/archive.phtml?wshow=paper&jrnid=dan&paperid=26964&option_lang=eng

*import*:Defintion of shorter, indicate the tree height got shorten and bf is 2 or -2 except root case.
if shorter is trigger, balance_factor updated will be postponed to rule judegement.
shorter=0, tree height remains unchanged; shorter=1, height got shorten.

删除后,根据shorter以及规则调整bf,并一直溯源到根结点,完成平衡。
*/
#include "public_avl.h"
#include "height_cnt.h"
#include "rotate.h"
#include <cassert>
#include "remove_node_yandaonan_using_height.h"

/*Prerequisites:node_p is a leaf.Judeged by ASSERT.
删除node_p,并更新node_p的父结点parent的孩子结点指向.
父结点已经加入stack
在此函数调整parent's bf
返回值:
1.根结点.若node_p为根结点,则返回为空树.函数返回值.
2.shorter,树高是否变短.函数引用值.触发shorter时,不更新bf
3.parent,父节点,函数指针
*/
AVLTree* shorter_leaf(AVLTree *root,AVLTree *node_p,AVLTree *parent,int &shorter)
{
	assert(node_p->lchild==NULL&&node_p->rchild==NULL);
	if(node_p==root)//alias parent==NULL
	{
		root=NULL;
		shorter=1;//height get shorten
	}
	else//node_p is not root
	{
		if(node_p==parent->lchild)
		{
			parent->lchild=NULL;
			switch(parent->balance_factor)
			{
				case -1:
					//parent->balance_factor-=1;
					shorter=1;
					break;
				case 0:
					parent->balance_factor-=1;//因为不触发shorter,此时更新
					shorter=0;
					break;
				case 1:
					parent->balance_factor-=1;
					shorter=1;//height get shorten,but not cause rotate,set to 1.
					break;				
			}
		}
		else//node_p is parent's rchild
		{
			parent->rchild=NULL;
			switch(parent->balance_factor)
			{
				case 1:
					//parent->balance_factor+=1;
					shorter=1;
					break;
				case 0:
					parent->balance_factor+=1;//因为不触发shorter,此时更新
					shorter=0;
					break;
				case -1:
					//parent has no lchild
					parent->balance_factor+=1;
					shorter=1;//height get shorten,but not cause rotate.But may be cause at grand-parent level or further.set to 1.
					break;
			}			
		}
	}
	free(node_p);
	return root;
}

/*不带父结点的移除法
Prerequisites:node_p has only one child.
return value:
1.shorter,whether the tree height got shorten, from function return value.
2.modified node_p,from function pointer.
3.node_stack,from function reference.
*/
int shorter_one_child(AVLTree* node_p,std::stack<AVLTree *> &node_stack,int shorter)
{
	assert(!(node_p->lchild!=NULL&&node_p->rchild!=NULL));//node_p has only one child
	AVLTree *ps_child=(node_p->lchild!=NULL?node_p->lchild:node_p->rchild);
	int bf_direction=(node_p->lchild!=NULL?-1:1);//do not need to update balance_factor,because shorter triggered.
	node_p->data=ps_child->data;
	node_p->lchild=ps_child->lchild;
	node_p->rchild=ps_child->rchild;
	free(ps_child);
	//**may be need to judge the bf exceed -2 or 2.
	switch(node_p->balance_factor)
	{
		case 0:
			node_p->balance_factor+=bf_direction;//it must be 1 or -1,could be update here
			shorter=0;
			break;
		case 1://judge the positive or negitive of bf_direction
			if(bf_direction<0)
			{
				node_p->balance_factor+=bf_direction;//if greater than 0,that is 2 which is no need to update bf here.			
				shorter=0;
			}
			else
				shorter=1;
			break;
		case -1:			
			if(bf_direction>0)
			{
				node_p->balance_factor+=bf_direction;
				shorter=0;
			}
			else
				shorter=1;
			break;
	}//	node_p->balance_factor+=bf_direction;
	printf("****judgegment here****%zu(%d)",node_p->data,node_p->balance_factor);
	if(shorter>0)
		node_stack.push(node_p);//only got shorten need to get in stack
//	shorter=1;
	return shorter;
}

/*
assuming node_p has 2 children
 using inorder successor(中序后继,ios for short) to replace node_p
 using inorder predecessor(中序前驱,ipd for short)to replace node_p
shorter should be judge after node_p's bf get changed
return value:
1.shoter sign, from function return value
*/
int shorter_double_children(AVLTree *node_p,std::stack<AVLTree *> &node_stack,int replace_method_inorder,int shorter)
{
	switch(replace_method_inorder)
	{
		case IOS://中序后继.在右子树中找到最小值
		{
			AVLTree *parent_ios=node_p;//中序后继的父结点可能是node_p
			AVLTree *ios=min_value_node(node_p->rchild,parent_ios);//node_p的中序后继.中序后继节点一定没有左子树.
			if(node_p==parent_ios)
			{
				parent_ios->rchild=ios->rchild;				
				switch(node_p->balance_factor)//node_p->balance_factor+=1;//left is get heavy
				{
					case 0:
						node_p->balance_factor=1;
						shorter=0;
						break;
					case -1:
						shorter=1;//bf should be 0.but do not update bf
						break;
					case 1:
						shorter=1;//bf should be 2,but not update here
						break;
				}
			}
			else//node_p is not ios's parent
			{//ios' parent bf should not be -1.ios must be ios_parent's left child
				switch(parent_ios->balance_factor)
				{
					case 0:
						parent_ios->balance_factor=-1;
						node_p->balance_factor+=0;
						break;
					case 1:
						parent_ios->balance_factor=0;
						//node_p->balance_factor+=1;
						switch(node_p->balance_factor)
						{
							case 1:
								shorter=1;//do not update bf
								break;
							case 0:
							case -1:
								node_p->balance_factor+=1;
								shorter=0;
								break;
						}
						//shorter=1;
						break;
				}
				parent_ios->lchild=ios->rchild;
			}
//			if(node_p->balance_factor>1)
//				shorter=1;
			node_p->data=ios->data;
			free(ios);
			break;
		}
		case IPD://中序前驱,在左子树中找到最大值
		{
			AVLTree *parent_ipd=node_p;
			AVLTree *ipd=max_value_node(node_p->lchild,parent_ipd);//ipd do not have rchild
			if(node_p==parent_ipd)
			{
				node_p->lchild=ipd->lchild;
				switch(node_p->balance_factor)
				{
					case 1:
						shorter=1;//got shorten. Do not update bf here
						break;
					case 0:
						shorter=0;						
						node_p->balance_factor=-1;
						break;
					case -1:
						shorter=1;//bf should be -2,but not update here
						break;
				}//node_p->balance_factor-=1;
			}
			else//node_p is not parent_ipd
			{
				switch(parent_ipd->balance_factor)//should be 0 and -1.
				{
					case 0:
						parent_ipd->balance_factor=1;
						node_p->balance_factor-=0;//unchanged
						break;
					case -1:
						parent_ipd->balance_factor=0;
						switch(node_p->balance_factor)
						{
							case -1:
								shorter=1;//got shorten. Do not update bf.
								break;
							case 0:
							case 1:
								node_p->balance_factor-=1;//未触发shorter,可更新bf
								break;
						}//node_p->balance_factor-=1;
						break;
				}
				parent_ipd->rchild=ipd->lchild;	
			}
			node_p->data=ipd->data;		
			free(ipd);
			break;
		}
	}
	if(shorter>0)
		node_stack.push(node_p);
	return shorter;
}

/*update the node balance factor based on rule,not on tree height
parameter:shorter could not be updated in the function. Because it will be used until the last parent-node.
shorter comes from successful removal action. If node was removed, shorter=1
Return value:
1.need_balance,from function reference.
2.parent, from function return value.
11,Jan,2024
*/
AVLTree* pd_update_bf_rule(AVLTree* parent,size_t key,int shorter,int &need_balance)
{
	if(shorter==1)//tree height got shorten.
	{
		if(key>parent->data)//go right to remove.So the left maybe un-balanced.
		{//LL or LR case
			switch(parent->balance_factor)//un-updated bf after remove node
			{
				case 1://original left child is greater than right.After removal, left is great further.
					if(parent->lchild!=NULL)
					{
						switch(parent->lchild->balance_factor)//y's bf
						{
							case 1://LL case
								parent->balance_factor=2;//z's bf=2
								need_balance=1;
								break;
							case 0://LL or LR case,y等重.建议按照LL处理
								parent->balance_factor=2;//z's bf=2
								need_balance=1;
								break;
							case -1://LR case
								parent->balance_factor=2;//z's bf=2
								need_balance=2;break;
						}
					}
					break;
				case 0://original left equals right
					//??should it do update bf?.Do not cause rotate, should not update bf here.But it should alter trail bf here.so do it.					
				case -1://original left is less than right
					//parent->balance_factor++;
					break;
			}
		}
		else if(key<parent->data)//go left to remove.So the right may be un-balanced.
		{//RR or RL case
			switch(parent->balance_factor)
			{
				case -1://original left child is less than right child
//				printf("execute RR or RL,but %zu(%d)",parent->data,parent->balance_factor);
					if(parent->rchild!=NULL)
					{
						switch(parent->rchild->balance_factor)
						{
							case -1://RR case
								parent->balance_factor=-2;
							case 0://RR case or RL case. Suggest to do RR case.
								parent->balance_factor=-2;
								//parent->rchild->balance_factor=-1;
								need_balance=3;break;
							case 1://RL case
								parent->balance_factor=-2;
								need_balance=4;break;
						}
					}
					break;
				case 0://original left equals to right					
				case 1://original left greater than right
					//parent->balance_factor--;//??
					break;
			}
		}
	}//end of if(shorter)
	return parent;
}

/*
Do rotate action and update bf.
need_balance to express category of rotation.
Caution:
LL&LR mixed type as to LL.
RR&RL mixed type as to RR.
Return value:
1.mubst, from function return value.
*/
AVLTree* pd_adjust_balance_using_rule(AVLTree *mubst,int need_balance)//mubst's bf has been updated when passed in this function
{
	switch(need_balance)
	{
		case 1://LL case,x remains unchanged
			if(mubst->lchild->balance_factor!=0)
			{
				mubst->balance_factor=0;//z
				mubst->lchild->balance_factor=0;//y				
			}
			else//混合类型,按LL处理
			{
				mubst->balance_factor=1;//z
				mubst->lchild->balance_factor=-1;//y
			}
			mubst=right_rotate_only(mubst);
			printf("Engage rpd_LL case.\n");//rule post delete
			break;
		case 2://LR case,most complicate.Has 4 cases.
		{
			AVLTree *ld=mubst->lchild->rchild;			
			switch(ld->balance_factor)
			{
				case 0://has 2 cases
					if(ld->lchild==NULL&&ld->rchild==NULL)//x等重无子
					{
						mubst->balance_factor=0;
						mubst->lchild->balance_factor=0;
					}
					else//x等重有子
					{
						mubst->balance_factor=1;
						mubst->lchild->balance_factor=0;
					}
					break;
				case 1:
					mubst->balance_factor=-1;
					mubst->lchild->balance_factor=0;
					break;
				case -1:
					mubst->balance_factor=0;
					mubst->lchild->balance_factor=1;
					break;
			}
			ld->balance_factor=0;
			mubst->lchild=left_rotate_only(mubst->lchild);
			mubst=right_rotate_only(mubst);
			printf("Engage rpd_LR case.\n");
			break;
		}
		case 3://RR case, x remains unchanged
			if(mubst->rchild->balance_factor!=0)
			{
				mubst->balance_factor=0;//z
				mubst->rchild->balance_factor=0;//y				
			}
			else//混合类型,按RR处理
			{
				mubst->balance_factor=-1;//z
				mubst->rchild->balance_factor=1;//y
			}
			mubst=left_rotate_only(mubst);
			printf("Engage rpd_RR case.\n");
			break;
		case 4://RL case
			AVLTree *rd=mubst->rchild->lchild;
			if(rd!=NULL)
				rd->balance_factor=0;
			if(mubst->balance_factor==-2
				&&mubst->lchild->balance_factor==1
				&&rd->balance_factor==-1)//case: z=-2&&y=1&&x=-1
				mubst->balance_factor=1;
			else
				mubst->balance_factor=0;
			mubst->rchild=right_rotate_only(mubst->rchild);
			mubst=left_rotate_only(mubst);
			printf("Engage rpd_RL case.\n");
			break;
	}//end of switch(need_balance)
	return mubst;
}

/*bst删除完成后的动作
2.遍历父路径,调整结点平衡因子.
3.连接祖父结点和父结点.
参数：
1.T,处理的树的根结点
2.node_stack,入参为删除结点删除路径上的所有父结点.
*/
AVLTree* post_deletion_rule(AVLTree* T,size_t key,std::stack<AVLTree *> &node_stack,int shorter)
{
	AVLTree *parent=NULL;
	int need_balance=0;//0,do not need balance;1,2,3,4,represent rotate type.
	while(!node_stack.empty())
	{
		parent=node_stack.top();
		node_stack.pop();
		printf("%zu,",parent->data);//the passed by parent node
		parent=pd_update_bf_rule(parent,key,shorter,need_balance);//assign the bf of parent,based on rule
		//adjust the balance factor of node parent
		if(need_balance>0)
		{
			parent=pd_adjust_balance_using_rule(parent,need_balance);//rotate and adjust balance_factor
			need_balance=0;
		}
//		parent=pd_adjust_balance_using_height(parent);//as if it is no use
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
2.遍历父路径,调整结点平衡因子.post_deletion_rule
3.连接祖父结点和父结点.post_deletion_rule
*/
AVLTree* remove_avl_tree_node_ydn_rule(AVLTree *T,size_t key,int replace_method_inorder)
{
	/*1.找到要删除的结点;
	2.如果该结点是叶子,删除;
	3.如果有单孩子,则删除该结点后,孩子结点顶上;
	4.如果有双孩子,则删除该结点后,结点的中序后继顶上(中序后继是被删结点右子树上最小的点),或者中序前驱顶上.
	5.从被删除的结点开始往上查找并更新各结点bf,找到第1个不平衡结点z,根据回溯路径上各级父结点x和y情况,仍然分为4类,LL,RR,LR,RL,旋转使之平衡
	6.从z开始继续回溯,找到路径上不平衡的结点,并调整平衡,直到找到根结点,调整结束。
	*/
	AVLTree *parent=NULL;
	std::stack<AVLTree *>node_stack;//record the node which key is passed by
	AVLTree *p=search_node(T,key,parent,node_stack);
	int shorter=0;//0,not shrink;1, has shrink in height.
	printf("Want to search:%zu.\n",key);
	if(p!=NULL)
	{
		shorter=1;//have found, it may be cause height shorten. Adjust shorter in next step.
		printf("Found key:%zu,parent:%zu.\n",p->data,parent!=NULL?parent->data:-1);
		//find inorder succssor and remove and re-balance
		if(p->lchild==NULL&&p->rchild==NULL)//p is leaf
		{
			printf("remove leaf node.\n");			
			//这里,需要一个shorter变量,来指示是否缩短.父节点bf也在此函数(based on shorter)调整
			T=shorter_leaf(T,p,parent,shorter);//处理结点p,返回处理后的根结点.父结点无需入stack,因为前面已经入过stack			
		}
		else if(p->lchild!=NULL&&p->rchild!=NULL)//p has 2 children
		{//use inorder successor(ios) to replace. ios should be re-evaluate the balance factor
			printf("remove node which has 2 children.\n");
			shorter=shorter_double_children(p,node_stack,replace_method_inorder,shorter);//set shorter
		}
		else//p has only one child
		{
			printf("remove node which has only one child.\n");
			shorter=shorter_one_child(p,node_stack,shorter);//whether shorter depends on bf(2 or -2).So there should not evaluate bf in it.
		}

		//calc the parent-trail balance factor
		printf("**shorter:%d.the node path will by(include itself):\n",shorter);
		T=post_deletion_rule(T,key,node_stack,shorter);//the 2nd process of parent on the trail stack.
		shorter=0;//??tree height got shorten process has finished.
		printf("\n");
	}
	else//not found
	{
		shorter=0;
		printf("Not found key:%zu.\n",key);
	}
	return T;
}