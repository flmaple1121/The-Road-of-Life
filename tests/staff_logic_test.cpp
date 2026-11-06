#include "../src/game.h"

#include <cassert>
#include <climits>
#include <iostream>
#include <sstream>
#include <vector>

struct Observer : GameObserver {
    void statusChanged(const Role&) override {}
    void clearConversation() override {}
};

struct Forms : StaffForms {
    bool cancel;
    bool invalidChange;
    explicit Forms(bool cancelled=false,bool invalid=false) : cancel(cancelled), invalidChange(invalid) {}

    bool hire(int limit,std::vector<Staff>& employees) override
    {
        assert(limit==5);
        employees.push_back(Staff{"Alex Smith","Male",20,0,20000});
        if(cancel) return false;
        employees.push_back(Staff{"Alex Smith","Other",30,0,30000});
        return true;
    }
    bool dismiss(const std::vector<Staff>& employees,int& index) override
    {
        if(cancel) return false;
        assert(employees.size()==7);
        index=6;
        return true;
    }
    bool salaryChange(const std::vector<Staff>& employees,int& index,int& change) override
    {
        if(cancel) return false;
        assert(employees.size()==6);
        index=5;
        change=invalidChange ? INT_MAX : 1000;
        return true;
    }
};

std::string play(Forms& forms)
{
    Observer observer;
    std::istringstream input("1\n1\n1\n2\n1\n1\n2\n5\n3\n6\n4\n");
    std::ostringstream output;
    std::streambuf* oldIn=std::cin.rdbuf(input.rdbuf());
    std::streambuf* oldOut=std::cout.rdbuf(output.rdbuf());
    std::streambuf* oldErr=std::cerr.rdbuf(output.rdbuf());
    std::cin.clear();
    int result=runGame(&observer,&forms);
    std::cin.rdbuf(oldIn);
    std::cout.rdbuf(oldOut);
    std::cerr.rdbuf(oldErr);
    assert(result==0);
    return output.str();
}

int main()
{
    int value=0;
    assert(parseIntegerInRange("20,000",0,MAX_MONTHLY_SALARY,value) && value==20000);
    assert(parseIntegerInRange(" -1,000 ",-1000,1000,value) && value==-1000);
    assert(parseIntegerInRange("1,000,000",0,MAX_MONTHLY_SALARY,value) && value==1000000);
    assert(!parseIntegerInRange("1,000,001",0,MAX_MONTHLY_SALARY,value));
    assert(!parseIntegerInRange("2,0,000",0,MAX_MONTHLY_SALARY,value));
    assert(!parseIntegerInRange("20,",0,MAX_MONTHLY_SALARY,value));
    assert(!parseIntegerInRange("20.5",0,MAX_MONTHLY_SALARY,value));
    assert(!parseIntegerInRange("999999999999999999999999999999",0,MAX_MONTHLY_SALARY,value));

    std::string error;
    Staff person={"A","Male",20,0,20000};
    assert(validateNewEmployee(person,error));
    person.age=17; assert(!validateNewEmployee(person,error));
    person.age=60; assert(!validateNewEmployee(person,error));
    person.age=59; person.salary=MAX_MONTHLY_SALARY+1; assert(!validateNewEmployee(person,error));
    person.salary=-1; assert(!validateNewEmployee(person,error));
    person.salary=0; person.sex="Unknown"; assert(!validateNewEmployee(person,error));
    person.sex="Other"; person.name=" "; assert(!validateNewEmployee(person,error));

    Forms success;
    std::string output=play(success);
    assert(output.find("Hired 2 employees:")!=std::string::npos);
    assert(output.find("Alex Smith | Male | 20 | 0 | 21000")!=std::string::npos);
    assert(output.find("Alex Smith | Other | 30 | 0 | 30000",output.find("has been dismissed"))==std::string::npos);

    Forms cancelled(true);
    output=play(cancelled);
    assert(output.find("Hiring canceled. No employees were hired.")!=std::string::npos);
    assert(output.find("Dismissal canceled. No employee was dismissed.")!=std::string::npos);
    assert(output.find("Salary change canceled.")!=std::string::npos);
    assert(output.find("Alex Smith | Male")==std::string::npos);
    assert(output.find("LinFeng | Male | 25 | 3 | 5000")!=std::string::npos);

    Forms invalid(false,true);
    output=play(invalid);
    size_t rejected=output.find("Invalid employee or salary change; no salary was changed.");
    assert(rejected!=std::string::npos);
    assert(output.find("Alex Smith | Male | 20 | 0 | 20000",rejected)!=std::string::npos);
}
