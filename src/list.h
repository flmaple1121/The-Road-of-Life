#include<iostream>
#include<stdlib.h>
#include<string>
#include<iomanip>
using namespace std;
#define MaxSize 30
struct Staff
{
    string name;
    string sex;
    int age;
    int worktime;
    int salary;
};
typedef Staff ElemType;
struct  List{
    ElemType list[MaxSize];
    int size;
};
ostream &operator<<(ostream &out, ElemType& a)
{
    out<<setw(12)<<setiosflags(ios::left)<<a.name
    <<setw(8)<<setiosflags(ios::left)<<a.sex
    <<setw(8)<<setiosflags(ios::left)<<a.age
    <<setw(8)<<setiosflags(ios::left)<<a.worktime
    <<setw(8)<<setiosflags(ios::left)<<a.salary;
    return out;
}
istream &operator>>(istream& in, ElemType& a)
{
    in>>a.name>>a.sex>>a.age>>a.salary;
    a.worktime=0;
    return in;
}
bool operator==(ElemType a,ElemType b)
{
    if(a.name==b.name)  return true;
    else return false;
}
void InitList(List &a)
{
    a.size=0;
}
void ClearList(List &a)
{
    a.size=0;
}
int LenthList(List &a)
{
    return a.size;
}
bool EmptyList(List &a)
{
    return a.size==0;
}
ElemType GetList(List &a,int pos)
{
    if(pos<1||pos>a.size)
    {
        cout<<"pos is out range!"<<endl;
        exit(1);
    }
    return a.list[pos-1];
}
void TraverseList(List &a)
{
    cout<<setw(12)<<setiosflags(ios::left)<<"Name"
        <<setw(8)<<setiosflags(ios::left)<<"Sex"
        <<setw(8)<<setiosflags(ios::left)<<"Age"
        <<setw(8)<<setiosflags(ios::left)<<"Years"
        <<setw(8)<<setiosflags(ios::left)<<"Salary"<<endl;
    for(int i=0;i<a.size;i++)
        cout<<a.list[i]<<endl;
    cout<<endl;
}
bool FindList(List &a,ElemType& item)
{
    for(int i=0;i<a.size;i++)
        if(a.list[i]==item)
        {
            item=a.list[i];
            return true;
        }
    return false;
}
bool UpdateList(List &a,const ElemType& item)
{
    for(int i=0;i<a.size;i++)
        if(a.list[i]==item)
        {
            a.list[i]=item;
            return true;
        }
    return false;
}
bool InsertList(List &a,const ElemType& item,int pos,int choice)
{
    if(pos<-1||pos>a.size+1)
    {
        cout<<"Invalid position!"<<endl;
        return false;
    }
    int i;
    if(pos==0)
    {
        if(choice==1)
        {
        for(i=0;i<a.size;i++)
           if(item.age<a.list[i].age)
              break;
        }
        else if(choice==2)
        {
        for(i=0;i<a.size;i++)
           if(item.worktime<a.list[i].worktime)
              break;
        }
        else
        {
        for(i=0;i<a.size;i++)
           if(item.salary<a.list[i].salary)
              break;
        }
        pos=i+1;
    }
    else if(pos==-1)
        pos=a.size+1;
    if(a.size==MaxSize)
    {
        cout<<"The staff list is full; cannot hire."<<endl;
        return false;
    }
    for(i=a.size;i>=pos;i--)
        a.list[i]=a.list[i-1];
    a.list[pos-1]=item;
    a.size++;
    return true;
}
bool DeleteList(List &a,ElemType& item,int pos)
{
    if(a.size==0)
    {
        cout<<"There are no employees to dismiss."<<endl;
        return false;
    }
    if(pos<-1||pos>a.size)
    {
        cout<<"Invalid position."<<endl;
        return false;
    }
    int i;
    if(pos==0)
    {
        for(i=0;i<a.size;i++)
            if(item==a.list[i])
               break;
        if(i==a.size)
            return false;
        pos=i+1;
    }
    else if(pos==-1)
        pos=a.size;
    item=a.list[pos-1];
    for(i=pos;i<a.size;i++)
        a.list[i-1]=a.list[i];
    a.size--;
    return true;
}
void SortList(List &a,int choice)
{
    List b;
    InitList(b);
    for(int i=0;i<a.size;i++)
       InsertList(b,a.list[i],0,choice);
    ClearList(a);
    a=b;
}
