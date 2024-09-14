#include "ConfigParser.hpp"

void run_server();

int main(int ac, char **av)
{
	try
	{
    	if (ac != 2)
			throw std::invalid_argument("Usage: " + std::string(av[0]) + " \".conf extension file\"");
		
    	const std::string &configFilePath = av[1];

		ConfigParser parser;
		std::vector<Server> servers;
		std::vector<int> _sockets;
		
    	servers = parser.configFileParser(configFilePath);
		Server::serverInfo(servers);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			int socked_fd = it->initSocket();
			_sockets.push_back(socked_fd);
		}

		 run_server();
	}
	catch(const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}

void run_server()
{
	while (true)
	{

	}
}