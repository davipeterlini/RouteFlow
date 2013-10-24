#include "MongoIpc.h"
#include <boost/thread.hpp>
#include <mongo/client/dbclient.h>
#include "../util/MongoFactory.h"
#include "../defs.h"

const string MongoIpc::FIELD_NAME::ID = "_id";
const string MongoIpc::FIELD_NAME::FROM = "from";
const string MongoIpc::FIELD_NAME::TO = "to";
const string MongoIpc::FIELD_NAME::TYPE = "type";
const string MongoIpc::FIELD_NAME::READ = "read";
const string MongoIpc::FIELD_NAME::CONTENT = "content";

MongoIpc::MongoIpc(string userId, string channel) {
	MongoFactory mf;
	this->databaseConnection = mf.createConnection();
	this->userId = userId;

	this->channel = MONGO_DB_NAME;
	this->channel.append(".");
	this->channel.append(channel);

	//create channel, if it doesn't exists
	this->databaseConnection->createCollection(channel, CC_SIZE, true);
	this->databaseConnection->ensureIndex(channel, BSON(FIELD_NAME::ID << 1));
	this->databaseConnection->ensureIndex(channel, BSON(FIELD_NAME::TO << 1));


}

void MongoIpc::send(IpcMessage* message) {
	boost::lock_guard<boost::mutex> lock(ipcMutex);


	mongo::BSONObjBuilder* dataBuilder = (mongo::BSONObjBuilder*) MongoIpcMessageFactory::fromMessageType(message);
	//mongo::BSONObjBuilder teste;
	//teste.append("Teste", "HAHAHA");
	for (int i = 0; i < MONGO_MAX_RETRIES; i++) {
		try {
			this->databaseConnection->insert(this->channel, dataBuilder->obj());
			break;
		} catch (mongo::DBException &e) {
			if (i + 1 == MONGO_MAX_RETRIES) {
				//TODO improve this message
				cout << "[ERROR]Mongo: Message could not be sent. Error: ("
						<< e.what() << ")" << endl;
				exit(1);
			}
			cout << "[ERROR]MongoIPC: Message not sent. Trying again in "
					<< MONGO_RETRY_INTERVAL << " seconds. [" << (i + 1) << "]"
					<< endl;
			sleep(MONGO_RETRY_INTERVAL);
		}
	}

	//free memory
	delete dataBuilder;

}

void MongoIpc::parallelListen(IpcMessageProcessor* messageProcessor) {
	boost::thread t(boost::bind(&MongoIpc::listen, this, messageProcessor));
	t.detach();
}

void MongoIpc::listen(IpcMessageProcessor* messageProcessor) {

	mongo::Query query = QUERY("to" << this->userId << "read" << false).sort("$natural");

	//TODO auto_ptr is deprecated
	auto_ptr<mongo::DBClientCursor> cur;

	while (true) {

		for (int i = 0; i < MONGO_MAX_RETRIES; i++) {
			try {
				cur = this->databaseConnection->query(this->channel, query,
						PENDINGLIMIT);

				break;
			} catch (mongo::DBException &e) {
				if (i + 1 == MONGO_MAX_RETRIES) {
					stringstream error;
					error
							<< "[ERROR]MongoIPC: Error retrieving unread messages \n"
							<< "Caused By: " << e.what();

					throw new runtime_error(error.str());
				}

				cout
						<< "[FAILOVER]MongoIPC: Error retrieving unread messages. Trying again in "
						<< MONGO_RETRY_INTERVAL <<
						" seconds. [" << (i + 1) << "/" << MONGO_MAX_RETRIES
						<< "]" << endl;
				sleep(MONGO_RETRY_INTERVAL);
			}
		}

		if (cur.get() == NULL) {
			// no results, just continue
			continue;
		}

		mongo::BSONObj envelope;

		while (true) {
			if (!cur->more()) {
				if (cur->isDead()) {
					break;
				}
				sleep(1);
				continue;
			}

			try {
				envelope = cur->nextSafe();

			} catch (mongo::DBException &e) {
				break;

			}

			IpcMessage* message = MongoIpcMessageFactory::fromMessageType(
					&envelope);
			messageProcessor->process(message);
			mongo::OID oid(message->getMessageId());

			for (int i = 0; i < MONGO_MAX_RETRIES; i++) {
				try {
					this->databaseConnection->update(this->channel,
							QUERY("_id" << oid),BSON("$set" << BSON(FIELD_NAME::READ << true)),false, true);

					break;
				} catch (mongo::DBException &e) {
					if(i + 1 == MONGO_MAX_RETRIES)
					{
						cout << "[ERROR]MongoIPC: The message (id: " << message->getMessageId() << ") could not be marked as read. Error: (" << e.what() << ")" << endl;
						exit(1);
					}

					cout << "[ERROR]MongoIPC: The message (id: " << message->getMessageId() << ") could not be marked as read. Trying again in " << MONGO_RETRY_INTERVAL << " seconds. [" << (i+1) << "]" << endl;
					sleep(MONGO_RETRY_INTERVAL);

				}
			}



			cout << "[OK]MongoIPC: The message (id: " << message->getMessageId()
					<< ") was marked as read." << endl;

			delete message;

		}

		usleep(50000); // 50ms
	}
}
