/*
	画像data class
		Programmed by T. Kuwabara
*/
//---------------------------------------------------------------------------
#ifndef imageDataH
#define imageDataH

#include <fstream>
#include <String>
using namespace std;

#define CLIP(x, MIN, MAX)	(x > MAX ? MAX : ((x < MIN) ? MIN : x))   //ttsuji追加
#define MAX(x,y) ((x) > (y) ? (x) : (y))

//円周率
//#define PI 3.141592
#define PI 3.141592653589793238462643383279//30桁(コンペア不一致対策として 141205 ttsuji)

#define _EPS 1.0e-6	// 浮動小数点精度  (181009 ttsuji)

//欠陥補正用
const int iBAD = 1;
const int iCHANGE_TH = 90;     //感度補正可否　隣接画素との差

class TFIMG;
class TIIMG;
class TSIMG;
class TTMA;
class TSINGLE_DATA;
class TRECTANGLE;
class TQUADRANGLE;
class TPENTAGON;

//DEVO3のラグキャリブ用

class TROI_Lag
{
private:
//public:
	int 	m_iX0;
	int 	m_iY0;
	int 	m_iX1;
	int		m_iY1;
	double*	m_dQLave;//NumOfFrame個確保

public:
	TROI_Lag();
	~TROI_Lag();
	void Set(int iX0, int iY0, int iX1, int iY1);
	int GetX0()	const {return m_iX0;}
	int GetX1()	const {return m_iX1;}
	int GetY0()	const {return m_iY0;}
	int GetY1()	const {return m_iY1;}
	double* GetQLave() const {return m_dQLave;}//先頭ポインタを取得
	void DeleteROILag();
	void AllocateQLave(int iNumOfFrame);
	void SetQLave(int iNumOfFrame, double* dQLave);
	void SetQLave(int iFrame, double dQLave);
	double&	operator[](int iFr)			{return m_dQLave[iFr];}//doubleへの参照を返すことで左辺にて使用可能（配列の値m_dQLave[iFr]を変更できる）
//	double	operator[](int iFr)			{return m_dQLave[iFr];}//左辺で使用するとコンパイルエラーとなり使用不可
	double	operator[](int iFr) const	{return m_dQLave[iFr];}//TROI_Lagオブジェクトがconstであった場合でも[]で配列にアクセスできるようにする

};

//---------------------------------------------------------------------------
int Cmp(const void *a, const void *b);
int IntCmp(const void *a, const void *b);
int FloatCmp(const void *a, const void *b);
int DoubleCmp(const void *a, const void *b);
int CharCmp(const void* x, const void* y);//ttsuji
void quick_sort(const int *val , int * ret_val , const int num);//ttsuji
double Radians(const double dDegree);//ttsuji
double Degrees(const double dRadian);//ttsuji
//---------------------------------------------------------------------------

/*
    2D-FFT用
*/


void rdft2d(int, int, int, double **, double *, int *, double *);
void cdft2d(int, int, int, double **, double *, int *, double *);
void makewt(int nw, int *ip, double *w);
void makect(int nc, int *ip, double *c);
void rdft(int n, int isgn, double *a, int *ip, double *w);
void cdft2d_sub(int n1, int n2, int isgn, double **a, double *t, int *ip, double *w);
void rdft2d_sub(int n1, int n2, int isgn, double **a);
void cdft(int n, int isgn, double *a, int *ip, double *w);
void makeipt(int nw, int *ip);
void cftfsub(int n, double *a, int *ip, int nw, double *w);
void cftbsub(int n, double *a, int *ip, int nw, double *w);
void rftfsub(int n, double *a, int nc, double *c);
void rftbsub(int n, double *a, int nc, double *c);
void bitrv2(int n, int *ip, double *a);
void bitrv216(double *a);
void bitrv208(double *a);
void cftf1st(int n, double *a, double *w);
void cftrec4(int n, double *a, int nw, double *w);
void cftleaf(int n, int isplt, double *a, int nw, double *w);
void cftfx41(int n, double *a, int nw, double *w);
void cftf161(double *a, double *w);
void cftf081(double *a, double *w);
void cftf040(double *a);
void cftx020(double *a);
void cftb040(double *a);
void bitrv2conj(int n, int *ip, double *a);
void bitrv216neg(double *a);
void bitrv208neg(double *a);
void cftb1st(int n, double *a, double *w);
int cfttree(int n, int j, int k, double *a, int nw, double *w);
void cftmdl1(int n, double *a, double *w);
void cftmdl2(int n, double *a, double *w);
void cftf162(double *a, double *w);
void cftf082(double *a, double *w);

//---------------------------------------------------------------------------
//extern void HorUmbMakeDif(TFIMG& profile, const int iAveNum, TFIMG& dif);
//bool bHorUmbMainLowPass(TFIMG& fimg, TFIMG& orgFimg, const int iType, bool bOutDebug = false, string MainLowPassFilter = "");

//extern int round(float fVal);
//extern int round(double dVal);
//extern int roundUp(float fVal);

extern unsigned short int round(int iVal);
extern int roundUp(float fVal);


#endif
