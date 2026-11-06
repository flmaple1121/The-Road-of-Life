#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<iomanip>
#include<sstream>
#include<cctype>
#include"game.h"
#include"list.h"
#include"Stack2.h"
#include"LinkQueue.h"
#define N 10
using namespace std;
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
void choose(Role *);
void company(int );
void school(Role *);
void assistant();
void attribute(Role *);
void study(Role *);
int work(Role *);
void displayConsoleStatus(Role *);
float *testnum;
// Study progresses from no degree (0) to bachelor's (4) to postgraduate (9).
int study_time=0;
static bool first_work=true;
static bool staff_initialized=false;
static List staff;
static GameObserver* game_observer=nullptr;
static StaffForms* staff_forms=nullptr;
struct InputClosed {};

int runGame(GameObserver* observer, StaffForms* forms)
{
    game_observer=observer;
    staff_forms=forms;
    study_time=0;
    first_work=true;
    staff_initialized=false;
    srand((unsigned)time(0));
    float test;
    testnum=&test;
    Role  r[3]={{"Chenxi","Male",18,150,60,30},
                {"Haochen","Male",18,110,150,40},
                {"Yuyao","Female",18,120,90,20}};
    Role *q;
    q=r;
    try {
    cout<<"Welcome to The Road of Life! Who would you like to play as?"<<endl;
    for(int i=0;i<3;i++,q++)
        cout<<i+1<<". "<<q->name<<endl;
    check(3,testnum);
    q=r+(int)*testnum-1;
    if(game_observer) game_observer->statusChanged(*q);
    cout<<"Name: "<<q->name<<"  Sex: "<<q->sex
        <<"  Age: "<<q->age<<endl<<"Wisdom: "
        <<q->wisdom<<"  Wealth: "<<q->wealth
        <<"  Experience: "<<q->experience<<endl<<endl;
    while(1)
    {
        cout<<"Choose an action:"<<endl<<"1. Study"<<endl
        <<"2. Work"<<endl<<"3. Clear the conversation"<<endl
        <<"4. End your journey" <<endl;
        check(4,testnum);
        if(*testnum==4)
        {
            cout<<"Your journey has come to an end."<<endl
                <<"Name: "<<q->name<<"  Sex: "<<q->sex
                <<"  Age: "<<q->age<<endl<<"Wisdom: "
                <<q->wisdom<<"  Wealth: "<<q->wealth
                <<"  Experience: "<<q->experience<<endl
                <<"Game over."<<endl;
            break;
        }
        choose(q);
        if(!game_observer) displayConsoleStatus(q);
    }
    } catch(const InputClosed&) {
        cout<<endl<<"Input closed. Goodbye."<<endl;
    }
    return 0;
}
#ifndef ROAD_OF_LIFE_GUI
int main()
{
    return runGame();
}
#endif
// Read tokens so malformed input can be retried and EOF ends the game cleanly.
string readToken()
{
    string token;
    if(cin>>token)
        return token;
    throw InputClosed();
}
bool parseIntegerInRange(const string& text, int minimum, int maximum, int& value)
{
    const string whitespace=" \t\r\n";
    size_t first=text.find_first_not_of(whitespace);
    if(first==string::npos) return false;
    size_t last=text.find_last_not_of(whitespace);
    string input=text.substr(first,last-first+1);
    string normalized;
    size_t pos=0;
    if(input[pos]=='+' || input[pos]=='-') normalized+=input[pos++];
    if(pos==input.size()) return false;
    int group=0;
    bool comma=false;
    for(;pos<input.size();++pos)
    {
        unsigned char c=static_cast<unsigned char>(input[pos]);
        if(c>='0' && c<='9') { normalized+=static_cast<char>(c); ++group; }
        else if(c==',')
        {
            if(group==0 || (!comma && group>3) || (comma && group!=3)) return false;
            comma=true;
            group=0;
        }
        else return false;
    }
    if(group==0 || (comma && group!=3)) return false;
    istringstream stream(normalized);
    long long parsed;
    char extra;
    if(!(stream>>parsed) || (stream>>extra) || parsed<minimum || parsed>maximum) return false;
    value=static_cast<int>(parsed);
    return true;
}
static bool validName(const string& name)
{
    if(name.find_first_not_of(" \t\r\n")==string::npos || name.find_first_of("\r\n\t")!=string::npos)
        return false;
    size_t characters=0;
    for(size_t i=0;i<name.size();++i)
        if((static_cast<unsigned char>(name[i]) & 0xc0)!=0x80) ++characters;
    return characters<=40;
}
bool validateNewEmployee(const Staff& employee, string& error)
{
    if(!validName(employee.name)) error="Enter a name of 1-40 characters.";
    else if(employee.sex!="Male" && employee.sex!="Female" && employee.sex!="Other")
        error="Choose Male, Female, or Other.";
    else if(employee.age<MIN_EMPLOYEE_AGE || employee.age>MAX_EMPLOYEE_AGE)
        error="Age must be between 18 and 59.";
    else if(employee.salary<0 || employee.salary>MAX_MONTHLY_SALARY)
        error="Monthly salary must be between 0 and 1,000,000.";
    else { error.clear(); return true; }
    return false;
}
void check(int n,float *choice)
{
    while(true)
    {
        int value;
        if(parseIntegerInRange(readToken(),1,n,value))
        {
            *choice=(float)value;
            return;
        }
        cout<<"Please enter a whole number from 1 to "<<n<<"."<<endl;
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
            cout<<"Your academic strengths have opened the door to a combined master's and PhD program."<<endl;
            cout<<"Choose a university for your master's and doctoral degrees:"<<endl
                <<"1. University of Cambridge"<<endl
                <<"2. University of Oxford"<<endl
                <<"3. Yale University"<<endl
                <<"4. Harvard University"<<endl;
        }
        else if(q->name=="Haochen")
        {
            cout<<"With your finances, a combined master's and PhD program is within reach."<<endl;
            cout<<"Choose a university for your master's and doctoral degrees:"<<endl
                <<"1. Stanford University"<<endl
                <<"2. KTH Royal Institute of Technology"<<endl
                <<"3. Technical University of Madrid"<<endl
                <<"4. Nanyang Technological University"<<endl;
        }
        else
        {
            cout<<"A combined master's and PhD program is now within reach."<<endl;
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
    cout<<"You've become a teaching assistant. Your supervisor needs a hand grading homework."<<endl
        <<"1. Grade each submission"<<endl<<"2. Grade them all automatically"<<endl;
    check(2,testnum);
    bool automatic=(*testnum==2);
    // Pending submissions use a stack; graded names and marks leave in FIFO order.
    while(!EmptyStack(waiting))
    {
        if(!automatic && rand()%N<=3 && sudden!=NULL)
        {
            Push(waiting,Pop(sudden));
            cout<<Peek(waiting)<<" has just handed in their homework."<<endl;
        }
        if(!automatic)
        {
            cout<<"How would you grade "<<Peek(waiting)<<"'s homework?"<<endl
                <<"1. Excellent  2. Good  3. Pass  4. Fail"<<endl
                <<"5. Grade the rest automatically"<<endl;
            check(5,testnum);
            automatic=(*testnum==5);
        }
        int num=automatic ? 1+rand()%4 : (int)*testnum;
        const char *grades[]={"Excellent","Good","Pass","Fail"};
        EnQueue(finished,Pop(waiting));
        EnQueue(finished,grades[num-1]);
    }
    ClearStack(sudden);
    cout<<"That's all the homework marked. Here are the results:"<<endl<<"Homework results:"<<endl
        <<"Name | Grade"<<endl;
    while(!EmptyQueue(finished))
    {
        Elem name=OutQueue(finished);
        Elem grade=OutQueue(finished);
        cout<<name<<" | "<<grade<<endl;
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
            cout<<"After four years, you've earned your bachelor's degree and met a wonderful teacher along the way."<<endl
                <<"Wisdom +50, experience +20, wealth -20."<<endl<<endl;
            q->wisdom+=50;
            q->experience+=20;
            q->wealth-=20;
          }
          else
          {
            cout<<"After four years, you've earned your bachelor's degree."<<endl
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
            cout<<"After four years, you've earned your bachelor's degree and met a wonderful teacher along the way."<<endl
                <<"Wisdom +40, experience +30, wealth -100."<<endl<<endl;
            q->wisdom+=40;
            q->experience+=30;
            q->wealth-=100;
            }
            else if(*testnum<=2)
            {
            cout<<"After four years, you've earned your bachelor's degree."<<endl
                <<"Wisdom +20, experience +10, wealth -40."<<endl<<endl;
            q->wisdom+=20;
            q->experience+=10;
            q->wealth-=40;
            }
            else
            {
            cout<<"After four years, you've earned your bachelor's degree."<<endl
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
            cout<<"After four years, you've earned your bachelor's degree and met a wonderful teacher along the way."<<endl
                <<"Wisdom +45, experience +20, wealth -20."<<endl<<endl;
            q->wisdom+=45;
            q->experience+=20;
            q->wealth-=20;
            }
            else
            {
            cout<<"After four years, you've earned your bachelor's degree."<<endl
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
            cout<<"After five years, you've completed your postgraduate studies with the help of a great supervisor."<<endl
                <<"Wisdom +60, experience +30, wealth is now 0."<<endl<<endl;
            q->wisdom+=60;
            q->experience+=30;
            q->wealth=0;
            }
            else
            {
            cout<<"After five years, you've completed your postgraduate studies."<<endl
                <<"Wisdom +40, experience +20, wealth is now 0."<<endl<<endl;
            q->wisdom+=40;
            q->experience+=20;
            q->wealth=0;
            }
        }
        else if(q->name=="Haochen")
        {
            if(rand()%N<=1)
            {
            cout<<"After five years, you've completed your postgraduate studies with the help of a great supervisor."<<endl
                <<"Wisdom +60, experience +30, wealth is now 20."<<endl<<endl;
            q->wisdom+=60;
            q->experience+=30;
            q->wealth=20;
            }
            else
            {
            cout<<"After five years, you've completed your postgraduate studies."<<endl
                <<"Wisdom +40, experience +20, wealth is now 20."<<endl<<endl;
            q->wisdom+=40;
            q->experience+=20;
            q->wealth=20;
            }
        }
        else
        {
            if(rand()%N<=2)
            {
            cout<<"After five years, you've completed your postgraduate studies with the help of a great supervisor."<<endl
                <<"Wisdom +55, experience +30, wealth is now 10."<<endl<<endl;
            q->wisdom+=55;
            q->experience+=30;
            q->wealth=10;
            }
            else
            {
            cout<<"After five years, you've completed your postgraduate studies."<<endl
                <<"Wisdom +35, experience +20, wealth is now 10."<<endl<<endl;
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
        cout<<"You've completed your studies. It's time to put them to work!"<<endl<<endl;
    }
    else
    {
        school(q);
        check(4,testnum);
        if(study_time==4)
          assistant();
        attribute(q);
        if(game_observer) game_observer->statusChanged(*q);
    }
}
int work(Role *q)
{
    if(q->age==18)
    {
        cout<<"You'll need to complete your bachelor's degree before you can start working."
            <<endl<<endl;
        return 0;
    }
    else if(q->age>=60)
    {
        cout<<"You've reached retirement age. Time to enjoy a well-earned rest!"<<endl<<endl;
        return 0;
    }
    if(study_time==4)
    {
        if(first_work)
        {
          cout<<"You've become the CEO of a publicly listed company!"<<endl
           <<"To stay in the role, you'll need to begin postgraduate studies after this period of work."<<endl;
           first_work=false;
        }
        else
        {
           cout<<"It's time to begin the postgraduate studies you need for your role."<<endl;
           study(q);
           // The staff also age during the five years spent studying.
           *testnum=6;
           company(5);
           if(q->age>=60)
           {
            cout<<"You've reached retirement age. Time to enjoy a well-earned rest!"<<endl<<endl;
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
        cout<<endl<<"Your hard work has paid off! After "<<worktime
            <<(worktime==1 ? " year of work:" : " years of work:")<<endl<<"Wealth +"<<worktime*30
            <<", experience +"<<worktime*15<<endl<<endl;
        q->experience+=worktime*15;
        q->wealth+=worktime*30;
        }
        else
        {
        cout<<endl<<"After "<<worktime<<(worktime==1 ? " year of work:" : " years of work:")<<endl
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
        cout<<endl<<"Your hard work has paid off! After "<<worktime
            <<(worktime==1 ? " year of work:" : " years of work:")<<endl
            <<"Wealth +"<<worktime*30
            <<", experience +"<<worktime*15<<endl<<endl;
        q->experience+=worktime*15;
        q->wealth+=worktime*30;
        }
        else
        {
        cout<<endl<<"After "<<worktime<<(worktime==1 ? " year of work:" : " years of work:")<<endl
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
        cout<<endl<<"Your hard work has paid off! After "<<worktime
            <<(worktime==1 ? " year of work:" : " years of work:")<<endl
            <<"Wealth +"<<worktime*30
            <<", experience +"<<worktime*20<<endl<<endl;
        q->experience+=worktime*20;
        q->wealth+=worktime*30;
        }
        else
        {
        cout<<endl<<"After "<<worktime<<(worktime==1 ? " year of work:" : " years of work:")<<endl
            <<"Wealth +"<<worktime*20
            <<", experience +"<<worktime*10<<endl;
        q->experience+=worktime*10;
        q->wealth+=worktime*20;
        }
    }
    q->age+=worktime;
    if(game_observer) game_observer->statusChanged(*q);
    return 0;
}
void displayConsoleStatus(Role *q)
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
        case 3:
            if(game_observer)
                game_observer->clearConversation();
            else {
#ifdef _WIN32
            system("cls");
#else
            cout<<"\033[2J\033[H"<<flush;
#endif
            }
            break;
    }
}

// The terminal accepts either separate answers or comma-separated fields.
static string readFormToken()
{
    string token=readToken();
    if(!token.empty() && token[token.size()-1]==',') token.erase(token.size()-1);
    return token;
}
static bool isCancel(const string& token)
{
    string lower=token;
    for(size_t i=0;i<lower.size();++i)
        lower[i]=static_cast<char>(tolower(static_cast<unsigned char>(lower[i])));
    return lower=="cancel";
}
static bool readFormNumber(int minimum,int maximum,int& value,bool zeroCancels=false)
{
    while(true)
    {
        string token=readFormToken();
        if(isCancel(token) || (zeroCancels && token=="0")) return false;
        if(parseIntegerInRange(token,minimum,maximum,value)) return true;
        cout<<"Please enter a whole number from "<<minimum<<" to "<<maximum
            <<", or type cancel."<<endl;
    }
}
static bool confirmConsoleAction()
{
    cout<<"1. Confirm  0. Cancel"<<endl;
    int answer;
    return readFormNumber(1,1,answer,true);
}
static int choosePartialHires(const vector<Staff>& employees)
{
    if(employees.empty()) return 2;
    cout<<"You have completed "<<employees.size()<<" employee record(s)."<<endl
        <<"1. Hire the completed employees"<<endl
        <<"2. Discard them and cancel"<<endl
        <<"3. Continue entering details"<<endl;
    int choice;
    if(!readFormNumber(1,3,choice)) return 2;
    return choice;
}
static bool consoleEmployee(int number,int count,Staff& item)
{
    item.worktime=0;
    cout<<"Employee "<<number<<" of "<<count<<" - name (1-40 characters): ";
    while(true)
    {
        item.name=readFormToken();
        if(isCancel(item.name)) return false;
        if(validName(item.name)) break;
        cout<<"Enter a name of 1-40 characters, or type cancel: ";
    }
    cout<<"Sex (1. Male  2. Female  3. Other): ";
    while(true)
    {
        string sex=readFormToken();
        if(isCancel(sex)) return false;
        for(size_t j=0;j<sex.size();++j)
            sex[j]=static_cast<char>(tolower(static_cast<unsigned char>(sex[j])));
        if(sex=="1" || sex=="male") { item.sex="Male"; break; }
        if(sex=="2" || sex=="female") { item.sex="Female"; break; }
        if(sex=="3" || sex=="other") { item.sex="Other"; break; }
        cout<<"Choose Male, Female, or Other, or type cancel: ";
    }
    cout<<"Age (18-59): ";
    if(!readFormNumber(MIN_EMPLOYEE_AGE,MAX_EMPLOYEE_AGE,item.age)) return false;
    cout<<"Monthly salary (0-1,000,000): ";
    return readFormNumber(0,MAX_MONTHLY_SALARY,item.salary);
}
static bool consoleHire(int limit,vector<Staff>& employees)
{
    cout<<"How many employees would you like to hire (1-"<<limit<<", 0 to cancel)? ";
    int count;
    if(!readFormNumber(1,limit,count,true)) return false;
    cout<<"Enter each employee's name, sex, age, and monthly salary. "
        <<"Separate fields with spaces, or commas followed by spaces. Type cancel at any prompt."<<endl;
    for(int i=0;i<count;)
    {
        Staff item;
        if(consoleEmployee(i+1,count,item)) { employees.push_back(item); ++i; continue; }
        int choice=choosePartialHires(employees);
        if(choice==1) return true;
        if(choice==2) { employees.clear(); return false; }
    }
    while(true)
    {
        cout<<"Confirm hiring "<<count<<(count==1 ? " employee?" : " employees?")<<endl;
        if(confirmConsoleAction()) return true;
        int choice=choosePartialHires(employees);
        if(choice==1) return true;
        if(choice==2) { employees.clear(); return false; }
    }
}
static bool consoleSelectEmployee(const vector<Staff>& employees,int& index)
{
    cout<<"Enter the employee's name, or type cancel: ";
    while(true)
    {
        string name=readFormToken();
        if(isCancel(name)) return false;
        for(size_t i=0;i<employees.size();++i)
            if(employees[i].name==name) { index=static_cast<int>(i); return true; }
        cout<<"I couldn't find that employee. Try again, or type cancel: ";
    }
}

void company(int worktime)
{
    // Keep the same staff between work periods, and reset them for a new game.
    if(!staff_initialized)
    {
      staff.size=5;
      staff.list[0]={"LinFeng","Male",25,3,5000};
      staff.list[1]={"XiaoYan","Male",35,10,10000};
      staff.list[2]={"LinJing","Female",28,5,7000};
      staff.list[3]={"JiangWan","Male",36,12,12000};
      staff.list[4]={"LeYun","Female",23,1,4000};
      staff_initialized=true;
    }
    ElemType item;
    // Recheck the same position after a removal: later employees shift left.
    for(int i=1;i<=staff.size;)
    {
        item=GetList(staff,i);
        if(item.age>=60)
        {
            cout<<item.name<<" has reached retirement age and left the company."<<endl;
            DeleteList(staff,item,i);
        }
        else
            ++i;
    }
    switch((int)*testnum)
    {
        case 1: {
                int available=MaxSize-staff.size;
                if(available==0)
                {
                    cout<<"The staff list is full, so you can't hire anyone else."<<endl;
                    break;
                }
                int maxHire=available<5 ? available : 5;
                vector<Staff> pending;
                bool confirmed=staff_forms ? staff_forms->hire(maxHire,pending) : consoleHire(maxHire,pending);
                if(!confirmed)
                {
                    cout<<"Hiring canceled. No employees were hired."<<endl;
                    break;
                }
                string error;
                bool valid=!pending.empty() && pending.size()<=static_cast<size_t>(maxHire);
                for(size_t i=0;i<pending.size() && valid;++i)
                    valid=validateNewEmployee(pending[i],error);
                if(!valid)
                {
                    cout<<"No employees were hired: "<<(error.empty() ? "invalid number of records." : error)<<endl;
                    break;
                }
                cout<<"Hired "<<pending.size()<<(pending.size()==1 ? " employee:" : " employees:")<<endl;
                for(size_t i=0;i<pending.size();++i)
                {
                    pending[i].worktime=0;
                    InsertList(staff,pending[i],-1,0);
                    cout<<pending[i]<<endl;
                }
                break;
        }
        case 2: {
                if(EmptyList(staff)) { cout<<"There are no employees to dismiss."<<endl; break; }
                vector<Staff> employees(staff.list,staff.list+staff.size);
                int index=-1;
                bool confirmed;
                if(staff_forms) confirmed=staff_forms->dismiss(employees,index);
                else
                {
                    TraverseList(staff);
                    confirmed=consoleSelectEmployee(employees,index);
                    if(confirmed)
                    {
                        cout<<"Dismiss "<<employees[index].name<<"?"<<endl;
                        confirmed=confirmConsoleAction();
                    }
                }
                if(!confirmed) { cout<<"Dismissal canceled. No employee was dismissed."<<endl; break; }
                if(index<0 || index>=staff.size) { cout<<"Invalid employee selection."<<endl; break; }
                item=staff.list[index];
                DeleteList(staff,item,index+1);
                cout<<item.name<<" has been dismissed."<<endl;
                break;
        }
        case 3: TraverseList(staff);break;
        case 4: {
                cout<<"How would you like to sort the staff list?"<<endl
                    <<"1. Age  2. Years of service  3. Monthly salary  0. Cancel"<<endl;
                int choice;
                if(!readFormNumber(1,3,choice,true)) { cout<<"Sorting canceled."<<endl; break; }
                SortList(staff,choice);
                TraverseList(staff);
                break;
        }
        case 5: {
                if(EmptyList(staff)) { cout<<"There are no employees whose salary can be changed."<<endl; break; }
                vector<Staff> employees(staff.list,staff.list+staff.size);
                int index=-1,change=0;
                bool confirmed;
                if(staff_forms) confirmed=staff_forms->salaryChange(employees,index,change);
                else
                {
                    TraverseList(staff);
                    confirmed=consoleSelectEmployee(employees,index);
                    if(confirmed)
                    {
                        cout<<"Salary change ("<<-employees[index].salary<<" to "
                            <<MAX_MONTHLY_SALARY-employees[index].salary<<", or cancel): ";
                        confirmed=readFormNumber(-employees[index].salary,
                            MAX_MONTHLY_SALARY-employees[index].salary,change);
                    }
                    if(confirmed)
                    {
                        cout<<"Apply this salary change?"<<endl;
                        confirmed=confirmConsoleAction();
                    }
                }
                if(!confirmed) { cout<<"Salary change canceled."<<endl; break; }
                if(index<0 || index>=staff.size ||
                    static_cast<long long>(staff.list[index].salary)+change<0 ||
                    static_cast<long long>(staff.list[index].salary)+change>MAX_MONTHLY_SALARY)
                {
                    cout<<"Invalid employee or salary change; no salary was changed."<<endl;
                    break;
                }
                staff.list[index].salary+=change;
                item=staff.list[index];
                if(change>0) cout<<"Salary increased."<<endl<<item<<endl;
                else if(change==0) cout<<"The salary is unchanged."<<endl<<item<<endl;
                else cout<<"Salary decreased."<<endl<<item<<endl;
                break;
        }
        case 6: staff+=worktime;break;
    }
}
