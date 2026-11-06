#include<iostream>
#include<stdlib.h>
#include<string>
using namespace std;
typedef string Elem;
struct SNode{
	Elem data;
	SNode *next;
}; 
void InitStack(SNode*& HS)
{
	HS=NULL; 
 } 
void ClearStack(SNode*& HS)
{
	SNode *cp,*np;
	cp=HS;
	while(cp!=NULL)
	{
		np=cp->next;
		delete cp;
		cp=np;
	}
	HS=NULL;
}
void Push(SNode*& HS,const Elem &item)
{
	SNode* newptr=new SNode;
	newptr->data=item;
	newptr->next=HS;
	HS=newptr;
}
Elem Pop(SNode*& HS)
{
	if(HS==NULL)
	{
		cout<<"已经批改完了哟！"<<endl;
		exit(1);
	}
	SNode *p=HS;
	HS=HS->next;
	Elem temp=p->data;
	delete p;
	return temp;
}
Elem Peek(SNode*& HS)
{
	if(HS==NULL)
	{
		cout<<"已经批改完了哟！"<<endl;
		exit(1);
	}
	return HS->data;
}
bool EmptyStack(SNode* HS)
{
	return HS==NULL;
}
