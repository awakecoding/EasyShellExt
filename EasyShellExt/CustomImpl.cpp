#include "pch.h"
#include "CustomImpl.h"
#include "LoggerHelper.h"

namespace esx {

enum Commands {
    CMD_RUN_ELEVATED = 0,
};

HRESULT CreateMenus(HMENU hMenu, UINT firstCommandId, UINT firstMenuIndex, USHORT& menuCount) {
    HRESULT hr = E_FAIL;

    {
        MENUITEMINFOW menuinfo = {0};
        menuinfo.cbSize = sizeof(MENUITEMINFOW);
        menuinfo.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
        menuinfo.wID = firstCommandId + CMD_RUN_ELEVATED;
        menuinfo.fType = MFT_STRING;
        menuinfo.dwTypeData = (LPWSTR)L"Run Elevated";
        menuinfo.fState = MFS_ENABLED;
        menuinfo.hbmpItem = NULL;

        if (!InsertMenuItemW(hMenu, firstMenuIndex + 0, TRUE, &menuinfo)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }

    menuCount = 1;

    hr = S_OK;

    return hr;
}

bool GetCommandHelpText(UINT targetCommandOffset, std::wstring& text) {
    return false;
}

bool GetCommandVerb(UINT targetCommandOffset, std::wstring& verb) {
    return false;
}

HRESULT InvokeMenuCommand(UINT targetCommandOffset) {
    HRESULT hr = E_FAIL;
    if (targetCommandOffset == (UINT)CMD_RUN_ELEVATED) {
    }
    return hr;
}
}  // namespace esx