// FileRW.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#pragma pack(1) 
# define ATTR_READ_ONLY         0x01 
# define ATTR_HIDDEN            0x02 
# define ATTR_SYSTEM            0x04 
# define ATTR_VOLUME_ID         0x08 
# define ATTR_DIRECTORY         0x10 
# define ATTR_ARCHIVE           0x20 
# define ATTR_LONG_NAME         (ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID) 
# define FAT_ITEM_UNLOCATED         0x00000000 
# define FAT_ITEM_LOCATED_LBOUND    0x00000002 
# define FAT_ITEM_LOCATED_UBOUND    0xFFFFFFEF 
# define FAT_ITEM_RESERVED_LBOUND   0xFFFFFFF0 
# define FAT_ITEM_RESERVED_UBOUND   0xFFFFFFF6 
# define FAT_ITEM_BAD_CLUSTER       0xFFFFFFF7 
# define FAT_ITEM_EOC_LBOUND        0xFFFFFFF8 
# define FAT_ITEM_EOC_UBOUND        0xFFFFFFFF 

struct BiosParameterBlock 
{ 
    /* FAT32 Structure Starting at Offset 11 */ 
    UINT16  BPB_BytePerSec;         //ÿ�������ֽ���ͨ��Ϊ512 
    UINT8   BPB_SecPerClus;         //ÿ�ص��������� 
    UINT16  BPB_ResvdSecCnt;        //���������� 
    UINT8   BPB_NumFATs;            //FAT������� 
    UINT16  BPB_RootEntCnt;         //FAT12/16��Ŀ¼��Ŀ¼���������FAT32Ϊ0 
    UINT16  BPB_TotSec16;           //FAT12/16ϵͳ��¼�����������������FAT32Ϊ0 
    UINT8   BPB_Media;              //���̽��� 
    UINT16  BPB_FATSz16;            //FAT12/16���ڱ���һ��FAT����ռ�õ������� 
    UINT16  BPB_SecPerTrk;          //ÿ�ŵ��������� 
    UINT16  BPB_NumHeads;           //��ͷ�� 
    UINT32  BPB_HiddSec;            //�������� 
    UINT32  BPB_TotSec32;           //FAT32��������������Ŀ 
     
    /* FAT32 Structure Starting at Offset 36 */ 
    UINT32  BPB_FATSz32;            //һ��FAT32����ռ�õ������� 
    UINT16  BPB_ExtFlag;            //��־λ 
    UINT16  BPB_FSVer;              //FAT32 version 
    UINT32  BPB_RootClus;           //FAT32 ��Ŀ¼���ڴ� 
    UINT16  BPB_FSInfo;             //FSINFO�ṹ���ڱ��������ڵ������� 
    UINT16  BPB_BkBootSec;          //Boot ��¼�ı����ڱ������ڵ������ţ�ͨ��Ϊ6 
    BYTE    BPB_Reserved[12];       //RESERVED 
	UINT8	BPB_PhysicalDriveNo;	//����������
	UINT8	BPB_ReservedFAT;		//����FAT32����
	UINT8	BPB_ExBootSig;			//��չ������ǩ
	UINT32	BPB_VolumeSN;			//�������
	char	BPB_VolumeLabel[11];	//���
	char	BPB_Systemid[8];		//ϵͳID
}; 

typedef struct BiosParameterBlock   FAT_BPB;
struct FAT32DirectoryEntry 
{ 
    BYTE    DIR_Name[11]; 
    UINT8   DIR_Attr; 
    UINT8   DIR_NTRes; 
    UINT8   DIR_CrtTimeTenth; 
    UINT16  DIR_CrtTime; 
    UINT16  DIR_CrtDate; 
    UINT16  DIR_LstAccDate; 
    UINT16  DIR_FstClusHI; 
    UINT16  DIR_WrtTime; 
    UINT16  DIR_WrtDate; 
    UINT16  DIR_FstClusLO; 
    UINT32  DIR_FileSize; 
}; 
typedef struct FAT32DirectoryEntry  FAT32_DIR_ENTRY; 
/* ��ȡFAT32�ļ�ϵͳ�е�һ�����������ı�ţ�λ�ڴ�2��*/ 
long long int GET_FAT_FIRST_DATA_SECTOR(FAT_BPB tmpbpb){
	return tmpbpb.BPB_ResvdSecCnt+tmpbpb.BPB_NumFATs*((tmpbpb.BPB_FATSz16!=0)?tmpbpb.BPB_FATSz16:tmpbpb.BPB_FATSz32);
}

FAT32_DIR_ENTRY *Get_Directory_Entry_Item (FAT_BPB *pFATBPB,UINT32 dwIndex,HANDLE hDevice) 
{	BYTE   chSectorBuffer[512]; 
	UINT32 dwDirectoryStartSector; 
    UINT16 wItemsPerSector; 
	DWORD dwcb;
    if ( (pFATBPB == NULL)||(hDevice == INVALID_HANDLE_VALUE)) 
    { 
        /* ��Ч������ */ 
        return NULL; 
    } 
	else
    { 
        /* ��ȡ��Ŀ¼���ڵ����� */ 
        
        
        dwDirectoryStartSector = GET_FAT_FIRST_DATA_SECTOR((*pFATBPB));  
        wItemsPerSector = pFATBPB->BPB_BytePerSec / sizeof(FAT32_DIR_ENTRY); 
        SetFilePointer(hDevice,(dwDirectoryStartSector + (dwIndex / wItemsPerSector))*512,NULL,FILE_BEGIN);
		ReadFile(hDevice,chSectorBuffer,512,&dwcb,NULL);
        (*(FAT32_DIR_ENTRY *)chSectorBuffer) = ((FAT32_DIR_ENTRY *)chSectorBuffer)[dwIndex*32]; 
    } 
     
    /* ���ؽ�� */ 
    return (FAT32_DIR_ENTRY *)chSectorBuffer; 
} 

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hDevice,txtDevice;
	UINT32 *fat32=NULL;
	BYTE buffer[512],buffer2[512];
	FAT32_DIR_ENTRY *dirtmp=NULL;
	FAT32_DIR_ENTRY dirrec;
	FAT_BPB *drivez=NULL;
	hDevice=CreateFile(_T("\\\\.\\Z:"),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if (hDevice == INVALID_HANDLE_VALUE){
		return 1;
	}
	DWORD tempdw;
	SetFilePointer(hDevice,0,NULL,FILE_BEGIN);
	ReadFile(hDevice,buffer,512,&tempdw,NULL);
	drivez=(FAT_BPB *)(buffer+11);
	dirtmp=Get_Directory_Entry_Item(drivez,0,hDevice);\
	dirrec=*dirtmp;
	txtDevice=CreateFile(_T("Z:\\sample.txt"),GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
	if (txtDevice == INVALID_HANDLE_VALUE){
		return 1;
	}
	/*SetFilePointer(hDevice,(drivez->BPB_ResvdSecCnt)*512,NULL,FILE_BEGIN);
	ReadFile(hDevice,buffer,512,&tempdw,NULL);
	fat32=(UNIT32 *)buffer;
	*(fat32+(dirtmp->DIR_FstClusLO))==0xffffff0f*/
	SetFilePointer(hDevice,(GET_FAT_FIRST_DATA_SECTOR(*(drivez))+(drivez->BPB_SecPerClus)*((dirrec.DIR_FstClusLO)-2))*512,NULL,FILE_BEGIN);
	ReadFile(hDevice,buffer2,512,&tempdw,NULL);
	WriteFile(txtDevice,buffer2,dirrec.DIR_FileSize,&tempdw,NULL);
	return 0;
}

