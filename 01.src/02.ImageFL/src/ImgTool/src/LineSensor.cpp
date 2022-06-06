//---------------------------------------------------------------------------

#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <string>
#include <string.h>
#include "CApplication.h"
#include "LineSensor.h"

using namespace std;

//---------------------------------------------------------------------------
/*
	Read "*.csv"
       	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TLNSR::ReadCsv(const string fileName)
{
    const char KUGIRI = ' ';        //data 区切りchar

    fstream file;
    int i, j;
    char tempChar[5 * 3000];
    char *pt;
    char *pos;
    int ctx, cty;
    CTDAT temp;
    bool strange;
    fstream debug;

    temp.NewDt(3000, 5000, LNSR_BIT_NUM);

    file.open(fileName.c_str(), ios::in);
    if(file.bad()) return(false);

    cty = 0;
    while(!file.eof()){
        file.getline(tempChar, sizeof(tempChar));
//        if(file.eof()) break;
        pt = tempChar;
		if(*pt < ' ') break;
        ctx = 0;
        while((pos = strchr(pt, KUGIRI)) != NULL){
        	*pos = 0;
            temp.Dt[cty][ctx] = (unsigned short int)atoi(pt);
            pt = pos + 1;
            ++ctx;
        }
        temp.Dt[cty][ctx] = (unsigned short int)atoi(pt);
        ++ctx;
        ++cty;
    }
/*    pt = tempChar;
    if(*pt >= '0'){
    	ctx = 0;
    	while((pos = strchr(pt, ',')) != NULL){
    		*pos = 0;
        	temp.Dt[cty][ctx] = atoi(pt);
        	pt = pos + 1;
        	++ctx;
    	}
    }
    temp.Dt[cty][ctx] = atoi(pt);
    ++ctx;
    ++cty;
*/
/*    file.getline(tempChar, sizeof(tempChar));
    while(!file.eof()){
        ctx = 0;
        pt = tempChar;
        while((pos = strchr(pt, ',')) != NULL){
        	*pos = 0;
            temp.Dt[cty][ctx] = atoi(pt);
            pt = pos + 1;
            ++ctx;
        }
        temp.Dt[cty][ctx] = atoi(pt);
        ++ctx;
        ++cty;
    	file.getline(tempChar, sizeof(tempChar));
    }
    pt = tempChar;
    if(*pt != 0){
    	ctx = 0;
    	while((pos = strchr(pt, ',')) != NULL){
    		*pos = 0;
        	temp.Dt[cty][ctx] = atoi(pt);
        	pt = pos + 1;
        	++ctx;
    	}
    }
    temp.Dt[cty][ctx] = atoi(pt);
    ++ctx;
    ++cty;
*/
    debug.open("debug.txt", ios::out|ios::trunc);
    debug << "x" << '\t' << "y" << '\t' << "value" << endl;
    strange = false;
    NewDt(cty, ctx, LNSR_BIT_NUM);
    for(i = 0 ; i < cty ; ++i){
    	for(j = 0 ; j < ctx ; ++j){
        	if (temp.Dt[i][j] > temp.DtMax){
            	strange = true ;
                Dt[i][j] = temp.DtMax;
                debug << (j + 1) << '\t' << (i + 1) << '\t' << temp.Dt[i][j] << endl;
        	}else {
        		Dt[i][j] = temp.Dt[i][j];
            }
        }
    }
    debug.close();

    if(strange == true)
    	CApplication::GetInstance()->MessageBox("異常なDataがありました", "Warning", MB_OK);

    return(true);
}
//---------------------------------------------------------------------------
/*
	Read "*.dta"
        dta file 16bit binary data を ascii　で出力 1 line \r\nで終了
       	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TDTA::ReadDta(const string fileName)
{
    int handle;		//file handler
    int fileLen;		//file length(byte)
    fstream file;
    int i, j;
    char tempChar[4 * 5000 + 2];             //4byte x 最大主走査画素数 + 2
    int ctx, cty;     //主・副画素数
    char *pt;
    int num;

    if((handle = open(fileName.c_str(), O_RDONLY)) == -1){
    	CApplication::GetInstance()->MessageBox(string(fileName + " openできませんでした").c_str(), "Error", MB_OK);
    	return(false);
    }
    fileLen = filelength(handle);
    close(handle);

    file.open(fileName.c_str(), ios::in);
    if(file.bad()) return(false);

    file.getline(tempChar, sizeof(tempChar));
    ctx = strlen(tempChar) / 4;               //1画素は4byte
    cty = (fileLen + 2) / (strlen(tempChar) + 2);                             //(ファイルの大きさ+おまけ) / (主画素数+\r\n)
    NewDt(cty, ctx, DTA_BIT_NUM);

    pt = tempChar;
    for(i = 0 ; i < ctx ; ++i){
          if(*pt > '9'){
                 num = *pt - 55;            //ABCDEFの数値変換
          }else{
              num = *pt - '0';
          }
          Dt[0][i] = (unsigned short int)num;
          Dt[0][i] <<= 4;
          ++pt;
          if(*pt > '9'){
                 num = *pt - 55;
          }else{
              num = *pt - '0';
          }
          Dt[0][i] += (unsigned short int)num;
          Dt[0][i] <<= 4;
          ++pt;
          if(*pt > '9'){
                 num = *pt - 55;
          }else{
              num = *pt - '0';
          }
          Dt[0][i] += (unsigned short int)num;
          Dt[0][i] <<= 4;
          ++pt;
          if(*pt > '9'){
                 num = *pt - 55;
          }else{
              num = *pt - '0';
          }
          Dt[0][i] += (unsigned short int)num;
          ++pt;
    }

    for(j = 1 ; j < cty ; ++j){
          file.getline(tempChar, sizeof(tempChar));
          pt = tempChar;
          for(i = 0 ; i < ctx ; ++i){
                if(*pt > '9'){
                       num = *pt - 55;
                 }else{
                       num = *pt - '0';
                 }
                 Dt[j][i] = (unsigned short int)num;
                 Dt[j][i] <<= 4;
                 ++pt;
                 if(*pt > '9'){
                        num = *pt - 55;
                 }else{
                       num = *pt - '0';
                 }
                 Dt[j][i] += (unsigned short int)num;
                 Dt[j][i] <<= 4;
                 ++pt;
                 if(*pt > '9'){
                        num = *pt - 55;
                 }else{
                        num = *pt - '0';
                 }
                 Dt[j][i] += (unsigned short int)num;
                 Dt[j][i] <<= 4;
                 ++pt;
                 if(*pt > '9'){
                        num = *pt - 55;
                 }else{
                        num = *pt - '0';
                 }
                 Dt[j][i] += (unsigned short int)num;
                 ++pt;
          }
    }

    file.close();
    return(true);
/*
    fstream file;
    int i, j;
    char tempChar[4 * 5000 + 2];
    AnsiString in;
    AnsiString num;
    int ctx, cty;
    TDAT temp;
 //   bool strange;
 //   fstream debug;

    temp.NewDt(5000, 5000, DTA_BIT_NUM);

    file.open(fileName.c_str(), ios::in);
    if(file.bad()) return(false);

    cty = 0;
    while(!file.eof()){
        file.getline(tempChar, sizeof(tempChar));
        in = tempChar;
        if(in.Length() < 4) break;
        ctx = 0;
        while(in.Length() >= 4){
            num = "0x" + in.SubString(1,4);
            temp.Dt[cty][ctx] = num.ToInt();
            in = in.SubString(5,in.Length() - 4);
            ++ctx;
        }
        ++cty;
    }

    debug.open("debug.txt", ios::out|ios::trunc);
    debug << "x" << '\t' << "y" << '\t' << "value" << endl;
    strange = false;

    NewDt(cty, ctx, DTA_BIT_NUM);
    for(i = 0 ; i < cty ; ++i){
    	for(j = 0 ; j < ctx ; ++j){
              	if (temp.Dt[i][j] > temp.DtMax){
            	strange = true ;
                Dt[i][j] = temp.DtMax;
                debug << (j + 1) << '\t' << (i + 1) << '\t' << temp.Dt[i][j] << endl;
        	} else if (temp.Dt[i][j] < 0){
        		strange = true ;
                Dt[i][j] = 0 ;
                debug << (j + 1) << '\t' << (i + 1) << '\t' << temp.Dt[i][j] << endl;
        	} else {

        		Dt[i][j] = temp.Dt[i][j];
            }
        }
    }
    debug.close();

    if(strange == true)
    	Application->MessageBox("異常なDataがありました", "Warning", MB_OK);

    return(true);
*/
}
//---------------------------------------------------------------------------
/*
	Read "*.bmp"
       	引数	File Name

        戻り値	成功->true
        		失敗->false
*/
bool TBMP24::ReadBitMap24(const string fileName)
{
    const int MAIN = 19;        //主画素位置(序数）
    const int SUB = 23;         //副画素位置(序数）
    const int HEDDER = 54;      //hedder byte数

    int handle;		//file handler
    int i, j, ct;
    unsigned char skip[HEDDER];
    unsigned char *buffer;
    int main, sub;

    if ((handle = open(fileName.c_str(), O_RDONLY|O_BINARY)) == -1){
    	CApplication::GetInstance()->MessageBox(string(fileName + " openできませんでした").c_str(), "Error", MB_OK);
    	return(false);
    }

    if (read(handle, skip, HEDDER) == -1){
    	CApplication::GetInstance()->MessageBox("読み飛ばしでエラー発生しました", "Error", MB_OK);
        close(handle);
        return(false);
    }

    main = skip[MAIN - 1] + skip[MAIN] * 256;
    sub = skip[SUB - 1] + skip[SUB] * 256;

    NewDt(sub, main, 10);

    buffer = new unsigned char[main * sub * 3];		//Bufferを確保
    if((read(handle, buffer, main * sub * 3)) == -1){
    	CApplication::GetInstance()->MessageBox(string(fileName + " readできませんでした").c_str(), "Error", MB_OK);
        close(handle);
        delete[] buffer;
        return(false);
    }
    close(handle);

    ct = 0;
    for(i = 0 ; i < sub ; ++i){
        for(j = 0 ; j < main ; ++j){
            Dt[i][j] = (unsigned short int)
                    (buffer[ct] + buffer[ct + 1] + buffer[ct + 2]);
            ct += 3;
        }
    }
    delete[] buffer;

    return(true);
}
//---------------------------------------------------------------------------
