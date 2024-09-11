#include "Location.hpp"
#include "ConfigParser.hpp"

int main(int ac, char **av)
{
	try
	{
    	if (ac != 2)
			throw std::invalid_argument("Usage: " + std::string(av[0]) + " <.conf extension file>");
		
    	std::string configFilePath = av[1];

		ConfigParser parser;
		std::vector<Server> servers;
		
		parser.checkArgument(configFilePath);
    	servers = parser.configFileParser(configFilePath);
	}
	catch(const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
