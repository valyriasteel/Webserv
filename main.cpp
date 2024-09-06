#include "Location.hpp"
#include "ConfigParser.hpp"

int main(int ac, char **av)
{
	try
	{
    	if (ac != 2)
			throw std::runtime_error("Usage: " + std::string(av[0]) + " <.conf extension file>");
		
    	const std::string &configFilePath = av[1];

		ConfigParser parser;
		parser.checkArgument(configFilePath);
    	std::vector<Server> servers = parser.configFileParser(configFilePath);
		

    	for (size_t i = 0; i < servers.size(); ++i)
		{
    	    std::cout << "Server " << (i + 1) << " listening on " << servers[i].getListen() << std::endl;
    	    const std::map<std::string, Location>& locations = servers[i].getLocations();
    	    for (std::map<std::string, Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    	        std::cout << "  Location " << it->first << " root: " << it->second.getRoot() << std::endl;
    	}
	}
	catch(const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
