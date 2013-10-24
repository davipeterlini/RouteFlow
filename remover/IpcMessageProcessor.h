/*
 * IpcMessageProcessor.h
 *
 *  Created on: Aug 4, 2013
 *      Author: vmware
 */

#ifndef IPCMESSAGEPROCESSOR_H_
#define IPCMESSAGEPROCESSOR_H_

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

#endif /* IPCMESSAGEPROCESSOR_H_ */
