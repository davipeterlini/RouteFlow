/*
 * MongoIpcMessageFactory.h
 *
 *  Created on: Aug 3, 2013
 *      Author: vmware
 */

#ifndef MONGOIPCMESSAGEFACTORY_H_
#define MONGOIPCMESSAGEFACTORY_H_
#include "IpcMessage.h"
#include <mongo/client/dbclient.h>


/**
 * This class implements a factory to build a Ipc Message object from Bson Object and vice versa
 */
class MongoIpcMessageFactory {
public:
	/**
	 * Receives BSONObj and build an
	 * ipc message object, based on message type
	 *
	 * @param mongoMessage Bson Object
	 * @return ipc message
	 */
	static IpcMessage* fromMessageType(mongo::BSONObj* mongoMessage);

	/**
	 * Receives the ipc message object and build a Bson Object,
	 * based on message type
	 *
	 * @param message ipc message
	 * @return Bson Object Builder
	 */
	static mongo::BSONObjBuilder* fromMessageType(IpcMessage* message);

private:
	MongoIpcMessageFactory();
};

#endif /* MONGOIPCMESSAGEFACTORY_H_ */
