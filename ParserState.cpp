#include "ParserState.hpp"

ParserState::ParserState(std::vector<Server> &servers) : currentServer(), currentLocation(), inServerBlock(false), inLocationBlock(false), listenSet(false), currentLocationName(), braceCount(0)
{
	servers = std::vector<Server>();
}