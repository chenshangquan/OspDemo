#ifndef _COMMON_H_
#define _COMMON_H_

#pragma once

#include "osp.h"
#include <Windows.h>
#include <iostream>
using namespace std;

#define BUF_SIZE          1024
#define LOG_FILE0          _T("temp\\OSPClient0.log")
#define LOG_FILE1          _T("temp\\OSPClient1.log")
#define LOG_SIZE          (1024*1024)

typedef enum emPRT_TYPE
{
	PRT_TYPE_OFF = 0,                 //��ӡ��;
	PRT_TYPE_NORMOL,                  //��ӡһ����ʾ��Ϣ;
	PRT_TYPE_COMMUNION,               //��ӡͨѶ��Ϣ;
	PRT_TYPE_FILE_TR,                 //��ӡ�ļ�������Ϣ;
	PRT_TYPE_OPEN_ALL                 //��ӡ���е���Ϣ;
}PRT_TYPE;

class CPrint
{
public:
	CPrint();
	~CPrint();

	//prt
	static void Print( u8 byPrintLevel);           //�ſ���ӡ
	static u8  GetPrintLevel();
	static bool  IsPrintCommunion();
	static bool  IsPrintFileTR();
	
	static void PrintMsg( char * format,... );     //��ӡ��������;
	static void PrintTimeMsg( char * format,... ); //��ӡ��������;
	static void PrintMsgNolog( char * format,... );//��ӡ����д����־��;

private:
	static void WriteLog(const char *pTemp);
private:
	static u8 m_byPrintLevel;

};

#define PRINTMSG CPrint::PrintMsg
#define PRINTMSG_TIME CPrint::PrintTimeMsg
#define PRINTMSG_NOLOG CPrint::PrintMsgNolog

#endif//_COMMON_H_