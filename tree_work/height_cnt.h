/*height of tree iter*/
#include <queue>
using namespace std;

#ifndef HEIGHT_STRUCTURE
#define HEIGHT_STRUCTURE
size_t height_of_tree(AVLTree *T)//cursive to get tree height
{
	if(T==NULL)
		return 0;
	else
	{
		size_t left_h,right_h;
		left_h=height_of_tree(T->lchild);
		right_h=height_of_tree(T->rchild);
		if(left_h>right_h)
			return left_h+1;
		else
			return right_h+1;
	}
}

size_t height_of_tree_iter(AVLTree *T)//using level traverse to get height of tree.using node_count variable.
{
	std::queue <AVLTree *>q;
	AVLTree *cur_node=NULL;//current node in the level
	size_t node_count=0,level=0;//level is height of tree
	if(T==NULL)
		return 0;
	else
	{
		q.push(T);
		while(!q.empty())
		{
			node_count=q.size();
			while(node_count>0)
			{
				cur_node=q.front();//get top of queue,not to quit
				if(cur_node->lchild!=NULL)
					q.push(cur_node->lchild);
				if(cur_node->rchild!=NULL)
					q.push(cur_node->rchild);
				node_count--;
				q.pop();//quit the queue
			}
			level++;
		}
		return level;
	}
}

size_t height_of_tree_iter2(AVLTree *T)//use last_pointer to represent the last node of current level
{
	AVLTree *cur_node=NULL;
	AVLTree *p_last_of_cur_level=NULL;
	size_t level=0;
	std::queue <AVLTree *>q;
	if(T==NULL)
		return 0;
	else
	{
		q.push(T);
		while(!q.empty())
		{
			p_last_of_cur_level=q.back();//end of queue
			while(p_last_of_cur_level!=cur_node)
			{
				cur_node=q.front();
				if(cur_node->lchild!=NULL)
					q.push(cur_node->lchild);
				if(cur_node->rchild!=NULL)
					q.push(cur_node->rchild);
				q.pop();
			}
			level++;
		}
	}
	return level;
}

size_t height_of_tree_iter3(AVLTree *T)//single loop for using node_count to calc hegiht
{
	AVLTree *cur_node=NULL;
	size_t node_count=0,level=0;
	std::queue <AVLTree *>q;
	if(T==NULL)
		return 0;
	else
	{
		q.push(T);
		node_count=q.size();
		while(!q.empty())//using single loop
		{			
			cur_node=q.front();
			if(cur_node->lchild!=NULL)
				q.push(cur_node->lchild);
			if(cur_node->rchild!=NULL)
				q.push(cur_node->rchild);
			q.pop();
			node_count--;
			if(node_count==0)//this level ends
			{
				node_count=q.size();
				level++;
			}			
		}
	}
	return level;
}

size_t cnt_node_of_tree(AVLTree *T)//得到树中总的结点数
{
	AVLTree *cur_node=NULL;
	size_t all_node_count=0,level_node_count=0;//all_node_count represents total node count. level_node_count represents current level node count.
	std::queue <AVLTree *>q;
	if(T==NULL)
		return 0;
	else
	{
		q.push(T);
		while(!q.empty())
		{
			level_node_count=q.size();
			all_node_count+=level_node_count;
			while(level_node_count>0)
			{
				cur_node=q.front();
				//insert oper
				if(cur_node->lchild!=NULL)
					q.push(cur_node->lchild);
				if(cur_node->rchild!=NULL)
					q.push(cur_node->rchild);
				level_node_count--;
				q.pop();
			}
		}
	}
	return all_node_count;
}
#endif