#include <stdexcept>
#include <algorithm>
#include <memory>
#include <functional>

#include "CurlWrapper.hpp"
#include "json.hpp"

namespace LokalSo {
using json = nlohmann::json;

// https://www.boost.org/doc/libs/1_59_0/libs/smart_ptr/sp_techniques.html#handle
using defer = std::shared_ptr<void>;

static void ltrim(std::string &s)
{
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

static void rtrim(std::string &s)
{
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

static void trim(std::string &s)
{
        rtrim(s);
        ltrim(s);
}

void Header::addHeader(std::string const& key, std::string const& value)
{
        this->headerMap.emplace(std::make_pair(key, value));
}
std::string Header::getValue(std::string const& key) const
{
        auto it = this->headerMap.find(key);
        if (it == this->headerMap.end())
                return "";
        return it->second;
}
std::unordered_map<std::string, std::string> const& Header::getHeader() const
{
        return this->headerMap;
}

static size_t res_body_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
        CurlWrapper* mem = static_cast<CurlWrapper*>(userp);
        size_t full_size = size * nmemb;

        mem->appendResponseBody(std::string{static_cast<char *>(contents),full_size});
        return full_size;
}

static size_t res_hdr_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
        CurlWrapper* mem = static_cast<CurlWrapper*>(userp);
        size_t full_size = size * nmemb;

        std::string headerStr(static_cast<char *>(contents), full_size);

        auto colon = headerStr.find(':');
        if (colon != std::string::npos) {
                std::string key = headerStr.substr(0, colon);
                std::string value = headerStr.substr(colon + 1);

                trim(key);
                trim(value);
                mem->addResponseHeader(key, value);
        }
        return full_size;
}


CurlWrapper::CurlWrapper()
{
        this->ch = curl_easy_init();

        curl_easy_setopt(this->ch, CURLOPT_WRITEFUNCTION, &res_body_callback);
        curl_easy_setopt(this->ch, CURLOPT_WRITEDATA, this);

        curl_easy_setopt(this->ch, CURLOPT_HEADERFUNCTION, &res_hdr_callback);
        curl_easy_setopt(this->ch, CURLOPT_HEADERDATA, this);

}
CurlWrapper::~CurlWrapper()
{
        if (this->ch != nullptr)
                curl_easy_cleanup(this->ch);

        if (this->sList != nullptr)
                curl_slist_free_all(this->sList);
}

void CurlWrapper::appendResponseBody(std::string const& responseBody)
{
        this->responseBody.append(responseBody);
}
void CurlWrapper::addResponseHeader(std::string const& key, std::string const& value)
{
        this->responseHeader.addHeader(key, value);
}

void CurlWrapper::setURL(std::string const& url)
{
        this->url = url;
}

void CurlWrapper::addHeader(std::string const& key, std::string const& value)
{
        this->requestHeader.addHeader(key, value);
}
void CurlWrapper::setMethod(std::string const& method)
{
        this->method = method;
}

void CurlWrapper::setReqBody(std::string const& requestBody)
{
        this->requestBody = requestBody;
}
void CurlWrapper::setUserAgent(std::string const& userAgent)
{
        this->userAgent = userAgent;
}


void CurlWrapper::execute()
{
        curl_easy_setopt(this->ch, CURLOPT_URL, this->url.c_str());
        curl_easy_setopt(this->ch, CURLOPT_USERAGENT, this->userAgent.c_str());
        curl_easy_setopt(this->ch, CURLOPT_CUSTOMREQUEST, this->method.c_str());
        curl_easy_setopt(this->ch, CURLOPT_POST, 1L);
        curl_easy_setopt(this->ch, CURLOPT_POSTFIELDS, this->requestBody.data());
        curl_easy_setopt(this->ch, CURLOPT_POSTFIELDSIZE, this->requestBody.size());

        for (auto const& headerMap:this->requestHeader.getHeader()) {
                // Content-Type: application/json

                std::string sep{": "};
                std::string headerVal{headerMap.first + sep + headerMap.second};
                this->sList =  curl_slist_append(this->sList, headerVal.c_str());
        }

        defer _(nullptr, std::bind([&]{ this->reset(); }));
        auto res = curl_easy_perform(ch);
        if (res != CURLE_OK) {
                throw std::runtime_error(curl_easy_strerror(res));
        }

        long http_code {0};

        curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
                std::string msg;

                try {
                        json j = json::parse(this->responseBody);
                        msg = j["message"];
                } catch (json::parse_error const& e) {
                        msg = "";
                }

                throw std::runtime_error("HTTP error: " + std::to_string(http_code) + ": " + msg);
        }
}


Header const& CurlWrapper::getReqHeader() const
{
        return this->requestHeader;
}
std::string const& CurlWrapper::getReqBody() const
{
        return this->requestBody;
}

Header const& CurlWrapper::getResHeader() const
{
        return this->responseHeader;
}

std::string const& CurlWrapper::getResBody() const
{
        return this->responseBody;
}

void CurlWrapper::reset()
{
        this->method = "GET";
        this->requestBody.clear();
        this->requestHeader = {};
        this->url.clear();
        // keep user agent inside the class

        if (this->sList != nullptr) {
                curl_slist_free_all(this->sList);
                this->sList = nullptr;
        }

        curl_easy_reset(this->ch);
}

} // namespace LokalSo