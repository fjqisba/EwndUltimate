#include "Ewnd.h"

#include "pluginsdk/_scriptapi_pattern.h"
#include "pluginsdk/_scriptapi_memory.h"

EProgramsType_t g_ProgramType = E_UNKNOWN;


unsigned int krnln_GetIDSubType(unsigned int ID)
{
	return ID & 0xF0000000;
}

unsigned int krnln_GetIDGroupType(unsigned int ID)
{
	return ID & 0xF000000;
}

bool krnln_IsMenuItemID(unsigned int ID)
{
	return krnln_GetIDGroupType(ID) == 0x6000000 && krnln_GetIDSubType(ID) == 0x20000000;
}

std::string ReadStr(unsigned char* pBuf)
{
	int Len = strlen((const char*)pBuf);
	std::string ret((const char*)pBuf, Len);
	return ret;
}

unsigned int CDR_ReadUInt(unsigned char*& currentPoint)
{
	unsigned int ret = *(unsigned int*)currentPoint;
	currentPoint += 4;
	return ret;
}

dsint CDR_ReadInt(unsigned char*& currentPoint)
{
	int ret = *(dsint*)currentPoint;
	currentPoint += 4;
	return ret;
}

std::vector<unsigned char> CDR_ReadCFreqMem(unsigned char*& currentPoint)
{
	std::vector<unsigned char> ret;
	int len = CDR_ReadInt(currentPoint);
	if (len > 0) {
		ret.resize(len);
		memcpy(&ret[0], currentPoint, len);
		currentPoint += len;
	}
	return ret;
}

std::string CDR_ReadCString(unsigned char*& currentPoint)
{
	std::string ret;
	int len = CDR_ReadInt(currentPoint);
	if (len > 0) {
		ret.resize(len);
		memcpy(&ret[0], currentPoint, len);
		currentPoint += len;
	}
	return ret;
}

duint ReadUInt(unsigned char* pBuf)
{
	return *(duint*)pBuf;
}

dsint ReadInt(unsigned char* pBuf)
{
	return *(dsint*)pBuf;
}

unsigned char* GetUnitDataPtr(unsigned char* propertyAddr)
{
	unsigned char* ret = propertyAddr;
	ret += 0x18;
	ret += qstrlen((const char*)ret) + 1;
	ret += qstrlen((const char*)ret) + 1;
	ret += 0x1C;
	ret += (ReadUInt(ret) + 1) * 4;
	ret += ReadUInt(ret) + 4;
	ret += qstrlen((const char*)ret) + 1;
	ret += 0xC;
	ret += ReadUInt(ret) * 8;
	ret += 0x18;
	return ret;
}

std::string GetEWindowTitle(unsigned char* pUnitDataPtr)
{
	struct WinFrom_UnitData
	{
		unsigned int version;
		unsigned int 边框;
		unsigned int 底图方式;
		unsigned int 底色;
		unsigned int 最大化按钮;
		unsigned int 最小化按钮;
		unsigned int 控制按钮;
		unsigned int 位置;
		unsigned int 可否移动;
		unsigned int 播放次数;
		unsigned int 回车下移焦点;
		unsigned int Esc键关闭;
		unsigned int F1键打开帮助;
		unsigned int 帮助标志值;
		unsigned int 在任务条中显示;
		unsigned int 随意移动;
		unsigned int 外形;
		unsigned int 总在最前;
		unsigned int 保持标题条激活;
		std::string 窗口类名;
		std::vector<unsigned char> 底图;
		std::vector<unsigned char> 图标;
		std::vector<unsigned char> 背景音乐;
		std::string 标题;
		std::string 帮助文件名;
	};

	WinFrom_UnitData tmpData = {};
	tmpData.version = CDR_ReadUInt(pUnitDataPtr);

	if (tmpData.version <= 6) {
		tmpData.边框 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.底图方式 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.底色 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.最大化按钮 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.最小化按钮 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.控制按钮 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.位置 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.可否移动 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.播放次数 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.回车下移焦点 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.Esc键关闭 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.F1键打开帮助 = CDR_ReadUInt(pUnitDataPtr);
		tmpData.帮助标志值 = CDR_ReadUInt(pUnitDataPtr);

		if (tmpData.version >= 2) {
			tmpData.在任务条中显示 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version >= 3) {
			tmpData.随意移动 = CDR_ReadUInt(pUnitDataPtr);
			tmpData.外形 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version >= 4) {
			tmpData.总在最前 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version >= 5) {
			tmpData.保持标题条激活 = CDR_ReadUInt(pUnitDataPtr);
		}
		if (tmpData.version == 6) {
			tmpData.窗口类名 = CDR_ReadCString(pUnitDataPtr);
		}

		tmpData.底图 = CDR_ReadCFreqMem(pUnitDataPtr);
		tmpData.图标 = CDR_ReadCFreqMem(pUnitDataPtr);
		tmpData.背景音乐 = CDR_ReadCFreqMem(pUnitDataPtr);

		tmpData.标题 = CDR_ReadCString(pUnitDataPtr);
		tmpData.帮助文件名 = CDR_ReadCString(pUnitDataPtr);
	}
	return tmpData.标题;
}

void Ewnd::ParseControlBasciProperty(unsigned char* lpControlInfo, mid_ControlInfo& out_Property)
{
	//无用字符串1?
	ReadStr(lpControlInfo);
	lpControlInfo += qstrlen((const char*)lpControlInfo) + 1;

	//存储数据?
	ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_left = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_top = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_width = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	unsigned int m_height = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//值为0,用来存储LoadCursorA返回的句柄值的
	unsigned int hCURSOR = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//父控件ID
	unsigned int fatherControlId = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	//子控件数目
	unsigned int childControlCount = ReadUInt(lpControlInfo);
	lpControlInfo += 4;

	for (unsigned int n = 0; n < childControlCount; ++n) {
		unsigned int tmpChildControlId = ReadUInt(lpControlInfo);
		lpControlInfo += 4;
		//out_Property.mVec_childControl.push_back(tmpChildControlId);
	}

	//未知偏移
	unsigned int offset2 = ReadUInt(lpControlInfo);
	lpControlInfo += offset2 + 4;

	//标记
	std::string m_tag = ReadStr(lpControlInfo);
	lpControlInfo += qstrlen((const char*)lpControlInfo) + 1;

	//未知的值
	lpControlInfo += 12;

	int dwEventCount = ReadInt(lpControlInfo);
	lpControlInfo += 4;
	for (int nIndexEvent = 0; nIndexEvent < dwEventCount; ++nIndexEvent) {
		mid_EventInfo tmpEvent;
		tmpEvent.m_nEventIndex = ReadInt(lpControlInfo);
		lpControlInfo += 4;
		tmpEvent.m_EventAddr = ReadUInt(lpControlInfo) + m_UserCodeStartAddr;
		lpControlInfo += 4;
		out_Property.mVec_eventInfo.push_back(tmpEvent);
	}
	return;
}


bool Ewnd::ParseKrnlInterface(duint lpKrnlEntry)
{
	lpKrnlEntry -= sizeof(mid_KrnlApp);
	duint recvLen = 0;
	
	if (!Script::Memory::Read(lpKrnlEntry, &m_KrnlApp, sizeof(mid_KrnlApp), &recvLen)) {
		return false;
	}

	return true;
}

bool Ewnd::ParseGUIResource(duint lpGUIStart, duint infoSize)
{
	std::vector<unsigned char> tmpGuiBuf;
	tmpGuiBuf.resize(infoSize);
	duint recvLen = 0;
	if (!Script::Memory::Read(lpGUIStart, &tmpGuiBuf[0], infoSize, &recvLen)) {
		return false;
	}

	//当前解析地址
	unsigned char* lpCurrentParseAddr = &tmpGuiBuf[0];
	std::vector<duint> vec_WindowId;
	duint dwTotalWindowCount = ReadUInt(&tmpGuiBuf[0]) >> 3;
	lpCurrentParseAddr += 4;

	for (unsigned int n = 0; n < dwTotalWindowCount; ++n) {
		vec_WindowId.push_back(ReadUInt(lpCurrentParseAddr));
		lpCurrentParseAddr += 4;
	}

	//编译器遗留值?
	for (unsigned int n = 0; n < dwTotalWindowCount; ++n) {
		//uint32 unknowId = ReadUInt(lpCurrentParseAddr);
		lpCurrentParseAddr += 4;
	}

	for (unsigned int nIndexWindow = 0; nIndexWindow < dwTotalWindowCount; ++nIndexWindow) {
		unsigned char* lpWindowInfo = lpCurrentParseAddr;

		mid_GuiInfo eGuiInfo;
		eGuiInfo.m_windowId = vec_WindowId[nIndexWindow];

		//暂时未知
		duint unKnownFieldA = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;
		duint unKnownFieldB = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//接下来跟着两个CString,都为空
		lpWindowInfo += 8;

		//单个窗口中的控件总个数
		duint dwTotalControlCount = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//单个窗口中的控件总占用大小
		duint dwTotalControlSize = ReadUInt(lpWindowInfo);
		lpWindowInfo += 4;

		//开始解析控件
		unsigned char* lpControlArray = lpWindowInfo;
		{
			//解析控件ID,例如0x160612BC
			std::vector<duint> vec_ControlId;
			for (unsigned int j = 0; j < dwTotalControlCount; ++j) {
				vec_ControlId.push_back(ReadUInt(lpControlArray));
				lpControlArray += 4;
			}

			//解析控件偏移
			std::vector<duint> vec_ControlOffset;
			for (unsigned int j = 0; j < dwTotalControlCount; ++j) {
				vec_ControlOffset.push_back(ReadUInt(lpControlArray));
				lpControlArray += 4;
			}

			//解析控件属性
			for (unsigned int nIndexControl = 0; nIndexControl < dwTotalControlCount; ++nIndexControl) {
				unsigned char* lpControlInfo = lpControlArray + vec_ControlOffset[nIndexControl];

				mid_ControlInfo eControlInfo;

				//控件占用的大小
				dsint dwControlSize = ReadInt(lpControlInfo);
				lpControlInfo += 4;

				eControlInfo.m_propertyAddr = lpGUIStart + (lpControlInfo - &tmpGuiBuf[0]);
				eControlInfo.m_propertySize = dwControlSize;

				//控件类型ID
				duint dwControlTypeId = ReadUInt(lpControlInfo);
				lpControlInfo += 4;

				//固定的20个空字节,保留使用?
				lpControlInfo += 20;

				if (dwControlTypeId == 0x10001) {
					lpControlInfo += strlen((const char*)lpControlInfo)+1;
					//这是主窗口
					ParseControlBasciProperty(lpControlInfo, eControlInfo);
					unsigned char* pControlAddr = &tmpGuiBuf[0] + (eControlInfo.m_propertyAddr - lpGUIStart);
					unsigned char* pUnitDataPtr = GetUnitDataPtr(pControlAddr);
					int UnitDataSize = eControlInfo.m_propertySize + (pControlAddr - pUnitDataPtr);
					if (UnitDataSize > 0) {
						eControlInfo.m_controlName = GetEWindowTitle(pUnitDataPtr);
					}
				}
				else if (krnln_IsMenuItemID(vec_ControlId[nIndexControl])) {
					lpControlInfo += 14;
					eControlInfo.m_controlName = ReadStr(lpControlInfo);
				}
				else {
					eControlInfo.m_controlName = ReadStr(lpControlInfo);
					lpControlInfo += strlen((const char*)lpControlInfo) + 1;

					ParseControlBasciProperty(lpControlInfo, eControlInfo);
				}


				eControlInfo.m_controlId = vec_ControlId[nIndexControl];
				eControlInfo.m_controlTypeId = dwControlTypeId;
				eGuiInfo.mVec_ControlInfo.push_back(eControlInfo);
			}
		}

		mVec_window.push_back(eGuiInfo);
		lpCurrentParseAddr = lpWindowInfo + dwTotalControlSize;
	}

	return true;
}

bool Ewnd::Parse_EStatic(duint eHeadAddr)
{
	EHead eHead;
	duint recvLen = 0;
	Script::Memory::Read(eHeadAddr, &eHead, sizeof(EHead), &recvLen);
	if (eHead.dwMagic != 0x3) {
		return false;
	}

	//没窗口玩蛇皮
	if (eHead.lpEWindow == 0 || eHead.dwEWindowSize <= 4) {
		return false;
	}

	duint dwKrnlEntry = eHead.lpEString;
	if (dwKrnlEntry == 0) {
		dwKrnlEntry = eHead.lpEWindow;
	}

	m_UserCodeStartAddr = eHead.lpStartCode;
	if (!ParseKrnlInterface(dwKrnlEntry)) {
		return false;
	}

	ParseGUIResource(eHead.lpEWindow, eHead.dwEWindowSize);

	return true;
}

bool Ewnd::InitEProgram(duint startAddr, duint size)
{
	g_ProgramType = E_UNKNOWN;
	duint eMagicHead = Script::Pattern::FindMem(startAddr, size, "50 64 89 25 00 00 00 00 81 EC AC 01 00 00 53 56 57");
	if (eMagicHead != 0) {
		g_ProgramType = E_STATIC;
		m_staticEHeadAddr = Script::Memory::ReadDword(eMagicHead + 0x26);
		return Parse_EStatic(m_staticEHeadAddr);
	}

	return true;
}