#include<iostream>
#include<stdlib.h>
#include<string>
using namespace std;
typedef string Elem;
struct QNode{
    Elem data;
    QNode* next;
};
struct LinkQueue{
    QNode* front;
    QNode* rear;
};
void InitQueue(LinkQueue& HQ)
{
    HQ.front=HQ.rear=NULL;
}
void ClearQueue(LinkQueue& HQ)
{
    QNode* cp=HQ.front;
    while(cp!=NULL)
    {
        HQ.front=HQ.front->next;
        delete cp;
        cp=HQ.front;
     }
    HQ.rear=NULL;
}
void EnQueue(LinkQueue& HQ,Elem item)
{
    QNode* newp=new QNode;
    newp->data=item;
    newp->next=NULL;
    if(HQ.rear==NULL)
       HQ.front=HQ.rear=newp;
    else
    {
       HQ.rear->next=newp;
       HQ.rear=newp;
    }
}
Elem OutQueue(LinkQueue& HQ)
{
    if(HQ.front==NULL){
        cout<<"The queue is empty; cannot remove an item."<<endl;
        exit(1);
    }
    Elem temp=HQ.front->data;
    QNode* cp=HQ.front;
    HQ.front=cp->next;
    if(HQ.front==NULL){
        HQ.rear=NULL;
    }
    delete cp;
    return temp;
}
Elem PeekQueue(LinkQueue& HQ)
{
    if(HQ.front==NULL)
    {
        cout<<"The queue is empty; cannot inspect an item."<<endl;
        exit(1);
    }
    return HQ.front->data;
}
bool EmptyQueue(LinkQueue& HQ)
{
    return HQ.front==NULL;
}
