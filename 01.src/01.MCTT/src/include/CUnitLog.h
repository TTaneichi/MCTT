/*
 * CUnitLog.h
 *
 *  Created on: 2021/05/10
 *      Author: 10097663
 */

#ifndef CUNITLOG_H_
#define CUNITLOG_H_


#define SIMLOG_NORM(...) \
  CUnitLog::GetInstance()->putLog( __VA_ARGS__ )

#define SIMLOG( fmt, ... ) \
	SIMLOG_NORM( "%s L.%d " fmt, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__ )

#define SIMLOG_GUI(...)\
    do{ CUnitLog::GetInstance()->putLog(__VA_ARGS__); CUnitLog::GetInstance()->putGuiLog(__VA_ARGS__);}while(0)

#define LOG_UPDATE()\
	CUnitLog::GetInstance()->Update();

#ifndef NULL
#define NULL 0
#endif

class CUnitLog{
public:

	//デストラクタ
	~CUnitLog();

	static CUnitLog* GetInstance();

    void putLog(const char* format,  ...);
    void putGuiLog(const char* format, ...);
    void Update();

private:

	//コンストラクタ
	CUnitLog();

	static CUnitLog *m_pInstance;

	int m_LogIdx;
	int m_LogMax;
	int m_logSize;
	char * m_LogTbl;

    double m_start_up_time; //tickカウント用の初期値

};


#endif /* CUNITLOG_H_ */
