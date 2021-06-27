#pragma once
#include "pluginsdk/bridgemain.h"
#include <QList>

enum EProgramsType_t
{
	E_UNKNOWN = 0,  //未知类型
	E_DYNAMIC,      //动态编译程序
	E_STATIC,       //静态编译程序
};

struct EHead
{
	duint dwMagic;  //未知,值固定为3
	duint szNone2;  //未知,值固定为0
	duint szNone3;  //未知,好像是个随机数,修改不影响程序
	duint lpStartCode;   //起始用户代码地址,不可修改
	duint lpEString;     //字符串资源,如果没有字符串资源,则为0
	duint dwEStringSize; //字符串资源大小,如果没有字符串资源,则为0
	duint lpEWindow;     //创建组件信息
	duint dwEWindowSize; //创建组件信息大小
	duint dwLibNum;      //支持库数量
	duint lpLibEntry;    //支持库信息入口
	duint dwApiCount;    //Api数量
	duint lpModuleName;  //指向模块名称
	duint lpApiName;     //指向Api名称
};

struct mid_KrnlApp
{
	duint krnl_MReportError;               //错误回调
	duint krnl_MCallDllCmd;                //DLL命令
	duint krnl_MCallLibCmd;                //三方支持库命令
	duint krnl_MCallKrnlLibCmd;            //核心支持库命令
	duint krnl_MReadProperty;              //读取组件属性
	duint krnl_MWriteProperty;             //设置组件属性
	duint krnl_MMalloc;                    //分配内存
	duint krnl_MRealloc;                   //重新分配内存
	duint krnl_MFree;                      //释放内存
	duint krnl_MExitProcess;               //结束
	duint krnl_MMessageLoop;               //窗口消息循环
	duint krnl_MLoadBeginWin;              //载入启动窗口
	duint krnl_MOtherHelp;                 //辅助功能
};

struct mid_EventInfo
{
	int m_nEventIndex;       //事件索引
	duint m_EventAddr;       //事件地址
};

struct mid_ControlInfo
{
	duint m_controlId;                           //控件自身ID
	std::string m_controlName;                   //控件名称
	duint m_controlTypeId;                       //控件所属数据类型的ID
	std::string m_controlTypeName;               //控件类型名称
	duint m_propertyAddr;                        //属性地址
	dsint m_propertySize;                        //属性大小
	std::vector<mid_EventInfo> mVec_eventInfo;   //事件处理
};

struct mid_GuiInfo
{
	duint m_windowId;                           //控件所属窗口ID
	std::vector<mid_ControlInfo> mVec_ControlInfo;   //窗口中的控件
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
	//静态编译头地址
	duint m_staticEHeadAddr;
	duint m_UserCodeStartAddr;
public:
	mid_KrnlApp m_KrnlApp;
	QList<mid_GuiInfo> mVec_window;
};