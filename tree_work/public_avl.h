/*public_avl.h*/
#include <stdlib.h>
#include <string>
#include <queue>
using namespace std;

#ifndef PUBLIC_AVL_STRUCTURE
#define PUBLIC_AVL_STRUCTURE
typedef int Status;
#define OK 65535
#define OVERFLOW 65536
#define ERROR 65537

#define IOS 0
#define IPD 1

//geek's remove method
#define REMOVE_RECU 0
//yandaonan's remove method,based on tree height
#define REMOVE_ITER_HEIGHT 1
////yandaonan's remove method,based on rule
#define REMOVE_ITER_RULE 2

//sign of DEBUG
#define WHETHER_DEBUG atoi((get_ini_value("general","debug","config.ini")).data())

//numeric define keyword
#define EQ(a,b) ((a)==(b))
#define LT(a,b) ((a)<(b))
#define LQ(a,b) ((a)<=(b))

/*
//string-type define keyword
#define EQ(a,b) (strcmp((a),(b))==0)
#define LT(a,b) (strcmp((a),(b))<0)
#define LQ(a,b) (strcmp((a),(b))<=0)*/


struct AVLTree
{
	//char data;
	size_t data;
	AVLTree * lchild;//left child
	AVLTree * rchild;//right child
	int balance_factor;//should be -1,0,1,use height to determinate it.
};

//output the node's data value and node's balance_factor
Status visit_integer_with_bf(size_t data,int balance_factor,int whether_display_bf)
{
	if(whether_display_bf)
	{	//括号内的是该结点平衡因子
		printf("%zu(%d), ",data,balance_factor);
	}
	else
	{
		printf("%zu,",data);
		//	printf("%Iu ,",data);
	}
	return OK;
}

Status PreOrderTraverse(AVLTree *T,int whether_display_bf)//先序遍历
{
	if(T)
	{
		if(visit_integer_with_bf(T->data,T->balance_factor,whether_display_bf))//root node content
		{
			if(PreOrderTraverse(T->lchild,whether_display_bf))
			{
				if(PreOrderTraverse(T->rchild,whether_display_bf))
				{
					return OK;
				}
			}
		}
	}
	return ERROR;
}

Status InOrderTraverse(AVLTree *T,int whether_display_bf)//中序遍历二叉树
{
	if(T)
	{
		if(InOrderTraverse(T->lchild,whether_display_bf))
		{
			if(visit_integer_with_bf(T->data,T->balance_factor,whether_display_bf))//root node content
			{
				if(InOrderTraverse(T->rchild,whether_display_bf))
				{
					return OK;
				}
			}
		}
	}
	return ERROR;
}

Status PostOrderTraverse(AVLTree *T,int whether_display_bf)//后序遍历二叉树
{
	if(T)
	{
		if(PostOrderTraverse(T->lchild,whether_display_bf))
		{
				if(PostOrderTraverse(T->rchild,whether_display_bf))
				{
					if(visit_integer_with_bf(T->data,T->balance_factor,whether_display_bf))//root node content
						return OK;
				}
		}
	}
	return ERROR;
}

Status LevelOrderTraverse_nonrecursive(AVLTree *T,int whether_display_bf)
{
	std::queue < AVLTree * > q;
	AVLTree * p=NULL;
	q.push(T);
	while(!q.empty())
	{
		p=q.front();
		visit_integer_with_bf(p->data,p->balance_factor,whether_display_bf);
		q.pop();
		if(p->lchild!=NULL)
			q.push(p->lchild);
		if(p->rchild!=NULL)
			q.push(p->rchild);
	}
	return OK;
}

//返回字符,如需数字,则使用atoi转换
string get_ini_value(string section, string key, string filename) {
    ifstream file(filename.c_str());
    string line;
    bool section_found = false;
    while (getline(file, line)) 
	{
		// Skip any lines that start with #
		if(line[0]=='#')
			continue;
        // Check if we have found the correct section
        if (line.find("[" + section + "]") != string::npos) {
            section_found = true;
            continue;
        }
        // Check if we are in the correct section and have found the correct key
        if (section_found && line.find(key) != string::npos) {
            size_t pos = line.find("=");
            return line.substr(pos + 1);
        }
    }
    return "";
}
#endif