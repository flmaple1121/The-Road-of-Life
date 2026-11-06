// Instantiate the app's own dialogs without showing them on the desktop.
#include "../src/staff_dialog_win32.cpp"

#include <cassert>

HFONT testFont(int dpi,int weight=FW_NORMAL)
{
    return CreateFontW(-MulDiv(14,dpi,96),0,0,0,weight,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
}

HWND createForm(HWND owner,Form& form)
{
    alignas(DWORD) EmptyTemplate data=dialogTemplate();
    return CreateDialogIndirectParamW(GetModuleHandleW(nullptr),&data.dialog,owner,
        formProcedure,reinterpret_cast<LPARAM>(&form));
}

void fits(HWND dialog,HWND item)
{
    assert(item!=nullptr);
    RECT client,rect;
    GetClientRect(dialog,&client);
    GetWindowRect(item,&rect);
    MapWindowPoints(nullptr,dialog,reinterpret_cast<POINT*>(&rect),2);
    assert(rect.left>=0 && rect.top>=0 && rect.right<=client.right && rect.bottom<=client.bottom);
}

int main()
{
    HWND owner=CreateWindowExW(0,L"STATIC",L"",WS_POPUP,0,0,900,700,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    assert(owner!=nullptr);
    const int scales[]={96,144};
    for(int dpi : scales)
    {
        HFONT font=testFont(dpi),bold=testFont(dpi,FW_BOLD);
        Form count(CountForm,dpi,font,bold);
        count.limit=5;
        HWND dialog=createForm(owner,count);
        assert(dialog!=nullptr);
        assert(SendMessageW(count.employeeBox,CB_GETCOUNT,0,0)==5);
        fits(dialog,count.employeeBox);
        fits(dialog,GetDlgItem(dialog,IDCANCEL));
        fits(dialog,GetDlgItem(dialog,IDOK));
        DestroyWindow(dialog);

        Form employee(EmployeeForm,dpi,font,bold);
        employee.number=1; employee.total=2;
        dialog=createForm(owner,employee);
        assert(dialog!=nullptr);
        fits(dialog,employee.name); fits(dialog,employee.sex);
        fits(dialog,employee.age); fits(dialog,employee.salary);
        fits(dialog,employee.error); fits(dialog,GetDlgItem(dialog,IDCANCEL));
        fits(dialog,GetDlgItem(dialog,IDOK));
        assert((GetWindowLongW(employee.name,GWL_STYLE) & (WS_VISIBLE|WS_TABSTOP))==(WS_VISIBLE|WS_TABSTOP));
        assert((GetWindowLongW(employee.name,GWL_EXSTYLE) & WS_EX_CLIENTEDGE)!=0);
        assert((GetWindowLongW(employee.age,GWL_EXSTYLE) & WS_EX_CLIENTEDGE)!=0);
        assert((GetWindowLongW(employee.salary,GWL_EXSTYLE) & WS_EX_CLIENTEDGE)!=0);
        assert(SendMessageW(employee.sex,CB_GETCOUNT,0,0)==4);
        SetWindowTextW(employee.name,L"A");
        formProcedure(dialog,WM_COMMAND,MAKEWPARAM(IDOK,BN_CLICKED),0);
        assert(windowText(employee.error).find(L"Choose Male, Female, or Other")!=std::wstring::npos);
        SendMessageW(employee.sex,CB_SETCURSEL,1,0);
        SetWindowTextW(employee.age,L"17");
        SetWindowTextW(employee.salary,L"20,000");
        formProcedure(dialog,WM_COMMAND,MAKEWPARAM(IDOK,BN_CLICKED),0);
        assert(windowText(employee.error).find(L"18 to 59")!=std::wstring::npos);
        SetWindowTextW(employee.age,L"20");
        SetWindowTextW(employee.salary,L"20,00");
        formProcedure(dialog,WM_COMMAND,MAKEWPARAM(IDOK,BN_CLICKED),0);
        assert(windowText(employee.error).find(L"0 to 1,000,000")!=std::wstring::npos);
        DestroyWindow(dialog);

        std::vector<Staff> staff;
        staff.push_back(Staff{"A","Male",20,0,5000});
        Form salary(SalaryForm,dpi,font,bold);
        salary.employees=&staff;
        dialog=createForm(owner,salary);
        assert(dialog!=nullptr);
        fits(dialog,salary.employeeBox); fits(dialog,salary.salary);
        fits(dialog,salary.error); fits(dialog,GetDlgItem(dialog,IDCANCEL));
        fits(dialog,GetDlgItem(dialog,IDOK));
        SendMessageW(salary.employeeBox,CB_SETCURSEL,1,0);
        updateEmployeeHint(salary);
        assert(windowText(salary.hint).find(L"-5000 to 995000")!=std::wstring::npos);
        SetWindowTextW(salary.salary,L"-5,001");
        formProcedure(dialog,WM_COMMAND,MAKEWPARAM(IDOK,BN_CLICKED),0);
        assert(windowText(salary.error).find(L"within the range")!=std::wstring::npos);
        DestroyWindow(dialog);

        Form dismissal(DismissForm,dpi,font,bold);
        dismissal.employees=&staff;
        dialog=createForm(owner,dismissal);
        assert(dialog!=nullptr);
        fits(dialog,dismissal.employeeBox); fits(dialog,dismissal.error);
        fits(dialog,GetDlgItem(dialog,IDCANCEL)); fits(dialog,GetDlgItem(dialog,IDOK));
        formProcedure(dialog,WM_COMMAND,MAKEWPARAM(IDOK,BN_CLICKED),0);
        assert(windowText(dismissal.error).find(L"Choose an employee")!=std::wstring::npos);
        DestroyWindow(dialog);

        Form partial(PartialForm,dpi,font,bold);
        partial.completed=2;
        dialog=createForm(owner,partial);
        assert(dialog!=nullptr);
        fits(dialog,GetDlgItem(dialog,ID_KEEP_COMPLETED));
        fits(dialog,GetDlgItem(dialog,ID_DISCARD_ALL));
        fits(dialog,GetDlgItem(dialog,ID_CONTINUE));
        DestroyWindow(dialog);
        DeleteObject(font);
        DeleteObject(bold);
    }

    // The hiring workflow offers all three outcomes after a partial batch.
    for(int decision : {ID_KEEP_COMPLETED,ID_DISCARD_ALL,ID_CONTINUE})
    {
        int step=0;
        std::vector<Staff> hires;
        bool accepted=collectHireDetails(2,96,nullptr,nullptr,hires,[&](Form& form) -> INT_PTR {
            ++step;
            if(step==1)
            {
                assert(form.kind==EmployeeForm && form.number==1);
                form.employee=Staff{"Alice","Female",25,0,5000};
                return IDOK;
            }
            if(step==2)
            {
                assert(form.kind==EmployeeForm && form.number==2);
                form.draft.name=L"Bob";
                return IDCANCEL;
            }
            if(step==3)
            {
                assert(form.kind==PartialForm && form.completed==1);
                return decision;
            }
            assert(decision==ID_CONTINUE && form.kind==EmployeeForm && form.draft.name==L"Bob");
            form.employee=Staff{"Bob","Male",29,0,8000};
            return IDOK;
        });
        if(decision==ID_KEEP_COMPLETED) assert(accepted && hires.size()==1 && hires[0].name=="Alice");
        if(decision==ID_DISCARD_ALL) assert(!accepted && hires.empty());
        if(decision==ID_CONTINUE) assert(accepted && hires.size()==2 && hires[1].name=="Bob");
    }

    // A fully filled current form can also be kept, even before pressing Next.
    int step=0;
    std::vector<Staff> currentHire;
    bool accepted=collectHireDetails(2,96,nullptr,nullptr,currentHire,[&](Form& form) -> INT_PTR {
        ++step;
        if(step==1)
        {
            assert(form.kind==EmployeeForm);
            form.draft.name=L"Alice";
            form.draft.sex=2;
            form.draft.age=L"25";
            form.draft.salary=L"5,000";
            return IDCANCEL;
        }
        assert(form.kind==PartialForm && form.completed==1 && form.includesDraft);
        return ID_KEEP_COMPLETED;
    });
    assert(accepted && currentHire.size()==1 && currentHire[0].name=="Alice" && currentHire[0].salary==5000);

    step=0;
    currentHire.clear();
    accepted=collectHireDetails(2,96,nullptr,nullptr,currentHire,[&](Form& form) -> INT_PTR {
        ++step;
        if(step==1)
        {
            form.draft.name=L"Alice";
            form.draft.sex=2;
            form.draft.age=L"25";
            form.draft.salary=L"5,000";
            return IDCANCEL;
        }
        if(step==2)
        {
            assert(form.kind==PartialForm && form.completed==1 && form.includesDraft);
            return ID_CONTINUE;
        }
        if(step==3)
        {
            assert(form.kind==EmployeeForm && form.number==1 && form.draft.name==L"Alice");
            form.employee=Staff{"Alice","Female",25,0,5000};
            return IDOK;
        }
        assert(form.kind==EmployeeForm && form.number==2);
        form.employee=Staff{"Bob","Male",29,0,8000};
        return IDOK;
    });
    assert(accepted && currentHire.size()==2 && currentHire[0].name=="Alice" && currentHire[1].name=="Bob");

    DestroyWindow(owner);
}
