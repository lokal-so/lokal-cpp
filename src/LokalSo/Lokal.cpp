// SPDX-License-Identifier: GPL-2.0-only

#include "Lokal.hpp"
#include "CurlWrapper.hpp"

#include <curl/curl.h>
#include <stdexcept>
#include <cstdio>

namespace LokalSo {

using json = nlohmann::json;

constexpr static const char lokal_so_banner[] = 
"    __       _         _             \n"
"   / /  ___ | | ____ _| |  ___  ___  \n"
"  / /  / _ \\| |/ / _  | | / __|/ _ \\ \n"
" / /__| (_) |   < (_| | |_\\__ \\ (_) |\n"
" \\____/\\___/|_|\\_\\__,_|_(_)___/\\___/ \n";

Tunnel::Tunnel(Lokal *lokal):
	lokal_(lokal)
{
}

Tunnel::~Tunnel(void) = default;

std::string Tunnel::create(void)
{
	std::string req_body;
	std::string res_body;
	json j;

	if (address_mdns_.length() == 0 && address_public_.length() == 0)
		throw std::runtime_error("Either LAN or Public address must be set");

	req_body = this->jsonSerialize();
	res_body = lokal_->post("/api/tunnel/start", req_body);

	if (res_body.length() == 0)
		throw std::runtime_error("Failed to create tunnel");

	try {
		j = json::parse(res_body);
	} catch (json::parse_error &e) {
		throw std::runtime_error("Failed to parse response: " + res_body);
	}

	if (startup_banner_)
		__showStartupBanner();

	return res_body;
}

void Tunnel::__showStartupBanner(void)
{
	std::string val;

	printf("%s\n\n", lokal_so_banner);
	printf("Minimum Lokal Client\t%s\n", LOKAL_SERVER_MIN_VERSION);

	if (this->address_public_.length())
		printf("Public Address\t\thttps://%s\n", this->address_public_.c_str());

	val = getLANAddress();
	if (val.length())
		printf("LAN Address\t\thttp://%s\n", val.c_str());

	printf("\n");
}

LOKAL_CONST_DEF_CPP(Lokal::TunnelTypeHTTP);

Lokal::Lokal(std::string base_url):
	base_url_(base_url)
{
	this->curl_pImpl = std::make_unique<CurlImpl>();
}

Lokal::~Lokal(void)
{
}

class Lokal::CurlImpl {
private:
	CurlWrapper curlWrapper;
public:
	CurlImpl() {}
	void setURL(std::string const& url) {
		return this->curlWrapper.setURL(url);
	}
	void addHeader(std::string const& key, std::string const& value) {
		return this->curlWrapper.addHeader(key, value);
	}
	void setMethod(std::string const& method) {
		return this->curlWrapper.setMethod(method);
	}
	void setReqBody(std::string const& body) {
		return this->curlWrapper.setReqBody(body);
	}
	void setUserAgent(std::string const& userAgent) {
		return this->curlWrapper.setUserAgent(userAgent);
	}
	void execute() {
		return this->execute();
	}
	Header const& getResHeader() const {
		return this->curlWrapper.getResHeader();

	}
	std::string const& getResBody() const {
		return this->curlWrapper.getResBody();
	}
};

std::string Lokal::post(std::string path, std::string data)
{
	std::string url {this->base_url_ + path};
	this->curl_pImpl->setURL(url);
	this->curl_pImpl->setMethod("POST");
	this->curl_pImpl->setUserAgent("Lokal Cpp - github.com/lokal-so/lokal-cpp");
	this->curl_pImpl->addHeader("Content-Type", "application/json");
	this->curl_pImpl->setReqBody(data);
	this->curl_pImpl->execute();

	// TODO: check server version
	std::string serverVersion = this->curl_pImpl->getResHeader().getValue("Lokal-Server-Version");
	// TODO: implement SemVer and lessThan method

	return this->curl_pImpl->getResBody();
}


} /* namespace LokalSo */
