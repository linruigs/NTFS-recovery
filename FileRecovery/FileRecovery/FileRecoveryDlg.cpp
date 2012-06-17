// FileRecoveryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileRecovery.h"
#include "FileRecoveryDlg.h"
#include "SectorOp.h"
#include <vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//****************全局变量************************
std::vector<NODEINFO> g_NodeVec;
std::vector<NODEINFO> g_ToBeRec;
INFOFDISK g_CurrentDrive;
UINT64 g_TotalBytes;
int	g_isFat32;
//************************************************
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFileRecoveryDlg 对话框




CFileRecoveryDlg::CFileRecoveryDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFileRecoveryDlg::IDD, pParent)
, m_DestDir(_T(""))
, m_Display(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileRecoveryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GETDIR, m_GetDir);
	DDX_Text(pDX, IDC_EDIT1, m_DestDir);
	DDX_Control(pDX, IDC_LIST4, m_DriveList);
	DDX_Control(pDX, IDC_TREE3, m_DirTree);
	DDX_Text(pDX, IDC_DISPLAY, m_Display);
	DDX_Control(pDX, IDC_PROGRESS2, m_Progress);
}

BEGIN_MESSAGE_MAP(CFileRecoveryDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_GETDIR, &CFileRecoveryDlg::OnBnClickedGetdir)
	ON_LBN_SELCHANGE(IDC_LIST4, &CFileRecoveryDlg::OnLbnSelchangeList4)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE3, &CFileRecoveryDlg::OnTvnSelchangedTree3)
	ON_NOTIFY(NM_CLICK, IDC_TREE3, &CFileRecoveryDlg::OnNMClickTree3)
	ON_BN_CLICKED(IDC_START, &CFileRecoveryDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// CFileRecoveryDlg 消息处理程序

BOOL CFileRecoveryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	HICON diricon=AfxGetApp()->LoadIcon(IDI_ICON);
	m_GetDir.SetIcon(diricon);
	DiskDetect();
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileRecoveryDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileRecoveryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFileRecoveryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileRecoveryDlg::OnBnClickedGetdir()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DestDir=GetPath();
	UpdateData(FALSE);
}

// 获取路径
CString CFileRecoveryDlg::GetPath(void)
{
	CString strPath = _T("");
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("请选择路径: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;    

	LPITEMIDLIST lpDlist = NULL;//用来保存返回信息的IDList
	bInfo.pidlRoot = lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo) ; //显示选择对话框
	if(lpDlist != NULL)  //用户按了确定按钮
	{
		TCHAR chPath[255]; //用来存储路径的字符串
		SHGetPathFromIDList(lpDlist, chPath);//把项目标识列表转化成字符串
		strPath = chPath; //将TCHAR类型的字符串转换为CString类型的字符串
	}
	return strPath;
}

// 本地磁盘扫描并显示列表
void CFileRecoveryDlg::DiskDetect(void)
{
	TCHAR buf[100]; 
	DWORD len = GetLogicalDriveStrings(sizeof(buf)/sizeof(TCHAR),buf);
	for (TCHAR *s=buf; *s; s+=_tcslen(s)+1) 
	{ 
		CString strDisks; 
		LPCTSTR sDrivePath = s;     //单个盘符 
		strDisks += sDrivePath;      
		strDisks += _T(" ");
		m_DriveList.AddString(strDisks);
		strDisks.Empty();
	} 
}

void CFileRecoveryDlg::OnLbnSelchangeList4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString drivename;
	if (m_DriveList.GetCurSel()>=0)
	{
		m_DriveList.GetText(m_DriveList.GetCurSel(),drivename);
		HTREEITEM hDirItem,sub;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_TREE);
		m_DirTree.DeleteAllItems();
		treeimage.Create(16,16,ILC_COLOR24|ILC_MASK,3,1);
		treeimage.Add(&bitmap,RGB(0,0,0));
		m_DirTree.SetImageList(&treeimage,TVSIL_NORMAL);
		g_isFat32=isFat32(drivename[0]-'A');
		GetDriveInfo(&g_CurrentDrive,drivename[0]-'A');
		g_NodeVec.clear();
		ListDisplay();
		hDirItem=m_DirTree.InsertItem(drivename,0,0);
		SearchIndex(drivename[0]-'A',IndexSec(),hDirItem);
		UpdateWindow();
	}
}

// 判断是否为FAT32文件系统
int CFileRecoveryDlg::isFat32(BYTE hDrive)
{
	int i;
	BYTE temp[1024] ;
	char FileSystem[6] ;
	ReadSectors(hDrive, 0, 1, temp);
	for (i = 0; i < 5; i++)
	{
		FileSystem[i] = temp[82 + i];
	}
	FileSystem[5]='\0';
	if (strcmp(FileSystem,"FAT32")==0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void CFileRecoveryDlg::ListDisplay(void)
{
	CString text(_T("磁盘信息\r\n"));	
	m_Display.Empty();
	m_Display+=text;
	if(g_isFat32==1)
	{
		text.Format(_T("是FAT32系统\r\n"));
		m_Display+=text;
		text.Format(_T("每扇区字节数：%d\r\n"),g_CurrentDrive.wByteOfEachSec);
		m_Display+=text;
		text.Format(_T("保留扇区数：%d\r\n"),g_CurrentDrive.wReserveSec);
		m_Display+=text;
		text.Format(_T("FAT表数目：%d\r\n"),g_CurrentDrive.bNumOfFat);
		m_Display+=text;
		text.Format(_T("FAT表长度：%d\r\n"),g_CurrentDrive.dwFatLenth);
		m_Display+=text;
		text.Format(_T("每簇扇区数：%d\r\n"),g_CurrentDrive.bSecsOfEachClu);
		m_Display+=text;
		text.Format(_T("根目录首簇：%d\r\n"),g_CurrentDrive.dwFirstCluOfIndex);
	}
	else
	{
		text.Format(_T("不是FAT32系统\r\n"));
	}
	m_Display+=text;
	UpdateData(FALSE);
}

void CFileRecoveryDlg::GetDriveInfo(INFOFDISK* thisdrive, BYTE hDrive)
{
	unsigned short wLow, wHigh;
	BYTE temp[1024];
	//磁盘标志
	thisdrive->hDrive=hDrive;
	//读BPB表
	ReadSectors(hDrive, 0, 1, temp);
	//每扇区字节数
	thisdrive->wByteOfEachSec = MakeWord(temp[11], temp[12]);
	//保留扇区数
	thisdrive->wReserveSec = MakeWord(temp[14], temp[15]);
	//FAT表数目
	thisdrive->bNumOfFat = temp[16];
	//FAT表长度
	wLow = MakeWord(temp[36], temp[37]);
	wHigh = MakeWord(temp[38], temp[39]);
	thisdrive->dwFatLenth = MakeLong(wLow, wHigh);
	//每簇扇区数
	thisdrive->bSecsOfEachClu = temp[13];
	//根目录首簇号
	wLow = MakeWord(temp[44], temp[45]);
	wHigh = MakeWord(temp[46], temp[47]);
	thisdrive->dwFirstCluOfIndex = MakeLong(wLow, wHigh);
}

// 获取FAT表的首扇区
UINT CFileRecoveryDlg::FatStartSec(void)
{
	return (UINT)g_CurrentDrive.wReserveSec;
}

// 获得第二FAT表的首扇区
UINT CFileRecoveryDlg::FatBakStartSec(void)
{
	UINT ret;
	ret=g_CurrentDrive.wReserveSec+g_CurrentDrive.dwFatLenth;
	return ret;
}

// 获得根目录表首扇区
UINT CFileRecoveryDlg::IndexSec(void)
{
	UINT ret;
	ret=g_CurrentDrive.wReserveSec+g_CurrentDrive.bNumOfFat*g_CurrentDrive.dwFatLenth+
		(g_CurrentDrive.dwFirstCluOfIndex-2)*g_CurrentDrive.bSecsOfEachClu;
	return ret;
}

// 转到该簇指向的扇区
UINT CFileRecoveryDlg::CluToSec(UINT dwClu)
{
	UINT ret;
	ret=g_CurrentDrive.wReserveSec+g_CurrentDrive.bNumOfFat*g_CurrentDrive.dwFatLenth+
		(dwClu-2)*g_CurrentDrive.bSecsOfEachClu;
	return ret;
}

// 判断该扇区是否为目录扇区
int CFileRecoveryDlg::IsIndexSec(BYTE hDrive, UINT dwSec)
{
	BYTE temp[512];
	ReadSectors(hDrive, dwSec, 1, temp);
	if (temp[11] == 0x0 || temp[11] > 63)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

// 遍历目录表项
void CFileRecoveryDlg::SearchIndex(BYTE hDrive, UINT dwSec, HTREEITEM fatherNode)
{
	BYTE temp[512];
	std::vector<char> namelist;
	int longname=0;
	UINT startsec=dwSec;
	ReadSectors(hDrive,startsec, 1, temp);
	for(int i=0;i<16;i++){
		if (temp[i * FDTSIZE] == DOT)//跳过指向自己的文件夹目录项
		{
			if (i == 15)//判断下一扇区是否仍为目录扇区
			{
				startsec++;
				if (IsIndexSec(hDrive, startsec)==1)
				{
					ReadSectors(hDrive, startsec, 1, temp);
					i = -1;
					dwSec++;
				}
			}
			continue;
		}
		if (temp[i * FDTSIZE + 11] == LNAME)//长文件名处理
		{
			longname = 1;
			for (int j = 0; j < FDTSIZE; j++)
			{
				if (j != 0 && j != 11 && j != 12 && j != 13 && j != 26 && j != 27 && temp[i * FDTSIZE + j] != 0xff)
				{
					namelist.push_back(temp[i * FDTSIZE + j]);
				}
			}
		}
		else if (temp[i * FDTSIZE + 11] == 0x0 || temp[i * FDTSIZE + 11] > 63)//该区域已非目录扇区
		{
			break;
		}
		else//处理读取到的FDT
		{
			CString filename;
			if(longname==1){//长文件名的短名FDT处理
				char *name=new char[namelist.size()];
				int lenth = 0;
				if (namelist.size() / 26 > 0)
				{
					for (int k = namelist.size() / 26 - 1; k >= 0; k--)
					{
						for (int y = 0; y < 26; y++)
						{
							name[lenth++] = namelist[k * 26 + namelist.size() % 26 + y];
						}
					}
				}
				if (namelist.size() % 26 > 0)
				{
					for (int y = 0; y < namelist.size() % 26; y++)
					{
						name[lenth++] = namelist[y];
					}
				}
				filename.Format(_T("%s\0"),name);
				delete []name;
				namelist.clear();
				longname=0;
			}
			else//短文件名FDT处理
			{
				char name[12];
				int k=0;
				for (k=0;k<8;k++){
					if (temp[i * FDTSIZE + k] != BLANK)
					{
						name[k] = temp[i * FDTSIZE + k];
					}
					else
					{
						break;
					}
				}
				char extname[3];
				int m = 0;
				if (temp[i * FDTSIZE + 11] != DIRFLAG)
				{
					for (m = 0; m < 3; m++)
					{
						if (temp[i * FDTSIZE + 8 + m] != BLANK)
						{
							extname[m] = temp[i * FDTSIZE + 8 + m];
						}
					}
				}
				for (int l = 0; l < m; l++)
				{
					if (l == 0)
					{
						name[k] = DOT;
					}
					name[k + 1 + l] = extname[l];
				}
				name[k + 1 + m] = '\0';
				filename=name;
			}
			if(temp[i*FDTSIZE+11]==DIRFLAG){//文件夹处理
				if (filename[filename.GetLength()-4]==_T('.')&&filename[filename.GetLength()-3]==_T('t')&&filename[filename.GetLength()-2]==_T('m')&&filename[filename.GetLength()-1]==_T('p'));
				else
				{
					HTREEITEM currentnode;
					NODEINFO nodetemp;
					nodetemp.FileOrDir=0;
					nodetemp.CreateDate=MakeWord(temp[i * FDTSIZE + 18], temp[i * FDTSIZE + 19]);
					nodetemp.StartClu=MakeLong(MakeWord(temp[i * FDTSIZE + 26], temp[i * FDTSIZE + 27]), MakeWord(temp[i * FDTSIZE + 20], temp[i * FDTSIZE + 21]));
					nodetemp.FileLength=MakeLong(MakeWord(temp[i * FDTSIZE + 28], temp[i * 32 + 29]), MakeWord(temp[i * FDTSIZE + 30], temp[i * FDTSIZE + 31]));
					nodetemp.Filename=filename;
					g_NodeVec.push_back(nodetemp);
					currentnode=m_DirTree.InsertItem(filename,1,1,fatherNode);
					m_DirTree.SetItemData(currentnode,g_NodeVec.size());
					UpdateWindow();
					SearchIndex(hDrive,CluToSec(nodetemp.StartClu),currentnode);//继续遍历该文件夹看是否有删除的文件
				}
			}
			else if (temp[i*FDTSIZE]==0xe5)//文件处理
			{
				HTREEITEM currentnode;
				NODEINFO nodetemp;
				nodetemp.FileOrDir=1;
				nodetemp.CreateDate=MakeWord(temp[i * FDTSIZE + 18], temp[i * FDTSIZE + 19]);
				nodetemp.StartClu=MakeLong(MakeWord(temp[i * FDTSIZE + 26], temp[i * FDTSIZE + 27]), MakeWord(temp[i * FDTSIZE + 20], temp[i * FDTSIZE + 21]));
				nodetemp.FileLength=MakeLong(MakeWord(temp[i * FDTSIZE + 28], temp[i * FDTSIZE + 29]), MakeWord(temp[i * FDTSIZE + 30], temp[i * FDTSIZE + 31]));
				nodetemp.Filename=filename;
				g_NodeVec.push_back(nodetemp);
				currentnode=m_DirTree.InsertItem(filename,2,2,fatherNode);
				m_DirTree.SetItemData(currentnode,g_NodeVec.size());
				UpdateWindow();
			}
		}
		if (i == 15)//检查下一扇区
		{
			startsec++;
			if (IsIndexSec(hDrive,startsec)==1)
			{
				ReadSectors(hDrive,startsec,1,temp);
				i=-1;
				startsec++;
			}
		}
	}
}

// 获取文件创建日期信息
CString CFileRecoveryDlg::GetFileDate(unsigned short date)
{
	CString ret;
	UINT year;
	UINT month;
	UINT day;
	UINT temp = (UINT)date;
	temp = temp & 0x1f;
	day = temp;
	temp = date;
	temp = temp >> 5;
	temp = temp & 0x0f;
	month = temp;
	temp = date;
	temp = temp >> 9;
	temp = temp +1980;
	year = temp;
	ret.Format(_T("%d-%d-%d"),year,month,day);
	return ret;
}

// 获得当前文件大小
CString CFileRecoveryDlg::GetFileSize(UINT filelenth)
{
	CString ret;
	double   l = 0;
	if (filelenth  > 1024*1024*1024)
	{
		l = filelenth / (1024 * 1024 * 1024 * 1.0);
		ret.Format(_T("%.3fGB"),l);
	}
	else if (filelenth > 1024 * 1024)
	{
		l = filelenth / (1024 * 1024 * 1.0);
		ret.Format(_T("%.3fMB"),l);
	}
	else if (filelenth > 1024)
	{
		l = filelenth / 1024*1.0;
		ret.Format(_T("%.3fKB"),l);
	}
	else
	{
		ret.Format(_T("%dB"),filelenth);
	}
	return ret;
}

void CFileRecoveryDlg::OnTvnSelchangedTree3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM currentnode;
	currentnode=m_DirTree.GetSelectedItem();
	NODEINFO selnode;
	if (m_DirTree.GetItemData(currentnode)!=0)
	{
		selnode=g_NodeVec.at(m_DirTree.GetItemData(currentnode)-1);
		TreeToDisplay(selnode);
	}
	*pResult = 0;
}

// 选中树节点信息的显示
void CFileRecoveryDlg::TreeToDisplay(NODEINFO selnode)
{
	m_Display.Empty();
	CString text;
	if (selnode.FileOrDir==0)
	{
		text.Format(_T("文件夹信息：\r\n"));
		m_Display+=text;
		text.Format(_T("文件夹大小："));
	}
	else
	{
		text.Format(_T("文件信息：\r\n"));
		m_Display+=text;
		text.Format(_T("文件大小："));
	}
	m_Display+=text;
	m_Display+=GetFileSize(selnode.FileLength);
	m_Display+=_T("\r\n");
	text.Format(_T("创建日期："));
	m_Display+=text;
	m_Display+=GetFileDate(selnode.CreateDate);
	m_Display+=_T("\r\n");
	UpdateData(FALSE);
}

// 设置子节点复选框状态
void CFileRecoveryDlg::SetChildCheck(HTREEITEM item, BOOL bCheck)
{
	HTREEITEM child = m_DirTree.GetChildItem(item);
	while(child)
	{
		m_DirTree.SetCheck(child, bCheck);
		SetChildCheck(child, bCheck);
		child = m_DirTree.GetNextItem(child, TVGN_NEXT);
	}
}

// 设置父节点的复选框状态
void CFileRecoveryDlg::SetParentCheck(HTREEITEM item, BOOL bCheck)
{
	HTREEITEM parent = m_DirTree.GetParentItem(item);
	if(parent == NULL)
	{
		return;
	}
	if(bCheck)
		m_DirTree.SetCheck(parent, bCheck);
	else
	{
		HTREEITEM bro = m_DirTree.GetNextItem(item, TVGN_NEXT);
		BOOL bFlag = false;
		while(bro)
		{
			if(m_DirTree.GetCheck(bro))
			{
				bFlag = true;
				break;
			}
			bro = m_DirTree.GetNextItem(bro, TVGN_NEXT);
		}
		if(!bFlag)
		{
			bro = m_DirTree.GetNextItem(item, TVGN_PREVIOUS);
			while(bro)
			{
				if(m_DirTree.GetCheck(bro))
				{
					bFlag = true;
					break;
				}
				bro = m_DirTree.GetNextItem(bro, TVGN_PREVIOUS);
			}
		}
		if(!bFlag)
			m_DirTree.SetCheck(parent, false);
	}
	SetParentCheck(parent, m_DirTree.GetCheck(parent));
}

// 设置各个节点的复选框状态
void CFileRecoveryDlg::SetItemCheckState(HTREEITEM item, BOOL bCheck)
{
	SetChildCheck(item, bCheck);
	SetParentCheck(item, bCheck);
}

void CFileRecoveryDlg::OnNMClickTree3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CPoint point;
	UINT uFlag;
	GetCursorPos(&point);
	m_DirTree.ScreenToClient(&point);

	HTREEITEM item = m_DirTree.HitTest(point, &uFlag);
	if((item) && (TVHT_ONITEMSTATEICON & uFlag))   
	{   
		BOOL bCheck = m_DirTree.GetCheck(item);
		SetItemCheckState(item, !bCheck);
	}   
}

// 遍历目录树
void CFileRecoveryDlg::TreeVisit(HTREEITEM hItem)
{
	//非叶子节点
	if(m_DirTree.ItemHasChildren(hItem))       
	{   
		HTREEITEM   hChildItem = m_DirTree.GetChildItem(hItem);       
		while(hChildItem!=NULL)       
		{   
			//递归遍历孩子节点 
			TreeVisit(hChildItem);     
			hChildItem  = m_DirTree.GetNextItem(hChildItem, TVGN_NEXT);       
		}       
	}  
	// 对满足条件的叶子节点进行操作 
	else if (m_DirTree.GetCheck(hItem))
	{
		g_ToBeRec.push_back(g_NodeVec.at(m_DirTree.GetItemData(hItem)-1));
	}
}

void CFileRecoveryDlg::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_DestDir.IsEmpty())
	{
		this->MessageBox(_T("指定路径不能为空！"));
	}
	else{
		g_ToBeRec.clear();
		g_TotalBytes=0;
		HTREEITEM rootitem=m_DirTree.GetRootItem();
		TreeVisit(rootitem);
		for (int i=0;i<g_ToBeRec.size();i++)
		{
			g_TotalBytes+=g_ToBeRec.at(i).FileLength;
		}
		m_Progress.SetRange32(0,g_TotalBytes);
		m_Progress.SetPos(0);
		for (int i=0;i<g_ToBeRec.size();i++)
		{
			CString destfile;
			BYTE buf[4096];
			destfile=m_DestDir+g_ToBeRec.at(i).Filename;
			HANDLE hDev=CreateFile(destfile,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
			int j;
			for (j=0;j<g_ToBeRec.at(i).FileLength/4096;j++)
			{
				ReadSectors(g_CurrentDrive.hDrive,CluToSec(g_ToBeRec.at(i).StartClu)+j*8,8,buf);
				WriteSectors(hDev,j*8,8,buf);
				if(j%100==0&&j!=0)
				{
					m_Progress.SetPos(m_Progress.GetPos()+100*4096);
					UpdateWindow();
				}
			}
			if (g_ToBeRec.at(i).FileLength % 4096 != 0)
			{
				DWORD dwTemp;
				ReadSectors(g_CurrentDrive.hDrive,CluToSec(g_ToBeRec.at(i).StartClu)+j*8,8,buf);
				WriteFile(hDev,buf,g_ToBeRec.at(i).FileLength % 4096,&dwTemp,NULL);
				m_Progress.SetPos(m_Progress.GetPos()+g_ToBeRec.at(i).FileLength % 4096);
				UpdateWindow();
			}
			CloseHandle(hDev);
		}
		m_Progress.SetPos(g_TotalBytes);
	}
}
