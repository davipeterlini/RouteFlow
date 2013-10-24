#ifndef MONGOFACTORY_H_
#define MONGOFACTORY_H_

#include <mongo/client/dbclient.h>
#include <mongo/client/dbclient_rs.h>

using namespace std;

class MongoFactory {
public:
	MongoFactory();

	/**
	 * Establishes a connection with MongoDB database
	 *
	 * @return connection to MongoDB database
	 */
	mongo::DBClientBase * createConnection();

private:
	string dbName;
	string connectionUri;
};

#endif /* MONGOFACTORY_H_ */
