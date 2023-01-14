#ifndef __CURL_HTTPS_H__
#define __CURL_HTTPS_H__

#include <curl/curl.h>
#include "dd_json.h"
class curl_http
{
	public:
	static curl_http* GetInstance();
	int iGetPubkey;	
	curl_http();
	~curl_http();
	int new_curl(std::string url, std::string postdata);
	int curl_get_pubkey(std::string url);

    int report_data( std::string postdata);
};

#endif