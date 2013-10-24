#ifndef IPCMESSAGE_H_
#define IPCMESSAGE_H_

#include <iostream>

using namespace std;


class IpcMessage {
public:
	IpcMessage();

	string getMessageId();
	void setMessageId(string messageId);

	string getFrom();
	void setFrom(string from);

	string getTo();
	void setTo(string to);

	bool isRead();
	void setRead(bool read);

	virtual int get_type() = 0;

private:
	string messageId;
	string from;
	string to;
	bool read;

};
#endif /* IPCMESSAGE_H_ */

