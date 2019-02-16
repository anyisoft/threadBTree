// threadBTree.cpp : 定义控制台应用程序的入口点。
//
#define USING_VS_CODE  0
#ifndef USING_VS_CODE
#include "stdafx.h"
#include "stdlib.h"
#else
#include <stdio.h>
#include <stdlib.h>
#endif


#define Status int
#define OK 1
#define ERROR 0
#define TElemType char

typedef enum { Link, Thread } PointerTag; // Link -- 0 指针 Thread -- 1 线索
typedef struct BiThrNode {
	TElemType data;
	struct BiThrNode *lchild, *rchild; // left, right child pointer
	struct BiThrNode *parent;
	PointerTag LTag, RTag;             // left, right thread tag
} BiThrNode, *BiThrTree;

// char Vexch[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
char Vexch[26] = {'A', 'B', 'D', 'H', '$', '$', 'I', '$', '$', 'E', 'J', '$', '$', '$', 'C', 'F', '$', '$', 'G', '$', '$'};
int i = 0;

/**
 * 二叉树的创建
 */
Status CreateBiThrTree(BiThrTree &T, BiThrTree &p)
{
	if (Vexch[i++] == '$') {
		T = NULL;
	}
	else {
		T = (BiThrTree)malloc(sizeof(BiThrNode) );
		if (!T) {
			return 0;
		}
		T->data = Vexch[i-1];
		T->parent = p;

		printf("%5c", T->data);

		T->LTag = Link;
		CreateBiThrTree(T->lchild, T); // 创建左子树
		T->RTag = Link;
		CreateBiThrTree(T->rchild, T); // 创建右子树
	}

	return 1;
}

Status visit(TElemType e)
{
	printf("%5c", e);
	return OK;
}

BiThrTree pre;  // global variable, always points to the node have accessed just now

void InThreading(BiThrTree p)
{
	if (p) {
		InThreading(p->lchild);

		if (!p->lchild) {
			p->LTag = Thread;
			p->lchild = pre;
		}

		if (!pre->rchild) {
			pre->RTag = Thread;
			pre->rchild = p;
		}
		pre = p;

		InThreading(p->rchild);
	}
}

void PreThreading(BiThrTree p)
{
	if (p) {
		if (!p->lchild) {
			p->LTag = Thread;
			p->lchild = pre;
		}

		if (!pre->rchild) {
			pre->RTag = Thread;
			pre->rchild = p;
		}

		pre = p;

		if (p->LTag == Link) {
			PreThreading(p->lchild);
		}

		if (p->RTag == Link) {
			PreThreading(p->rchild);
		}
	}
}

void PostThreading(BiThrTree p)
{
	if (p) {
		PostThreading(p->lchild);
		PostThreading(p->rchild);
		if (!p->lchild) {
			p->LTag = Thread;
			p->lchild = pre;
		}

		if (pre && !pre->rchild) {
			pre->RTag = Thread;
			pre->rchild = p;
		}

		pre = p;
	}
}

/**
 * 建立头结点，中序线索二叉树
 */
Status InOrderThreading(BiThrTree &Thrt, BiThrTree T)
{
	if (!(Thrt = (BiThrTree)malloc(sizeof(BiThrNode) ) ) ) {
		return ERROR;
	}

	// 建立头结点
	Thrt->RTag = Link;
	Thrt->rchild = Thrt;
	if (!T) {
		Thrt->lchild = Thrt;
		Thrt->LTag = Link;
	}
	else {
		pre = Thrt;
		Thrt->lchild = T;
		Thrt->LTag = Link;

		InThreading(T);

		pre->rchild = Thrt;
		pre->RTag = Thread;
		Thrt->rchild = pre;
	}

	return OK;
}

/**
 * 建立头结点，前序线索二叉树
 */
Status PreOrderThreading(BiThrTree &Thrt, BiThrTree T)
{
	if (!(Thrt = (BiThrTree)malloc(sizeof(BiThrNode) ) ) ) {
		return ERROR;
	}

	Thrt->RTag = Thread;
	Thrt->rchild = Thrt;
	Thrt->LTag = Link;
	if (!T) {
		Thrt->lchild = Thrt;
	}
	else {
		Thrt->lchild = T;
		pre = Thrt;
		PreThreading(T);
		pre->RTag = Thread;
		Thrt->rchild = pre;
	}
}

/**
 * 中序遍历线索二叉树的非递归算法，对每个数据元素调用函数visit
 * T 指向头结点，头结点的左链lchild 指向根结点，可参见线索化算法
 */
Status InOrderTraverse_Thr(BiThrTree T, Status( *visit)(TElemType e) )
{
	BiThrTree p;
	p = T->lchild; // p 指向根节点
	while (p != T) {
		while (p->LTag == Link) {
			p = p->lchild;
		}
		visit(p->data);
		while (p->RTag == Thread && p->rchild != T) {
			p = p->rchild;
			visit(p->data);
		}
		p = p->rchild;
	}

	return OK;
}

/**
 * 前序遍历线索二叉树
 * 非递归，对每个数据元素调用函数visit
 * T 指向头结点，头结点的lchild 指向根节点
 */
Status PreOrderTraverse_Thr(BiThrTree T, Status(* visit)(TElemType e) )
{
	BiThrTree p;

	p = T->lchild;
	while (p != T && p != NULL) {
		visit(p->data);
		if (p->LTag == Link) {
			p = p->lchild;
		}
		else {
			p = p->rchild;
		}
	}

	return OK;
}

Status PostOrderTraverse_Thr(BiThrTree T, Status(* visit)(TElemType e) )
{
	BiThrTree p;

	p = T;
	pre = NULL;

	while (p!=NULL) {
		while (p->LTag == Link) {
			p = p->lchild;
		}

		while (p->RTag == Thread) {
			visit(p->data);
			pre = p;
			p = p->rchild;
		}

		if (p == T) {
			visit(p->data);
			break;
		}

		while (p && p->rchild == pre ) {
			visit(p->data);
			pre = p;
			p = p->parent;
		}

		if (p && p->RTag == Link) {
			p = p->rchild;
		}
	}

	return OK;
}

#ifndef USING_VS_CODE
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
/*	BiThrTree T, inorderT;

	printf("create tree...\n");
	CreateBiThrTree(T);

	printf("\n中序遍历线索二叉树\n");
	InOrderThreading(inorderT, T);
	InOrderTraverse_Thr(inorderT, visit);
	printf("\n");*/

/*	BiThrTree T, preT;
	printf("创建树...\n");
	CreateBiThrTree(T);
	printf("\n前序遍历线索二叉树\n");
	PreOrderThreading(preT, T);
	PreOrderTraverse_Thr(preT, visit);
	printf("\n");*/

	BiThrTree postT;

	printf("创建树...\n");
	pre = NULL;
	CreateBiThrTree(postT, pre);
	printf("\n后序遍历线索二叉树\n");
	PostThreading(postT);
	PostOrderTraverse_Thr(postT, visit);
	printf("\n");

	return 0;
}

