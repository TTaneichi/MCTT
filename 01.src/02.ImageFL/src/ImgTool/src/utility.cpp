/*
 * utility.cpp
 *
 *  Created on: 2021/04/01
 *      Author: 10097663
 */
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include <sys/stat.h>
#include <direct.h>
#include <time.h>
#include <unistd.h>

/*
     プレフィックスを簡単に付加する。パスがついてても大丈夫。
     AnsiString outFile = AddPrefix( inFile, "p" ); のように使う。
*/
string AddPrefix( string FileName, string prefix ){
      return     ExtractFilePath( FileName )
               + prefix
               + ExtractFileName( FileName );
}
//---------------------------------------------------------------------------
/*
     プレフィックスをとる。パスがついてても大丈夫。
     3文字とる場合、
     AnsiString outFile = RemovePrefix( inFile, 3 ); のように使う。
*/
string RemovePrefix( string FileName, int length ){
      return     ExtractFilePath( FileName )
               + ExtractFileName( FileName ).substr(length);
}
//ファイルパスを返す
string ExtractFilePath( string FileName){

	int found = FileName.find_last_of("/\\");

	if (found == string::npos){
		return "";
	}

	return FileName.substr(0,found);
}

//ファイル名を返す
string ExtractFileName( string FileName){

	int found = FileName.find_last_of("/\\");

	return FileName.substr(found+1);
}
//ファイル拡張子を返す
string ExtractFileExt( string FileName){

	string FileNameWithoutPath = ExtractFileName(FileName);

	int found = FileNameWithoutPath.find_last_of(".");

	if (found == string::npos){
		return "";
	}

	return FileNameWithoutPath.substr(found);
}
//ディレクトリが存在するか
bool DirectoryExists( string FilePath){

    struct _stat s;
    int rc = _stat(FilePath.c_str(), &s);

    if (rc == 0){
    	return true;
    }

    return false;
}
//ディレクトリを作成する
bool CreateDir( string FilePath){

	int rc = _mkdir(FilePath.c_str());

    if (rc == 0){
    	return true;
    }

    return false;

}
//拡張子を変更する
string ChangeFileExt( string FileName, string ExtString){

	int found = FileName.find_last_of(".");
	if (found == string::npos){
		return FileName;
	}

	string retFileName = FileName.substr(0,found) + ExtString;
	return retFileName;

}
//---------------------------------------------------------------------------
/*
	 vector の指定した順位パーセンタイル範囲の平均を返す。
	 バーセンタいるを順位に直す際の端数は切りすて。
     参照で引いているが、ソートして返すので注意。
*/
float vecpercentile( std::vector<int> &vec, float fx, float fy)
{
	if (fx < 0 || fy < 0 || fx > 1 || fy > 1){
		return -8.0;
	}

	if (vec.size() < 1){
		return -4.0;
	}

	std::vector<int>::iterator iter  = vec.begin();
	std::sort(vec.begin(),vec.end());

	int ix, iy;
	ix = (int)(fx * vec.size()) - 1;//切り捨て
	if (ix < 0) ix = 0;

	iy = (int)(fy * vec.size());//切り捨て
	if (iy < 0) iy = 0;

	float ftmp;
	if (ix < iy){
		ftmp = std::accumulate(iter + ix, iter + iy, 0.0) / (iy - ix);
	}
	else if (iy < ix){
		ftmp = std::accumulate(iter + iy, iter + ix, 0.0) / (ix - iy);
	}
	else{
		ftmp = vec[ix];
	}

	return ftmp;
}
//---------------------------------------------------------------------------

void randomize(){

	srand((unsigned int)time(NULL));
}

int random(const int max){

	return (rand() % max);

}



