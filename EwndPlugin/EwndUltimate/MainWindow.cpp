#include "MainWindow.h"
#include "Ewnd.h"
#include <QMenu>
#include <QVariant>
#include <QTextCodec>
#include <QMessageBox>
#include "pluginsdk/_scriptapi_memory.h"
#include "pluginsdk/_scriptapi_gui.h"
#include "public.h"


Q_DECLARE_METATYPE(mid_GuiInfo*)

MainWindow::~MainWindow()
{
	if (m_pEwnd) {
		delete m_pEwnd;
		m_pEwnd = nullptr;
	}
}

void MainWindow::on_ButtonClicked(bool bChecked)
{
	if (!m_pEwnd->mVec_window.size()) {
		ui.outMsg->appendPlainText(QStringLiteral("窗口个数为0"));
		return;
	}

	std::vector<unsigned char> ShellCode = {
		0xC8,0x00,0x00,0x00,                //enter 0x0,0x0		
	};
	for (unsigned int nWindowIndex = 0; nWindowIndex < m_pEwnd->mVec_window.size(); ++nWindowIndex) {
		std::vector<mid_EventInfo>& vec_Events = m_pEwnd->mVec_window[nWindowIndex].mVec_ControlInfo[0].mVec_eventInfo;
		for (unsigned int nEventIndex = 0; nEventIndex < vec_Events.size(); ++nEventIndex) {
			//窗口创建完毕,窗口首次激活
			if (vec_Events[nEventIndex].m_nEventIndex == 0 || vec_Events[nEventIndex].m_nEventIndex == 8) {
				Script::Memory::WriteByte(vec_Events[nEventIndex].m_EventAddr, 0xC3);
			}
		}
		
		std::vector<unsigned char> windowShell = {
			0x68,0x00,0x00,0x00,0x00,       //push windowId
			0xBB,0x00,0x00,0x00,0x00,       //mov ebx,LoadWindow
			0xFF,0xD3,                      //call ebx
			0x83,0xC4,0x4,                  //add esp,0x4
			0x6A,0x00,                      //push 0x0
			0x6A,0x01,                      //push 0x1
			0x6A,0xFF,                      //push -0x1
			0x6A,0x5,                       //push 0x5
			0x68,0x00,0x00,0x00,0x00,       //push ControlId
			0x68,0x00,0x00,0x00,0x00,       //push WindowId
			0xBB,0x00,0x00,0x00,0x00,       //mov ebx,SetWindowProperty
			0xFF,0xD3,                      //call ebx
			0x83,0xC4,0x18,                 //add esp,0x18
			0x6A,0x00,                      //push 0x0
			0x6A,0x00,                      //push 0x0
			0x6A,0xFF,                      //push -0x1
			0x6A,0x6,                       //push 0x6
			0x68,0x00,0x00,0x00,0x00,       //push ControlId
			0x68,0x00,0x00,0x00,0x00,       //push WindowId
			0xBB,0x00,0x00,0x00,0x00,       //mov ebx,SetWindowProperty
			0xFF,0xD3,                      //call ebx
			0x83,0xC4,0x18,                 //add esp,0x18
		};
		mid_GuiInfo* pGuiInfo = &m_pEwnd->mVec_window[nWindowIndex];
		WriteUInt(&windowShell[1], pGuiInfo->m_windowId);
		WriteUInt(&windowShell[6], m_pEwnd->m_KrnlApp.krnl_MLoadBeginWin);
		WriteUInt(&windowShell[24], pGuiInfo->mVec_ControlInfo[0].m_controlId);
		WriteUInt(&windowShell[29], pGuiInfo->m_windowId);
		WriteUInt(&windowShell[34], m_pEwnd->m_KrnlApp.krnl_MWriteProperty);
		WriteUInt(&windowShell[52], pGuiInfo->mVec_ControlInfo[0].m_controlId);
		WriteUInt(&windowShell[57], pGuiInfo->m_windowId);
		WriteUInt(&windowShell[62], m_pEwnd->m_KrnlApp.krnl_MWriteProperty);
		ShellCode.insert(ShellCode.end(), windowShell.begin(), windowShell.end());
	}

	std::vector<unsigned char> endLoop = {
		0xBB,0x00,0x00,0x00,0x00,       //mov ebx,MessageLoop
		0xFF,0xD3,                      //call ebx
		0xC9,                           //leave
		0xC3                            //ret
	};
	WriteUInt(&endLoop[1], m_pEwnd->m_KrnlApp.krnl_MMessageLoop);
	ShellCode.insert(ShellCode.end(), endLoop.begin(), endLoop.end());

	duint shellCodeBuf = Script::Memory::RemoteAlloc(0, 0x1000);
	ui.outMsg->appendPlainText(QStringLiteral("申请内存成功:0x") + QString::number(shellCodeBuf, 16));
	duint recvLen = 0;
	Script::Memory::Write(shellCodeBuf, ShellCode.data(), ShellCode.size(), &recvLen);

	HANDLE hThread = CreateRemoteThread(DbgGetProcessHandle(), 0, 0, (LPTHREAD_START_ROUTINE)shellCodeBuf, 0, 0, 0);
	if (hThread == 0) {
		return;
	}
	CloseHandle(hThread);
	return;
}

void MainWindow::on_LoadWindow()
{
	QList<QListWidgetItem*> vec_Window = ui.windowList->selectedItems();
	if (!vec_Window.size()) {
		return;
	}

	std::vector<unsigned char> ShellCode = {
		0xC8,0x00,0x00,0x00,                //enter 0x0,0x0		
	};
	for (unsigned int n = 0; n < vec_Window.size(); ++n) {
		std::vector<unsigned char> windowShell = {
			0x68,0x00,0x00,0x00,0x00,       //push windowId
			0xBB,0x00,0x00,0x00,0x00,       //mov ebx,LoadWindow
			0xFF,0xD3,                      //call ebx
			0x83,0xC4,0x4,                  //add esp,0x4
			0x6A,0x00,                      //push 0x0
			0x6A,0x01,                      //push 0x1
			0x6A,0xFF,                      //push -0x1
			0x6A,0x5,                       //push 0x5
			0x68,0x00,0x00,0x00,0x00,       //push ControlId
			0x68,0x00,0x00,0x00,0x00,       //push WindowId
			0xBB,0x00,0x00,0x00,0x00,       //mov ebx,SetWindowProperty
			0xFF,0xD3,                      //call ebx
			0x83,0xC4,0x18,                 //add esp,0x18
			0x6A,0x00,                      //push 0x0
			0x6A,0x00,                      //push 0x0
			0x6A,0xFF,                      //push -0x1
			0x6A,0x6,                       //push 0x6
			0x68,0x00,0x00,0x00,0x00,       //push ControlId
			0x68,0x00,0x00,0x00,0x00,       //push WindowId
			0xBB,0x00,0x00,0x00,0x00,       //mov ebx,SetWindowProperty
			0xFF,0xD3,                      //call ebx
			0x83,0xC4,0x18,                 //add esp,0x18
		};

		mid_GuiInfo* pGuiInfo = vec_Window[n]->data(Qt::UserRole).value<mid_GuiInfo*>();
		WriteUInt(&windowShell[1], pGuiInfo->m_windowId);
		WriteUInt(&windowShell[6], m_pEwnd->m_KrnlApp.krnl_MLoadBeginWin);
		WriteUInt(&windowShell[24], pGuiInfo->mVec_ControlInfo[0].m_controlId);
		WriteUInt(&windowShell[29], pGuiInfo->m_windowId);
		WriteUInt(&windowShell[34], m_pEwnd->m_KrnlApp.krnl_MWriteProperty);
		WriteUInt(&windowShell[52], pGuiInfo->mVec_ControlInfo[0].m_controlId);
		WriteUInt(&windowShell[57], pGuiInfo->m_windowId);
		WriteUInt(&windowShell[62], m_pEwnd->m_KrnlApp.krnl_MWriteProperty);
		ShellCode.insert(ShellCode.end(),windowShell.begin(),windowShell.end());
	}

	std::vector<unsigned char> endLoop = {
		0xBB,0x00,0x00,0x00,0x00,       //mov ebx,MessageLoop
		0xFF,0xD3,                      //call ebx
		0xC9,                           //leave
		0xC3                            //ret
	};
	WriteUInt(&endLoop[1],m_pEwnd->m_KrnlApp.krnl_MMessageLoop);
	ShellCode.insert(ShellCode.end(), endLoop.begin(), endLoop.end());

	duint shellCodeBuf = Script::Memory::RemoteAlloc(0, 0x1000);
	ui.outMsg->appendPlainText(QStringLiteral("申请内存成功:0x") + QString::number(shellCodeBuf, 16));
	duint recvLen = 0;
	Script::Memory::Write(shellCodeBuf, ShellCode.data(), ShellCode.size(), &recvLen);

	HANDLE hThread = CreateRemoteThread(DbgGetProcessHandle(), 0, 0, (LPTHREAD_START_ROUTINE)shellCodeBuf, 0, 0, 0);
	if (hThread == 0) {
		return;
	}
	CloseHandle(hThread);
	return;
}

void MainWindow::on_listMenu(const QPoint& pos)
{
	QListWidgetItem* curItem = ui.windowList->itemAt(pos);
	if (curItem == NULL) {
		return;
	}
	
	QMenu* popMenu = new QMenu(this);
	QAction* deleteSeed = new QAction(QStringLiteral("载入"), this);
	popMenu->addAction(deleteSeed);
	connect(deleteSeed, SIGNAL(triggered()), this, SLOT(on_LoadWindow()));
	popMenu->exec(QCursor::pos());
	delete popMenu;
	delete deleteSeed;
}


MainWindow::MainWindow(Ewnd* pEwnd, QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::Dialog| Qt::WindowCloseButtonHint);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowIcon(QIcon(":/ico/pizza.png"));
	m_pEwnd = pEwnd;

	ui.windowList->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.windowList,SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(on_listMenu(QPoint)));
	connect(ui.oneKeyTest, SIGNAL(clicked(bool)),this,SLOT(on_ButtonClicked(bool)));
	
	//————————————————————————————————————————

	ui.outMsg->appendPlainText(QStringLiteral("窗口总数:") + QString::number(m_pEwnd->mVec_window.size()));

	//按照窗体的控件个数进行排序
	qSort(m_pEwnd->mVec_window.begin(), m_pEwnd->mVec_window.end(), [](const mid_GuiInfo& a,const mid_GuiInfo& b) {
		return a.mVec_ControlInfo.size() > b.mVec_ControlInfo.size();
	});


	QTextCodec* codec = QTextCodec::codecForName("GB2312");
	for (unsigned int n = 0; n < m_pEwnd->mVec_window.size(); ++n) {
		int row = ui.windowList->count();

		QString windowId;
		windowId.sprintf("[0x%X]", m_pEwnd->mVec_window[n].m_windowId);
		std::string controlName = m_pEwnd->mVec_window[n].mVec_ControlInfo[0].m_controlName;
		QString windowName = codec->toUnicode(controlName.c_str(), controlName.length());

		QListWidgetItem* pItem = new QListWidgetItem(windowId + ":" + windowName);
		ui.windowList->insertItem(row, pItem);
		pItem->setData(Qt::UserRole, QVariant::fromValue<mid_GuiInfo*>(&m_pEwnd->mVec_window[n]));
	}

}