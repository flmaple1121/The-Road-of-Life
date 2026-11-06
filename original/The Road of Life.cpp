#include<iostream>
#include<stdlib.h> 
#include<time.h>
#include<string>
#include<iomanip>
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
int check(int ,float *);
void choose(Role *);
void company(int );
void school(Role *);
void assistant();
void attribute(Role *);
void study(Role *);
int work(Role *);
void ability(Role *);
float *testnum;
int study_time=0;
int main()
{
	float test;
	testnum=&test;
	Role  r[3]={{"晨曦","男",18,150,60,30},
	            {"皓晨","男",18,110,150,40},
				{"雨媱","女",18,120,90,20}};
	Role *q;
	q=r;
	cout<<"请选择一个角色开启你的人生之路！"<<endl;
	for(int i=0;i<3;i++,q++)
	    cout<<i+1<<"."<<q->name<<" ";
	cout<<endl;
	check(3,testnum);
	q=r+(int)*testnum-1;
	cout<<"姓名："<<q->name<<"  性别："<<q->sex
	    <<"  年龄："<<q->age<<endl<<"智慧："
		<<q->wisdom<<"   财富："<<q->wealth
		<<"  阅历："<<q->experience<<endl<<endl;
	while(1)
	{
		cout<<"你有以下选择："<<endl<<"1.学习"<<endl
	    <<"2.找工作"<<endl<<"3.查看我现在的能力"<<endl 
		<<"4.清屏"<<endl<<"5.来世再选" <<endl;
	    check(5,testnum);
	    if(*testnum==5)
	    {
	    	cout<<"你的人生止步于此"<<endl
			    <<"姓名："<<q->name<<"  性别："<<q->sex
	            <<"  年龄："<<q->age<<endl<<"智慧："
		        <<q->wisdom<<"   财富："<<q->wealth
		        <<"  阅历："<<q->experience<<endl
				<<"游戏结束";
			break; 
		}
		choose(q); 
	} 
	return 0;
}
int check(int n,float *testnum)
{
	if(cin>>*testnum)
	{
	if((float)((int)*testnum)==*testnum)
	  {
	  	for(int i=1;i<=n;i++)
	  	     if((int)*testnum==i)
	  	        return 1;
	  	cout<<"输入数据有误，请重新输入！"<<endl;
	  	check(n,testnum);	 
	  }
	else
	{
		cout<<"输入数据有误，请重新输入！"<<endl;
		check(n,testnum);
	}
	}
	else
	{
		cout<<"输入数据有误，请重新输入！"<<endl;
		cin.clear();                          
		cin.sync(); 
		check(n,testnum);
	}   
}
void school(Role *q)
{
	if(study_time==0)
	{
		if(q->name=="晨曦")
		{
			cout<<"请选择你攻读学士学位要进入的大学："<<endl
			    <<"1.浙江大学"<<endl
			    <<"2.南京大学"<<endl
			    <<"3.清华大学"<<endl
			    <<"4.北京大学"<<endl;
		} 
		else if(q->name=="皓晨")
		{
			cout<<"请选择你攻读学士学位要进入的大学："<<endl
			    <<"1.暨南大学"<<endl
			    <<"2.华南师范大学"<<endl
			    <<"3.巴塞罗那大学"<<endl
			    <<"4.蔚蓝海岸大学"<<endl;
		}
		else
		{
			cout<<"请选择你攻读学士学位要进入的大学："<<endl
			    <<"1.中山大学"<<endl
			    <<"2.华南理工大学"<<endl
			    <<"3.厦门大学"<<endl
			    <<"4.武汉大学"<<endl;
		}
	}
	else if(study_time==4)
	{
		if(q->name=="晨曦")
		{
			cout<<"因为你的智慧值较高，可获得硕博连读的机会"<<endl;
		    cout<<"请选择你攻读硕士和博士学位要进入的大学："<<endl
		        <<"1.剑桥大学"<<endl
			    <<"2.牛津大学"<<endl
			    <<"3.耶鲁大学"<<endl
			    <<"4.哈佛大学"<<endl;
		} 
		else if(q->name=="皓晨")
		{
			cout<<"因为你的财富值较高，可获得硕博连读的机会"<<endl;
	    	cout<<"请选择你攻读硕士和博士学位要进入的大学："<<endl
		        <<"1.斯坦福大学"<<endl
			    <<"2.瑞典皇家理工大学"<<endl
			    <<"3.马德里理工大学"<<endl
			    <<"4.南洋理工大学"<<endl;
		}
		else
		{
			cout<<"因为你的颜值较高，可获得硕博连读的机会"<<endl;
		    cout<<"请选择你攻读硕士和博士学位要进入的大学："<<endl
		        <<"1.北京大学"<<endl
			    <<"2.清华大学"<<endl
			    <<"3.浙江大学"<<endl
			    <<"4.南京大学"<<endl;
		}
	}
}
void assistant()
{
	srand((unsigned)time(0));
	Elem student1[10]={"关羽","张飞","刘备","马超","黄忠"
	,"赵云","姜维","诸葛亮","徐庶","蒋祎"};
	Elem student2[5]={"曹操","曹植","曹丕","曹冲","曹昂"};
	LinkQueue finished;
	SNode *waiting,*sudden;
	int num;
	InitStack(waiting);
	InitStack(sudden);
	InitQueue(finished);
	for(int i=0;i<10;i++)
		Push(waiting,student1[i]);
	for(int i=0;i<5;i++)
	    Push(sudden,student2[i]);
	cout<<"你已成为导师的助教，请帮助导师批改作业！"<<endl
	    <<"1.仔细批改  "<<"2.一键批改"<<endl;
	check(2,testnum);
	int mark=*testnum;
	switch((int)*testnum)
	{
		case 1:while(!EmptyStack(waiting))
		       {
		       	   if(rand()%N<=3&&sudden!=NULL)
		       	   {
		       	   	Push(waiting,Pop(sudden));
		       	   	cout<<Peek(waiting)<<"同学来交作业了！"<<endl;
				   }
		       	   cout<<Peek(waiting)<<"同学的作业的等级"<<endl
		       	       <<"1.优秀  2.良好  3.及格  4.不及格  5.一键批改"<<endl;
		       	   check(5,testnum);
		case 2:    do
		       	   {
		       	   	if(*testnum==5||mark==2) num=1+rand()%4;
		       	    else num=*testnum;
					EnQueue(finished,Pop(waiting));
		       	   Elem grade;
		       	   switch(num)
		       	   {
		       	   case 1:grade="优秀";EnQueue(finished,grade);break; 
		       	   case 2:grade="良好";EnQueue(finished,grade);break;
		       	   case 3:grade="合格";EnQueue(finished,grade);break;
		       	   case 4:grade="不及格";EnQueue(finished,grade);break;
				   }
			       }while((mark==2||*testnum==5)&&waiting!=NULL);
			   }
			   cout<<"已经改完了哟！"<<endl<<"批改后学生的作业"<<endl
			       <<setw(8)<<setiosflags(ios::left)<<"姓名"
				   <<setw(8)<<setiosflags(ios::left)<<"成绩"<<endl;
			       
			   while(!EmptyQueue(finished))
			    {
			    	cout<<setw(8)<<setiosflags(ios::left)<<OutQueue(finished);
			    	cout<<setw(8)<<setiosflags(ios::left)<<OutQueue(finished)
					    <<endl;
				}break;
	} 
	   
}
void attribute(Role *q)
{
	if(study_time==0)
	{
		if(q->name=="晨曦")
	    {
		  if(rand()%N<=3)
		  {
			cout<<"你已完成4年本科学业，且恭喜你遇到了好老师"<<endl
			    <<"你的智慧值增加50，阅历增加20，财富减少20"<<endl<<endl;
			q->wisdom+=50;
			q->experience+=20;
			q->wealth-=20;
		  }
		  else
		  {
			cout<<"你已完成4年本科学业"<<endl
			    <<"你的智慧值增加30，阅历增加10，财富减少40"<<endl<<endl;
			q->wisdom+=30;
			q->experience+=10;
			q->wealth-=40;
		  }
	    }
	    else if(q->name=="皓晨")
		{
			if(rand()%N<=1&&*testnum>=3)
	    	{
			cout<<"你已完成4年本科学业，且恭喜你遇到了好老师"<<endl
			    <<"你的智慧值增加40，阅历增加30，财富减少100"<<endl<<endl;
			q->wisdom+=40;
			q->experience+=30;
			q->wealth-=100;
		    }
		    else if(*testnum<=2)
		    {
			cout<<"你已完成4年本科学业"<<endl
			    <<"你的智慧值增加20，阅历增加10，财富减少40"<<endl<<endl;
			q->wisdom+=20;
			q->experience+=10;
			q->wealth-=40;
		    }
		    else
		    {
			cout<<"你已完成4年本科学业"<<endl
			    <<"你的智慧值增加20，阅历增加15，财富减少100"<<endl<<endl;
			q->wisdom+=20;
			q->experience+=15;
			q->wealth-=100;
		    }
        } 
        else
        {
        	if(rand()%N<=2)
		    {
			cout<<"你已完成4年本科学业，且恭喜你遇到了好老师"<<endl
			    <<"你的智慧值增加45，阅历增加20，财富减少20"<<endl<<endl;
			q->wisdom+=45;
			q->experience+=20;
			q->wealth-=20;
		    }
		    else
		    {
			cout<<"你已完成4年本科学业"<<endl
			    <<"你的智慧值增加25，阅历增加10，财富减少40"<<endl<<endl;
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
    	if(q->name=="晨曦")
    	{
    		if(rand()%N<=3)
		    {
			cout<<"你已完成5年研究生学业，且恭喜你遇到了好导师"<<endl
			    <<"你的智慧值增加60，阅历增加30，财富减为0"<<endl<<endl;
			q->wisdom+=60;
			q->experience+=30;
			q->wealth=0;
	        }
	        else
	        {
		    cout<<"你已完成5年研究生学业"<<endl
			    <<"你的智慧值增加40，阅历增加20，财富减为0"<<endl<<endl;
		    q->wisdom+=40;
		    q->experience+=20;
		    q->wealth=0;
 	        }
		} 
		else if(q->name=="皓晨")
		{
			if(rand()%N<=1)
		    {
			cout<<"你已完成5年研究生学业，且恭喜你遇到了好导师"<<endl
			    <<"你的智慧值增加60，阅历增加30，财富减为20"<<endl<<endl;
			q->wisdom+=60;
			q->experience+=30;
			q->wealth=20;
	        }
	        else
	        {
		    cout<<"你已完成5年研究生学业"<<endl
			    <<"你的智慧值增加40，阅历增加20，财富减为20"<<endl<<endl;
		    q->wisdom+=40;
		    q->experience+=20;
		    q->wealth=20;
 	        }
		}
		else
		{
			if(rand()%N<=2)
		    {
			cout<<"你已完成5年研究生学业，且恭喜你遇到了好导师"<<endl
			    <<"你的智慧值增加55，阅历增加30，财富减为10"<<endl<<endl;
			q->wisdom+=55;
			q->experience+=30;
			q->wealth=10;
	        }
	        else
	        {
		    cout<<"你已完成5年研究生学业"<<endl
			    <<"你的智慧值增加35，阅历增加20，财富减为10"<<endl<<endl;
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
		cout<<"你的学历已经够高，还是找工作去吧！"<<endl<<endl;
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
		cout<<"你正处在进入大学学习的年纪，务必好好学习"
		    <<endl<<endl;
		return 0;
	}
	else if(q->age>=60)
	{
		cout<<"你已超过60岁，速速退休吧！"<<endl<<endl;
		return 0;		 
	}
	if(study_time==4)
    {
    	if(flag==true)
    	{
    	  cout<<"你成为了一家上市公司的CEO"<<endl
    	   <<"因为你仅有本科只有学历，所以在这次工作完成后请尽快修读研究生学业"
	       <<"以保证你能继续在这家公司工作"<<endl;
	       flag=false;
		}
		else
		{
		   cout<<"尽快修读研究生课程！"<<endl;
           study(q);
           *testnum=6;
           company(5);
		}
	} 
    cout<<"请选择你要工作的年限(每次最多连续工作5年)："<<endl; 
	check(5,testnum);
	int worktime=*testnum;
	do{
		cout<<"你有以下权力，请选择你要执行的操作："<<endl
	    <<"1.招聘员工"<<endl<<"2.解雇员工"<<endl
	    <<"3.查看所有员工的资料"<<endl
	    <<"4.将员工的资料按照一定顺序排列好"<<endl
	    <<"5.给干得好的员工加薪"<<endl
	    <<"6.结束工作"<<endl;
		check(6,testnum);
		company(worktime);
	}while(*testnum!=6);
	if(q->name=="晨曦")
	{
		if(rand()%N<=3)	
	    {
		cout<<endl<<"因你工作出色，受到老板赏识,经过这"<<worktime
		    <<"年的工作后"<<endl<<"你的财富值增加"<<worktime*30
			<<",阅历增加"<<worktime*15<<endl<<endl;
		q->experience+=worktime*15;
		q->wealth+=worktime*30;
	    }
	    else
	    {
		cout<<endl<<"经过这"<<worktime<<"年的工作后"<<endl
			<<"你的财富值增加"<<worktime*25
			<<",阅历增加"<<worktime*12<<endl;
		q->experience+=worktime*12;
		q->wealth+=worktime*25;
        }
	} 
	else if(q->name=="皓晨")
	{
		if(rand()%N<=1)	
	    {
		cout<<endl<<"因你工作出色，受到老板赏识,经过这"<<worktime
		    <<"年的工作后"<<endl
			<<"你的财富值增加"<<worktime*30
			<<",阅历增加"<<worktime*15<<endl<<endl;
		q->experience+=worktime*15;
		q->wealth+=worktime*30;
	    }
	    else
	    {
		cout<<endl<<"经过这"<<worktime<<"年的工作后"<<endl
			<<"你的财富值增加"<<worktime*20
			<<",阅历增加"<<worktime*10<<endl;
		q->experience+=worktime*10;
		q->wealth+=worktime*20;
        }
	}
	else
	{
		if(rand()%N<=2)	
	    {
		cout<<endl<<"因你工作出色，受到老板赏识,经过这"<<worktime
		    <<"年的工作后"<<endl
			<<"你的财富值增加"<<worktime*30
			<<",阅历增加"<<worktime*20<<endl<<endl;
		q->experience+=worktime*20;
		q->wealth+=worktime*30;
	    }
	    else
	    {
		cout<<endl<<"经过这"<<worktime<<"年的工作后"<<endl
			<<"你的财富值增加"<<worktime*20
			<<",阅历增加"<<worktime*10<<endl;
		q->experience+=worktime*10;
		q->wealth+=worktime*20;
        }
	}
	q->age+=worktime;
	return 0;
}
void ability(Role *q)
{
	cout<<"年龄："<<q->age<<"  智慧："<<q->wisdom
	    <<endl<<"财富："<<q->wealth<<"  阅历："
	    <<q->experience<<endl<<endl;
}
void choose(Role *q)
{
	srand((unsigned)time(0));
	switch((int)*testnum)
	{
	    case 1:study(q);break;
		case 2:work(q);break;
        case 3:ability(q);break;
        case 4:system("cls");break;
    }
}
void company(int worktime)
{
	static bool once=true;
	static List staff;
    if(once)
    {
      staff.size=5;
      staff.list[0]={"林峰","男",25,3,5000};
      staff.list[1]={"萧炎","男",35,10,10000};
      staff.list[2]={"林静","女",28,5,7000};
      staff.list[3]={"江晚","男",36,12,12000};
      staff.list[4]={"乐蕴","女",23,1,4000};
      once=false;
	}
    ElemType item;
    for(int i=1;i<=staff.size;i++)
    {
    	item=GetList(staff,i);
    	if(item.age>=60)
    	{
    		cout<<item.name<<"的年龄已超过60岁，自觉退休啦！"<<endl;
    		DeleteList(staff,item,0);
		}
	}
	if(EmptyList(staff))
	{
	   cout<<"公司没人啦，请尽快招聘新员工！"<<endl;
	   *testnum=1;
	}
    switch((int)*testnum)
	{
		case 1:	cout<<"请选择要招聘员工的个数(每次不超过5个)：";
			    check(5,testnum);
			    cout<<"请填写你所招聘员工的信息："<<endl
			        <<"姓名  性别  年龄  月薪"<<endl; 
			    for(int i=0;i<(int)*testnum;i++)
		        {
		    	cin>>item;
		    	InsertList(staff,item,-1,0);
			    }break;
		case 2: TraverseList(staff);
		        cerr<<"请输入名字选择你要解雇的员工:";
		        cin>>item.name;
		        while(!FindList(staff,item))
                {
                	cout<<"查无此人，重新输入！"<<endl;
                	cin>>item.name;
				}
		        DeleteList(staff,item,0);
		        if(item.worktime>=10)
		        { 
		          cout<<(item.sex=="男"?"他":"她")
                      <<"有着丰富的经验，你确定要解雇"
                      <<(item.sex=="男"?"他":"她") 
				      <<"吗？"<<endl<<"1.就是要解雇"<<endl
					  <<"2.算了不解雇了"<<endl;
				  check(2,testnum);
				  if(*testnum==2)
				    InsertList(staff,item,-1,0); 
				}break;
		case 3: TraverseList(staff);break;
		case 4: cout<<"以何种方式排序："<<endl
		            <<"1.年龄  2.工龄  3.月薪"<<endl;
		        check(3,testnum);
		        SortList(staff,(int)*testnum);
		        TraverseList(staff);break;
		case 5: TraverseList(staff);
		        int extra;
		        cerr<<"你选择加薪的员工是:";
		        cin>>item.name;
                while(!FindList(staff,item))
                {
                	cout<<"查无此人，重新输入！"<<endl;
                	cin>>item.name;
				}
		        cerr<<"要加多少月薪:";
		        cin>>extra;
		        if(extra<=0)  cout<<"说好的加薪呢？"<<endl;
		        item.salary+=extra;
		        UpdateList(staff,item);
		        if(extra>0)  cout<<"加薪成功！"<<endl<<item<<endl;
		        else if(extra==0)  cout<<"有区别吗？"<<endl<<item<<endl;
		        else   cout<<"居然是降薪，太狠了！"<<endl<<item<<endl;
	            break;
	    case 6: staff+=worktime;break;
	}           
}
