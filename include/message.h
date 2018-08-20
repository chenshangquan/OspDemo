
class CMessage : public CObject
{
public:
    void Serialize(CArchive &a);

    CMessage();
    CMessage(int nType);
    CMessage(int nType, CString strFileName, DWORD dwFileSize);

    virtual ~CMessage();

public:
    int m_nType;
    CString m_strFileName;
    DWORD m_dwFileSize;
};