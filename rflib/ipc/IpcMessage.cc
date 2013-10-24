#include "Ipc.h"
string IpcMessage::getMessageId(){
	return this->messageId;
}

void IpcMessage::setMessageId(string messageId) {
	this->messageId = messageId;
}

string IpcMessage::getFrom() {
	return from;
}

void IpcMessage::setFrom(string from) {
	this->from = from;
}

string IpcMessage::getTo() {
	return to;
}

void IpcMessage::setTo(string to) {
	this->to = to;
}

bool IpcMessage::isRead() {
	return read;
}

void IpcMessage::setRead(bool read) {
	this->read = read;
}

IpcMessage::IpcMessage() {
	this->messageId = "null";
	this->to = "null";
	this->from = "null";
	this->read = false;
}
