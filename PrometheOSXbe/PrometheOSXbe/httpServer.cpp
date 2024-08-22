#include "httpServer.h"
#include "fileSystem.h"
#include "stringUtility.h"
#include "socketUtility.h"
#include "utils.h"

#include <fstream>
#include <istream>
#include <iostream>

namespace {

    SOCKADDR_IN mConnectionAddress;
    uint64_t mListenSocket;
    pointerVector<HttpClientSocket*>* mClientSockets = NULL;
    int mPortIn;
    int m_iXferPort;
    uint64_t passive_socket;
	bool mStopRequested;
	HANDLE mListenThreadHandle;

	HttpOnGetCallback mOnGetCallback;
	HttpOnPostCallback mOnPostCallback;
	HttpOnResponseSentCallback mOnResponseSentCallback;
}

bool WINAPI httpServer::listenThread(LPVOID lParam)
{
	while (mStopRequested == false) 
	{
		refresh();
		Sleep(50);
	}

	if (mListenSocket != INVALID_SOCKET)
	{
		int result = closesocket((SOCKET)mListenSocket);
		if (result != 0)
		{
			utils::debugPrint("Error: Close socket failed: %i\n", WSAGetLastError());
		}
		mListenSocket = INVALID_SOCKET;
	}

	for (unsigned int i = 0; i < mClientSockets->count(); ++i)
	{
		HttpClientSocket* clientSocket = mClientSockets->get(i);

		int result = socketUtility::endBrokerSocket(clientSocket->socket);
		if (result == EXIT_FAILURE)
		{
			utils::debugPrint("Error: Close socket failed: %i\n", WSAGetLastError());
		}
		clientSocket->socket = INVALID_SOCKET;
	}

	return false;
}

void httpServer::registerOnGetCallback(HttpOnGetCallback callback)
{
	mOnGetCallback = callback;
}

void httpServer::removeOnGetCallback()
{
	mOnGetCallback = NULL;
}

void httpServer::registerOnPostCallback(HttpOnPostCallback callback)
{
	mOnPostCallback = callback;
}

void httpServer::removeOnPostCallback()
{
	mOnPostCallback = NULL;
}

void httpServer::registerOnResponseSentCallback(HttpOnResponseSentCallback callback)
{
	mOnResponseSentCallback = callback;
}

void httpServer::removeOnResponseSentCallback()
{
	mOnResponseSentCallback = NULL;
}

utils::dataContainer* httpServer::generateResponse(const int errorCode, const char* message)
{
	utils::debugPrint("Response: %s\n", message);
	utils::dataContainer* dataContainer = new utils::dataContainer();
	dataContainer->data = stringUtility::formatString("HTTP/1.1 %i %s\r\nContent-Length: 0\r\n\r\n", errorCode, message);
	dataContainer->size = strlen(dataContainer->data);
	return dataContainer;
}

utils::dataContainer* httpServer::redirect()
{
	utils::debugPrint("Redirect: index.html\n");
	utils::dataContainer* dataContainer = new utils::dataContainer();
	dataContainer->data = strdup("HTTP/1.1 301 Moved Permanently\r\nLocation: /index.html\r\n\r\n");
	dataContainer->size = strlen(dataContainer->data);
	return dataContainer;
}

void httpServer::init()
{
	mStopRequested = false;

	mListenThreadHandle = NULL;

	mPortIn = 80;
	mClientSockets = new pointerVector<HttpClientSocket*>(true);

	utils::debugPrint("Socket server init on port %i\n", mPortIn);

	memset(&mConnectionAddress, 0, sizeof(SOCKADDR_IN));
	mConnectionAddress.sin_family = AF_INET;
	mConnectionAddress.sin_addr.s_addr = htonl( INADDR_ANY );
	mConnectionAddress.sin_port = htons(mPortIn);

	mListenSocket =  socketUtility::createSocket(mConnectionAddress, true);
	if (mListenSocket == NULL)
	{
		return;
	}

	mListenThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)listenThread, 0, 0, NULL);
	if (mListenThreadHandle) {
		SetThreadPriority(mListenThreadHandle, 2);
	}
}

void httpServer::close()
{
	mStopRequested = true;
	WaitForSingleObject(mListenThreadHandle, INFINITE);
	CloseHandle(mListenThreadHandle);
}

RequestBody* httpServer::getRequestBody(utils::dataContainer* input, uint32_t& index)
{
	char* request = NULL;
	char* requestVerb = NULL;
	char* requestPath = NULL;
	char* requestQuery = NULL;
	pointerVector<char*>* requestHeaders = NULL;
	pointerVector<char*>* requestParts = NULL;
	pointerVector<char*>* queryParts = NULL;
	RequestBody* result = NULL;

	request = getLine(input, index);
	utils::debugPrint("Request: %s\n", request);

	requestParts = stringUtility::split(request, " ", false);
	if (requestParts->count() != 3)
	{
		utils::debugPrint("Error: Unexpected request format");
		goto cleanup;
	}

	requestVerb = strdup(requestParts->get(0));
	requestPath = stringUtility::replace(requestParts->get(1), "/", "\\");
	requestQuery = strdup("");

	queryParts = stringUtility::split(requestPath, "?", false);
	if (queryParts->count() == 2)
	{
		free(requestPath);
		requestPath = strdup(queryParts->get(0));
		free(requestQuery);
		requestQuery = strdup(queryParts->get(1));
	}

	utils::debugPrint("Verb: %s\n", requestVerb);
	utils::debugPrint("Path: %s\n", requestPath);
	utils::debugPrint("Query: %s\n", requestQuery);

	result = new RequestBody();
	result->verb = strdup(requestVerb);
	result->path = strdup(requestPath);
	result->query = strdup(requestQuery);
	result->headers = getHeaders(input, index);

cleanup:

	free(request);
	free(requestVerb);
	free(requestPath);
	free(requestQuery);
	delete(requestHeaders);
	delete(requestParts);
	delete(queryParts);
	return result;
}

void httpServer::refresh()
{
	const int result = socketUtility::getReadStatus(mListenSocket);
	if (result == SOCKET_ERROR)
	{
		utils::debugPrint("Error: Socket status failed: %i\n", WSAGetLastError());
		return;
	}

	//utils::debugPrint("read status = %i\n", result);

	if (result == 1)
	{
		int client_size = sizeof(sockaddr_in);

		HttpClientSocket* clientSocket = new HttpClientSocket();

		clientSocket->totalSent = 0;
		clientSocket->totalReceived = 0;
		clientSocket->socket = accept((SOCKET)mListenSocket, (struct sockaddr*)&clientSocket->address, &client_size);
		
		if (clientSocket->socket == INVALID_SOCKET)
		{
			utils::debugPrint("Error: Socket accept failed: %i\n", WSAGetLastError());
			return;
		}
		mClientSockets->add(clientSocket);
		utils::debugPrint("Connected client: %i\n", (int)mClientSockets->count());
	}
	
	// Read incoming socket data

	for (unsigned int i = 0; i < mClientSockets->count(); ++i)
	{
		HttpClientSocket* current_client = mClientSockets->get(i);

		const int status = socketUtility::getReadStatus(current_client->socket);
		if (status == SOCKET_ERROR)
		{
			const int result = socketUtility::endBrokerSocket(current_client->socket);
			if (result == EXIT_FAILURE)
			{
				utils::debugPrint("Error: Socket end failed: %i\n", WSAGetLastError());
			}
			current_client->socket = INVALID_SOCKET;
			continue;
		}

		if (status == 1)
		{
			const int available_data_size = socketUtility::getAvailableDataSize(current_client->socket);
			if (available_data_size > 0)
			{
				if (current_client->requestBody == NULL)
				{
					utils::debugPrint("Body size = %lu\n", available_data_size);

					utils::dataContainer* dataContainer = new utils::dataContainer(available_data_size);

					const int bytes_received = socketUtility::receiveSocketData(current_client->socket, dataContainer->data, available_data_size);
					if (bytes_received == -1)
					{
						continue;
					}

					uint32_t index = 0;
					current_client->requestBody = getRequestBody(dataContainer, index);

					utils::debugPrint("Body size vs index) = %lu, index = %lu, diff = %lu\n", available_data_size, index, available_data_size - index);

					pointerVector<char*>* contentLengthParts = getHeaderValueParts(current_client->requestBody->headers, "Content-Length:");
					if (contentLengthParts->count() == 1)
					{
						current_client->contentLength = stringUtility::toInt(contentLengthParts->get(0));
					}

					if (current_client->contentLength > 0)
					{
						uint32_t bytesToCopy = available_data_size - index;
						current_client->dataIn = new utils::dataContainer(dataContainer->data + index, current_client->contentLength, bytesToCopy);
						current_client->totalReceived = bytesToCopy;
					}

					delete(contentLengthParts);
				}
				else
				{
					uint32_t used = current_client->totalReceived;
					const int bytes_received = socketUtility::receiveSocketData(current_client->socket, (char*)(&current_client->dataIn->data[used]), available_data_size);
					if (bytes_received == -1)
					{
						continue;
					}
					current_client->totalReceived+=bytes_received;
					utils::debugPrint("Total Recieved %lu\n", current_client->totalReceived);
				}
			}
		}
	}

	// Process outgoing socket data

	for (unsigned int i = 0; i < mClientSockets->count(); ++i)
	{
		HttpClientSocket* current_client = mClientSockets->get(i);
		
		if (current_client->socket == INVALID_SOCKET)
		{
			continue;
		}

		if (current_client->requestBody != NULL && current_client->totalReceived == current_client->contentLength) 
		{
			if (stringUtility::equals(current_client->requestBody->verb, "GET", false))
			{
				current_client->dataOut = processGet(current_client->requestBody);
			}
			else if (stringUtility::equals(current_client->requestBody->verb, "POST", false))
			{
				current_client->dataOut = processPost(current_client->requestBody, current_client->dataIn);
			}
			else
			{
				char* errorMessage = stringUtility::formatString("Unsupported Verb %s", current_client->requestBody->verb);
				current_client->dataOut = generateResponse(405, errorMessage);
				free(errorMessage);
			}

			const uint32_t total = current_client->dataOut->size;
			uint32_t remaining = current_client->dataOut->size - current_client->totalSent;
			while (remaining > 0)
			{
				const int bytes_sent = socketUtility::sendSocketData(current_client->socket, (const char*)&current_client->dataOut->data[current_client->totalSent], remaining);
				if (bytes_sent == -1)
				{
					int socketError = WSAGetLastError();
					if (socketError == WSAWOULDBLOCK)
					{
						utils::debugPrint("Socket busy, retrying...");
						Sleep(50);
						continue;
					}

					utils::debugPrint("Error: Client %i send failed: %i\n", (int)i, WSAGetLastError());
					const int result = socketUtility::endBrokerSocket(current_client->socket);
					if (result == EXIT_FAILURE)
					{
						utils::debugPrint("Error: Socket end failed: %i\n", WSAGetLastError());
					}
					current_client->socket = INVALID_SOCKET;
					break;
				}
				current_client->totalSent += bytes_sent;
				remaining = current_client->dataOut->size - current_client->totalSent;
				utils::debugPrint("Send progress %i of %i\n", current_client->totalSent, total);
			}

			const int result = socketUtility::endBrokerSocket(current_client->socket);
			if (result == EXIT_FAILURE)
			{
				utils::debugPrint("Error: Socket end failed: %i\n", WSAGetLastError());
			}
			current_client->socket = INVALID_SOCKET;

			if (mOnResponseSentCallback != NULL)
			{
				mOnResponseSentCallback();
			}
		}
	}

	// Remove finished sockets

	if (mClientSockets->count() > 0)
	{
		for (size_t i = mClientSockets->count(); i-- > 0;)
		{
			HttpClientSocket* current_client = mClientSockets->get(i);
			if (current_client->socket == INVALID_SOCKET)
			{
				mClientSockets->remove(i);
			}
		}
	}
}

char* httpServer::getLine(utils::dataContainer* input, uint32_t& index)
{
	uint32_t length = 0;
	for (uint32_t i = index; i < input->size; i++)
	{
		if (input->data[i] == '\r')
		{
			continue;
		}
		if (input->data[i] == '\n')
		{
			break;
		}
		length++;
	}

	char* result = (char*)malloc(length + 1);

	uint32_t offset = 0;
	for (uint32_t i = index; i < input->size; i++)
	{
		index = i + 1;
		if (input->data[i] == '\r')
		{
			continue;
		}
		if (input->data[i] == '\n')
		{
			break;
		}
		result[offset] = input->data[i];
		offset++;
	}

	result[length] = 0;
	return result;
}

pointerVector<char*>* httpServer::getHeaders(utils::dataContainer* input, uint32_t& index)
{
	pointerVector<char*>* headers = new pointerVector<char*>(false);
	while (index < input->size)
	{
		char* line = getLine(input, index);
		if (strlen(line) == 0)
		{
			break;
		}
		utils::debugPrint("Header: %s\n", line);
		headers->add(line);
	}
	return headers;
}

pointerVector<char*>* httpServer::getHeaderValueParts(pointerVector<char*>* headers, const char* header)
{
	pointerVector<char*>* result = new pointerVector<char*>(false);

	for (uint32_t i = 0; i < headers->count(); i++)
	{
		if (stringUtility::startsWith(headers->get(i), header, true) == false)
		{
			continue;
		}

		char* temp = stringUtility::substr(headers->get(i), strlen(header), -1);
		char* headerValue = stringUtility::trim(temp, ' ');

		pointerVector<char*>* headerValueParts = stringUtility::split(headerValue, ";", true);

		for (uint32_t j = 0; j < headerValueParts->count(); j++) 
		{
			utils::debugPrint("HeaderValuePart for %s %s\n", header, headerValueParts->get(j));
			result->add(strdup(headerValueParts->get(j)));
		}

		free(temp);
		free(headerValue);
		delete(headerValueParts);

		return result;
	}

	return result;
}

utils::dataContainer* httpServer::processGet(RequestBody* requestBody)
{
	utils::dataContainer* getResponse = NULL;
	char* extension = NULL;
	char* mimeType = NULL;
	char* contentLength = NULL;
	char* header = NULL;

	if (mOnGetCallback != NULL)
	{
		getResponse = mOnGetCallback(requestBody->path, requestBody->query);
	}
	else
	{
		getResponse = generateResponse(404, "Not found");
	}

	free(extension);
	free(mimeType);
	free(contentLength);
	free(header);
	return getResponse;
}

utils::dataContainer* httpServer::processPost(RequestBody* requestBody, utils::dataContainer* input)
{
	utils::dataContainer* postResponse = NULL;
	pointerVector<char*>* contentTypeParts = NULL;
	pointerVector<char*>* contentLengthParts = NULL;
	pointerVector<FormPart*>* formParts = NULL;
	char* boundary = NULL;
	char* boundaryDelimiter = NULL;

	contentTypeParts = getHeaderValueParts(requestBody->headers, "Content-Type:");
	if (contentTypeParts->count() != 2)
	{
		postResponse = generateResponse(500, "Unexpected Content-Type format");
		goto cleanup;
	}

	contentLengthParts = getHeaderValueParts(requestBody->headers, "Content-Length:");
	if (contentLengthParts->count() != 1)
	{
		postResponse = generateResponse(500, "Unexpected Content-Length format");
		goto cleanup;
	}

	uint32_t contentLength = stringUtility::toInt(contentLengthParts->get(0));

	if (stringUtility::equals(contentTypeParts->get(0), "multipart/form-data", false) == false)
	{
		postResponse = generateResponse(500, "Invalid Content-Type, expected 'multipart/form-data'");
		goto cleanup;
	}

	boundary = stringUtility::splitAfter(contentTypeParts->get(1), "boundary=");
	if (strlen(boundary) == 0)
	{
		postResponse = generateResponse(500, "Unexpected boundary format");
		goto cleanup;
	}

	boundaryDelimiter = stringUtility::formatString("--%s", boundary);

	formParts = new pointerVector<FormPart*>(true);

	uint32_t index = stringUtility::find(input->data, input->size, 0, boundaryDelimiter, false);
	while (index != StringNotFouund) 
	{
		index += strlen(boundaryDelimiter) + 2;
		pointerVector<char*>* formHeaders = getHeaders(input, index);

		uint32_t startPos = index;
		index = stringUtility::find(input->data, input->size, index, boundaryDelimiter, false);
		if (index == StringNotFouund)
		{
			delete(formHeaders);
			break;
		}

		uint32_t bodyLength = (index - startPos) - 2;
		utils::dataContainer* formData = new utils::dataContainer(input->data + startPos, bodyLength, bodyLength);

		FormPart* formPart = new FormPart(formHeaders, formData);
		formParts->add(formPart);
	}

	for (int i = 0; i < (int)formParts->count(); i++)
	{
		FormPart* part = formParts->get(i);
		utils::debugPrint("Form part length = %lu\n", part->body->size);
		utils::debugPrint("Form part text = %s\n", part->body->data);
	}

	if (mOnPostCallback != NULL)
	{
		postResponse = mOnPostCallback(requestBody->path, requestBody->query, formParts);
		goto cleanup;
	}
	else
	{
		postResponse = generateResponse(404, "Not found");
		goto cleanup;
	}

cleanup:
	delete(contentTypeParts);
	delete(contentLengthParts);
	delete(formParts);
	free(boundary);
	free(boundaryDelimiter);
	return postResponse;
}

char* httpServer::getMimeType(const char* extension) 
{
	char* result = NULL;

	char* lowerExtension = stringUtility::lowerCase(extension);

	if (strcmp(lowerExtension, ".html") == 0) {
		result = strdup("text/html; charset=utf-8");
	} else if (strcmp(lowerExtension, ".gif") == 0) {
		result = strdup("image/gif");
	} else if (strcmp(lowerExtension, ".jpg") == 0) {
		result = strdup("image/jpeg");
	} else if (strcmp(lowerExtension, ".png") == 0) {
		result = strdup("image/png");
	} else if (strcmp(lowerExtension, ".svg") == 0) {
		result = strdup("image/svg+xml");
	} else if (strcmp(lowerExtension, ".ico") == 0) {
		result = strdup("image/vnd.microsoft.icon");
	} else if (strcmp(lowerExtension, ".css") == 0) {
		result = strdup("text/css; charset=utf-8");
	} else if (strcmp(lowerExtension, ".txt") == 0) {
		result = strdup("text/plain; charset=utf-8");
	} else if (strcmp(lowerExtension, ".js") == 0) {
		result = strdup("text/javascript; charset=utf-8");
	} else if (strcmp(lowerExtension, ".json") == 0) {
		result = strdup("application/json; charset=utf-8");
	} else {
		result = strdup("application/octet-stream");
	}

	free(lowerExtension);
	return result;
}