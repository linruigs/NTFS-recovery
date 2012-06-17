#include <afxwin.h>         // MFC core and standard components
#define DOT		0x2e
#define BLANK	0x20
#define DIRFLAG	0x10
#define LNAME	0x0f
#define FDTSIZE	32
BOOL ReadSectors(BYTE hDrive, DWORD dwStartSec, WORD wSecs, LPBYTE lpSecBuf);
BOOL WriteSectors(HANDLE hDev, DWORD dwStartSec, WORD wSecs, LPBYTE lpSecBuf);
unsigned short MakeWord(unsigned char bLow,unsigned char bHeight);
unsigned long MakeLong(unsigned short bLow,unsigned short bHeight);
unsigned short LoWord(unsigned long bLong);
unsigned short HiWord(unsigned long bLong);
unsigned char LoByte(unsigned short bWord);
unsigned char HiByte(unsigned short bWord);
