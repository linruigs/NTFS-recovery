// FileRW.cpp : 定义控制台应用程序的入口点。
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
    UINT16  BPB_BytePerSec;         //每扇区的字节数通常为512 
    UINT8   BPB_SecPerClus;         //每簇的扇区数量 
    UINT16  BPB_ResvdSecCnt;        //保留扇区数 
    UINT8   BPB_NumFATs;            //FAT表的数量 
    UINT16  BPB_RootEntCnt;         //FAT12/16根目录中目录项的项数，FAT32为0 
    UINT16  BPB_TotSec16;           //FAT12/16系统记录卷标中扇区的总数，FAT32为0 
    UINT8   BPB_Media;              //磁盘介质 
    UINT16  BPB_FATSz16;            //FAT12/16用于保存一个FAT标所占用的扇区数 
    UINT16  BPB_SecPerTrk;          //每磁道的扇区数 
    UINT16  BPB_NumHeads;           //磁头数 
    UINT32  BPB_HiddSec;            //隐藏扇区 
    UINT32  BPB_TotSec32;           //FAT32卷中扇区的总数目 
     
    /* FAT32 Structure Starting at Offset 36 */ 
    UINT32  BPB_FATSz32;            //一个FAT32表中占用的扇区数 
    UINT16  BPB_ExtFlag;            //标志位 
    UINT16  BPB_FSVer;              //FAT32 version 
    UINT32  BPB_RootClus;           //FAT32 根目录所在簇 
    UINT16  BPB_FSInfo;             //FSINFO结构体在保留扇区内的扇区号 
    UINT16  BPB_BkBootSec;          //Boot 记录的备份在保留区内的扇区号，通常为6 
    BYTE    BPB_Reserved[12];       //RESERVED 
	UINT8	BPB_PhysicalDriveNo;	//物理驱动号
	UINT8	BPB_ReservedFAT;		//保留FAT32分区
	UINT8	BPB_ExBootSig;			//扩展引导标签
	UINT32	BPB_VolumeSN;			//分区序号
	char	BPB_VolumeLabel[11];	//卷标
	char	BPB_Systemid[8];		//系统ID
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
/* 获取FAT32文件系统中第一个数据扇区的编号（位于簇2）*/ 
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
        /* 无效的输入 */ 
        return NULL; 
    } 
	else
    { 
        /* 获取根目录所在的扇区 */ 
        
        
        dwDirectoryStartSector = GET_FAT_FIRST_DATA_SECTOR((*pFATBPB));  
        wItemsPerSector = pFATBPB->BPB_BytePerSec / sizeof(FAT32_DIR_ENTRY); 
        SetFilePointer(hDevice,(dwDirectoryStartSector + (dwIndex / wItemsPerSector))*512,NULL,FILE_BEGIN);
		ReadFile(hDevice,chSectorBuffer,512,&dwcb,NULL);
        (*(FAT32_DIR_ENTRY *)chSectorBuffer) = ((FAT32_DIR_ENTRY *)chSectorBuffer)[dwIndex*32]; 
    } 
     
    /* 返回结果 */ 
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

