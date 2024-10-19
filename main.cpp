#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include <signal.h>

void sig_handler(int signo)
{
	std::cerr << "SIGPIPE caught: " << signo << std::endl;
}

int main(int ac, char **av)
{
	try
	{
    	if (ac > 2)
			throw std::invalid_argument("Usage: " + std::string(av[0]) + " \".conf extension file\" or ./webserv");
		
		signal(SIGPIPE, SIG_IGN);
    	const std::string &configFilePath = ac == 1 ? "server.conf" : av[1];

		ConfigParser parser;
		std::vector<Server> servers;

    	servers = parser.configFileParser(configFilePath);
		Server::printServerInfo(servers);
		ServerManager manager(servers);
		manager.initializeSockets();
		manager.run();
	}
	catch(const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}