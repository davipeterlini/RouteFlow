import pymongo as mongo
import sys
import time
from rflib.defs import *

class MongoFactory:
    def __init__(self):
        self._connection_uri = MONGO_ADDRESS;
        self._db_name = MONGO_DB_NAME
    
    def create_connection(self):
        connection = None;
        
        for i in xrange(0, MONGO_MAX_RETRIES):
            try:
                connection = mongo.MongoClient(self._connection_uri)
                
                break;
            
                #self._producer_connection.write_concern = {'w':1, 'wtimeout': 1000}
                #print "DBINFO: Connection established with the following default ",
                #print "write concern -> ", self._producer_connection.write_concern
            
            except:
                if (i + 1) == MONGO_MAX_RETRIES:
                    print "Warning: Connection to Database failed. ",
                    print "Some operations might not be completed!"
                    print "ERR1; ", sys.exc_info()
                
                print "[RECOVERING]MongoIPC: Message not sent. ",
                print "Trying again in ", MONGO_RETRY_INTERVAL, " seconds. ",
                print "[", (i+1), "]"
                
                time.sleep(MONGO_RETRY_INTERVAL)
        
        print "Connection Established";
        return connection;
            
        
