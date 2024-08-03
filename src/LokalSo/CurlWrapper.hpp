#pragma once

#ifndef LOKALSO__CURL_WRAPPER_HPP
#define LOKALSO__CURL_WRAPPER_HPP

#include <curl/curl.h>

#include <string>
#include <unordered_map>

namespace LokalSo {
class Header
{
private:
        std::unordered_map<std::string, std::string> headerMap;
public:
        Header() = default;
        Header(std::unordered_map<std::string, std::string> const& headerMap): headerMap(headerMap) {}

        void addHeader(std::string const& key, std::string const& value);
        std::string getValue(std::string const& key) const;
        std::unordered_map<std::string, std::string> const& getHeader() const;
};

class CurlWrapper
{
private:
        CURL *ch;
        curl_slist *sList {nullptr};

        std::string url;
        std::string method{"GET"};
        std::string userAgent;
        std::string requestBody;
        Header requestHeader;

        std::string responseBody;
        Header responseHeader;

        void reset();

public:
        CurlWrapper();
        ~CurlWrapper();
        void appendResponseBody(std::string const& body);
        void addResponseHeader(std::string const& key, std::string const& value);

        void setURL(std::string const& url);
        void addHeader(std::string const& key, std::string const& value);
        void setMethod(std::string const& method);
        void setReqBody(std::string const& body);
        void setUserAgent(std::string const& userAgent);
        void execute();
        Header const& getResHeader() const;
        std::string const& getResBody() const;

        Header const& getReqHeader() const;
        std::string const& getReqBody() const;
};

} // namespace LokalSo
#endif