#include "StdAfx.h"
#include "ospdemo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void SendFuction(CMessage *const pMsg)
{
    // ��ô���ָ��
    CWnd *pBoxOne = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT4);
    // ��ֵ;
    CString cMsgContent(pMsg->content);
    pBoxOne->SetWindowText(cMsgContent);

    // ��ȡԴ����
    g_uNodeNum = pMsg->srcnode;
}

void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    u16 wCurEvent = pMsg->event;
    /*�õ���ǰ��Ϣ������;*/

    switch (wCurEvent)
    {
        /*���ݲ�ͬ����Ϣ���ͽ��д���;*/
    case EVENT_REQ:
        //ReqFuction(pMsg);
        NextState(STATE_ACK);				//���״̬��Ծ;
        SetTimer(EVENT_TIMEOUT, 1000); 		//���ж�ʱ����;
        break;
    case EVENT_SEND:
        SendFuction(pMsg);
        OspPrintf(TRUE, FALSE, "received a message.\n");
        NextState(STATE_WORK);
        break;
    case EVENT_TERM:
        //Term_Fuction(pMsg);
        KillTimer(EVENT_TIMEOUT);
        NextState(IDLE_STATE);
        //�ظ�EVENT_QUIT��Ϣ
        // post(pMsg-> srcid, EVENT_QUIT, NULL, 0, pMsg->srcnode);
        break;
    case EVENT_TIMEOUT:
        //Timeout_Function();
        OspPrintf(TRUE, FALSE, "send my msg3\n");
        break;
    case EVENT_START_WORK:
        OspPrintf(TRUE, FALSE, "send my msg2\n");
        NextState(STATE_WORK);
    default:
        // Def_Fuction(pMsg);
        break;
    }

    return;
}

void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    //do something
    return;
}