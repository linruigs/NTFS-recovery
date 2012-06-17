// FileRecoveryDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
struct INFOFDISK
	{
		//根目录首簇
        UINT dwFirstCluOfIndex;
		//FAT表长度
        UINT dwFatLenth;
		//每扇区字节数
		unsigned short wByteOfEachSec;
        //保留扇区数
        unsigned short wReserveSec;
        //FAT表数目
        BYTE bNumOfFat;
        //每簇扇区数
        BYTE bSecsOfEachClu;
        //磁盘标志
		BYTE hDrive;
    };
struct NODEINFO
    {
		//文件的起始簇
        UINT StartClu;
        //文件长度
        UINT FileLength;
		//创建日期
		unsigned short CreateDate;
		//是文件还是文件夹
		unsigned short FileOrDir;
		//文件名
		CString Filename;
    };
// CFileRecoveryDlg 对话框
class CFileRecoveryDlg : public CDialog
{
// 构造
public:
	CFileRecoveryDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILERECOVERY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGetdir();
	// 获取路径
	CString GetPath(void);
	CButton m_GetDir;
	CString m_DestDir;
	// 本地磁盘扫描并显示列表
	void DiskDetect(void);
	CListBox m_DriveList;
	afx_msg void OnLbnSelchangeList4();
	CTreeCtrl m_DirTree;
	CImageList treeimage;
	int isFat32(BYTE hDrive);
	void ListDisplay(void);
	void GetDriveInfo(INFOFDISK* thisdrive, BYTE hDrive);
	CString m_Display;
	// 获取FAT表的首扇区
	UINT FatStartSec(void);
	// 获得第二FAT表的首扇区
	UINT FatBakStartSec(void);
	// 获得根目录表首扇区
	UINT IndexSec(void);
	// 转到该簇指向的扇区
	UINT CluToSec(UINT dwClu);
	// 判断该扇区是否为目录扇区
	int IsIndexSec(BYTE hDrive, UINT dwSec);
	// 遍历目录表项
	void SearchIndex(BYTE hDrive,UINT dwSec, HTREEITEM fatherNode);
	// 获取文件创建日期信息
	CString GetFileDate(unsigned short date);
	// 获得当前文件大小
	CString GetFileSize(UINT filelenth);
	afx_msg void OnTvnSelchangedTree3(NMHDR *pNMHDR, LRESULT *pResult);
	// 选中树节点信息的显示
	void TreeToDisplay(NODEINFO selnode);
	// 设置子节点复选框状态
	void SetChildCheck(HTREEITEM item, BOOL bCheck);
	// 设置父节点的复选框状态
	void SetParentCheck(HTREEITEM item, BOOL bCheck);
	// 设置各个节点的复选框状态
	void SetItemCheckState(HTREEITEM item, BOOL bCheck);
	afx_msg void OnNMClickTree3(NMHDR *pNMHDR, LRESULT *pResult);
	// 遍历目录树
	void TreeVisit(HTREEITEM hItem);
	afx_msg void OnBnClickedStart();
	CProgressCtrl m_Progress;
};
