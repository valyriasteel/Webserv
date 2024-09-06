#include "Server.hpp"

Server::Server()
{
	client_max_body_size = 0;
}

Server::Server(int port, std::vector<Server> &r) : _port(port)
{
	if(!initSocket())
		throw std::runtime_error("Error: Socket initialization failed");
	if(!bindSocket())
		throw std::runtime_error("Error: Socket binding failed");
	if(!listenSocket())
		throw std::runtime_error("Error: Socket listening failed");
	std::vector<Server> &request = r;
}

void Server::setListen(const std::string &l)
{
	_listen = l;
}

void Server::setErrorPage(int code, const std::string &path)
{
	_error_pages[code] = path;
}

void Server::setClientMaxBodySize(int size)
{
	client_max_body_size = size;
}

void Server::addLocation(const std::string &name, const Location &location)
{
	_locations[name] = location;
}

const std::string &Server::getListen() const
{
	return _listen;
}

const std::map<int, std::string> &Server::getErrorPages() const
{
	return _error_pages;
}

int Server::getClientMaxBodySize() const
{
	return client_max_body_size;
}

const std::map<std::string, Location> &Server::getLocations() const
{
	return _locations;
}
