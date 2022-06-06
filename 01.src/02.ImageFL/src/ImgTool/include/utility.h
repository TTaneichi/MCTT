//---------------------------------------------------------------------------
//画像とは直接関係ない、便利ツールとか数学関数をまとめる。
//

#ifndef utilityH
#define utilityH
//#include <Dialogs.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>

using namespace std;

//---------------------------------------------------------------------------
//void FileSort( TOpenDialog *Dialog );
//プレフィックスを簡単に付加する。パスがついてても大丈夫。
string AddPrefix( string FileName, string prefix );
// プレフィックスをとる。パスがついてても大丈夫。
string RemovePrefix( string FileName, int length );
//ファイルパスを返す
string ExtractFilePath( string FileName);
//ファイル名を返す
string ExtractFileName( string FileName);
//ファイル拡張子を返す
string ExtractFileExt( string FileName);
//ディレクトリが存在するか
bool DirectoryExists( string FilePath);
//ディレクトリを作成する
bool CreateDir( string FilePath);
//拡張子を変更する
string ChangeFileExt( string FileName, string ExtString);
//vector の指定した順位パーセンタイル範囲の平均を返す。
float vecpercentile( std::vector<int> &vec, float x, float y);
//乱数初期化
void randomize();
//乱数生成
int random(const int max);
//---------------------------------------------------------------------------
#endif

