#include "staff_dialog_win32.h"
#include "win_text.h"

#include <algorithm>
#include <functional>
#include <string>

namespace {

enum FormKind { CountForm, EmployeeForm, DismissForm, SalaryForm, PartialForm };
enum { ID_NAME=1101, ID_SEX, ID_AGE, ID_SALARY, ID_EMPLOYEE, ID_ERROR, ID_HINT,
       ID_KEEP_COMPLETED, ID_DISCARD_ALL, ID_CONTINUE };

struct Draft {
    std::wstring name, age, salary;
    int sex;
    Draft() : sex(0) {}
};

struct Form {
    FormKind kind;
    int dpi, limit, number, total, completed, selection, change;
    HFONT font, heading;
    const std::vector<Staff>* employees;
    Staff employee;
    Draft draft;
    HWND name, sex, age, salary, employeeBox, error, hint;

    Form(FormKind type, int scale, HFONT body, HFONT title)
        : kind(type), dpi(scale), limit(0), number(0), total(0), completed(0),
          selection(-1), change(0), font(body), heading(title), employees(nullptr),
          employee(), name(nullptr), sex(nullptr), age(nullptr), salary(nullptr),
          employeeBox(nullptr), error(nullptr), hint(nullptr) {}
};

int px(const Form& form,int value) { return MulDiv(value,form.dpi,96); }

HWND control(HWND dialog,const Form& form,const wchar_t* type,const std::wstring& text,
             DWORD style,int id,int x,int y,int width,int height,DWORD extended=0)
{
    HWND item=CreateWindowExW(extended,type,text.c_str(),WS_CHILD|WS_VISIBLE|style,
        px(form,x),px(form,y),px(form,width),px(form,height),dialog,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),GetModuleHandleW(nullptr),nullptr);
    SendMessageW(item,WM_SETFONT,reinterpret_cast<WPARAM>(form.font),TRUE);
    return item;
}

void text(HWND dialog,const Form& form,const std::wstring& value,int x,int y,int width,int height)
{
    control(dialog,form,L"STATIC",value,0,0,x,y,width,height);
}

void heading(HWND dialog,const Form& form,const std::wstring& value)
{
    HWND item=control(dialog,form,L"STATIC",value,0,0,24,18,470,28);
    SendMessageW(item,WM_SETFONT,reinterpret_cast<WPARAM>(form.heading),TRUE);
}

HWND edit(HWND dialog,const Form& form,int id,int x,int y,int width,int maxLength)
{
    HWND item=control(dialog,form,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL,id,
        x,y,width,28,WS_EX_CLIENTEDGE);
    SendMessageW(item,EM_SETLIMITTEXT,maxLength,0);
    return item;
}

HWND combo(HWND dialog,const Form& form,int id,int x,int y,int width)
{
    return control(dialog,form,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST,
        id,x,y,width,210);
}

void button(HWND dialog,const Form& form,const std::wstring& value,int id,int x,int y,int width,bool primary)
{
    control(dialog,form,L"BUTTON",value,WS_TABSTOP|(primary ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON),
        id,x,y,width,34);
}

std::wstring windowText(HWND item)
{
    int length=GetWindowTextLengthW(item);
    std::wstring result(static_cast<size_t>(length)+1,L'\0');
    GetWindowTextW(item,&result[0],length+1);
    result.resize(static_cast<size_t>(length));
    return result;
}

void captureDraft(Form& form)
{
    form.draft.name=windowText(form.name);
    form.draft.age=windowText(form.age);
    form.draft.salary=windowText(form.salary);
    form.draft.sex=static_cast<int>(SendMessageW(form.sex,CB_GETCURSEL,0,0));
}

void error(Form& form,const std::wstring& message,HWND field)
{
    SetWindowTextW(form.error,message.c_str());
    if(field) SetFocus(field);
}

int selectedEmployee(const Form& form)
{
    int selected=static_cast<int>(SendMessageW(form.employeeBox,CB_GETCURSEL,0,0));
    return selected-1;
}

void updateEmployeeHint(Form& form)
{
    int index=selectedEmployee(form);
    if(index<0 || index>=static_cast<int>(form.employees->size()))
    {
        SetWindowTextW(form.hint,L"Choose an employee to see their details.");
        return;
    }
    const Staff& item=(*form.employees)[index];
    std::wstring message;
    if(form.kind==SalaryForm)
        message=L"Current salary: "+std::to_wstring(item.salary)+L". Allowed change: "+
            std::to_wstring(-item.salary)+L" to "+std::to_wstring(MAX_MONTHLY_SALARY-item.salary)+L".";
    else
        message=L"Age: "+std::to_wstring(item.age)+L"   Years of service: "+std::to_wstring(item.worktime)+
            L"   Monthly salary: "+std::to_wstring(item.salary)+L".";
    SetWindowTextW(form.hint,message.c_str());
}

void populateEmployees(Form& form)
{
    SendMessageW(form.employeeBox,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"Choose an employee..."));
    for(size_t i=0;i<form.employees->size();++i)
    {
        const Staff& item=(*form.employees)[i];
        std::wstring label=std::to_wstring(i+1)+L". "+wintext::widen(item.name)+
            L"  |  age "+std::to_wstring(item.age)+L"  |  salary "+std::to_wstring(item.salary);
        SendMessageW(form.employeeBox,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(label.c_str()));
    }
    SendMessageW(form.employeeBox,CB_SETCURSEL,0,0);
}

void layout(HWND dialog,Form& form)
{
    int width=500,height=240;
    std::wstring title;
    if(form.kind==CountForm) { title=L"Hire employees"; width=470; height=225; }
    else if(form.kind==EmployeeForm) { title=L"Employee details"; width=520; height=450; }
    else if(form.kind==DismissForm) { title=L"Dismiss an employee"; width=550; height=265; }
    else if(form.kind==SalaryForm) { title=L"Change monthly salary"; width=570; height=365; }
    else { title=L"Cancel hiring?"; width=570; height=240; }
    SetWindowTextW(dialog,title.c_str());
    RECT ownerRect={};
    GetWindowRect(GetWindow(dialog,GW_OWNER),&ownerRect);
    int x=ownerRect.left+(ownerRect.right-ownerRect.left-px(form,width))/2;
    int y=ownerRect.top+(ownerRect.bottom-ownerRect.top-px(form,height))/2;
    RECT workArea={};
    if(SystemParametersInfoW(SPI_GETWORKAREA,0,&workArea,0))
    {
        x=std::max(static_cast<int>(workArea.left),std::min(x,static_cast<int>(workArea.right)-px(form,width)));
        y=std::max(static_cast<int>(workArea.top),std::min(y,static_cast<int>(workArea.bottom)-px(form,height)));
    }
    SetWindowPos(dialog,nullptr,x,y,px(form,width),px(form,height),SWP_NOZORDER|SWP_NOACTIVATE);

    if(form.kind==CountForm)
    {
        heading(dialog,form,L"How many employees would you like to hire?");
        text(dialog,form,L"Number of employees (1-"+std::to_wstring(form.limit)+L")",24,57,400,22);
        form.employeeBox=combo(dialog,form,ID_EMPLOYEE,24,82,405);
        for(int i=1;i<=form.limit;++i)
        {
            std::wstring value=std::to_wstring(i);
            SendMessageW(form.employeeBox,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(value.c_str()));
        }
        SendMessageW(form.employeeBox,CB_SETCURSEL,0,0);
        button(dialog,form,L"Cancel",IDCANCEL,223,142,94,false);
        button(dialog,form,L"Next",IDOK,329,142,100,true);
        SetFocus(form.employeeBox);
    }
    else if(form.kind==EmployeeForm)
    {
        heading(dialog,form,L"Employee "+std::to_wstring(form.number)+L" of "+std::to_wstring(form.total));
        text(dialog,form,L"Name (1-40 characters)",24,53,450,20);
        form.name=edit(dialog,form,ID_NAME,24,76,454,40);
        text(dialog,form,L"Sex",24,112,450,20);
        form.sex=combo(dialog,form,ID_SEX,24,135,454);
        const wchar_t* sexes[]={L"Choose an option...",L"Male",L"Female",L"Other"};
        for(int i=0;i<4;++i) SendMessageW(form.sex,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(sexes[i]));
        text(dialog,form,L"Age (18-59)",24,171,450,20);
        form.age=edit(dialog,form,ID_AGE,24,194,454,12);
        text(dialog,form,L"Monthly salary (0-1,000,000)",24,230,450,20);
        form.salary=edit(dialog,form,ID_SALARY,24,253,454,20);
        form.error=control(dialog,form,L"STATIC",L"",0,ID_ERROR,24,290,454,42);
        button(dialog,form,L"Cancel",IDCANCEL,254,354,94,false);
        button(dialog,form,form.number==form.total ? L"Hire employees" : L"Next",IDOK,358,354,120,true);
        SetWindowTextW(form.name,form.draft.name.c_str());
        SetWindowTextW(form.age,form.draft.age.c_str());
        SetWindowTextW(form.salary,form.draft.salary.c_str());
        SendMessageW(form.sex,CB_SETCURSEL,form.draft.sex,0);
        SetFocus(form.name);
    }
    else if(form.kind==DismissForm || form.kind==SalaryForm)
    {
        heading(dialog,form,form.kind==DismissForm ? L"Choose an employee to dismiss" : L"Choose an employee and salary change");
        text(dialog,form,L"Employee",24,55,490,20);
        form.employeeBox=combo(dialog,form,ID_EMPLOYEE,24,78,width-48);
        populateEmployees(form);
        form.hint=control(dialog,form,L"STATIC",L"Choose an employee to see their details.",0,ID_HINT,24,116,width-48,38);
        if(form.kind==SalaryForm)
        {
            text(dialog,form,L"Salary change (positive or negative whole number)",24,161,width-48,20);
            form.salary=edit(dialog,form,ID_SALARY,24,184,width-48,20);
            text(dialog,form,L"The resulting monthly salary must be between 0 and 1,000,000.",24,219,width-48,35);
            form.error=control(dialog,form,L"STATIC",L"",0,ID_ERROR,24,252,width-48,40);
            button(dialog,form,L"Cancel",IDCANCEL,width-252,292,94,false);
            button(dialog,form,L"Apply change",IDOK,width-148,292,100,true);
        }
        else
        {
            form.error=control(dialog,form,L"STATIC",L"",0,ID_ERROR,24,154,width-48,30);
            button(dialog,form,L"Cancel",IDCANCEL,width-252,192,94,false);
            button(dialog,form,L"Dismiss",IDOK,width-148,192,100,true);
        }
        SetFocus(form.employeeBox);
    }
    else
    {
        heading(dialog,form,L"What would you like to do?");
        text(dialog,form,L"You have completed "+std::to_wstring(form.completed)+
            L" employee record(s). You can hire those employees, discard them, or continue entering details.",
            24,58,width-48,65);
        button(dialog,form,L"Discard all",ID_DISCARD_ALL,24,145,140,false);
        button(dialog,form,L"Continue",ID_CONTINUE,178,145,140,true);
        button(dialog,form,L"Hire completed",ID_KEEP_COMPLETED,332,145,188,false);
        SetFocus(GetDlgItem(dialog,ID_CONTINUE));
    }
}

INT_PTR CALLBACK formProcedure(HWND dialog,UINT message,WPARAM wparam,LPARAM lparam)
{
    Form* form=reinterpret_cast<Form*>(GetWindowLongPtrW(dialog,DWLP_USER));
    if(message==WM_INITDIALOG)
    {
        form=reinterpret_cast<Form*>(lparam);
        SetWindowLongPtrW(dialog,DWLP_USER,reinterpret_cast<LONG_PTR>(form));
        layout(dialog,*form);
        return FALSE;
    }
    if(!form) return FALSE;
    if(message==WM_CTLCOLORSTATIC && reinterpret_cast<HWND>(lparam)==form->error)
    {
        SetTextColor(reinterpret_cast<HDC>(wparam),RGB(170,35,35));
        SetBkMode(reinterpret_cast<HDC>(wparam),TRANSPARENT);
        return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_BTNFACE));
    }
    if(message==WM_CLOSE)
    {
        if(form->kind==EmployeeForm) captureDraft(*form);
        EndDialog(dialog,form->kind==PartialForm ? ID_CONTINUE : IDCANCEL);
        return TRUE;
    }
    if(message!=WM_COMMAND) return FALSE;
    int id=LOWORD(wparam);
    if(id==ID_EMPLOYEE && HIWORD(wparam)==CBN_SELCHANGE && form->hint)
    {
        updateEmployeeHint(*form);
        return TRUE;
    }
    if(id==IDCANCEL)
    {
        if(form->kind==EmployeeForm) captureDraft(*form);
        EndDialog(dialog,form->kind==PartialForm ? ID_CONTINUE : IDCANCEL);
        return TRUE;
    }
    if(form->kind==PartialForm && (id==ID_KEEP_COMPLETED || id==ID_DISCARD_ALL || id==ID_CONTINUE))
    {
        EndDialog(dialog,id);
        return TRUE;
    }
    if(id!=IDOK) return FALSE;
    if(form->kind==CountForm)
    {
        form->selection=static_cast<int>(SendMessageW(form->employeeBox,CB_GETCURSEL,0,0))+1;
        EndDialog(dialog,IDOK);
        return TRUE;
    }
    if(form->kind==EmployeeForm)
    {
        captureDraft(*form);
        std::wstring name=wintext::trim(form->draft.name);
        if(name.empty() || name.size()>40) { error(*form,L"Enter a name of 1-40 characters.",form->name); return TRUE; }
        if(form->draft.sex<1 || form->draft.sex>3) { error(*form,L"Choose Male, Female, or Other.",form->sex); return TRUE; }
        int age,salary;
        if(!parseIntegerInRange(wintext::narrow(form->draft.age),MIN_EMPLOYEE_AGE,MAX_EMPLOYEE_AGE,age))
        { error(*form,L"Enter an age from 18 to 59.",form->age); return TRUE; }
        if(!parseIntegerInRange(wintext::narrow(form->draft.salary),0,MAX_MONTHLY_SALARY,salary))
        { error(*form,L"Enter a monthly salary from 0 to 1,000,000.",form->salary); return TRUE; }
        const char* sexes[]={"","Male","Female","Other"};
        form->employee=Staff{wintext::narrow(name),sexes[form->draft.sex],age,0,salary};
        std::string messageText;
        if(!validateNewEmployee(form->employee,messageText))
        { error(*form,wintext::widen(messageText),form->name); return TRUE; }
        EndDialog(dialog,IDOK);
        return TRUE;
    }
    if(form->kind==DismissForm || form->kind==SalaryForm)
    {
        int index=selectedEmployee(*form);
        if(index<0 || index>=static_cast<int>(form->employees->size()))
        { error(*form,L"Choose an employee first.",form->employeeBox); return TRUE; }
        if(form->kind==SalaryForm)
        {
            int current=(*form->employees)[index].salary;
            int change;
            if(!parseIntegerInRange(wintext::narrow(windowText(form->salary)),
                                    -current,MAX_MONTHLY_SALARY-current,change))
            {
                error(*form,L"Enter a whole-number change within the range shown above.",form->salary);
                return TRUE;
            }
            form->change=change;
        }
        form->selection=index;
        EndDialog(dialog,IDOK);
        return TRUE;
    }
    return FALSE;
}

#pragma pack(push,2)
struct EmptyTemplate {
    DLGTEMPLATE dialog;
    WORD menu;
    WORD windowClass;
    WORD title;
};
#pragma pack(pop)

EmptyTemplate dialogTemplate()
{
    EmptyTemplate dialog={};
    dialog.dialog.style=WS_POPUP|WS_CAPTION|WS_SYSMENU|DS_MODALFRAME;
    dialog.dialog.cx=250;
    dialog.dialog.cy=150;
    return dialog;
}

INT_PTR show(HWND owner,Form& form)
{
    alignas(DWORD) EmptyTemplate dialog=dialogTemplate();
    return DialogBoxIndirectParamW(GetModuleHandleW(nullptr),&dialog.dialog,owner,
        formProcedure,reinterpret_cast<LPARAM>(&form));
}

bool collectHireDetails(int total,int dpi,HFONT font,HFONT heading,std::vector<Staff>& employees,
                        const std::function<INT_PTR(Form&)>& present)
{
    std::vector<Staff> pending;
    Draft draft;
    for(int i=0;i<total;)
    {
        Form employee(EmployeeForm,dpi,font,heading);
        employee.number=i+1;
        employee.total=total;
        employee.draft=draft;
        if(present(employee)==IDOK)
        {
            pending.push_back(employee.employee);
            draft=Draft();
            ++i;
            continue;
        }
        draft=employee.draft;
        if(pending.empty()) return false;
        Form partial(PartialForm,dpi,font,heading);
        partial.completed=static_cast<int>(pending.size());
        INT_PTR choice=present(partial);
        if(choice==ID_KEEP_COMPLETED) { employees=pending; return true; }
        if(choice==ID_DISCARD_ALL || choice==-1) return false;
    }
    employees=pending;
    return true;
}

} // namespace

bool showHireForm(HWND owner,int dpi,HFONT font,HFONT heading,int limit,std::vector<Staff>& employees)
{
    Form count(CountForm,dpi,font,heading);
    count.limit=limit;
    if(show(owner,count)!=IDOK) return false;
    return collectHireDetails(count.selection,dpi,font,heading,employees,
        [owner](Form& form) { return show(owner,form); });
}

bool showDismissForm(HWND owner,int dpi,HFONT font,HFONT heading,const std::vector<Staff>& employees,int& index)
{
    Form form(DismissForm,dpi,font,heading);
    form.employees=&employees;
    if(show(owner,form)!=IDOK) return false;
    index=form.selection;
    return true;
}

bool showSalaryForm(HWND owner,int dpi,HFONT font,HFONT heading,const std::vector<Staff>& employees,int& index,int& change)
{
    Form form(SalaryForm,dpi,font,heading);
    form.employees=&employees;
    if(show(owner,form)!=IDOK) return false;
    index=form.selection;
    change=form.change;
    return true;
}
