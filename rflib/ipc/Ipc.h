#ifndef IPC_H_
#define IPC_H_

#include <iostream>
#include <boost/function/function_fwd.hpp>

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

/**
 * This is a base class for all ipc message processor
 */
class IpcMessageProcessor {
public:
	/**
	 * This method will be called to process the message
	 * @param message message to be processed
	 */
	virtual void process(IpcMessage* message) = 0;
};


class Ipc {
public:
	/**
	 * Send a message
	 *
	 * @param message The message
	 */
	virtual void send(IpcMessage* message) = 0;

	/**
	 * Start listening new messages and processing them, without use Thread
	 *
	 * @param messageProcessor object that will process the message
	 */
	virtual void listen(IpcMessageProcessor* messageProcessor) = 0;

	/**
	 * Start a thread to listening new messages and processing them
	 *
	 * @param messageProcessor object that will process the message
	 */
	virtual void parallelListen(IpcMessageProcessor* messageProcessor) = 0;

};

#endif /* IPC_H_ */
