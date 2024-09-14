#include "Server.hpp"
#include <sstream>
#include <iomanip>

Server::Server()
{
	_serverName = "";
	_serverRoot = "";
	_serverIndex = "";
	_ip = "";
	_port = -1;
	_clientMaxBodySize = -1;
	_locations = std::vector<Location>();
	_errorPages = std::map<int, std::string>();
	_currentLocation = NULL;
}

void Server::serverStart()
{
	if(!initSocket())
		throw std::runtime_error("Error: Socket initialization failed");
	if(!bindSocket())
		throw std::runtime_error("Error: Socket binding failed");
	if(!listenSocket())
		throw std::runtime_error("Error: Socket listening failed");
}

Server::~Server()
{
	_locations.clear();
}

bool Server::operator==(const Server &server)
{
	bool name = _serverName == server._serverName;
	bool port = _port == server._port;
	bool ip = _ip == server._ip;

	return name || (port && ip);
}

bool Server::isServerValid()
{
	if (_serverName.empty() || _port == -1 || _ip.empty() || _serverRoot.empty() || _serverIndex.empty() || _locations.empty() || _clientMaxBodySize == -1 || _errorPages.empty())
		return false;
	return true;
}

int Server::stringToInt(std::string &str)
{
    std::stringstream ss(str);
    int num;
    ss >> num;
    if (ss.fail())
        throw std::runtime_error("Error: Invalid number");
    return num;
}

void Server::setErrorPage(std::string &code, std::string &path)
{
	int code_int = stringToInt(code);
	if (code_int >= 100 && code_int < 600 && _errorPages.find(code_int) == _errorPages.end())
		_errorPages[code_int] = path;
	else
		throw std::runtime_error("Error: Invalid error code or error code already set");
}

void Server::locationDirective(std::string &key, std::string &value)
{
	std::string locationSet[4] = {"path", "allow_methods", "index", "autoindex"};
	if (key == locationSet[0])
		_currentLocation->setPath(value);
	else if (key == locationSet[1])
		_currentLocation->setAllowMethods(value);
	else if (key == locationSet[2])
		_currentLocation->setIndex(value);
	else if (key == locationSet[3])
		_currentLocation->setAutoindex(value == "on");
	else
		throw std::runtime_error("Error: Invalid location directive");
}

void Server::addLocation()
{
	_locations.push_back(Location());
	_currentLocation = &_locations.back();
}

void Server::setName(std::string &name)
{
	if (_serverName.empty() && !name.empty())
		_serverName = name;
	else
		throw std::runtime_error("Error: Server name empty or already set");
}

void Server::setRoot(std::string &root)
{
	if (_serverRoot.empty() && !root.empty())
		_serverRoot = root;
	else
		throw std::runtime_error("Error: Server root empty or already set");
}

void Server::setIndex(std::string &index)
{
	if (_serverIndex.empty() && !index.empty())
		_serverIndex = index;
	else
		throw std::runtime_error("Error: Server index empty or already set");
}

void Server::setIp(std::string &ip)
{
	if (_ip.empty() && !ip.empty())
		_ip = ip;
	else
		throw std::runtime_error("Error: Server ip empty or already set");
}

void Server::setPort(int port)
{
	if (port > 0 && port < 65535 && _port == -1)
		_port = port;
	else
		throw std::runtime_error("Error: Server port empty or already set");
}

void Server::setClientMaxBodySize(int size)
{
	if (_clientMaxBodySize == -1 && size >= 0)
		_clientMaxBodySize = size;
	else
		throw std::runtime_error("Error: Server client_max_body_size empty or already set");
}

void Server::serverDirective(std::string &key, std::string &value)
{
	std::string serverSet[6] = {"server_name", "root", "index", "host", "port", "client_max_body_size"};
	if (key == serverSet[0])
		setName(value);
	else if (key == serverSet[1])
		setRoot(value);
	else if (key == serverSet[2])
		setIndex(value);
	else if (key == serverSet[3])
		setIp(value);
	else if (key == serverSet[4])
		setPort(stringToInt(value));
	else if (key == serverSet[5])
		setClientMaxBodySize(stringToInt(value));
	else
		throw std::runtime_error("Error: Invalid server directive");
}

std::string& Server::getServerName()
{
	return _serverName;
}

std::string& Server::getServerRoot()
{
	return _serverRoot;
}

std::string& Server::getServerIndex()
{
	return _serverIndex;
}

std::string& Server::getIp()
{
	return _ip;
}

int& Server::getPort()
{
	return _port;
}

int& Server::getClientMaxBodySize()
{
	return _clientMaxBodySize;
}

std::vector<Location>& Server::getLocations()
{
	return _locations;
}

std::map<int, std::string>& Server::getErrorPages()
{
	return _errorPages;
}

std::string& Server::getErrorPath(int code)
{
	if (_errorPages.find(code) != _errorPages.end())
		return _errorPages[code];
	throw std::runtime_error("Error: Error code not found");
}

Location* Server::getCurrentLocation()
{
	return _currentLocation;
}

void Server::serverInfo(std::vector<Server> &server)
{
	int totalWidth = 80;
	std::cout << "\033[32m" "┌" "\033[1m" "SERVER" << (server.size() == 1 ? "─" : "S") << " INFO" "\033[0m" "\033[32m" "──────────────────────────────────────────────────────────────┐" "\033[0m" "\n";
	for (std::vector<Server>::iterator it = server.begin(); it != server.end(); it++)
	{
		std::string serverInfo = " Server " "\033[1m" + it->getServerName() +  "\033[0m" " is running on " "\033[1m" + it->getIp() + "\033[0m" " and listening on port " "\033[1m" + std::to_string(it->getPort()) + "\033[0m";
		std::cout << "\033[32m" "│" "\033[0m" << std::left << std::setw(totalWidth + 18) << serverInfo << "\033[32m" "│" "\033[0m" "\n";
	}
	std::cout << "\033[32m" "└──────────────────────────────────────────────────────────────────────────┘" "\033[0m" << std::endl;
}