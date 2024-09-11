#include "Location.hpp"
#include "ConfigParser.hpp"
#include <iomanip>

void serverinfo(std::vector<Server> server)
{
	int totalWidth = 80;
	std::cout << "\033[32m" "┌" "\033[1m" "SERVER" << (server.size() == 1 ? "─" : "S") << " INFO" "\033[0m" "\033[32m" "──────────────────────────────────────────────────────────────┐" "\033[0m" "\n";
	for (std::vector<Server>::iterator it = server.begin(); it != server.end(); it++) {
	std::string serverInfo = " Server " "\033[1m" + it->getServerName() +  "\033[0m" " is running on " "\033[1m" + it->getIp() + "\033[0m" " and listening on port " "\033[1m" + std::to_string(it->getPort()) + "\033[0m";
	std::cout << "\033[32m" "│" "\033[0m" << std::left << std::setw(totalWidth + 18) << serverInfo << "\033[32m" "│" "\033[0m" "\n";
	}
	std::cout << "\033[32m" "└──────────────────────────────────────────────────────────────────────────┘" "\033[0m" << std::endl;
}

int main(int ac, char **av)
{
	try
	{
    	if (ac != 2)
			throw std::invalid_argument("Usage: " + std::string(av[0]) + " <.conf extension file>");
		
    	std::string configFilePath = av[1];

		ConfigParser parser;
		std::vector<Server> servers;
		
    	servers = parser.configFileParser(configFilePath);
		serverinfo(servers);
	}
	catch(const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
