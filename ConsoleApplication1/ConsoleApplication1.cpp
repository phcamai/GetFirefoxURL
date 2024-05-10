#include <Windows.h>
#include <AtlBase.h>
#include <AtlCom.h>
#include <UIAutomation.h>
#include <stdlib.h>
#define UNICODE

int main()
{
    if FAILED(CoInitialize(nullptr))
        return 0;
    struct coinit { ~coinit() { CoUninitialize(); } } cleanup;

    //find the first visible window in firefox
    HWND hwnd = NULL;
    while (true)
    {
        hwnd = FindWindowEx(0, hwnd, L"MozillaWindowClass", NULL);
        if (!hwnd)
        {
            printf("Firefox window not found.\n");
            return 0;
        }
        if (IsWindowVisible(hwnd))
            break;
    }

    //initialize UIAutomation
    CComPtr<IUIAutomation> uia;
    if FAILED(uia.CoCreateInstance(CLSID_CUIAutomation))
        return 0;

    CComPtr<IUIAutomationElement> root, navigation, editbox;
    CComPtr<IUIAutomationCondition> c1, c2;

    //find root from hwnd handle
    if FAILED(uia->ElementFromHandle(hwnd, &root))
        return 0;

    //find navigation bar as child of root
    uia->CreatePropertyCondition(UIA_AutomationIdPropertyId,
        CComVariant(L"nav-bar"), &c1);
    if FAILED(root->FindFirst(TreeScope_Children, c1, &navigation))
        return 0;

    //find editbox under as descendant of navigation
    uia->CreatePropertyCondition(UIA_AutomationIdPropertyId,
        CComVariant(L"urlbar-input"), &c2);
    if FAILED(navigation->FindFirst(TreeScope_Descendants, c2, &editbox))
        return 0;

    //get the string in editbox 
    CComVariant url;
    if FAILED(editbox->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &url))
        return 0;
    if (url.bstrVal)
        wprintf(L"[%s]\n", url.bstrVal);

    return 0;
}
