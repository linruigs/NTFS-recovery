/* 
 * File:   main.cpp
 * Author: gaochenxi
 *
 * Created on 2012年6月5日, 下午8:48
 */

#include <cstdlib>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;

/*
 * 
 */
int compare(CString a, BYTE b[],int fcount) {
    int count;
    int isit = 0;
	for (count = 1; count < a.GetLength(); count++) {
        if (b[fcount+count] == a[count]) {
            isit = 1;
            continue;
        } else {
            isit=0;
            return 0;
        }
    }
    if(b[count]==' '||count==8){
        return 1;
    }
    else
        return 0;
}

int compareex(CString a, BYTE b[],int fcount) {
    int isit;
    if (a[0] == b[fcount+8]&&a[1] == b[fcount+9]&&a[2] == b[fcount+10]) {
        return 1;
    } else
        return 0;
}

int recover(CString filename,CString exname,CString disk,CString save) {
    HANDLE h, dec;
    int counter;
    int Dosremain;
    int NumofFat;
    int SecperClu;
    int SecsofFat;
    int isfount = 0;
    int BeginofDa;
    int BytperSec;

    BYTE bpbbuff[512];
    BYTE dirbuff[512];

    h = CreateFile(_T("\\\\.\\I:"), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    DWORD temp;

    SetFilePointer(h, 0, NULL, FILE_BEGIN);
    ReadFile(h, bpbbuff, 512, &temp, NULL);
    // cout<<(UINT)bpbbuff[0]<<"  "<<(UINT)bpbbuff[1]<<"  "<<(UINT)bpbbuff[2]<<"  "<<(UINT)bpbbuff[3]<<"  "<<"\n";
    Dosremain = (int) bpbbuff[15]*256 + (int) bpbbuff[14];
    NumofFat = (int) bpbbuff[16];
    BytperSec = (int) bpbbuff[12] * pow((double)16, (double)2)+(int) bpbbuff[11];
    SecsofFat = (int) bpbbuff[39] * pow((double)16, (double)6)+(int) bpbbuff[38] * pow((double)16, (double)4)+(int) bpbbuff[37] * pow((double)16, (double)2)+(int) bpbbuff[36];
    SecperClu = (int) bpbbuff[13];

    BeginofDa = Dosremain + NumofFat*SecsofFat;
    cout<<"please enter the file's name:";
    int startclu;
    int filesize;
    for (counter = 0; isfount == 0 && counter < 500; counter++) {
        SetFilePointer(h, (BeginofDa + counter)*512, NULL, FILE_BEGIN);
        ReadFile(h, dirbuff, 512, &temp, NULL);
        int facount;

        for (facount = 0; facount <= 480; facount = facount + 32) {
            if (compare(filename,dirbuff,facount)==1&&compareex(exname,dirbuff,facount)==1) {
                startclu = (int) dirbuff[facount + 21] * pow((double)16,(double) 6)+(int) dirbuff[facount + 20] * pow((double)16,(double) 4)+(int) dirbuff[facount + 27] * pow((double)16,(double) 2)+(int) dirbuff[facount + 26];
                filesize = (int) dirbuff[facount + 31] * pow((double)16,(double) 6)+(int) dirbuff[facount + 30] * pow((double)16,(double) 4)+(int) dirbuff[facount + 29] * pow((double)16,(double) 2)+(int) dirbuff[facount + 28];
                cout << startclu << "  " << filesize << "\n";
                isfount = 1;
                break;
            }
        }
    }

    int recount;
    int cluoffile;
    int justright;
    if (filesize % (BytperSec * SecperClu) == 0) {
        cluoffile = filesize / (BytperSec * SecperClu);
        justright = 1;
    } else {
        cluoffile = filesize / (BytperSec * SecperClu) + 1;
        justright = 0;
    }
	CString rod=disk+":\\"+save;
	WCHAR *b=new WCHAR[rod.GetLength()];
    for(int i=0;i<rod.GetLength();i++)
    b[i] = rod[i];
    b[rod.GetLength()] = '\0';

		
    dec = CreateFile(b, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);

    BYTE *databuff=new BYTE[BytperSec * SecperClu];
   //if (justright == 1) {
        for (recount = 0; recount < cluoffile; recount++) {
            SetFilePointer(h, (BeginofDa + (startclu - 2) * SecperClu)*512 + recount * BytperSec*SecperClu, NULL, FILE_BEGIN);
            ReadFile(h, databuff, BytperSec*SecperClu, &temp, NULL);
            cout << (int) databuff[0] << "  " << (int) databuff[1] << "  " << (int) databuff[2] << "  " << (int) databuff[3] << "\n";
            WriteFile(dec, databuff, BytperSec*SecperClu, &temp, NULL);
        }
    //}
    /*else {
        for (recount = 0; recount < cluoffile-1; recount++) {
            SetFilePointer(h, (BeginofDa + (startclu - 2) * SecperClu)*512 + recount * BytperSec*SecperClu, NULL, FILE_BEGIN);
            ReadFile(h, databuff, BytperSec*SecperClu, &temp, NULL);
            cout << (int) databuff[0] << "  " << (int) databuff[1] << "  " << (int) databuff[2] << "  " << (int) databuff[3] << "\n";
            WriteFile(dec, databuff, BytperSec*SecperClu, &temp, NULL);
        }
            SetFilePointer(h, (BeginofDa + (startclu - 2) * SecperClu)*512 + (recount+1) * BytperSec*SecperClu, NULL, FILE_BEGIN);
            ReadFile(h, databuff, BytperSec*SecperClu, &temp, NULL);
            cout << (int) databuff[0] << "  " << (int) databuff[1] << "  " << (int) databuff[2] << "  " << (int) databuff[3] << "\n";
            WriteFile(dec, databuff, filesize % (BytperSec * SecperClu), &temp, NULL);

    }*/
    /* SetFilePointer(h,BeginofDa*512,NULL,FILE_BEGIN);
       ReadFile(h,dirbuff,512,&temp,NULL);
       if (h == INVALID_HANDLE_VALUE){
           cout<<"ssssss\n";
           }
       if(dirbuff[0]=='X'){
           cout<<"ddddddddd\n";
       }*/
    cout << (int) dirbuff[0] << "  " << (int) dirbuff[1] << "  " << (int) dirbuff[2] << "  " << (int) dirbuff[3] << "  " << (int) dirbuff[4] << "\n";
    cout << Dosremain << "  " << NumofFat << "\n";
    cout << h << "\n";
    return 0;
}

