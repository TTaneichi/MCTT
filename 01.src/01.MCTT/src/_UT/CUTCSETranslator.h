/*
 * CUTCSETranslator.h
 *
 *  CSETranslatorのテストクラス
 *      Author: 10097663
 */

#ifndef UT_CUTCSETRANSLATOR_H_
#define UT_CUTCSETRANSLATOR_H_

class CUT_CSETranslator {
public:
	CUT_CSETranslator();
	virtual ~CUT_CSETranslator();

	void Execute();

private:
	void MakeMessageTest();

	void TranslateMessageTest();

	//データの表示
	void Diplay(unsigned char* message, int length);
};

#endif /* UT_CUTCSETRANSLATOR_H_ */
