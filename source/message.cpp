#include "stdafx.h"
#include "message.h"

// 默认构造函数;
CMessage::CMessage()
{
    m_nType = -1;
    m_strFileName = _T("");
    m_dwFileSize = 0;
}

// 只需发送一般的控制信息使用;
CMessage::CMessage(int nType)
{
    m_nType = nType;
    m_strFileName = _T("");
    m_dwFileSize = 0;
}

// 需要发送文件名及大小时使用;
CMessage::CMessage(int nType, CString strFileName, DWORD dwFileSize)
{
    m_nType = nType;
    m_strFileName = strFileName;
    m_dwFileSize = dwFileSize;
}

void CMessage::Serialize(CArchive &ar)
{
    if (ar.IsStoring())
    {
        ar << m_nType;
        ar << m_strFileName;
        ar << m_dwFileSize;
    }
    else
    {
        ar >> m_nType;
        ar >> m_strFileName;
        ar >> m_dwFileSize;
    }
}