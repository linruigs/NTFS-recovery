// FileRecoveryDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
struct INFOFDISK
	{
		//��Ŀ¼�״�
        UINT dwFirstCluOfIndex;
		//FAT����
        UINT dwFatLenth;
		//ÿ�����ֽ���
		unsigned short wByteOfEachSec;
        //����������
        unsigned short wReserveSec;
        //FAT����Ŀ
        BYTE bNumOfFat;
        //ÿ��������
        BYTE bSecsOfEachClu;
        //���̱�־
		BYTE hDrive;
    };
struct NODEINFO
    {
		//�ļ�����ʼ��
        UINT StartClu;
        //�ļ�����
        UINT FileLength;
		//��������
		unsigned short CreateDate;
		//���ļ������ļ���
		unsigned short FileOrDir;
		//�ļ���
		CString Filename;
    };
// CFileRecoveryDlg �Ի���
class CFileRecoveryDlg : public CDialog
{
// ����
public:
	CFileRecoveryDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILERECOVERY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGetdir();
	// ��ȡ·��
	CString GetPath(void);
	CButton m_GetDir;
	CString m_DestDir;
	// ���ش���ɨ�貢��ʾ�б�
	void DiskDetect(void);
	CListBox m_DriveList;
	afx_msg void OnLbnSelchangeList4();
	CTreeCtrl m_DirTree;
	CImageList treeimage;
	int isFat32(BYTE hDrive);
	void ListDisplay(void);
	void GetDriveInfo(INFOFDISK* thisdrive, BYTE hDrive);
	CString m_Display;
	// ��ȡFAT���������
	UINT FatStartSec(void);
	// ��õڶ�FAT���������
	UINT FatBakStartSec(void);
	// ��ø�Ŀ¼��������
	UINT IndexSec(void);
	// ת���ô�ָ�������
	UINT CluToSec(UINT dwClu);
	// �жϸ������Ƿ�ΪĿ¼����
	int IsIndexSec(BYTE hDrive, UINT dwSec);
	// ����Ŀ¼����
	void SearchIndex(BYTE hDrive,UINT dwSec, HTREEITEM fatherNode);
	// ��ȡ�ļ�����������Ϣ
	CString GetFileDate(unsigned short date);
	// ��õ�ǰ�ļ���С
	CString GetFileSize(UINT filelenth);
	afx_msg void OnTvnSelchangedTree3(NMHDR *pNMHDR, LRESULT *pResult);
	// ѡ�����ڵ���Ϣ����ʾ
	void TreeToDisplay(NODEINFO selnode);
	// �����ӽڵ㸴ѡ��״̬
	void SetChildCheck(HTREEITEM item, BOOL bCheck);
	// ���ø��ڵ�ĸ�ѡ��״̬
	void SetParentCheck(HTREEITEM item, BOOL bCheck);
	// ���ø����ڵ�ĸ�ѡ��״̬
	void SetItemCheckState(HTREEITEM item, BOOL bCheck);
	afx_msg void OnNMClickTree3(NMHDR *pNMHDR, LRESULT *pResult);
	// ����Ŀ¼��
	void TreeVisit(HTREEITEM hItem);
	afx_msg void OnBnClickedStart();
	CProgressCtrl m_Progress;
};
