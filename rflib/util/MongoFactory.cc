#include "MongoFactory.h"
#include "../defs.h"

using namespace std;
MongoFactory::MongoFactory() {
	this->connectionUri = MONGO_CONNECTION_URI;
	this->dbName = MONGO_DB_NAME;
}

mongo::DBClientBase * MongoFactory::createConnection() {
	mongo::DBClientBase * connection;

	for (int i = 0; i < MONGO_MAX_RETRIES; i++) {
		try {
			string error = "";
			mongo::ConnectionString connectionString =
					mongo::ConnectionString::parse(this->connectionUri, error);

			if (!connectionString.isValid())
				throw new std::runtime_error(
						"[ERROR]MongoFactory: The Mongo Connection URI is invalid. Correct it and run me again.");

			connection = connectionString.connect(error);

			if (error != "")
				throw new mongo::DBException(error, 0); //TODO review the code param
//		    	cout << "[WARNING]MongoFactory: A connection to the database could not be established" << endl <<
//		    			"Caused By: " << error << endl;

			break;

		} catch (mongo::DBException &e) {

			if (i + 1 == MONGO_MAX_RETRIES) {
				stringstream error;
				error
						<< "[WARNING]MongoFactory: A connection to the database could not be established\n"
						<< "Caused By: " << e.what();

				throw new runtime_error(error.str());
				//TODO find a nice way to exit...
			}

			cout
					<< "[FAILOVER]MongoFactory: A connection to the database could not be established. Trying again in "
					<< MONGO_RETRY_INTERVAL <<
					" seconds. [" << (i + 1) << "/" << MONGO_MAX_RETRIES << "]"
					<< endl;

			sleep(MONGO_RETRY_INTERVAL);
		}
	}

	return connection;

}

