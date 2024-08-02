// SPDX-License-Identifier: GPL-2.0-only

#ifndef LOKALSO__LOKAL_HPP
#define LOKALSO__LOKAL_HPP

#ifndef __fjson
#define __fjson
#endif

#include <string>
#include <vector>
#include <memory>

#include "json.hpp"

#ifndef LOKAL_CONST_DEF
#define LOKAL_CONST_DEF_HPP(type, x) constexpr static const type x
#define LOKAL_CONST_DEF_CPP(x) decltype(x) constexpr const x
#endif

#ifndef LOKAL_SERVER_MIN_VERSION
#define LOKAL_SERVER_MIN_VERSION "v0.6.0"
#endif

namespace LokalSo {

class Lokal;

class Tunnel {
public:
	using json = nlohmann::json;

	Tunnel(Lokal *lokal);
	~Tunnel(void);

	inline Tunnel *setLocalAddress(std::string local_address)
	{
		local_address_ = local_address;
		return this;
	}

	inline Tunnel *setTunnelType(std::string tunnel_type)
	{
		tunnel_type_ = tunnel_type;
		return this;
	}

	inline Tunnel *setInpsection(bool inspect)
	{
		inspect_ = inspect;
		return this;
	}

	inline Tunnel *setLANAddress(std::string lan_address)
	{
		lan_address = lan_address.substr(0, lan_address.find(".local"));
		address_mdns_ = lan_address;
		return this;
	}

	inline Tunnel *setPublicAddress(std::string public_address)
	{
		address_public_ = public_address;
		return this;
	}

	inline Tunnel *setName(std::string name)
	{
		name_ = name;
		return this;
	}

	inline Tunnel *ignoreDuplicate(bool ignore_duplicate = true)
	{
		ignore_duplicate_ = ignore_duplicate;
		return this;
	}

	inline Tunnel *showStartupBanner(bool startup_banner = true)
	{
		startup_banner_ = startup_banner;
		return this;
	}

	inline const std::string &getName(void)
	{
		return name_;
	}

	inline const std::string &getTunnelType(void)
	{
		return tunnel_type_;
	}

	inline const std::string &getLocalAddress(void)
	{
		return local_address_;
	}

	inline const std::string &getServerId(void)
	{
		return server_id_;
	}

	inline const std::string &getAddressTunnel(void)
	{
		return address_tunnel_;
	}

	inline int getAddressTunnelPort(void)
	{
		return address_tunnel_port_;
	}

	inline const std::string &getAddressPublic(void)
	{
		return address_public_;
	}

	inline const std::string getLANAddress(void)
	{
		/*
		 * If the last characters are not ".local", then append
		 * ".local" to the string.
		 */
		std::string ret = address_mdns_;

		if (!ret.length())
			return ret;

		if (ret.compare(ret.length() - 6, 6, ".local"))
			ret += ".local";

		return ret;
	}

	inline bool getInspect(void)
	{
		return inspect_;
	}

	inline const std::string &getDescription(void)
	{
		return description_;
	}

	inline bool getIgnoreDuplicate(void)
	{
		return ignore_duplicate_;
	}

	std::string create(void);

private:
	__fjson std::string name_;
	__fjson std::string tunnel_type_;
	__fjson std::string local_address_;
	__fjson std::string server_id_;
	__fjson std::string address_tunnel_;
	__fjson uint16_t address_tunnel_port_;
	__fjson std::string address_public_;
	__fjson std::string address_mdns_;
	__fjson std::string description_;
	__fjson bool inspect_;
	__fjson bool ignore_duplicate_;

	inline std::string jsonSerialize(void)
	{
		json j;

		j["name"] = name_;
		j["tunnel_type"] = tunnel_type_;
		j["local_address"] = local_address_;
		j["server_id"] = server_id_;
		j["address_tunnel"] = address_tunnel_;
		j["address_tunnel_port"] = address_tunnel_port_;
		j["address_public"] = address_public_;
		j["address_mdns"] = address_mdns_;
		j["inspect"] = inspect_;
		j["description"] = description_;
		j["ignore_duplicate"] = ignore_duplicate_;

		return j.dump();
	}

	void __showStartupBanner(void);

	bool startup_banner_ = false;
	Lokal *lokal_;
};

class Lokal {
public:
	LOKAL_CONST_DEF_HPP(char, TunnelTypeHTTP[]) = "HTTP";

	Lokal(std::string base_url = "http://127.0.0.1:6174");
	~Lokal(void);

	inline void setBaseUrl(std::string base_url)
	{
		base_url_ = base_url;
	}

	inline const std::string &getBaseUrl(void)
	{
		return base_url_;
	}

	inline Tunnel *newTunnel(void)
	{
		std::unique_ptr<Tunnel> tunnel = std::make_unique<Tunnel>(this);
		Tunnel *t = tunnel.get();

		tunnels_.push_back(std::move(tunnel));
		return t;
	}

	std::string post(std::string path, std::string data);
	class CurlImpl;


private:
	std::unique_ptr<CurlImpl> curl_pImpl;
	std::string base_url_;
	std::vector<std::unique_ptr<Tunnel>> tunnels_;
};

} /* namespace LokalSo */

#endif /* #ifndef LOKALSO__LOKAL_HPP */
