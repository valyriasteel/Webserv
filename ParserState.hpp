#ifndef PARSERSTATE_HPP
# define PARSERSTATE_HPP

# include "Server.hpp"
# include "Location.hpp"

class ParserState
{
	public:
		Server currentServer;
		Location currentLocation;
		bool inServerBlock;
		bool inLocationBlock;
		bool listenSet;
		std::string currentLocationName;
		int braceCount;
		ParserState(std::vector<Server> &servers);
};

#endif