#pragma once

#include "xboxinternals.h"
#include "pointerVector.h"
#include "utils.h"
#include <xtl.h>
#include <winsockx.h>
#include <string>

typedef struct RequestBody {
	char* verb;
	char* path;
	char* query;
	pointerVector<char*>* headers;
} RequestBody;

typedef struct FormPart {

	pointerVector<char*>* headers;
	utils::dataContainer* body;

	FormPart() : headers(NULL), body(NULL) {}

	FormPart(pointerVector<char*>* headers, utils::dataContainer* body) : headers(headers), body(body) {}

	~FormPart()
	{
		delete(this->headers);
		delete(this->body);
	}

} FormPart;

typedef struct HttpClientSocket {

    uint64_t socket;
    SOCKADDR address;
	RequestBody* requestBody;
	uint32_t contentLength;
	utils::dataContainer* dataIn;
    int totalReceived;
    utils::dataContainer* dataOut;
    int totalSent;

	HttpClientSocket()
	{
		socket = 0;
		memset(&address, 0, sizeof(address));
		requestBody = NULL;
		contentLength = 0;
		dataIn = NULL;
		totalReceived = 0;
		dataOut = NULL;
		totalSent = 0;
	}

	~HttpClientSocket()
	{
		delete(this->requestBody);
		delete(this->dataIn);
		delete(this->dataOut);
	}

} HttpClientSocket;

typedef utils::dataContainer* (*HttpOnGetCallback)(const char* path, const char* query);
typedef utils::dataContainer* (*HttpOnPostCallback)(const char* path, const char* query, pointerVector<FormPart*>* formParts);
typedef void (*HttpOnResponseSentCallback)();

class httpServer 
{
public:
	static utils::dataContainer* generateResponse(const int errorCode, const char* message);
	static utils::dataContainer* redirect();
    static void init();
    static void close();
	static void registerOnGetCallback(HttpOnGetCallback callback);
	static void removeOnGetCallback();
	static void registerOnPostCallback(HttpOnPostCallback callback);
	static void removeOnPostCallback();
	static void registerOnResponseSentCallback(HttpOnResponseSentCallback callback);
	static void removeOnResponseSentCallback();
    static char* getMimeType(const char* extension);
private:
	static bool WINAPI listenThread(LPVOID lParam);
	static RequestBody* getRequestBody(utils::dataContainer* input, uint32_t& index);
    static void refresh();
	static char* getLine(utils::dataContainer* input, uint32_t& index);
	static pointerVector<char*>* getHeaders(utils::dataContainer* input, uint32_t& index);
	static pointerVector<char*>* getHeaderValueParts(pointerVector<char*>* headers, const char* header);
	static utils::dataContainer* processGet(RequestBody* requestBody);
	static utils::dataContainer* processPost(RequestBody* requestBody, utils::dataContainer* input);
};
