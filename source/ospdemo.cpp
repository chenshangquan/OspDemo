#include "StdAfx.h"
#include "ospdemo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void SendFuction(CMessage *const pMsg)
{
    // 获得窗口指针
    CWnd *pBoxOne = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT4);
    // 赋值;
    CString cMsgContent(pMsg->content);
    pBoxOne->SetWindowText(cMsgContent);

    // 获取源结点号
    g_uNodeNum = pMsg->srcnode;
}

void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    u16 wCurEvent = pMsg->event;
    /*得到当前消息的类型;*/

    switch (wCurEvent)
    {
        /*根据不同的消息类型进行处理;*/
    case EVENT_REQ:
        //ReqFuction(pMsg);
        NextState(STATE_ACK);				//完成状态跳跃;
        SetTimer(EVENT_TIMEOUT, 1000); 		//进行定时操作;
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
        //回复EVENT_QUIT消息
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