/*
avl tree remove node.iterative,based on rule.
基于研道难5.14二叉排序树,
but use rule not tree height.
https://www.geeksforgeeks.org/deletion-in-an-avl-tree/
https://dl.acm.org/doi/10.1145/800197.806043
https://en.wikipedia.org/wiki/AVL_tree#Comparison_to_other_structures
https://dl.acm.org/doi/pdf/10.1145/355609.362340
https://zhjwpku.com/assets/pdf/AED2-10-avl-paper.pdf
https://www.mathnet.ru/php/archive.phtml?wshow=paper&jrnid=dan&paperid=26964&option_lang=eng
https://www.cs.emory.edu/~cheung/Courses/253/Syllabus/Trees/
https://www.cs.emory.edu/~cheung/Courses/253/Syllabus/Trees/AVL-delete.html
http://www.cs.emory.edu/~cheung/Courses/253/Syllabus/book/PDF/avltrees.pdf

20240312
https://cs.stackexchange.com/questions/16313/updating-an-avl-tree-based-on-balance-factors
http://www.cs.toronto.edu/~toni/Courses/263-2015/lectures/lec04-balanced-augmentation.pdf

*import*:Defintion of shorter, indicate the tree height got shorten and bf is 2 or -2 except root case.
if shorter is trigger, balance_factor updated will NOT be postponed to rule judegement.
shorter=0, tree height remains unchanged; 
shorter=1, height got shorten, should be the current situation; 
shorter=2, the parent trail to process bf. It indicates the process is not the current removing situation.
shorter为2时,指示当前不是结点删除现场,而且堆栈中的某个结点,可能需要调整bf

删除后,根据shorter以及规则调整bf,并一直溯源到根结点,完成平衡。
add a stack, indicator the direction of left or right.
*/
#include "public_avl.h"
#include "height_cnt.h"
#include "rotate.h"
#include "height_cnt.h"
#include <cassert>
#include "remove_node_yandaonan_using_height.h"
#include "remove_node_geek.h"

/*Prerequisites:node_p is a leaf.Judeged by ASSERT.
删除node_p,并更新node_p的父结点parent的孩子结点指向.
父结点已经加入stack
在此函数调整parent's bf
返回值:
1.根结点.若node_p为根结点,则返回为空树.函数返回值.
2.shorter,树高是否变短.函数引用值.//触发shorter时,不更新bf
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
		if(node_p==parent->lchild)//node_p is parent's lchild
		{
			parent->lchild=NULL;
			switch(parent->balance_factor)
			{
				case -1:
					parent->balance_factor-=1;//parent->balance_factor=2
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
					parent->balance_factor+=1;//parent->balance_factor=2;
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

/*remove a node which has one child without parent.
Prerequisites:node_p has only one child.
return value:
1.shorter,whether the tree height got shorten, from function return value.
2.modified node_p,from function pointer.
3.node_stack,from function reference.
*/
int shorter_one_child(AVLTree* node_p,std::stack<AVLTree *> &node_stack,std::stack< int > &dir_stack,int shorter)
{
	assert(!(node_p->lchild!=NULL&&node_p->rchild!=NULL) && !(node_p->lchild==NULL&&node_p->rchild==NULL) );//node_p has only one child
	AVLTree *ps_child=(node_p->lchild!=NULL?node_p->lchild:node_p->rchild);
	int bf_direction=(node_p->lchild!=NULL?-1:1);//bf_direction=-1,node_p's left child will replace node_p,so bf will minus 1.
	node_p->data=ps_child->data;
	node_p->lchild=ps_child->lchild;
	node_p->rchild=ps_child->rchild;
	free(ps_child);
	printf("bf_direction:%d.\n",bf_direction);
	switch(node_p->balance_factor)
	{//case 0://should not exist this situation.because node_p has only one child,so bf != 0
		case 1://left child exists.so bf_direction should <0, should not >0
			node_p->balance_factor+=bf_direction;//if greater than 0,that is 2 which is no need to update bf here.			
			//shorter=0;//the parent trail maybe need to adjust,so do not change shorter sign.
			break;
		case -1://right child exists. so bf_direction should >0, should not <0
			node_p->balance_factor+=bf_direction;
			//shorter=0;//the parent trail maybe need to adjust,so do not change shorter sign.
			break;
	}//	node_p->balance_factor+=bf_direction;
	//printf("****judgegment here****%zu(%d)",node_p->data,node_p->balance_factor);
	node_stack.push(node_p);//do not use//only got shorten need to get in stack
	if(bf_direction<0)
		dir_stack.push(TYPE_LEFT);//??replacement occurs at left
	else
		dir_stack.push(TYPE_RIGHT);//??
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
int shorter_double_children(AVLTree *node_p,std::stack<AVLTree *> &node_stack,std::stack< int > &dir_stack,int replace_method_inorder,int shorter)
{
	assert(node_p->lchild!=NULL&&node_p->rchild!=NULL);
	//int special_process=0;//special add to stack.The stack is added in the ipd search process.So there is no need to add stack.
	AVLTree *parent_ipd=NULL;//as a function scope global variable
	switch(replace_method_inorder)
	{
		case IPD://中序前驱,在左子树中找到最大值
		{
			parent_ipd=node_p;
			AVLTree *ipd=max_value_node(node_p->lchild,parent_ipd,node_stack,dir_stack);//ipd do not have rchild. ipd is max child in node_p's left subtree.
			if(node_p==parent_ipd)//node_p is parent_ipd itself
			{
				node_p->lchild=ipd->lchild;
				switch(node_p->balance_factor)
				{
					case 1://**checked
						shorter=1;//got shorten. Current situation, update bf here!
						node_p->balance_factor-=1;
						break;
					case 0:
						shorter=0;						
						node_p->balance_factor=-1;
						break;
					case -1:
						shorter=1;
						node_p->balance_factor-=1;//bf should be -2.Current situation, update here
						break;
				}//node_p->balance_factor-=1;
			}
			else//node_p is not parent_ipd
			{
				switch(parent_ipd->balance_factor)//should be 0 and -1.
				{//**checked case 1.
					case 1:
						//node_p->balance_factor-=1;//it may be -2.//it should nothing to do with this case.
						parent_ipd->balance_factor+=1;//need to add to stack and rotate
						//special_process=1;//there is possible that parent_ipd got unbalanced,so added it to stack.
						shorter=1;
						break;
					case 0:
						parent_ipd->balance_factor=1;
						shorter=0;//?added 20240318
						//node_p->balance_factor-=0;//unchanged
						break;
					case -1://**need to re-check
						parent_ipd->balance_factor=0;//tree height has shorten
						shorter=1;
						//should have nothing to do with node_p.But it cause many confuse now.
						//node_p->balance_factor-=1;//may cause rotate when node_p->balance_factor=-2
						
						/*switch(node_p->balance_factor)
						{
							case -1:
								shorter=1;//got shorten. Do not update bf.
								node_p->balance_factor-=1;
								shorter=1;
								break;
							case 0:
							case 1:
								node_p->balance_factor-=1;//未触发shorter,可更新bf
								shoter=1;
								break;
						}//node_p->balance_factor-=1;
						*/
						break;
				}
				parent_ipd->rchild=ipd->lchild;	
			}
			node_p->data=ipd->data;		
			free(ipd);
			break;
		}//end of case IPD
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
						node_p->balance_factor+=1;
						shorter=0;
						break;
					case -1:
						shorter=1;//bf should be 0.but do not update bf
						break;
					case 1:
						shorter=1;
						node_p->balance_factor+=1;//bf should be 2,but update here
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
						break;
				}
				parent_ios->lchild=ios->rchild;
			}
			node_p->data=ios->data;
			free(ios);
			break;
		}//end of case:IOS
	}// end of switch(replace_method_inorder)
//may not use this line
//	node_stack.push(node_p);
//the stack is added in the ipd search process.So there is no need to add stack.
//	if(special_process==1)//stack is Last In First Out, LIFO
//		node_stack.push(parent_ipd);//interesting case. Process first
	return shorter;
}

/*update the node balance factor based on rule,not on tree height
parameter:
1.shorter comes from successful removal action. If node was removed, shorter=1. If shorter=2, the node is in the parent trail.
  shorter could not be updated in the function. Because it will be used until the last parent-node.
z is mubst.y is the height in z's subtree. x is the heighter in y's subtree.
2.dir_action,the remove action occured at which direction of current node named "parent".TYPE_LEFT or TYPE_RIGHT.
3.replace_method_inorder,IPD or IOS.
4.indicator_of_children_case, whether the case is 2 children case.
Return value:
1.need_balance,from function reference.
2.parent, from function return value.
3.shorter, from function reference.

Revision history:
19th,Mar,2024.
added dir_action.
added replace_method_inorder.
added indicator_of_children_case.

21st,Feb,2024.
Modified,remove key and parent->data comparision.
add bf=2 or bf=-2 judgement.

11,Jan,2024.
Created.
*/
AVLTree* pd_update_bf_rule(AVLTree* parent,int dir_action,int &shorter,int &need_balance,
int replace_method_inorder,int indicator_of_children_case)
{
	if(shorter>0)//tree height got shorten.
	{
		switch(parent->balance_factor)//un-updated bf after remove node.z,mubst
		{
			case 2://left is too heavy. this action should do at removing process.
			case 1://original left child is greater than right.At the edge of adjustment,then may be need_balance
			{
				//parent->balance_factor=get_balance(parent);//use tree height to replace
				if(parent->balance_factor==1)//new method
				{
					if(dir_action==TYPE_LEFT && shorter==2)//After removal,left is smaller, right get heavier.
						parent->balance_factor--;
					else if(dir_action==TYPE_RIGHT && shorter==2)
						parent->balance_factor++;
				}				
				//the following is case 2
				if(parent->balance_factor>1 && parent->lchild!=NULL)//LL or LR
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
			}
			case 0://original left equals right
			{
			//In the double children and IPD case, bf from 0 to 1,then balanced. impossible bf from 0 to -1(may be IOS with 2 children case).
			//give special case when double children and IPD, and when double children and IOS.
				//should it do update bf?Do not cause rotate, should not update bf here.But it should alter trail bf here.so do it.
				//printf("Before:%zu(%d),shorter=%d.",parent->data,parent->balance_factor,shorter);//,
					//indicator_of_children_case,replace_method_inorder);
				//parent->balance_factor=get_balance(parent);
				if(dir_action==TYPE_LEFT && shorter==2)//removing action occurs at left child subtree.right get heavy.
					parent->balance_factor--;
				else if(dir_action==TYPE_RIGHT && shorter==2)
					parent->balance_factor++;
				if(indicator_of_children_case==1&&replace_method_inorder==IPD&&
					parent->balance_factor==1)//2 children case. bf from 0 to 1.the shorter has been ABSORBED.
				{
					shorter=0;//re-verify the case to many level
				}//**prepare for IOS case
				//printf("After %zu(%d),shorter=%d is here.\n",parent->data,parent->balance_factor,shorter);
				/*
				if(key<parent->data && shorter==2)//removing action occurs at left child subtree.right get heavy.
					parent->balance_factor--;
				else if(key>parent->data && shorter==2)
					parent->balance_factor++;
				*/
				break;
			}
			case -1://at the edge adjust bf, then may be need_balance.
			case -2://original left child is less than right child
			{
				//parent->balance_factor=get_balance(parent);//temporarily unavailable
				if(parent->balance_factor==-1)
				{
				if(dir_action==TYPE_RIGHT && shorter==2)
					parent->balance_factor++;
				else if(dir_action==TYPE_LEFT && shorter==2)
					parent->balance_factor--;//parent->balance_factor=-2;
				}				
				//the following is case -2			
				if(parent->balance_factor<-1 &&  parent->rchild!=NULL)//RR or RL
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
			}
		}
	}//end of if(shorter)
	return parent;
}

/*
Do rotate action and update bf based on rule.
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
			else//mixed type, as LL case.
			{
				mubst->balance_factor=1;//z
				mubst->lchild->balance_factor=-1;//y
			}
			mubst=right_rotate_only(mubst);
			printf("Engage rpd_LL case.\n");//rule post delete
			break;
		case 2://LR case,most complicated.Has 4 cases.Filtered by pd_udpate_bf_rule
		{
			AVLTree *ld=mubst->lchild->rchild;			
			switch(ld->balance_factor)//total has 4 cases
			{
				case 0://has 2 cases.There is no LL mixed case,because it has been blocked by pd_udapte_bf_rule
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
			else//mixed type, as RR case.
			{
				mubst->balance_factor=-1;//z
				mubst->rchild->balance_factor=1;//y
			}
			mubst=left_rotate_only(mubst);
			printf("Engage rpd_RR case.\n");
			break;
		case 4://RL case.only 4 case.when mubst->rchild->bf==1
			assert(mubst->rchild!=NULL);
			assert(mubst->rchild->lchild!=NULL);
			AVLTree *rd=mubst->rchild->lchild;
			/*printf("mubst:%zu(%d). mubst->rchild:%zu(%d). mubst->rchild->lchild:%zu(%d).\n",mubst->data,mubst->balance_factor,
				mubst->rchild->data,mubst->rchild->balance_factor,
				rd->data,rd->balance_factor);*/
			switch(rd->balance_factor)
			{
				case 0://has 2 cases,but same result.x等重有子和x等重无子都是一样的结果.
					mubst->balance_factor=0;//z
					mubst->rchild->balance_factor=0;//y
					rd->balance_factor=0;//x
					break;
				case 1:
					mubst->balance_factor=0;//z
					mubst->rchild->balance_factor=-1;//y
					rd->balance_factor=0;//x
					break;
				case -1:
					mubst->balance_factor=1;//z
					mubst->rchild->balance_factor=0;//y
					rd->balance_factor=0;//x
					break;
			}			
			mubst->rchild=right_rotate_only(mubst->rchild);
			mubst=left_rotate_only(mubst);
			printf("Engage rpd_RL case.\n");
			break;
	}//end of switch(need_balance)
	return mubst;
}

/*Post BST remove node behavior
2.遍历父路径,调整结点平衡因子.
3.连接祖父结点和父结点.
pass parameter:
4.indicator_of_children_case,if we engage double children case
参数：
1.new parent,处理后的结合树的根结点.或者NULL(未处理完),as function return value.
2.node_stack,入参为删除结点删除路径上的所有父结点,return as function reference.
3.dir_stack,corresponding to node_stack, same size. According the left or right direction where 
	a node removal occured.return as function reference.
*/
AVLTree* post_deletion_rule(std::stack<AVLTree *> &node_stack,std::stack< int > &dir_stack,int &shorter,int replace_method_inorder,
int indicator_of_children_case)
{
	assert(node_stack.size()==dir_stack.size());//the size of 2 stacks must be equal
	AVLTree *parent=NULL;
	int need_balance=0;//0,do not need balance;1,2,3,4,represent rotate type.
	int dir_action=-1;
	while(!node_stack.empty())
	{
		parent=node_stack.top();
		AVLTree *old_parent=parent;//prepare for grand_parent con-junction
		node_stack.pop();
		dir_action=dir_stack.top();
		dir_stack.pop();
		printf("pass:%zu(%d),%s,shorter=%d.",parent->data,parent->balance_factor,dir_action==TYPE_LEFT?"type_left":"type_right",shorter);//the passed by parent node
		parent=pd_update_bf_rule(parent,dir_action,shorter,need_balance,
			replace_method_inorder,indicator_of_children_case);//assign the bf of parent,based on rule
		//adjust the balance factor of node parent
//		printf("need_balance=%d.",need_balance);
		if(need_balance>0)
		{
			parent=pd_adjust_balance_using_rule(parent,need_balance);//rotate and adjust balance_factor
			need_balance=0;
		}
		//connect the grand-parent and parent. parent become the child of grand-parent
		AVLTree *temp_node=pd_junction_rule(parent,old_parent,node_stack);
		if(temp_node!=NULL)//root is parent
		{
			parent=temp_node;
			return parent;
		}
		if(shorter==1)
			shorter=2;		
	}
	return NULL;
//	return T;
}

/*在给定的avl树中删除一个搜索到的值的结点,并调整平衡.返回处理完成的树的根
1.实现bst删除
2.遍历父路径,调整结点平衡因子.如果该结点为mubst,旋转.post_deletion_rule
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
	std::stack< int >dir_stack;//same size as node_stack.record the left or right that remove action actually happen.
	AVLTree *p=search_node(T,key,parent,node_stack,dir_stack);
	int shorter=0;//0,not shrink;1, has shrink in height.
	int indicator_of_children_case=0;//double children case.0,no; 1,yes
	if(WHETHER_DEBUG)
		printf("Want to search:%zu.\n",key);
	if(p!=NULL)
	{
		shorter=1;//have found, it may be cause height shorten. Adjust shorter in next step.
		printf("Found key:%zu,parent:%zu.\n",p->data,parent!=NULL?parent->data:-1);
		//find inorder succssor and remove and re-balance
		if(p->lchild==NULL&&p->rchild==NULL)//p is leaf
		{
			printf("remove leaf node.\n");			
			//这里,需要一个shorter变量,来指示是否缩短.父节点bf也在此函数(based on shorter)现场调整
			T=shorter_leaf(T,p,parent,shorter);//处理结点p,返回处理后的根结点.父结点无需入stack,因为前面已经入过stack			
		}
		else if(p->lchild!=NULL&&p->rchild!=NULL)//p has 2 children
		{//use inorder successor(ios) to replace. ios should be re-evaluate the balance factor
			printf("remove node which has 2 children.\n");
			indicator_of_children_case=1;//it's double children case.
			shorter=shorter_double_children(p,node_stack,dir_stack,replace_method_inorder,shorter);//set shorter
		}
		else//p has only one child
		{
			printf("remove node which has only one child.\n");
			shorter=shorter_one_child(p,node_stack,dir_stack,shorter);//whether shorter depends on bf(2 or -2).So there should not evaluate bf in it.
		}
		//shorter should be different from after 3 cases.shorter adjust in post_deletion_rule

		//calc the parent-trail balance factor
		printf("**shorter:%d.key:%zu.node_size:%d.dir_size:%d.the node path will by(include itself):\n",shorter,key,node_stack.size(),dir_stack.size());
		//T=post_deletion_rule(T,key,node_stack,shorter);//the 2nd process of parent on the trail stack.
		AVLTree *temp_node=post_deletion_rule(node_stack,dir_stack,shorter,replace_method_inorder,indicator_of_children_case);//the 2nd process of parent on the trail stack.
		if(temp_node!=NULL)
			T=temp_node;
		shorter=0;//tree height got shorten process has finished.
	}
	else//not found
	{
		shorter=0;
		printf("Not found key:%zu.\n",key);
	}
	return T;
}