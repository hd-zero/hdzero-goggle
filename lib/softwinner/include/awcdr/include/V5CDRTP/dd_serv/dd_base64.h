#ifndef __DD_BASE64_H__
#define __DD_BASE64_H__

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);
std::string UrlEncode(const std::string& str);

#endif
