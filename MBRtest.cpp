// MBRtest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#define DPT_START_ADDRESS      0x01BE 
# define OS_INDICATOR_NTFS     0x07 
#define DPT_PARTITION_START_SECTOR(__DPT_ITEM_ADDR)    \(__DPT_ITEM_ADDR)->RelativeSectors 
using namespace std;
struct DiskPartitionTableItem 
{ 
    BYTE        ActivePartition; 
    BYTE        StartHead; 
    unsigned short   StartSector:6; 
    unsigned short   StartCylinder:10; 
    BYTE        OSIndicator; 
    BYTE        EndHead; 
    unsigned short   EndSector:6; 
    unsigned short   EndCylinder:10; 
    UINT32      RelativeSectors; 
    UINT32      TotalSectors; 
};
typedef struct DiskPartitionTableItem DPT_ITEM; 
DPT_ITEM *Get_Next_Disk_Partition (BYTE *pchMasterBootSector,DPT_ITEM *pdptPartition) 
{ 
    if (pchMasterBootSector == NULL) 
    { 
        /* 无效的输入 */ 
        return NULL; 
    }     
     
    if (pdptPartition == NULL) 
    { 
        /* 要求返回第一个分区表项 */ 
        pdptPartition = (DPT_ITEM *)(pchMasterBootSector + DPT_START_ADDRESS); 
    } 
    else 
    { 
        /* 指向下一个分区表项 */ 
        pdptPartition += 1; 
    } 
     
    /* 进行分区表项的有效性检测 */ 
    { 
        UINT8 n = 0; 
        BOOL bIfValid = FALSE; 
        for (n = 0;n < sizeof(DPT_ITEM);n++) 
        { 
            if (((BYTE *)pdptPartition)[n] != 0x00) 
            { 
                bIfValid = TRUE; 
                break; 
            } 
        } 
        if (!bIfValid) 
        { 
            /* 表项为空 */ 
            return NULL; 
        } 
    } 
     
    return pdptPartition; 
} 

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hDevice;
	BYTE buffer[1024]={0};
	DPT_ITEM *pDPTItem = NULL; 
	char sb;
	int count=0;
	hDevice=CreateFile(_T("\\\\.\\PhysicalDrive0"),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if (hDevice == INVALID_HANDLE_VALUE){
		return 1;
	}
	SetFilePointer(hDevice,0,NULL,FILE_BEGIN);
	DWORD dwcb;
	ReadFile(hDevice,buffer,512,&dwcb,NULL);
	do{
		count++;
		pDPTItem = Get_Next_Disk_Partition(buffer,pDPTItem); 
		if ((pDPTItem != NULL) && (pDPTItem->OSIndicator == OS_INDICATOR_NTFS)) { 
			cout<<"disk "<<count<<" is NTFS\n";
			continue;
		} 
		cout<<"disk "<<count<<" is not NTFS\n";
	}while(pDPTItem != NULL);
	while(cin>>sb){
		if (sb=='a')
			break;
	}
	return 0;
}

