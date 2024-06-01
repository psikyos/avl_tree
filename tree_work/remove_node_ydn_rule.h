/*
avl tree remove node.iterative,based on rule.
基于研道难5.14二叉排序树,but use rule not tree height.
https://www.geeksforgeeks.org/deletion-in-an-avl-tree/
https://www.cs.usfca.edu/~galles/visualization/AVLtree.html
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

*import*:Defintion of shorter, indicate the tree height got shorten.
if shorten is trigger, balance_factor updated will NOT be postponed to rule judegement.It called the deletion scene.
shorter=0, tree height remains unchanged; 
shorter=1, height got shorten, should be the deletion scene;
shorter=2, the parent trail to process bf. It indicates the process is not the current removing situation, means not the deletion scene,it's the parent trail.
shorter=2,指示当前不是结点删除现场,而且堆栈中的某个结点,可能需要调整bf

After deletion, adjust bf according to shorter,direction and rules, and trace the source all the way to the root node to complete the balance.It will update bf until the last parent-node(root).
add dir_stack, indicator the direction of left or right. 
dir_stack instead of avoiding key and node value compare because it's not correct when replace occurs such as remove node with 2 children.
*/
#include "public_avl.h"
#include "height_cnt.h"
#include "rotate.h"
#include "height_cnt.h"
#include <cassert>
#include "remove_node_yandaonan_using_height.h"
#include "remove_node_geek.h"

/*The deletion scene of removing a leaf, adjust parent's bf.
Prerequisites:node_p is a leaf.Judeged by ASSERT.
delete node_p, and update the child point of node_p's parent.
parent has added to stack in previous function.So it does not push to stack here.
Return value:
1.root node of avl tree.It returns NULL if node_p is root.Functioni return value.
2.shorter,indicate whether tree height got shorten. function reference.
3.parent with altered child pointer,return from function pointer.
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
					parent->balance_factor-=1;//Deletion scene. Do not trigger shorter,so update bf here.
					shorter=0;
					break;
				case 1:
					parent->balance_factor-=1;
					shorter=1;//height get shorten,set to 1.
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
					parent->balance_factor+=1;//Deletion scene. Do not trigger shorter,so update bf here.
					shorter=0;
					break;
				case -1:
					//parent has no lchild
					parent->balance_factor+=1;
					shorter=1;//height get shorten,But may cause rotate at grand-parent level or further.set to 1.
					break;
			}			
		}
	}
	free(node_p);
	return root;
}

/*remove a node which has one child.There is no parent paremeter.
Prerequisites:node_p has only one child.
return value:
1.shorter,whether the tree height got shorten, from function return value.
2.modified node_p,from function pointer.
3.node_stack,from function reference.
4.dir_stack,from function reference.
*/
int shorter_one_child(AVLTree* node_p,std::stack<AVLTree *> &node_stack,std::stack< int > &dir_stack,int shorter)
{
	assert(!(node_p->lchild!=NULL&&node_p->rchild!=NULL) && !(node_p->lchild==NULL&&node_p->rchild==NULL) );//node_p has only one child
	AVLTree *ps_child=(node_p->lchild!=NULL?node_p->lchild:node_p->rchild);
	int bf_direction=(node_p->lchild!=NULL?-1:1);//bf_direction=-1,node_p's left child will replace node_p,so bf will minus 1. and vice versa.
	node_p->data=ps_child->data;
	node_p->lchild=ps_child->lchild;
	node_p->rchild=ps_child->rchild;
	free(ps_child);
	switch(node_p->balance_factor)
	{//case 0://should not exist this situation.because node_p has only one child,so bf != 0
		case 1://left child exists.so bf_direction should <0, should not >0
			node_p->balance_factor+=bf_direction;//if greater than 0,that is 2.
			shorter=1;//the parent trail maybe need to adjust,so do not change shorter default sign.
			break;
		case -1://right child exists. so bf_direction should >0, should not <0
			node_p->balance_factor+=bf_direction;
			shorter=1;//the parent trail maybe need to adjust,so do not change shorter default sign.
			break;
	}
	//though node_p pushed to stack,but shorter is 1, so DO NOT update the bf in the trail.
	//but if we do not push the node_p, it will destory the define of shorter=1.
	//So we need to push the node in stack
	node_stack.push(node_p);
	if(bf_direction<0)
		dir_stack.push(TYPE_LEFT);//replacement occurs at left
	else
		dir_stack.push(TYPE_RIGHT);
	return shorter;
}

/*assuming node_p has 2 children. Remove node_p and replace it by child node in the sub tree.
 using inorder successor(中序后继,ios for short) to replace node_p
 using inorder predecessor(中序前驱,ipd for short)to replace node_p
shorter should be judge after node_p's bf get changed
return value:
1.shoter sign, from function return value
*/
int shorter_double_children(AVLTree *node_p,std::stack<AVLTree *> &node_stack,std::stack< int > &dir_stack,int replace_method_inorder,int shorter)
{
	assert(node_p->lchild!=NULL&&node_p->rchild!=NULL);	
	AVLTree *parent_ipd=NULL;//as a function scope global variable
	switch(replace_method_inorder)
	{
		case IPD://iorder predecessor,find the MAX value in the node_p's left child.中序前驱,在node_p左子树中找到最大值.
		{
			parent_ipd=node_p;
			AVLTree *ipd=max_value_node(node_p->lchild,parent_ipd,node_stack,dir_stack);//ipd MUST NOT have rchild. ipd is max child in node_p's left subtree.
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
			{//node_p'bf has nothing to do with these 3 cases, will be update in grand-parent trail.
				switch(parent_ipd->balance_factor)//should be 0 and -1.
				{//**checked case 1.
					case 1:
						parent_ipd->balance_factor+=1;//need to add to stack and rotate						
						shorter=1;
						break;
					case 0:
						parent_ipd->balance_factor=1;
						shorter=0;//?added 20240318
						break;
					case -1://**need to re-check
						parent_ipd->balance_factor=0;//tree height has shorten
						shorter=1;
						break;
				}
				parent_ipd->rchild=ipd->lchild;	
			}
			node_p->data=ipd->data;		
			free(ipd);
			break;
		}//end of case IPD
		case IOS://inorder successor, find the MIN value in the node_p's right child.中序后继.在右子树中找到最小值
		{
			AVLTree *parent_ios=node_p;//node_p may be the parent of ios
			AVLTree *ios=min_value_node(node_p->rchild,parent_ios,node_stack,dir_stack);//ios is node_p's inorder successor.ios MUST NOT have left child.
			if(node_p==parent_ios)
			{
				node_p->rchild=ios->rchild;				
				switch(node_p->balance_factor)//left is get heavy
				{
					case 1:						
						node_p->balance_factor+=1;//bf should be 2, update here in deletion scene.
						shorter=1;
						break;
					case 0:
						node_p->balance_factor+=1;
						shorter=0;//bf should be 0.ABSORBED here.
						break;
					case -1:
						node_p->balance_factor+=1;
						shorter=1;
						break;					
				}
			}
			else//node_p is not ios's parent
			{//ios must be ios_parent's left child
				switch(parent_ios->balance_factor)
				{//node_p->balance_factor updated in trail grand-parent 			
					case 1:
						parent_ios->balance_factor-=1;
						shorter=1;									
						break;
					case 0:
						parent_ios->balance_factor=-1;
						shorter=0;//shorten ABSORBED here
						break;					
					case -1:
						parent_ios->balance_factor-=1;
						shorter=1;
						break;
				}
				parent_ios->lchild=ios->rchild;
			}
			node_p->data=ios->data;
			free(ios);
			break;
		}//end of case:IOS
	}// end of switch(replace_method_inorder)
//	node_stack.push(node_p);//the node_p is added in the ipd or ios search process.So there is no need to add stack.
	return shorter;
}

/*update the node balance factor based on rule,not on tree height, before rotation.
IN parameter:
1.shorter comes from successful removal action. If node was removed, shorter=1. If shorter=2, the node is in the parent trail.
z is mubst.y is the height in z's subtree. x is the heighter in y's subtree.
2.dir_action,the remove action occured at which direction of current node named "parent".TYPE_LEFT or TYPE_RIGHT.
3.replace_method_inorder,IPD or IOS.
4.indicator_of_children_case, whether the case is 2 children case.
Return value:
1.parent, from function return value.
2.need_balance,from function reference.
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
				if(parent->balance_factor==1)
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
						case 0://LL or LR case,y has same weight children. Suggest to do as LL case.
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
				//In the 2 children and IPD case, bf from 0 to 1,then shorter has been ABSORBED.
				//In the 2 children and IOS case, bf from 0 to -1,then shorter has been ABSORBED.
				//should it do update bf?Do not cause rotate, should not update bf here.But it should alter trail bf here.so do it.
				//parent->balance_factor=get_balance(parent);
				if(dir_action==TYPE_LEFT && shorter==2)//removing action occurs at left child subtree.right get heavy.
					parent->balance_factor--;
				else if(dir_action==TYPE_RIGHT && shorter==2)
					parent->balance_factor++;
				if(indicator_of_children_case==1&&replace_method_inorder==IPD&&
					parent->balance_factor==1)//IPD 2 children case.
				{
					shorter=0;//bf from 0 to 1, shorten has been ABSORBED.
				}
				if(indicator_of_children_case==1&&replace_method_inorder==IOS&&
					parent->balance_factor==-1)//**prepare for IOS 2 children case.
				{
					shorter=0;//bf from 0 to -1,shorten has been ABSORBED.//20240411
				}
				//printf("After %zu(%d),shorter=%d is here.\n",parent->data,parent->balance_factor,shorter);				
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
							//parent->rchild->balance_factor=-1;//?? what is this?
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
z is the mubst.
y is the most heaviest subtree of z. 
x is the most heaviest subtree of y.
Caution:
LL&LR mixed type as to LL.
RR&RL mixed type as to RR.
Return value:
1.mubst, from function return value.
*/
AVLTree* pd_rotate_using_rule(AVLTree *mubst,int need_balance)//mubst's bf has been updated when passed in this function
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
			printf("Engage rpd_LL case.\n");//rpd prefix represents rule post delete
			break;
		case 2://LR case.Has 4 cases. The 5th, LR&LL mixed case was filtered by pd_udpate_bf_rule.
		{
			AVLTree *ld=mubst->lchild->rchild;			
			switch(ld->balance_factor)
			{//case 0 should have 2 cases.But there is no LL mixed case,because it has been blocked by pd_udapte_bf_rule
				case 0://x等重有子和x等重无子都是一样的结果.Same result when x with subtrees and x without subtrees, whose subtrees are same tree height.
					mubst->balance_factor=0;
					mubst->lchild->balance_factor=0;
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
		case 4://RL case.only 4 case.when mubst->rchild->bf==1. The 5th, RL&&RR case was filtered by pd_udpate_bf_rule.
			assert(mubst->rchild!=NULL);
			assert(mubst->rchild->lchild!=NULL);
			AVLTree *rd=mubst->rchild->lchild;
			switch(rd->balance_factor)
			{
				case 0://has 2 cases,but same result.Same result when x with subtrees and x without subtrees, whose subtrees are same tree height.
					mubst->balance_factor=0;//z
					mubst->rchild->balance_factor=0;//y
					break;
				case 1:
					mubst->balance_factor=0;//z
					mubst->rchild->balance_factor=-1;//y
					break;
				case -1:
					mubst->balance_factor=1;//z
					mubst->rchild->balance_factor=0;//y
					break;
			}
			rd->balance_factor=0;//x
			mubst->rchild=right_rotate_only(mubst->rchild);
			mubst=left_rotate_only(mubst);
			printf("Engage rpd_RL case.\n");
			break;
	}//end of switch(need_balance)
	return mubst;
}

/*Post BST remove node behavior.
1.Traverse the parent path and adjust the node balance factor.
2.Rotate if necessary.
3.Con-junction the grandparent node and the parent node.
4.adjust shorter sign to 2 if shorter is 1. (2 indicates in the grand-parent trail. 1 represents the deletion scene.)
IN parameter:
1....
5.indicator_of_children_case:  Currently we engage double children case.
OUT parameter：
1.new parent,based on pd_junction_rule' value,new tree root,or NULL(during process).
	If return at end,root is NULL.As function return value.
2.node_stack,parent node stack in trail of removed node,return as function reference.
3.dir_stack,corresponding to node_stack, same size. According the left or right direction where 
	a node removal occured.return as function reference.
4.shorter, the indicator of tree got shorten or indicates process in parent trail.
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
		if(need_balance>0)
		{
			parent=pd_rotate_using_rule(parent,need_balance);//rotate and adjust balance_factor
			need_balance=0;
		}
		//connect the grand-parent(old parent) and parent. parent become the child of grand-parent
		AVLTree *temp_node=pd_junction_rule(parent,old_parent,node_stack);
		if(temp_node!=NULL)//The condition indicates that root is parent.We have reach the final node in trail.
		{
			parent=temp_node;
			return parent;
		}//if temp_node is NULL, continue to process. The pd_junction_rule 's target is to process the trail until reach root.
		if(shorter==1)
			shorter=2;		
	}
	return NULL;//empty tree. It means that root is NULL.
}

/*main thread of avl tree deletion. Delete a node when found in the given avl tree and adjust the balance.
IN parameter:
replace_method_inorder, when removed node has 2 children,how to replace it in its subtree.
return value:
1.the root of get processed tree. function return value.
*/
AVLTree* remove_avl_tree_node_ydn_rule(AVLTree *T,size_t key,int replace_method_inorder)
{
	/*
1.execute the BST tree deletion. 
2.Traverse the parent path and adjust the node balance factor. If the node is mubst, rotate.
3.Junction grand-parent and parent.
	*/
	/*1.Find the node to be deleted. Add all nodes and directions on the path from the root to the target node to the stack.
	2.Process deletion scene:
		2.1 If node is a leaf,remove it and adjust shorter;
		2.2 If node has 1 child, the child will replace the removed node. Shorter should always be 1.
		2.3 If node has 2 children, ipd or ios node will replace removed node.
	update the relative node bf in deletion scene.
	(搜索路径上的祖父结点不在删除现场更新,在后续处理堆栈时更新,即从堆栈取出结点,更新其bf,必要时旋转.)
The grandparent node on the search path is not updated at the deletion scene, but is updated when the stack is subsequently processed, 
that is, the node is taken from the stack, its bf is updated, and rotated if necessary.
	3.从被删除的结点开始往上查找并更新各祖先结点bf,找到第1个不平衡结点z,以及z相关的y和x,若是最小不平衡子树,仍然分为4类,LL,RR,LR,RL,
		旋转使之平衡
Starting from the deleted node, search upwards and update each ancestor node's bf, find the first unbalanced node z, and the y and x related to z.
If it is the MUBST, classfied and rotate to balance. The classified are 4 types, LL,RR,LR,RL.
	4.从z开始继续回溯,找到路径上不平衡的各级父结点,并调整平衡,直到根结点,调整结束.
	Start backtracking from z, find the unbalanced parent nodes(MUBST) at all levels on the path, and adjust the balance until the root node is reached.
	*/
	AVLTree *parent=NULL;
	std::stack<AVLTree *>node_stack;//record the node which key is passed by
	std::stack< int >dir_stack;//same size as node_stack.record the left or right that remove action actually happened.
	AVLTree *p=search_node(T,key,parent,node_stack,dir_stack);
	int shorter=0;//0,not shrink;1, has shrink in height; 2, process in parent trail.
	int indicator_of_children_case=0;//double children case.0,no; 1,yes.
	if(WHETHER_DEBUG)
		printf("Want to search:%zu.\n",key);
	if(p!=NULL)//have found wanted value
	{
		shorter=1;//default value. it may cause height shorten. Adjust shorter in deletion scene.
		printf("Found key:%zu,parent:%zu.\n",p->data,parent!=NULL?parent->data:-1);
		if(p->lchild==NULL&&p->rchild==NULL)//p is leaf
		{//shorter indicates whether tree got shorten. parent'bf adjust in deletion scene.
			printf("remove leaf node.\n");			
			//parent'll not push into stack,because parent has been pushed in stack during search.
			T=shorter_leaf(T,p,parent,shorter);//process p,return root when finished.
		}
		else if(p->lchild!=NULL&&p->rchild!=NULL)//p has 2 children
		{//use inorder predecessor (ipd) or inorder successor(ios) to replace.
			printf("remove node which has 2 children.\n");
			indicator_of_children_case=1;//it's double children case.
			shorter=shorter_double_children(p,node_stack,dir_stack,replace_method_inorder,shorter);//set shorter
		}
		else//p has only one child
		{
			printf("remove node which has only one child.\n");
			shorter=shorter_one_child(p,node_stack,dir_stack,shorter);
		}
		//shorter should be different from 3 cases listed before.shorter adjust in next function and step.

		//calc the parent-trail balance factor
		printf("**shorter:%d.key:%zu.node_size:%d.dir_size:%d.the node path will by(include itself):\n",shorter,key,node_stack.size(),dir_stack.size());
		//T=post_deletion_rule(T,key,node_stack,shorter);//the 2nd process of parent on the trail stack.
		AVLTree *temp_node=post_deletion_rule(node_stack,dir_stack,shorter,replace_method_inorder,indicator_of_children_case);//the 2nd process of parent on the trail stack.
		if(temp_node!=NULL)//when root is NULL, the case has been processed in previous function.
			T=temp_node;
		shorter=0;//tree height got shorten process has finished.
		//indicator_of_children_case=0;//should not further use
	}
	else//not found
	{
		shorter=0;
		//indicator_of_children_case=0;//should not further use
		printf("Not found key:%zu.\n",key);
	}
	return T;
}