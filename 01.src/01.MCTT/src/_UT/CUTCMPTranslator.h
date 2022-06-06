/*
 * CUTCMPTranslator.h
 *
 *  Created on: 2021/05/17
 *      Author: 10097663
 */

#ifndef UT_CUTCMPTRANSLATOR_H_
#define UT_CUTCMPTRANSLATOR_H_

class CUTCMPTranslator {
public:
	CUTCMPTranslator();
	virtual ~CUTCMPTranslator();

	void Execute();

private:
	void MakeMessageTest();

	void TranslateMessageTest();

	//データの表示
	void Display(unsigned char* message, int length);


};

#endif /* UT_CUTCMPTRANSLATOR_H_ */
