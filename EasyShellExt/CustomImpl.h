#ifndef SHELL_EXT_CUSTOM_IMPL_H_
#define SHELL_EXT_CUSTOM_IMPL_H_
#pragma once
#include <string>

namespace esx {
HRESULT CreateMenus(HMENU hMenu, UINT firstCommandId, UINT firstMenuIndex, USHORT& menuCount);
bool GetCommandHelpText(UINT targetCommandOffset, std::wstring& text);
bool GetCommandVerb(UINT targetCommandOffset, std::wstring& verb);
HRESULT InvokeMenuCommand(UINT targetCommandOffset);
}  // namespace esx

#endif  // !SHELL_EXT_CUSTOM_IMPL_H_
