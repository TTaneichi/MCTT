/*
 * AbstNotifyRasBox.h
 *
 *  Created on: 2021/06/21
 *      Author: 10097663
 */

#ifndef ABSTNOTIFYRASBOX_H_
#define ABSTNOTIFYRASBOX_H_

enum ERasBoxNotifyType{
	ERBNT_CONNECT = 0,		//接続完了
	ERBNT_DISCONNECT,		//接続切れ
	ERBNT_RES_COMPEX,		//照射完了通知
};


class AbstNotifyRasBox {
public:
	AbstNotifyRasBox();
	virtual ~AbstNotifyRasBox();

	virtual void UpdateRB(ERasBoxNotifyType type) = 0;			//通知


};

#endif /* ABSTNOTIFYRASBOX_H_ */
