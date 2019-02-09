// threadBTree.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdlib.h"

#define Status int
#define OK 1
#define ERROR 0
#define TElemType char

typedef enum { Link, Thread } PointerTag; // Link -- 0 ָ�� Thread -- 1 ����
typedef struct BiThrNode {
	TElemType data;
	struct BiThrNode *lchild, *rchild; // left, right child pointer
	PointerTag LTag, RTag;             // left, right thread tag
} BiThrNode, *BiThrTree;

// char Vexch[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
char Vexch[26] = {'A', 'B', 'D', 'H', '$', '$', 'I', '$', '$', 'E', 'J', '$', '$', '$', 'C', 'F', '$', '$', 'G', '$', '$'};
int i = 0;

/**
 * �������Ĵ���
 */
Status CreateBiThrTree(BiThrTree &T)
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
		printf("%5c", T->data);
		T->LTag = Link;
		CreateBiThrTree(T->lchild); // ����������
		T->RTag = Link;
		CreateBiThrTree(T->rchild); // ����������
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

/**
 * ����ͷ��㣬��������������
 */
Status InOrderThreading(BiThrTree &Thrt, BiThrTree T)
{
	if (!(Thrt = (BiThrTree)malloc(sizeof(BiThrNode) ) ) ) {
		return ERROR;
	}

	// ����ͷ���
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
 * ������������������ķǵݹ��㷨����ÿ������Ԫ�ص��ú���visit
 * T ָ��ͷ��㣬ͷ��������lchild ָ�����㣬�ɲμ��������㷨
 */
Status InOrderTraverse_Thr(BiThrTree T, Status( *visit)(TElemType e) )
{
	BiThrTree p;
	p = T->lchild; // p ָ����ڵ�
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

int _tmain(int argc, _TCHAR* argv[])
{
	BiThrTree T, inorderT;

	printf("create tree...\n");
	CreateBiThrTree(T);

	printf("\n�����������������\n");
	InOrderThreading(inorderT, T);
	InOrderTraverse_Thr(inorderT, visit);
	printf("\n");

	return 0;
}

