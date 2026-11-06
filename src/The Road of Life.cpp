#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<iomanip>
#include<sstream>
#include"list.h"
#include"Stack2.h"
#include"LinkQueue.h"
#define N 10
using namespace std;
struct Role
{
    string name;
    string sex;
    int age;
    int wisdom;
    int wealth;
    int experience;
};
void operator+=(List& a,int b)
{
    for(int i=0;i<a.size;i++)
    {
        a.list[i].age+=b;
        a.list[i].worktime+=b;
    }
}
void check(int, float *);
string readToken();
int readInteger();
void choose(Role *);
void company(int );
void school(Role *);
void assistant();
void attribute(Role *);
void study(Role *);
int work(Role *);
void ability(Role *);
float *testnum;
// Study progresses from no degree (0) to bachelor's (4) to postgraduate (9).
int study_time=0;
int main()
{
    srand((unsigned)time(0));
    float test;
    testnum=&test;
    Role  r[3]={{"Chenxi","Male",18,150,60,30},
                {"Haochen","Male",18,110,150,40},
                {"Yuyao","Female",18,120,90,20}};
    Role *q;
    q=r;
    cout<<"Choose a character to begin your journey through life!"<<endl;
    for(int i=0;i<3;i++,q++)
        cout<<i+1<<"."<<q->name<<" ";
    cout<<endl;
    check(3,testnum);
    q=r+(int)*testnum-1;
    cout<<"Name: "<<q->name<<"  Sex: "<<q->sex
        <<"  Age: "<<q->age<<endl<<"Wisdom: "
        <<q->wisdom<<"  Wealth: "<<q->wealth
        <<"  Experience: "<<q->experience<<endl<<endl;
    while(1)
    {
        cout<<"Choose an action:"<<endl<<"1. Study"<<endl
        <<"2. Work"<<endl<<"3. View your attributes"<<endl
        <<"4. Clear the screen"<<endl<<"5. End this life" <<endl;
        check(5,testnum);
        if(*testnum==5)
        {
            cout<<"Your journey ends here."<<endl
                <<"Name: "<<q->name<<"  Sex: "<<q->sex
                <<"  Age: "<<q->age<<endl<<"Wisdom: "
                <<q->wisdom<<"  Wealth: "<<q->wealth
                <<"  Experience: "<<q->experience<<endl
                <<"Game over.";
            break;
        }
        choose(q);
    }
    return 0;
}
// Read tokens so malformed input can be retried and EOF ends the game cleanly.
string readToken()
{
    string token;
    if(cin>>token)
        return token;
    cout<<endl<<"Input closed. Goodbye."<<endl;
    exit(0);
}
int readInteger()
{
    while(true)
    {
        istringstream input(readToken());
        int value;
        char extra;
        if((input>>value) && !(input>>extra))
            return value;
        cout<<"Please enter a whole number: ";
    }
}
void check(int n,float *choice)
{
    while(true)
    {
        int value=readInteger();
        if(value>=1 && value<=n)
        {
            *choice=(float)value;
            return;
        }
        cout<<"Invalid choice. Please try again."<<endl;
    }
}
void school(Role *q)
{
    if(study_time==0)
    {
        if(q->name=="Chenxi")
        {
            cout<<"Choose a university for your bachelor's degree:"<<endl
                <<"1. Zhejiang University"<<endl
                <<"2. Nanjing University"<<endl
                <<"3. Tsinghua University"<<endl
                <<"4. Peking University"<<endl;
        }
        else if(q->name=="Haochen")
        {
            cout<<"Choose a university for your bachelor's degree:"<<endl
                <<"1. Jinan University"<<endl
                <<"2. South China Normal University"<<endl
                <<"3. University of Barcelona"<<endl
                <<"4. Universite Cote d'Azur"<<endl;
        }
        else
        {
            cout<<"Choose a university for your bachelor's degree:"<<endl
                <<"1. Sun Yat-sen University"<<endl
                <<"2. South China University of Technology"<<endl
                <<"3. Xiamen University"<<endl
                <<"4. Wuhan University"<<endl;
        }
    }
    else if(study_time==4)
    {
        if(q->name=="Chenxi")
        {
            cout<<"Your wisdom earns you a place on a combined postgraduate program."<<endl;
            cout<<"Choose a university for your master's and doctoral degrees:"<<endl
                <<"1. University of Cambridge"<<endl
                <<"2. University of Oxford"<<endl
                <<"3. Yale University"<<endl
                <<"4. Harvard University"<<endl;
        }
        else if(q->name=="Haochen")
        {
            cout<<"Your wealth earns you a place on a combined postgraduate program."<<endl;
            cout<<"Choose a university for your master's and doctoral degrees:"<<endl
                <<"1. Stanford University"<<endl
                <<"2. KTH Royal Institute of Technology"<<endl
                <<"3. Technical University of Madrid"<<endl
                <<"4. Nanyang Technological University"<<endl;
        }
        else
        {
            cout<<"Your good looks earn you a place on a combined postgraduate program."<<endl;
            cout<<"Choose a university for your master's and doctoral degrees:"<<endl
                <<"1. Peking University"<<endl
                <<"2. Tsinghua University"<<endl
                <<"3. Zhejiang University"<<endl
                <<"4. Nanjing University"<<endl;
        }
    }
}
void assistant()
{
    Elem student1[10]={"Guan Yu","Zhang Fei","Liu Bei","Ma Chao","Huang Zhong"
    ,"Zhao Yun","Jiang Wei","Zhuge Liang","Xu Shu","Jiang Yi"};
    Elem student2[5]={"Cao Cao","Cao Zhi","Cao Pi","Cao Chong","Cao Ang"};
    LinkQueue finished;
    SNode *waiting,*sudden;
    InitStack(waiting);
    InitStack(sudden);
    InitQueue(finished);
    for(int i=0;i<10;i++)
        Push(waiting,student1[i]);
    for(int i=0;i<5;i++)
        Push(sudden,student2[i]);
    cout<<"You are a teaching assistant. Help your supervisor grade the homework!"<<endl
        <<"1. Grade individually  "<<"2. Grade all automatically"<<endl;
    check(2,testnum);
    bool automatic=(*testnum==2);
    // Pending submissions use a stack; graded names and marks leave in FIFO order.
    while(!EmptyStack(waiting))
    {
        if(!automatic && rand()%N<=3 && sudden!=NULL)
        {
            Push(waiting,Pop(sudden));
            cout<<Peek(waiting)<<" submitted their homework!"<<endl;
        }
        if(!automatic)
        {
            cout<<Peek(waiting)<<"'s homework grade:"<<endl
                <<"1. Excellent  2. Good  3. Pass  4. Fail  5. Grade remaining automatically"<<endl;
            check(5,testnum);
            automatic=(*testnum==5);
        }
        int num=automatic ? 1+rand()%4 : (int)*testnum;
        const char *grades[]={"Excellent","Good","Pass","Fail"};
        EnQueue(finished,Pop(waiting));
        EnQueue(finished,grades[num-1]);
    }
    ClearStack(sudden);
    cout<<"All homework has been graded!"<<endl<<"Homework results:"<<endl
        <<setw(16)<<setiosflags(ios::left)<<"Name"
        <<"Grade"<<endl;
    while(!EmptyQueue(finished))
    {
        Elem name=OutQueue(finished);
        Elem grade=OutQueue(finished);
        cout<<setw(16)<<setiosflags(ios::left)<<name<<grade<<endl;
    }
}
void attribute(Role *q)
{
    if(study_time==0)
    {
        if(q->name=="Chenxi")
        {
          if(rand()%N<=3)
          {
            cout<<"You completed four years of bachelor's studies and met a great teacher!"<<endl
                <<"Wisdom +50, experience +20, wealth -20."<<endl<<endl;
            q->wisdom+=50;
            q->experience+=20;
            q->wealth-=20;
          }
          else
          {
            cout<<"You completed four years of bachelor's studies."<<endl
                <<"Wisdom +30, experience +10, wealth -40."<<endl<<endl;
            q->wisdom+=30;
            q->experience+=10;
            q->wealth-=40;
          }
        }
        else if(q->name=="Haochen")
        {
            if(rand()%N<=1&&*testnum>=3)
            {
            cout<<"You completed four years of bachelor's studies and met a great teacher!"<<endl
                <<"Wisdom +40, experience +30, wealth -100."<<endl<<endl;
            q->wisdom+=40;
            q->experience+=30;
            q->wealth-=100;
            }
            else if(*testnum<=2)
            {
            cout<<"You completed four years of bachelor's studies."<<endl
                <<"Wisdom +20, experience +10, wealth -40."<<endl<<endl;
            q->wisdom+=20;
            q->experience+=10;
            q->wealth-=40;
            }
            else
            {
            cout<<"You completed four years of bachelor's studies."<<endl
                <<"Wisdom +20, experience +15, wealth -100."<<endl<<endl;
            q->wisdom+=20;
            q->experience+=15;
            q->wealth-=100;
            }
        }
        else
        {
            if(rand()%N<=2)
            {
            cout<<"You completed four years of bachelor's studies and met a great teacher!"<<endl
                <<"Wisdom +45, experience +20, wealth -20."<<endl<<endl;
            q->wisdom+=45;
            q->experience+=20;
            q->wealth-=20;
            }
            else
            {
            cout<<"You completed four years of bachelor's studies."<<endl
                <<"Wisdom +25, experience +10, wealth -40."<<endl<<endl;
            q->wisdom+=25;
            q->experience+=10;
            q->wealth-=40;
            }
        }
        q->age+=4;
        study_time+=4;
    }
    else if(study_time==4)
    {
        if(q->name=="Chenxi")
        {
            if(rand()%N<=3)
            {
            cout<<"You completed five years of postgraduate studies and met a great supervisor!"<<endl
                <<"Wisdom +60, experience +30, wealth set to 0."<<endl<<endl;
            q->wisdom+=60;
            q->experience+=30;
            q->wealth=0;
            }
            else
            {
            cout<<"You completed five years of postgraduate studies."<<endl
                <<"Wisdom +40, experience +20, wealth set to 0."<<endl<<endl;
            q->wisdom+=40;
            q->experience+=20;
            q->wealth=0;
            }
        }
        else if(q->name=="Haochen")
        {
            if(rand()%N<=1)
            {
            cout<<"You completed five years of postgraduate studies and met a great supervisor!"<<endl
                <<"Wisdom +60, experience +30, wealth set to 20."<<endl<<endl;
            q->wisdom+=60;
            q->experience+=30;
            q->wealth=20;
            }
            else
            {
            cout<<"You completed five years of postgraduate studies."<<endl
                <<"Wisdom +40, experience +20, wealth set to 20."<<endl<<endl;
            q->wisdom+=40;
            q->experience+=20;
            q->wealth=20;
            }
        }
        else
        {
            if(rand()%N<=2)
            {
            cout<<"You completed five years of postgraduate studies and met a great supervisor!"<<endl
                <<"Wisdom +55, experience +30, wealth set to 10."<<endl<<endl;
            q->wisdom+=55;
            q->experience+=30;
            q->wealth=10;
            }
            else
            {
            cout<<"You completed five years of postgraduate studies."<<endl
                <<"Wisdom +35, experience +20, wealth set to 10."<<endl<<endl;
            q->wisdom+=35;
            q->experience+=20;
            q->wealth=10;
            }
        }
        q->age+=5;
        study_time+=5;
    }
}
void study(Role *q)
{
    if(study_time==9)
    {
        cout<<"You have completed your studies. Try working instead!"<<endl<<endl;
    }
    else
    {
        school(q);
        check(4,testnum);
        if(study_time==4)
          assistant();
        attribute(q);
    }
}
int work(Role *q)
{
    static bool flag=true;
    if(q->age==18)
    {
        cout<<"You are old enough for university. Study before you start working."
            <<endl<<endl;
        return 0;
    }
    else if(q->age>=60)
    {
        cout<<"You are at least 60 years old. It is time to retire!"<<endl<<endl;
        return 0;
    }
    if(study_time==4)
    {
        if(flag==true)
        {
          cout<<"You have become the CEO of a publicly listed company."<<endl
           <<"With only a bachelor's degree, you must pursue postgraduate studies after this period of work "
           <<"to continue at the company."<<endl;
           flag=false;
        }
        else
        {
           cout<<"You must complete postgraduate studies now!"<<endl;
           study(q);
           // The staff also age during the five years spent studying.
           *testnum=6;
           company(5);
           if(q->age>=60)
           {
            cout<<"You are at least 60 years old. It is time to retire!"<<endl<<endl;
            return 0;
           }
        }
    }
    cout<<"Choose how many years to work (1-5):"<<endl;
    check(5,testnum);
    int worktime=*testnum;
    do{
        cout<<"Choose a management action:"<<endl
        <<"1. Hire employees"<<endl<<"2. Dismiss an employee"<<endl
        <<"3. View all employees"<<endl
        <<"4. Sort employees"<<endl
        <<"5. Change an employee's salary"<<endl
        <<"6. Finish working"<<endl;
        check(6,testnum);
        company(worktime);
    }while(*testnum!=6);
    if(q->name=="Chenxi")
    {
        if(rand()%N<=3)
        {
        cout<<endl<<"Your excellent work was recognized! After "<<worktime
            <<" years of work:"<<endl<<"Wealth +"<<worktime*30
            <<", experience +"<<worktime*15<<endl<<endl;
        q->experience+=worktime*15;
        q->wealth+=worktime*30;
        }
        else
        {
        cout<<endl<<"After "<<worktime<<" years of work:"<<endl
            <<"Wealth +"<<worktime*25
            <<", experience +"<<worktime*12<<endl;
        q->experience+=worktime*12;
        q->wealth+=worktime*25;
        }
    }
    else if(q->name=="Haochen")
    {
        if(rand()%N<=1)
        {
        cout<<endl<<"Your excellent work was recognized! After "<<worktime
            <<" years of work:"<<endl
            <<"Wealth +"<<worktime*30
            <<", experience +"<<worktime*15<<endl<<endl;
        q->experience+=worktime*15;
        q->wealth+=worktime*30;
        }
        else
        {
        cout<<endl<<"After "<<worktime<<" years of work:"<<endl
            <<"Wealth +"<<worktime*20
            <<", experience +"<<worktime*10<<endl;
        q->experience+=worktime*10;
        q->wealth+=worktime*20;
        }
    }
    else
    {
        if(rand()%N<=2)
        {
        cout<<endl<<"Your excellent work was recognized! After "<<worktime
            <<" years of work:"<<endl
            <<"Wealth +"<<worktime*30
            <<", experience +"<<worktime*20<<endl<<endl;
        q->experience+=worktime*20;
        q->wealth+=worktime*30;
        }
        else
        {
        cout<<endl<<"After "<<worktime<<" years of work:"<<endl
            <<"Wealth +"<<worktime*20
            <<", experience +"<<worktime*10<<endl;
        q->experience+=worktime*10;
        q->wealth+=worktime*20;
        }
    }
    q->age+=worktime;
    return 0;
}
void ability(Role *q)
{
    cout<<"Age: "<<q->age<<"  Wisdom: "<<q->wisdom
        <<endl<<"Wealth: "<<q->wealth<<"  Experience: "
        <<q->experience<<endl<<endl;
}
void choose(Role *q)
{
    switch((int)*testnum)
    {
        case 1:study(q);break;
        case 2:work(q);break;
        case 3:ability(q);break;
        case 4:
#ifdef _WIN32
            system("cls");
#else
            cout<<"\033[2J\033[H"<<flush;
#endif
            break;
    }
}
void company(int worktime)
{
    // Keep the same staff between work periods.
    static bool once=true;
    static List staff;
    if(once)
    {
      staff.size=5;
      staff.list[0]={"LinFeng","Male",25,3,5000};
      staff.list[1]={"XiaoYan","Male",35,10,10000};
      staff.list[2]={"LinJing","Female",28,5,7000};
      staff.list[3]={"JiangWan","Male",36,12,12000};
      staff.list[4]={"LeYun","Female",23,1,4000};
      once=false;
    }
    ElemType item;
    // Recheck the same position after a removal: later employees shift left.
    for(int i=1;i<=staff.size;)
    {
        item=GetList(staff,i);
        if(item.age>=60)
        {
            cout<<item.name<<" is at least 60 years old and has retired."<<endl;
            DeleteList(staff,item,0);
        }
        else
            ++i;
    }
    if(EmptyList(staff))
    {
       cout<<"The company has no employees. Please hire someone!"<<endl;
       *testnum=1;
    }
    switch((int)*testnum)
    {
        case 1: {
                int available=MaxSize-staff.size;
                if(available==0)
                {
                    cout<<"The staff list is full; cannot hire."<<endl;
                    break;
                }
                int maxHire=available<5 ? available : 5;
                cout<<"How many employees would you like to hire (1-"<<maxHire<<")? ";
                check(maxHire,testnum);
                int count=(int)*testnum;
                cout<<"Enter each new employee's details:"<<endl
                    <<"Name  Sex  Age  Monthly salary"<<endl;
                for(int i=0;i<count;i++)
                {
                item.name=readToken();
                item.sex=readToken();
                item.age=readInteger();
                item.salary=readInteger();
                item.worktime=0;
                InsertList(staff,item,-1,0);
                }break; }
        case 2: TraverseList(staff);
                cerr<<"Enter the name of the employee to dismiss: ";
                item.name=readToken();
                while(!FindList(staff,item))
                {
                    cout<<"Employee not found. Try again."<<endl;
                    item.name=readToken();
                }
                if(item.worktime>=10)
                {
                  cout<<item.name<<" has extensive experience. Dismiss this employee?"
                      <<endl<<"1. Dismiss"<<endl
                      <<"2. Keep"<<endl;
                  check(2,testnum);
                  if(*testnum==2) break;
                }
                DeleteList(staff,item,0);
                break;
        case 3: TraverseList(staff);break;
        case 4: cout<<"Sort by:"<<endl
                    <<"1. Age  2. Years of service  3. Monthly salary"<<endl;
                check(3,testnum);
                SortList(staff,(int)*testnum);
                TraverseList(staff);break;
        case 5: TraverseList(staff);
                int extra;
                cerr<<"Enter the name of the employee: ";
                item.name=readToken();
                while(!FindList(staff,item))
                {
                    cout<<"Employee not found. Try again."<<endl;
                    item.name=readToken();
                }
                cerr<<"Monthly salary change: ";
                extra=readInteger();
                if(extra<=0)  cout<<"That is not a raise!"<<endl;
                item.salary+=extra;
                UpdateList(staff,item);
                if(extra>0)  cout<<"Salary increased!"<<endl<<item<<endl;
                else if(extra==0)  cout<<"The salary is unchanged."<<endl<<item<<endl;
                else   cout<<"That was a pay cut!"<<endl<<item<<endl;
                break;
        case 6: staff+=worktime;break;
    }
}
