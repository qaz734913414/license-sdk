#include "http_client.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

namespace tutils {
HttpClient::HttpClient(const string& url)
{
	m_pCurl = curl_easy_init();
	if(!m_pCurl)
	{
		cout << "init curl error" << endl;
		exit(-1);
	}
	curl_easy_setopt(m_pCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(m_pCurl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, onWriteData);
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, 3);
}

HttpClient::HttpClient(const string& serial, const string& cpuid, const string& ip)
{
	m_pCurl = curl_easy_init();
	if(!m_pCurl)
	{
		cout << "init curl error" << endl;
		exit(-1);
	}
	string url = "http://" + ip + "/license?serialNum=" + serial + "&id1=" + cpuid;
	cout << url << endl;
	curl_easy_setopt(m_pCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(m_pCurl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, onWriteData);
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, 3);
}

HttpClient::~HttpClient()
{
	curl_easy_cleanup(m_pCurl);
}


size_t HttpClient::onWriteData(void* buffer, size_t size, size_t nmemb, void* p)
{
	string* pStr = (string*)p;
	if(!pStr || !buffer)
	{
		return -1;
	}

	pStr->append((char*)buffer, size * nmemb);
	return nmemb;
}

int HttpClient::doPost(const std::string& body, std::string& resp)
{
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&resp);
	int res = curl_easy_perform(m_pCurl);

	return res;
}

int HttpClient::doGet(std::string& resp)
{
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&resp);
	int res = curl_easy_perform(m_pCurl);

	return res;
}
}
