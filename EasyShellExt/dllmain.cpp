#include "pch.h"
#include "resource.h"
// Generated files
#include "shellext_h.h"
#include "shellext_i.c"
#include "Utils.h"
#include <ShlObj.h>

HMODULE gCurrentModule = NULL;

class EasyShellExtModule : public ATL::CAtlDllModuleT<EasyShellExtModule> {
   public:
    DECLARE_LIBID(LIBID_EASYSHELLEXTENSIONLib)
};

class EasyShellExtModule _AtlModule;

// Returns a class factory to create an object of the requested type.
STDAPI DllGetClassObject(_In_ REFCLSID clsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv) {
    std::string clsid_str = esx::GuidToInterfaceName(clsid);
    std::string riid_str = esx::GuidToInterfaceName(riid);

    HRESULT hr = _AtlModule.DllGetClassObject(clsid, riid, ppv);

    return hr;
}

// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void) {
    HRESULT hr = _AtlModule.DllCanUnloadNow();

    if (hr == S_OK) {
        return S_OK;
    }

    return S_FALSE;
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer(void) {
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();

    if (SUCCEEDED(hr)) {
        // Notify the Shell to pick the changes:
        // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
        // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
        // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
        // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
        SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
    }

    return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer(void) {
    // unregisters object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllUnregisterServer();

    if (SUCCEEDED(hr)) {
        // Notify the Shell to pick the changes:
        // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
        // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
        // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
        // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
        SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
    }

    return hr;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            gCurrentModule = hModule;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
