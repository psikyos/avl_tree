/*
bst tree remove node follow the geek website.Part of avl tree delete a node.
https://www.geeksforgeeks.org/deletion-in-binary-search-tree/
the complete recursive method it provides.
*/
#include "public_avl.h"
#include "rotate.h"

#ifndef REMOVE_NODE_GEEK
#define REMOVE_NODE_GEEK
/*找到结点p的中序前驱,inorder predecessor
假设结点p不为空.
返回值:
1.node_p的中序前驱ipd,函数返回值.
*/
AVLTree *max_node_gk(AVLTree *psleftchild)
{
	AVLTree *ipd=psleftchild;//inorder predecessor
	while(ipd->rchild!=NULL)
	{
		ipd=ipd->rchild;
	}
	return ipd;
}

/*找到结点p的中序后继,即右子树最小值,一定在p的右子树的最左下的结点,且值在p的右子树中最小.
假设p结点不为空.
返回值:
1.node_p的中序后继ios,函数返回值.
*/
AVLTree *min_node_gk(AVLTree* psrightchild)
{
	AVLTree *ios=psrightchild;//inorder successor equals ios.
	while(ios->lchild!=NULL)
	{
		//ios_parent=ios;//in the cursive method, it do not need the ios_parent
		ios=ios->lchild;
	}
	return ios;
}

int get_balance(AVLTree* T)
{
	int left_height=height_of_tree_iter(T->lchild);
	int right_height=height_of_tree_iter(T->rchild);
	int balance_factor=left_height-right_height;
	return balance_factor;
}

/*find key in the tree. if node is found,remove the node and rebalance all the node of the parent trail.
return value:
the root of the tree,from function value.
*/
AVLTree* delete_node_geek(AVLTree *root,size_t key,int replace_method_inorder)
{
	if(root==NULL)
		return NULL;

	if(key<root->data)//go left
	{
		root->lchild=delete_node_geek(root->lchild,key,replace_method_inorder);
	}
	else if(key>root->data)//go right
	{
		root->rchild=delete_node_geek(root->rchild,key,replace_method_inorder);
	}
	else//equal. find the key,and ready to delete root node
	{//3 cases.T is leaf,or has one child,or has 2 children.
		if(root->lchild==NULL||root->rchild==NULL)
		{
			AVLTree *temp_node=root->lchild!=NULL?root->lchild:root->rchild;
			if(temp_node==NULL)//root has no child
			{
				free(root);//remove leaf
				root=NULL;
			}
			else//root has just 1 child
			{
				//copy the content of non-empty child.difference between root and temp_node address. equals the 3 step:
				//root->data=temp_node->data;root->lchild=temp_node->lchild;root->rchild=temp_node->rchild;
				*root=*temp_node;
				//printf("root:(%d)%zu,lchild=%d,rchild=%d.\ntemp:(%d)%zu,lchild=%d,rchild=%d.\n",root,root->data,root->lchild,root->rchild,
				//	temp_node,temp_node->data,temp_node->lchild,temp_node->rchild);
				free(temp_node);//assign all the value to root,then remove temp_node
			}
		}
		else//root has 2 children.use inorder successor to replace deleted node
		{
			if(replace_method_inorder==IOS)
			{
				AVLTree *ios=min_node_gk(root->rchild);//inorder successor equals ios.
				//get the inorder successor of node root
				root->data=ios->data;
				root->rchild=delete_node_geek(root->rchild,ios->data,replace_method_inorder);//remove ios node
			}
			else if(replace_method_inorder==IPD)
			{
				AVLTree *ipd=max_node_gk(root->lchild);//inorder predecessor equals ipd.
				root->data=ipd->data;
				root->lchild=delete_node_geek(root->lchild,ipd->data,replace_method_inorder);
			}
		}
	}
	if(root==NULL)//root is leaf, has been deleted.
		return NULL;
	//adjust balance factor
	int old_balance_factor=root->balance_factor;	
	root->balance_factor=get_balance(root);
	if(WHETHER_DEBUG)
		printf("pass:%zu(%d).",root->data,root->balance_factor);
	if(root->balance_factor>1)//LL or LR
	{
		if(get_balance(root->lchild)>=0)//LL
		{
			printf("Engage gk_LL.\n");
			root=right_rotate(root);
		}
		else if(get_balance(root->lchild)<0)//LR
		{
			printf("Engage gk_LR.\n");
			root->lchild=left_rotate(root->lchild);
			root=right_rotate(root);
		}
	}
	else if(root->balance_factor<-1)//RR or RL
	{
		if(get_balance(root->rchild)<=0)//RR
		{
			printf("Engage gk_RR.\n");
			root=left_rotate(root);
		}
		else if(get_balance(root->rchild)>0)//RL
		{
			printf("Engage gk_RL.\n");
			root->rchild=right_rotate(root->rchild);
			root=left_rotate(root);
		}
	}
	return root;
}
#endif