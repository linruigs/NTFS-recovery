#include "stdafx.h"
#include "SectorOp.h"
BOOL ReadSectors(BYTE hDrive, DWORD dwStartSec, WORD wSecs, LPBYTE lpSecBuf){
	if (hDrive <0)
	{
		return 0;
	}
	WCHAR DevName[] = _T("\\\\.\\A:");
	DevName[4] ='A' + hDrive;
	HANDLE hDev;
	hDev = CreateFile(DevName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDev == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	SetFilePointer(hDev, 512 * dwStartSec, 0, FILE_BEGIN);
	DWORD dwCB;
	BOOL ret = ReadFile(hDev, lpSecBuf, 512 * wSecs, &dwCB, NULL);
	CloseHandle(hDev);
	return ret;
}
BOOL WriteSectors(HANDLE hDev, DWORD dwStartSec, WORD wSecs, LPBYTE lpSecBuf){
	if (hDev == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	SetFilePointer(hDev, 512 * dwStartSec, 0, FILE_BEGIN);
	DWORD dwCB;
	BOOL ret = WriteFile(hDev, lpSecBuf, 512 * wSecs, &dwCB, NULL);
	return ret;
}
unsigned short MakeWord(unsigned char bLow,unsigned char bHeight){
	return MAKEWORD(bLow,bHeight);
}
unsigned long MakeLong(unsigned short bLow,unsigned short bHeight){
	return MAKELONG(bLow,bHeight);
}
unsigned short LoWord(unsigned long bLong){
	return LOWORD(bLong);
}
unsigned short HiWord(unsigned long bLong){
	return HIWORD(bLong);
}
unsigned char LoByte(unsigned short bWord){
	return LOBYTE(bWord);
}
unsigned char HiByte(unsigned short bWord){
	return HIBYTE(bWord);
}