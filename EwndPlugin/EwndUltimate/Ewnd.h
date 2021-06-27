#pragma once
#include "pluginsdk/bridgemain.h"
#include <QList>

enum EProgramsType_t
{
	E_UNKNOWN = 0,  //δ֪����
	E_DYNAMIC,      //��̬�������
	E_STATIC,       //��̬�������
};

struct EHead
{
	duint dwMagic;  //δ֪,ֵ�̶�Ϊ3
	duint szNone2;  //δ֪,ֵ�̶�Ϊ0
	duint szNone3;  //δ֪,�����Ǹ������,�޸Ĳ�Ӱ�����
	duint lpStartCode;   //��ʼ�û������ַ,�����޸�
	duint lpEString;     //�ַ�����Դ,���û���ַ�����Դ,��Ϊ0
	duint dwEStringSize; //�ַ�����Դ��С,���û���ַ�����Դ,��Ϊ0
	duint lpEWindow;     //���������Ϣ
	duint dwEWindowSize; //���������Ϣ��С
	duint dwLibNum;      //֧�ֿ�����
	duint lpLibEntry;    //֧�ֿ���Ϣ���
	duint dwApiCount;    //Api����
	duint lpModuleName;  //ָ��ģ������
	duint lpApiName;     //ָ��Api����
};

struct mid_KrnlApp
{
	duint krnl_MReportError;               //����ص�
	duint krnl_MCallDllCmd;                //DLL����
	duint krnl_MCallLibCmd;                //����֧�ֿ�����
	duint krnl_MCallKrnlLibCmd;            //����֧�ֿ�����
	duint krnl_MReadProperty;              //��ȡ�������
	duint krnl_MWriteProperty;             //�����������
	duint krnl_MMalloc;                    //�����ڴ�
	duint krnl_MRealloc;                   //���·����ڴ�
	duint krnl_MFree;                      //�ͷ��ڴ�
	duint krnl_MExitProcess;               //����
	duint krnl_MMessageLoop;               //������Ϣѭ��
	duint krnl_MLoadBeginWin;              //������������
	duint krnl_MOtherHelp;                 //��������
};

struct mid_EventInfo
{
	int m_nEventIndex;       //�¼�����
	duint m_EventAddr;       //�¼���ַ
};

struct mid_ControlInfo
{
	duint m_controlId;                           //�ؼ�����ID
	std::string m_controlName;                   //�ؼ�����
	duint m_controlTypeId;                       //�ؼ������������͵�ID
	std::string m_controlTypeName;               //�ؼ���������
	duint m_propertyAddr;                        //���Ե�ַ
	dsint m_propertySize;                        //���Դ�С
	std::vector<mid_EventInfo> mVec_eventInfo;   //�¼�����
};

struct mid_GuiInfo
{
	duint m_windowId;                           //�ؼ���������ID
	std::vector<mid_ControlInfo> mVec_ControlInfo;   //�����еĿؼ�
};



class Ewnd
{
public:
	bool InitEProgram(duint startAddr, duint size);
private:
	bool Parse_EStatic(duint eHeadAddr);
	bool ParseKrnlInterface(duint lpKrnlEntry);
	bool ParseGUIResource(duint lpGUIStart, duint infoSize);
	void ParseControlBasciProperty(unsigned char* lpControlInfo, mid_ControlInfo& out_Property);
public:
	duint m_codeBase;
	duint m_codeSize;
	//��̬����ͷ��ַ
	duint m_staticEHeadAddr;
	duint m_UserCodeStartAddr;
public:
	mid_KrnlApp m_KrnlApp;
	QList<mid_GuiInfo> mVec_window;
};