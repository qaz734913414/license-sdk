#ifndef HTTP_CLIENT_H__
#define HTTP_CLIENT_H__

#include <string>
#include <curl/curl.h>

namespace tutils {

class HttpClient
{
public:
	HttpClient(const std::string&);
	HttpClient(const std::string&, const std::string&, const std::string&);
	~HttpClient();

	int doPost(const std::string& body, std::string& resp);
	int doGet(std::string& resp);

private:
	static size_t onWriteData(void* buffer, size_t size, size_t nmemb, void* p);
private:
	CURL *m_pCurl;
};
}
#endif
