/*avl use height to determinate balance factor*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "public_avl.h"
#include "height_cnt.h"
#include "avl_recur.h"
#include "avl_iter_using_height.h"
#include "avl_book.h"
#include "avl_iter.h"
#include "remove_node_yandaonan_using_height.h"
#include "remove_node_geek.h"
#include "remove_node_ydn_rule.h"

void all_traverse(AVLTree *T,int whether_display_bf)//遍历AVL树,whether_display_bf:是否显示平衡因子bf
{
	if(T!=NULL)
	{
		printf("\nRoot's value:%d.",T->data);
		printf("\nPreOrderTraverse_iterative:");
		PreOrderTraverse(T,whether_display_bf);
		printf("\nInOrderTraverse_iterative:");
		InOrderTraverse(T,whether_display_bf);
		printf("\nPostOrderTraverse_iterative:");
		PostOrderTraverse(T,whether_display_bf);
		printf("\nLevelOrderTraverse_iterative:");
		LevelOrderTraverse_nonrecursive(T,whether_display_bf);
	}
	else
		printf("\nRoot is empty.");
	printf("\n");
}

void insertion_method_talk(int insert_method)
{
	switch(insert_method)
	{
		case 1:
			printf("(insert_recursive).\n");//递归的
			break;
		case 2:
			printf("(insert_iter_with_height).\n");
			break;
		case 3:
			printf("(insert_with_rule).\n");
			break;
		case 4:
			printf("(insert_with_book).\n");
			break;
	}
}

/*input example,end with #:
使用bst建树
使用AVL建树
1.输入12 34 5678#↵
  或输入12,34,5678#↵
得到如下二叉树bst(树高3):
  12
    34 
      5678
触发RR,得到avl(树高2):
  34
12  5678
2.输入45 24 53 45 12 24 90#↵
得到如下二叉树bst(树高3):
    45
  24  53
12     90
不触发任何旋转,得到avl(树高3):
    45
  24  53
12     90
3.50,66,60,26,21,30,70,68#↵(研道难)
得到如下二叉树bst
    45
  24  53
12     90
触发RL,LL,LR,RR,最终avl为(树高4):
	  50
  26     66
21 30  60    70
		   68
4.50,26,21,30,66,60,70,68#↵(研道难)
得到如下二叉树bst(树高4):
    50
  26    66
21 30  60   70
           68 
触发LL,RR,最终avl为(树高4):
      50
  26    66
21 30  60   70
           68
5.26,21,30,50,60,66,68,70#↵(研道难)
得到如下bst(树高7):
  26 
21   30
      50
	   60
	    66 
		 68
		  70
触发3次RR,最终avl应为(树高4)：
    50
  26    66
21  30 60  68
			70
6.45,24,12#↵
触发LL,最终为:
  24
12 45
7.45,50,60#↵
触发RR,最终为:
  50
45 60
8.30,10,80,5,70,90,20,4,6,13,25,100,12,28,15#
特殊输入30,10,80,5,70,90,20,4,6#
*/
AVLTree* createAVLTree(AVLTree *T,int insert_method)
{//avl建立树,不存在先中后序的输入序列
	//printf("root's node version AVL iter_rule_bf:");
	//左右分布时,可以考虑中序输入avl结点.
	printf("AVLTree,Debug Mode:%s.",WHETHER_DEBUG?"Yes":"No");	
	//decide to show WHETHER_DEBUG mode.
	char ch;
	size_t num;
	int taller=0;//variable for book
	insertion_method_talk(insert_method);//display the message once	
	printf("separated by ',' or space,end with '#':");
	while(1)
	{
		if(scanf("%zu%c",&num,&ch)==2)
		{
			//process entered num,once process one number,
			if(WHETHER_DEBUG)
				printf("get '%zu ''%c',",num,ch);
			switch(insert_method)
			{
				case 1:
					T=insert_avl_node(T,num);//取消注释生成avlcur程序
					break;
				case 2:
					T=insert_avl_node_iter_with_height(T,num);//使用树高计算bf,建立avl树
					break;
				case 3:
					T=insert_avl_node_with_rule(T,num);//建立avl树,使用规则计算bf
					break;
				case 4:
					T=insert_avl_node_recu_book(T,num,taller);//教科书上的avl代码
					break;
			}
//			T=BST_insert_iter(T,num);//在树T中插入结点,建立BST树.在avl中不用的代码
			if(ch=='#')
				break;
		}
		else
		{
			// Clear input buffer to prevent infinite loop on invalid input
			while(getchar()!='\n');
		}
	}
	return T;
}

/*移除结点并遍历整个树
入参:
2.whether_display_bf,遍历时是否显示平衡因子bf
3.replace_method_inorder,当结点有2个孩子时,使用中序前驱或者中序后继进行替换.
*/
void removeAVLTree_and_traverse(AVLTree *root,int whether_display_bf,int replace_method_inorder,int removal_method)
{
	printf("Rmove node replace method(%d):",replace_method_inorder);
	if(replace_method_inorder==IOS)
		printf("IOS,inorder successor");//中序后继
	else if(replace_method_inorder==IPD)
		printf("IPD,inorder predecessor");//中序前驱
	printf("\n");
	size_t r_key;//key need to be remove
	do
	{
		printf("Enter a number to remove(%s method.# to end):",removal_method==REMOVE_RECU?"recu":(removal_method==REMOVE_ITER_HEIGHT?"iter_height":"iter_rule"));
		if(	scanf("%zu",&r_key)==1 )
		{
			if(removal_method==REMOVE_ITER_HEIGHT)
				root=remove_avl_tree_node_yandaonan(root,r_key,replace_method_inorder);//研道难的bst删除思路,迭代,基于树高
			else if(removal_method==REMOVE_ITER_RULE)
				root=remove_avl_tree_node_ydn_rule(root,r_key,replace_method_inorder);//研道难的bst删除思路,迭代,基于规则
			else if(removal_method==REMOVE_RECU)
				root=delete_node_geek(root,r_key,replace_method_inorder);//geek网站的代码,递归
			all_traverse(root,whether_display_bf);
			char c=getchar();
			if(c=='#')
				break;
			else
				ungetc(c,stdin);// put the character back to input stream
		}
		else
			break;
	}
	while(1);
}

int main()
{
	string section_name = "general";
    string key_name = "view_balance_factor";
    string file_name = "config.ini";
	string value=get_ini_value(section_name, key_name, file_name);
	int whether_display_bf=atoi(value.data());
	
	//debug sign read in public_avl.h
	
	//read insertion method
	section_name="insertion";	key_name="insertional_method";
	value=get_ini_value(section_name, key_name, file_name);//fetch the insertion method
	int insert_method=atoi(value.data());
	
	AVLTree *root=NULL;
	root=createAVLTree(root,insert_method);
	size_t height_recu=height_of_tree(root);
	size_t height1=height_of_tree_iter(root);
	size_t height2=height_of_tree_iter2(root);
	size_t height3=height_of_tree_iter3(root);
	size_t node_count=cnt_node_of_tree(root);
	printf("height:%zu=%zu=%zu=%zu.Count of tree=%zu.\n",height_recu,height1,height2,height3,node_count);
	all_traverse(root,whether_display_bf);

	//procedue to remove node or entire tree
	//step1:get replace method
	section_name="deletion";	key_name="replace_method_inorder";
	value=get_ini_value(section_name, key_name, file_name);//取中序前驱或者中序后继的值
	int replace_method_inorder=atoi(value.data());//1,中序前驱inorder predecessor; 0,中序后继inorder successor.
	//step2:get removal method
	section_name="deletion";	key_name="removal_method";
	value=get_ini_value(section_name, key_name, file_name);//取移除结点方法的值
	int removal_method=atoi(value.data());//1,迭代iterative; 0,递归recursive
	removeAVLTree_and_traverse(root,whether_display_bf,replace_method_inorder,removal_method);
	return 0;
}