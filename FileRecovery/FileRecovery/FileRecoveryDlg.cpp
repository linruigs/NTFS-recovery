// FileRecoveryDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileRecovery.h"
#include "FileRecoveryDlg.h"
#include "SectorOp.h"
#include <vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//****************ȫ�ֱ���************************
std::vector<NODEINFO> g_NodeVec;
std::vector<NODEINFO> g_ToBeRec;
INFOFDISK g_CurrentDrive;
UINT64 g_TotalBytes;
int	g_isFat32;
//************************************************
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CFileRecoveryDlg �Ի���




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


// CFileRecoveryDlg ��Ϣ�������

BOOL CFileRecoveryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	HICON diricon=AfxGetApp()->LoadIcon(IDI_ICON);
	m_GetDir.SetIcon(diricon);
	DiskDetect();
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileRecoveryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFileRecoveryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileRecoveryDlg::OnBnClickedGetdir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_DestDir=GetPath();
	UpdateData(FALSE);
}

// ��ȡ·��
CString CFileRecoveryDlg::GetPath(void)
{
	CString strPath = _T("");
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("��ѡ��·��: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;    

	LPITEMIDLIST lpDlist = NULL;//�������淵����Ϣ��IDList
	bInfo.pidlRoot = lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo) ; //��ʾѡ��Ի���
	if(lpDlist != NULL)  //�û�����ȷ����ť
	{
		TCHAR chPath[255]; //�����洢·�����ַ���
		SHGetPathFromIDList(lpDlist, chPath);//����Ŀ��ʶ�б�ת�����ַ���
		strPath = chPath; //��TCHAR���͵��ַ���ת��ΪCString���͵��ַ���
	}
	return strPath;
}

// ���ش���ɨ�貢��ʾ�б�
void CFileRecoveryDlg::DiskDetect(void)
{
	TCHAR buf[100]; 
	DWORD len = GetLogicalDriveStrings(sizeof(buf)/sizeof(TCHAR),buf);
	for (TCHAR *s=buf; *s; s+=_tcslen(s)+1) 
	{ 
		CString strDisks; 
		LPCTSTR sDrivePath = s;     //�����̷� 
		strDisks += sDrivePath;      
		strDisks += _T(" ");
		m_DriveList.AddString(strDisks);
		strDisks.Empty();
	} 
}

void CFileRecoveryDlg::OnLbnSelchangeList4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

// �ж��Ƿ�ΪFAT32�ļ�ϵͳ
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
	CString text(_T("������Ϣ\r\n"));	
	m_Display.Empty();
	m_Display+=text;
	if(g_isFat32==1)
	{
		text.Format(_T("��FAT32ϵͳ\r\n"));
		m_Display+=text;
		text.Format(_T("ÿ�����ֽ�����%d\r\n"),g_CurrentDrive.wByteOfEachSec);
		m_Display+=text;
		text.Format(_T("������������%d\r\n"),g_CurrentDrive.wReserveSec);
		m_Display+=text;
		text.Format(_T("FAT����Ŀ��%d\r\n"),g_CurrentDrive.bNumOfFat);
		m_Display+=text;
		text.Format(_T("FAT���ȣ�%d\r\n"),g_CurrentDrive.dwFatLenth);
		m_Display+=text;
		text.Format(_T("ÿ����������%d\r\n"),g_CurrentDrive.bSecsOfEachClu);
		m_Display+=text;
		text.Format(_T("��Ŀ¼�״أ�%d\r\n"),g_CurrentDrive.dwFirstCluOfIndex);
	}
	else
	{
		text.Format(_T("����FAT32ϵͳ\r\n"));
	}
	m_Display+=text;
	UpdateData(FALSE);
}

void CFileRecoveryDlg::GetDriveInfo(INFOFDISK* thisdrive, BYTE hDrive)
{
	unsigned short wLow, wHigh;
	BYTE temp[1024];
	//���̱�־
	thisdrive->hDrive=hDrive;
	//��BPB��
	ReadSectors(hDrive, 0, 1, temp);
	//ÿ�����ֽ���
	thisdrive->wByteOfEachSec = MakeWord(temp[11], temp[12]);
	//����������
	thisdrive->wReserveSec = MakeWord(temp[14], temp[15]);
	//FAT����Ŀ
	thisdrive->bNumOfFat = temp[16];
	//FAT����
	wLow = MakeWord(temp[36], temp[37]);
	wHigh = MakeWord(temp[38], temp[39]);
	thisdrive->dwFatLenth = MakeLong(wLow, wHigh);
	//ÿ��������
	thisdrive->bSecsOfEachClu = temp[13];
	//��Ŀ¼�״غ�
	wLow = MakeWord(temp[44], temp[45]);
	wHigh = MakeWord(temp[46], temp[47]);
	thisdrive->dwFirstCluOfIndex = MakeLong(wLow, wHigh);
}

// ��ȡFAT���������
UINT CFileRecoveryDlg::FatStartSec(void)
{
	return (UINT)g_CurrentDrive.wReserveSec;
}

// ��õڶ�FAT���������
UINT CFileRecoveryDlg::FatBakStartSec(void)
{
	UINT ret;
	ret=g_CurrentDrive.wReserveSec+g_CurrentDrive.dwFatLenth;
	return ret;
}

// ��ø�Ŀ¼��������
UINT CFileRecoveryDlg::IndexSec(void)
{
	UINT ret;
	ret=g_CurrentDrive.wReserveSec+g_CurrentDrive.bNumOfFat*g_CurrentDrive.dwFatLenth+
		(g_CurrentDrive.dwFirstCluOfIndex-2)*g_CurrentDrive.bSecsOfEachClu;
	return ret;
}

// ת���ô�ָ�������
UINT CFileRecoveryDlg::CluToSec(UINT dwClu)
{
	UINT ret;
	ret=g_CurrentDrive.wReserveSec+g_CurrentDrive.bNumOfFat*g_CurrentDrive.dwFatLenth+
		(dwClu-2)*g_CurrentDrive.bSecsOfEachClu;
	return ret;
}

// �жϸ������Ƿ�ΪĿ¼����
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

// ����Ŀ¼����
void CFileRecoveryDlg::SearchIndex(BYTE hDrive, UINT dwSec, HTREEITEM fatherNode)
{
	BYTE temp[512];
	std::vector<char> namelist;
	int longname=0;
	UINT startsec=dwSec;
	ReadSectors(hDrive,startsec, 1, temp);
	for(int i=0;i<16;i++){
		if (temp[i * FDTSIZE] == DOT)//����ָ���Լ����ļ���Ŀ¼��
		{
			if (i == 15)//�ж���һ�����Ƿ���ΪĿ¼����
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
		if (temp[i * FDTSIZE + 11] == LNAME)//���ļ�������
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
		else if (temp[i * FDTSIZE + 11] == 0x0 || temp[i * FDTSIZE + 11] > 63)//�������ѷ�Ŀ¼����
		{
			break;
		}
		else//�����ȡ����FDT
		{
			CString filename;
			if(longname==1){//���ļ����Ķ���FDT����
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
			else//���ļ���FDT����
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
			if(temp[i*FDTSIZE+11]==DIRFLAG){//�ļ��д���
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
					SearchIndex(hDrive,CluToSec(nodetemp.StartClu),currentnode);//�����������ļ��п��Ƿ���ɾ�����ļ�
				}
			}
			else if (temp[i*FDTSIZE]==0xe5)//�ļ�����
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
		if (i == 15)//�����һ����
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

// ��ȡ�ļ�����������Ϣ
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

// ��õ�ǰ�ļ���С
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

// ѡ�����ڵ���Ϣ����ʾ
void CFileRecoveryDlg::TreeToDisplay(NODEINFO selnode)
{
	m_Display.Empty();
	CString text;
	if (selnode.FileOrDir==0)
	{
		text.Format(_T("�ļ�����Ϣ��\r\n"));
		m_Display+=text;
		text.Format(_T("�ļ��д�С��"));
	}
	else
	{
		text.Format(_T("�ļ���Ϣ��\r\n"));
		m_Display+=text;
		text.Format(_T("�ļ���С��"));
	}
	m_Display+=text;
	m_Display+=GetFileSize(selnode.FileLength);
	m_Display+=_T("\r\n");
	text.Format(_T("�������ڣ�"));
	m_Display+=text;
	m_Display+=GetFileDate(selnode.CreateDate);
	m_Display+=_T("\r\n");
	UpdateData(FALSE);
}

// �����ӽڵ㸴ѡ��״̬
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

// ���ø��ڵ�ĸ�ѡ��״̬
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

// ���ø����ڵ�ĸ�ѡ��״̬
void CFileRecoveryDlg::SetItemCheckState(HTREEITEM item, BOOL bCheck)
{
	SetChildCheck(item, bCheck);
	SetParentCheck(item, bCheck);
}

void CFileRecoveryDlg::OnNMClickTree3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

// ����Ŀ¼��
void CFileRecoveryDlg::TreeVisit(HTREEITEM hItem)
{
	//��Ҷ�ӽڵ�
	if(m_DirTree.ItemHasChildren(hItem))       
	{   
		HTREEITEM   hChildItem = m_DirTree.GetChildItem(hItem);       
		while(hChildItem!=NULL)       
		{   
			//�ݹ�������ӽڵ� 
			TreeVisit(hChildItem);     
			hChildItem  = m_DirTree.GetNextItem(hChildItem, TVGN_NEXT);       
		}       
	}  
	// ������������Ҷ�ӽڵ���в��� 
	else if (m_DirTree.GetCheck(hItem))
	{
		g_ToBeRec.push_back(g_NodeVec.at(m_DirTree.GetItemData(hItem)-1));
	}
}

void CFileRecoveryDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_DestDir.IsEmpty())
	{
		this->MessageBox(_T("ָ��·������Ϊ�գ�"));
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
