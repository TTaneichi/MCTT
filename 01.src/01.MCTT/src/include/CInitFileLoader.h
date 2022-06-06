/*
 * CInitFileLoader.h
 *
 *  Created on: 2021/05/07
 *      Author: 10097663
 */

#ifndef CINITFILELOADER_H_
#define CINITFILELOADER_H_

class CInitFileLoader {
public:
	CInitFileLoader();
	virtual ~CInitFileLoader();

	bool LoadInitFile();
private:
	//読み込んだ文字列をチェックしてデータとして格納する
	void checkAndSetData(char* buf, int size);

};

#endif /* CINITFILELOADER_H_ */
