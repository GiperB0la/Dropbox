#include "../include/DragAndDrop.hpp"


bool isDragAndDrop = false;
std::unordered_set<std::string> pathsDragAndDrop;


HRESULT __stdcall DragAndDrop::QueryInterface(REFIID riid, void** ppvObject)
{
    if (riid == IID_IUnknown || riid == IID_IDropTarget) {
        *ppvObject = static_cast<IDropTarget*>(this);
        AddRef();
        return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG __stdcall DragAndDrop::AddRef()
{
    return ++m_refCount;
}

ULONG __stdcall DragAndDrop::Release()
{
    if (--m_refCount == 0) {
        delete this;
        return 0;
    }
    return m_refCount;
}

HRESULT __stdcall DragAndDrop::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    isDragAndDrop = true;
    *pdwEffect = DROPEFFECT_COPY;
    return S_OK;
}

HRESULT __stdcall DragAndDrop::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    *pdwEffect = DROPEFFECT_COPY;
    return S_OK;
}

HRESULT __stdcall DragAndDrop::DragLeave()
{
    isDragAndDrop = false;
    return S_OK;
}

HRESULT __stdcall DragAndDrop::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    isDragAndDrop = false;

    FORMATETC format = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM medium;

    if (SUCCEEDED(pDataObj->GetData(&format, &medium))) {
        HDROP hDrop = reinterpret_cast<HDROP>(GlobalLock(medium.hGlobal));
        UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

        for (UINT i = 0; i < fileCount; ++i) {
            char filePath[MAX_PATH];
            DragQueryFileA(hDrop, i, filePath, MAX_PATH);

            std::string filePathStr(filePath);
            std::cout << "File dropped: " << filePathStr << std::endl;
            pathsDragAndDrop.insert(filePathStr);
        }

        GlobalUnlock(medium.hGlobal);
        ReleaseStgMedium(&medium);
    }

    *pdwEffect = DROPEFFECT_COPY;
    return S_OK;
}