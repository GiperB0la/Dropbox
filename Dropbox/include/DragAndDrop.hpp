#pragma once
#include <windows.h>
#include <shobjidl.h>
#include <iostream>
#include <unordered_set>


extern bool isDragAndDrop;
extern std::unordered_set<std::string> pathsDragAndDrop;

class DragAndDrop : public IDropTarget
{
public:
    DragAndDrop(HWND hwnd) : m_hwnd(hwnd), m_refCount(1) {}

public:
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;
    HRESULT __stdcall DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    HRESULT __stdcall DragLeave() override;
    HRESULT __stdcall Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;

private:
    HWND m_hwnd;
    ULONG m_refCount;
};