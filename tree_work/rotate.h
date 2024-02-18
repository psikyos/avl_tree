/*rotate.h
旋转处理*/
#include "public_avl.h"
#include "height_cnt.h"

#ifndef ROTATE_STRUCTURE
#define ROTATE_STRUCTURE
AVLTree * right_rotate(AVLTree *root)
{
	AVLTree *new_root=root->lchild;
	AVLTree *moved_node=new_root->rchild;
	new_root->rchild=root;
	root->lchild=moved_node;

	//修改balance.new_root和root变量
	//1.处理new_root的bf
	size_t left=height_of_tree(new_root->lchild);
	size_t right=height_of_tree(new_root->rchild);
	new_root->balance_factor=left-right;
	//2.处理root的bf
	left=height_of_tree(root->lchild);
	right=height_of_tree(root->rchild);
	root->balance_factor=left-right;
	return new_root;
}

AVLTree * left_rotate(AVLTree *root)//使用返回结点值的方法,处理父结点的问题.更新了root和new_root的bf
{
	AVLTree *new_root=root->rchild;
	AVLTree *moved_node=new_root->lchild;
	new_root->lchild=root;
	root->rchild=moved_node;

	//process balance_factor. including new_root and root variable
	//1.new_root balance_factor
	size_t left=height_of_tree(new_root->lchild);
	size_t right=height_of_tree(new_root->rchild);
	new_root->balance_factor=left-right;
	//2.root balance_factor
	left=height_of_tree(root->lchild);
	right=height_of_tree(root->rchild);
	root->balance_factor=left-right;
	return new_root;
}

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
#endif