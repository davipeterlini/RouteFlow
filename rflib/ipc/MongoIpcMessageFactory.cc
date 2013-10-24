/*
 * MongoIpcMessageFactory.cpp
 *
 *  Created on: Aug 3, 2013
 *      Author: vmware
 */

#include "MongoIpc.h"
#include "RFProtocol.h"
#include <iostream>
#include <mongo/bson/bsonobj.h>

using namespace std;

IpcMessage* MongoIpcMessageFactory::fromMessageType(mongo::BSONObj* mongoMessage) {
	IpcMessage* ipcMessage;
	mongo::BSONObj messageContent = (*mongoMessage)[MongoIpc::FIELD_NAME::CONTENT].Obj();

	// Set the specific fields of the message of the defined type
	switch ((*mongoMessage)[MongoIpc::FIELD_NAME::TYPE].Int()) {

	case PORT_REGISTER:
		ipcMessage = new PortRegister();

		((PortRegister*)ipcMessage)->set_vm_id(string_to<uint64_t>(messageContent["vm_id"].String()));
		((PortRegister*)ipcMessage)->set_vm_port(string_to<uint32_t>(messageContent["vm_port"].String()));
		((PortRegister*)ipcMessage)->set_hwaddress(MACAddress(messageContent["hwaddress"].String()));
		break;

	case PORT_CONFIG:
		ipcMessage = new PortConfig();

		((PortConfig*)ipcMessage)->set_vm_id(string_to<uint64_t>(messageContent["vm_id"].String()));
		((PortConfig*)ipcMessage)->set_vm_port(string_to<uint32_t>(messageContent["vm_port"].String()));
		((PortConfig*)ipcMessage)->set_operation_id(string_to<uint32_t>(messageContent["operation_id"].String()));
		break;

	case DATAPATH_PORT_REGISTER:
		ipcMessage = new DatapathPortRegister();

		((DatapathPortRegister*)ipcMessage)->set_ct_id(string_to<uint64_t>(messageContent["ct_id"].String()));
		((DatapathPortRegister*)ipcMessage)->set_dp_id(string_to<uint64_t>(messageContent["dp_id"].String()));
		((DatapathPortRegister*)ipcMessage)->set_dp_port(string_to<uint32_t>(messageContent["dp_port"].String()));
		break;

	case DATAPATH_DOWN:
		ipcMessage = new DatapathDown();

		((DatapathDown*)ipcMessage)->set_ct_id(string_to<uint64_t>(messageContent["ct_id"].String()));
		((DatapathDown*)ipcMessage)->set_dp_id(string_to<uint64_t>(messageContent["dp_id"].String()));
		break;

	case VIRTUAL_PLANE_MAP:
		ipcMessage = new VirtualPlaneMap();

		((VirtualPlaneMap*)ipcMessage)->set_vm_id(string_to<uint64_t>(messageContent["vm_id"].String()));
		((VirtualPlaneMap*)ipcMessage)->set_vm_port(string_to<uint32_t>(messageContent["vm_port"].String()));
		((VirtualPlaneMap*)ipcMessage)->set_vs_id(string_to<uint64_t>(messageContent["vs_id"].String()));
		((VirtualPlaneMap*)ipcMessage)->set_vs_port(string_to<uint32_t>(messageContent["vs_port"].String()));
		break;

	case DATA_PLANE_MAP:
		ipcMessage = new DataPlaneMap();

		((DataPlaneMap*)ipcMessage)->set_ct_id(string_to<uint64_t>(messageContent["ct_id"].String()));
		((DataPlaneMap*)ipcMessage)->set_dp_id(string_to<uint64_t>(messageContent["dp_id"].String()));
		((DataPlaneMap*)ipcMessage)->set_dp_port(string_to<uint32_t>(messageContent["dp_port"].String()));
		((DataPlaneMap*)ipcMessage)->set_vs_id(string_to<uint64_t>(messageContent["vs_id"].String()));
		((DataPlaneMap*)ipcMessage)->set_vs_port(string_to<uint32_t>(messageContent["vs_port"].String()));
		break;

	case ROUTE_MOD:
		ipcMessage = new RouteMod();
		((RouteMod*)ipcMessage)->set_mod(string_to<uint8_t>(messageContent["mod"].String()));
		((RouteMod*)ipcMessage)->set_id(string_to<uint64_t>(messageContent["id"].String()));
		((RouteMod*)ipcMessage)->set_matches(MatchList::to_vector(messageContent["matches"].Array()));
		((RouteMod*)ipcMessage)->set_actions(ActionList::to_vector(messageContent["actions"].Array()));
		((RouteMod*)ipcMessage)->set_options(OptionList::to_vector(messageContent["options"].Array()));
		break;
		//TODO find an alternative to match, option and actions converter

	default:
		//TODO use exceptions
		cout << "ERROR: Unknown message type" << endl;
		break;
	}

	// Set the common message fields
	ipcMessage->setMessageId((*mongoMessage)[MongoIpc::FIELD_NAME::ID].OID().toString());
	ipcMessage->setFrom((*mongoMessage)[MongoIpc::FIELD_NAME::FROM].String());
	ipcMessage->setTo((*mongoMessage)[MongoIpc::FIELD_NAME::TO].String());
	ipcMessage->setRead((*mongoMessage)[MongoIpc::FIELD_NAME::READ].Bool());

	return ipcMessage;
}

mongo::BSONObjBuilder* MongoIpcMessageFactory::fromMessageType(IpcMessage* message) {

	mongo::BSONObjBuilder *mongoMessage = new mongo::BSONObjBuilder;

	// Set the common message fields
	mongoMessage->append(MongoIpc::FIELD_NAME::FROM, message->getFrom());
	mongoMessage->append(MongoIpc::FIELD_NAME::TO, message->getTo());
	mongoMessage->append(MongoIpc::FIELD_NAME::TYPE, message->get_type());
	mongoMessage->append(MongoIpc::FIELD_NAME::READ, message->isRead());

	mongo::BSONObjBuilder *content = new mongo::BSONObjBuilder;

	// Set the specific fields of the message of the defined type
	switch (message->get_type()) {

	case PORT_REGISTER:
		content->append("vm_id", to_string<uint64_t>(((PortRegister*) message)->get_vm_id()));
		content->append("vm_port", to_string<uint32_t>(((PortRegister*) message)->get_vm_port()));
		content->append("hwaddress", ((PortRegister*) message)->get_hwaddress().toString());
		break;

	case PORT_CONFIG:
		content->append("vm_id", to_string<uint64_t>(((PortConfig*) message)->get_vm_id()));
		content->append("vm_port", to_string<uint32_t>(((PortConfig*) message)->get_vm_port()));
		content->append("operation_id", to_string<uint32_t>(((PortConfig*) message)->get_operation_id()));

		break;
	case DATAPATH_PORT_REGISTER:
	    content->append("ct_id", to_string<uint64_t>(((DatapathPortRegister*) message)->get_ct_id()));
	    content->append("dp_id", to_string<uint64_t>(((DatapathPortRegister*) message)->get_dp_id()));
	    content->append("dp_port", to_string<uint32_t>(((DatapathPortRegister*) message)->get_dp_port()));

		break;

	case DATAPATH_DOWN:
	    content->append("ct_id", to_string<uint64_t>(((DatapathDown*) message)->get_ct_id()));
	    content->append("dp_id", to_string<uint64_t>(((DatapathDown*) message)->get_dp_id()));
		break;

	case VIRTUAL_PLANE_MAP:
	    content->append("vm_id", to_string<uint64_t>(((VirtualPlaneMap*) message)->get_vm_id()));
	    content->append("vm_port", to_string<uint32_t>(((VirtualPlaneMap*) message)->get_vm_port()));
	    content->append("vs_id", to_string<uint64_t>(((VirtualPlaneMap*) message)->get_vs_id()));
	    content->append("vs_port", to_string<uint32_t>(((VirtualPlaneMap*) message)->get_vs_port()));
		break;

	case DATA_PLANE_MAP:
	    content->append("ct_id", to_string<uint64_t>(((DataPlaneMap*) message)->get_ct_id()));
	    content->append("dp_id", to_string<uint64_t>(((DataPlaneMap*) message)->get_dp_id()));
	    content->append("dp_port", to_string<uint32_t>(((DataPlaneMap*) message)->get_dp_port()));
	    content->append("vs_id", to_string<uint64_t>(((DataPlaneMap*) message)->get_vs_id()));
	    content->append("vs_port", to_string<uint32_t>(((DataPlaneMap*) message)->get_vs_port()));
		break;

	case ROUTE_MOD:
		content->append("mod", to_string<uint16_t>(((RouteMod*) message)->get_mod()));
		content->append("id", to_string<uint64_t>(((RouteMod*) message)->get_id()));
		content->appendArray("matches", MatchList::to_BSON(((RouteMod*) message)->get_matches()));
		content->appendArray("actions", ActionList::to_BSON(((RouteMod*) message)->get_actions()));
		content->appendArray("options", OptionList::to_BSON(((RouteMod*) message)->get_options()));
		break;

	default:
		//TODO use exceptions
		cout << "ERROR: Unknown message type" << endl;
		break;
	}

	mongoMessage->append(MongoIpc::FIELD_NAME::CONTENT, content->obj());

	return mongoMessage;
}

