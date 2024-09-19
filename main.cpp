#include "ConfigParser.hpp"
#include "ServerManager.hpp"

int main(int ac, char **av)
{
	try
	{
    	if (ac != 2)
			throw std::invalid_argument("Usage: " + std::string(av[0]) + " \".conf extension file\"");
		
    	const std::string &configFilePath = av[1];

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