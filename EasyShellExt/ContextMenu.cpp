#include "pch.h"
#include "ContextMenu.h"
#include <shellapi.h>
#include "StringEncode.h"
#include "Utils.h"
#include "LoggerHelper.h"

OBJECT_ENTRY_AUTO(CLSID_EasyShellExt, ContextMenu)

ContextMenu::ContextMenu() {
    ESX_LOG(INFO) << __FUNCTION__ << "(), new instance " << esx::ToHexString(this);

#if SA_QUERYINTERFACE_IMPL == 0
    // reference counter must be initialized to 0 even if we are actually creating an instance. 
    // A reference to this instance will be added when the instance will be queried by explorer.exe.
    refCount_ = 0;
#elif SA_QUERYINTERFACE_IMPL == 1
    refCount_ = 1;
#endif

    isBackGround_ = false;
    previousMenu_ = 0;
}

ContextMenu::~ContextMenu() {
}

HRESULT STDMETHODCALLTYPE ContextMenu::QueryContextMenu(HMENU hMenu, UINT menuIndex, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    // Only allow standard right-click (not in other modes like drag & drop)
    if (uFlags & CMF_DEFAULTONLY)
        return S_OK;

    // Insert a new menu item for "Run Elevated"
    InsertMenu(hMenu, menuIndex, MF_BYPOSITION, idCmdFirst, L"Run Elevated");

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(1));  // Return success with 1 added item
}

HRESULT STDMETHODCALLTYPE ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
    //define the type of structure pointed by pici
    const char* structName = "UNKNOWN";
    if (pici->cbSize == sizeof(CMINVOKECOMMANDINFO))
        structName = "CMINVOKECOMMANDINFO";
    else if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
        structName = "CMINVOKECOMMANDINFOEX";

    //define how we should interpret pici->lpVerb
    std::string verb;
    if (IS_INTRESOURCE(pici->lpVerb)) {
#pragma warning(push)
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
        verb = std::to_string(reinterpret_cast<int>(pici->lpVerb));
#pragma warning(pop)
    }
    else {
        verb = pici->lpVerb;
    }

    ESX_LOG(INFO) << __FUNCTION__ << "(), pici->cbSize=" << structName
                  << ", pici->fMask=" << pici->fMask
                  << ", pici->lpVerb=" << verb << " this=" << esx::ToHexString(this);

    //validate
    if (!IS_INTRESOURCE(pici->lpVerb))
        return E_INVALIDARG;  //don't know what to do with pici->lpVerb

    UINT targetCommandOffset = LOWORD(pici->lpVerb);  // matches the command_id offset (command id of the selected menu - command id of the first menu)

    //From this point, it is safe to use class members without other threads interference
    CriticalSectionGuard cs_guard(&cs_);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) {
    std::string typeStr = esx::GetGetCommandStringFlags(uType);
    std::string typeHex = esx::StringPrintf("0x%08x", uType);

    // only show this log in verbose mode
    ESX_LOG(INFO) << __FUNCTION__ << "(), idCmd=" << idCmd
                  << ", cchMax=" << cchMax
                  << " this=" << esx::ToHexString(this)
                  << ", type=" << typeHex << ":" << typeStr;

    UINT targetCommandOffset = (UINT)idCmd;  // matches the command_id offset (command id of the selected menu substracted by command id of the first menu)

    //From this point, it is safe to use class members without other threads interference
    CriticalSectionGuard cs_guard(&cs_);
    std::wstring w;
    std::string ansi;

    if (uType == GCS_HELPTEXTA || uType == GCS_HELPTEXTW) {
        return S_OK;
    }
    else if (uType == GCS_VERBA || uType == GCS_VERBW) {
        return S_OK;
    }

    //Build up tooltip string
    switch (uType) {
        case GCS_HELPTEXTA: {
            ansi = esx::StringEncode::UnicodeToAnsi(w);
            //ANIS tooltip handling
            lstrcpynA(pszName, ansi.c_str(), cchMax);
            return S_OK;
        } break;
        case GCS_HELPTEXTW: {
            //UNICODE tooltip handling
            lstrcpynW((LPWSTR)pszName, w.c_str(), cchMax);
            return S_OK;
        } break;
        case GCS_VERBA: {
            ansi = esx::StringEncode::UnicodeToAnsi(w);
            //ANIS tooltip handling
            lstrcpynA(pszName, ansi.c_str(), cchMax);
            return S_OK;
        } break;
        case GCS_VERBW: {
            //UNICODE tooltip handling
            lstrcpynW((LPWSTR)pszName, w.c_str(), cchMax);
            return S_OK;
        } break;
        case GCS_VALIDATEA:
        case GCS_VALIDATEW: {
            return S_OK;
        } break;
    }

    ESX_LOG(ERROR) << __FUNCTION__ << "(), unknown flags: " << uType;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE ContextMenu::Initialize(
    LPCITEMIDLIST pIDFolder,
    LPDATAOBJECT pDataObj,
    HKEY hRegKey) {
    if (!pDataObj)
        return E_INVALIDARG;

    // Get the selected file path
    FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM stg;

    if (FAILED(pDataObj->GetData(&fmt, &stg)))
        return E_FAIL;

    HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
    if (!hDrop)
        return E_FAIL;

    wchar_t filePath[MAX_PATH] = {0};
    if (DragQueryFileW(hDrop, 0, filePath, MAX_PATH)) {
        // Check if the file is an .exe
        std::wstring extension = wcsrchr(filePath, L'.');
        if (!_wcsicmp(extension.c_str(), L".exe")) {
            std::wstring selectedFile_ = filePath;  // Store the file for later use
        }
        else {
            GlobalUnlock(stg.hGlobal);
            ReleaseStgMedium(&stg);
            return E_FAIL;  // Prevent menu from appearing
        }
    }

    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);

    return S_OK;  // Allow menu to appear
}
