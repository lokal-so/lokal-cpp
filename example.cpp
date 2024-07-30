// SPDX-License-Identifier: GPL-2.0-only

#include <LokalSo/Lokal.hpp>
#include <cstdio>

using LokalSo::Lokal;
using LokalSo::Tunnel;

int main(void)
{
	static const char address[] = "127.0.0.1:1234";

	std::string ret;
	Lokal lk;

	Tunnel *tun = lk.newTunnel()
		->setName("Gin Test")
		->setTunnelType(Lokal::TunnelTypeHTTP)
		->setLANAddress("backend.local")
		->setLocalAddress(address)
		->showStartupBanner()
		->ignoreDuplicate();

	ret = tun->create();
	printf("resp: %s\n", ret.c_str());
	return 0;
}
