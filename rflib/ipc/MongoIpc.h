#ifndef MONGOIPC_H_
#define MONGOIPC_H_

#include "Ipc.h"
#include <boost/function/function_fwd.hpp>
#include <mongo/client/dbclient.h>
#include <mongo/client/dbclient_rs.h>

// 1 MB for the capped collection
#define CC_SIZE 1048576

// Handle a maximum of 10 messages at a time
#define PENDINGLIMIT 10

/**
 * This class implements a database(Mongo) version of Ipc
 */
class MongoIpc: public Ipc {
public:
	// List of field names used by mongo
	struct FIELD_NAME {
		static const string ID;
		static const string FROM;
		static const string TO;
		static const string TYPE;
		static const string READ;
		static const string CONTENT;
	};
	/**
	 * Build a new communication channel
	 * @param userId The unique identifier of the process
	 * @param channel The mongo collection where messages will be saved and read from
	 */
	MongoIpc(string userId, string channel);

	/**
	 * Send a message
	 *
	 * @param message The message
	 */
	void send(IpcMessage* message);

	/**
	 * Start a thread listening new messages and processing them
	 *
	 * @param messageProcessor object that will process the message
	 */
	void listen(IpcMessageProcessor* messageProcessor);

	/**
	 * Start a thread listening new messages and processing them
	 *
	 * @param messageProcessor object that will process the message
	 */
	void parallelListen(IpcMessageProcessor* messageProcessor);

private:
	mongo::DBClientBase * databaseConnection;
	string channel; // Mongo collection where messages will be saved
	string userId; 	// A unique identifier of the process that will use it
	boost::mutex ipcMutex;

};

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

#endif /* MONGOIPC_H_ */
