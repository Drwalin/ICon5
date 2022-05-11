
#include <Context.hpp>
#include <Loop.hpp>
#include <Socket.hpp>

#include <cstring>
#include <string_view>

#include <Debug.hpp>

std::atomic<int> received_counter = 0;

int main(int argc, char** argv) {
	std::shared_ptr<net::Loop> loop = net::Loop::Make();
	std::shared_ptr<net::Context> context = net::Context::Make(loop, [=](
				std::shared_ptr<net::Socket> socket,
				bool isClient, std::string ip) {
				DEBUG(" Connected from: %s\n", ip.c_str());
			},
			[=](std::shared_ptr<net::Socket> socket, int ec, void* edata) {
				DEBUG(" Close connection");
			},
			[=](net::Buffer& buffer, std::shared_ptr<net::Socket> socket) {
				std::string_view v((char*)buffer.Data(), buffer.Size()-1);
				printf(" received: %s\n", v.data());
				fflush(stdout);
				loop->AllcastSend(buffer, socket);
			},
			"cert/user.key", "cert/user.crt", "cert/rootca.crt", NULL);
	
	try {
		context->InternalStartListening("127.0.0.1",
				argc >= 2 ? atoi(argv[1]) : 32367);
		loop->Run();
	} catch (...) {
		printf(" EXCEPTION... FAILED\n\n");
		fflush(stdout);
		exit(1);
	}
	return 0;
}

