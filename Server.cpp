#include "Server.hpp"
#include <sstream>
#include <iomanip>

Server::Server()
{
	_serverName = "";
	_serverRoot = "";
	_ip = "";
	_serverIndex = "";
	_port = -1;
	_clientMaxBodySize = -1;
	_currentLocation = NULL;
}

bool Server::operator==(const Server &server) const
{
	bool name = _serverName == server._serverName;
	bool port = _port == server._port;
	bool ip = _ip == server._ip;

	return name || (port && ip);
}

bool Server::isServerValid() const
{
	if (_serverName.empty() || _port == -1 || _ip.empty() || _serverRoot.empty() || _locations.empty() || _clientMaxBodySize == -1 || _errorPages.empty() || _serverIndex.empty())
		return false;
	return true;
}

int Server::stringToInt(const std::string &str) const
{
    std::stringstream ss(str);
    int num;
    ss >> num;
    if (ss.fail())
        throw std::runtime_error("Error: Convert string to int failed");
	else if (num > std::numeric_limits<int>::max())
		throw std::runtime_error("Error: Int overflow");
    return num;
}

void Server::setErrorPage(const std::string &code, const std::string &path, bool inServer)
{
	if (!inServer)
		throw std::runtime_error("Error: No assignment can be made without a server directive");
	int code_int = stringToInt(code);
	if (code_int >= 100 && code_int < 600 && _errorPages.find(code_int) == _errorPages.end())
		_errorPages[code_int] = path;
	else
		throw std::runtime_error("Error: Invalid error code or duplicate");
}

void Server::locationDirective(const std::string &key, const std::string &value, bool inServer)
{
	if (!inServer)
		throw std::runtime_error("Error: No assignment can be made without a server directive");
	std::string locationSet[4] = {"path", "allow_methods", "index", "autoindex"};
	if (key == locationSet[0])
		_currentLocation->setPath(value);
	else if (key == locationSet[1])
		_currentLocation->setAllowMethods(value);
	else if (key == locationSet[2])
		_currentLocation->setIndex(value);
	else if (key == locationSet[3])
		_currentLocation->setAutoindex(value);
	else
		throw std::runtime_error("Error: Invalid location directive");
}

void Server::addLocation()
{
	_locations.push_back(Location());
	_currentLocation = &_locations.back();
}

void Server::setName(const std::string &name)
{
	if (_serverName.empty() && !name.empty())
		_serverName = name;
	else
		throw std::runtime_error("Error: Server name empty or duplicate");
}

void Server::setRoot(const std::string &root)
{
	if (_serverRoot.empty() && !root.empty())
		_serverRoot = root;
	else
		throw std::runtime_error("Error: Server root empty or duplicate");
}

void Server::setIp(const std::string &ip)
{
	if (_ip.empty() && !ip.empty())
		_ip = ip;
	else
		throw std::runtime_error("Error: Server ip empty or duplicate");
}

void Server::setPort(int port)
{
	if (port > 0 && port < 65535 && _port == -1)
		_port = port;
	else
		throw std::runtime_error("Error: Server port invalid or duplicate");
}

void Server::setClientMaxBodySize(int size)
{
	if (_clientMaxBodySize == -1 && size >= 0)
		_clientMaxBodySize = size;
	else
		throw std::runtime_error("Error: Server client_max_body_size invalid or duplicate");
}

void Server::setServerIndex(const std::string &index)
{
	if (_serverIndex.empty() && !index.empty())
		_serverIndex = index;
	else
		throw std::runtime_error("Error: Server index empty or duplicate");
}

void Server::setFd(int fd)
{
	_fd = fd;
}

void Server::serverDirective(const std::string &key, const std::string &value, bool inServer)
{
	if (!inServer)
		throw std::runtime_error("Error: No assignment can be made without a server directive");
	std::string serverSet[6] = {"server_name", "root", "host", "port", "client_max_body_size", "default_index"};
	if (key == serverSet[0])
		setName(value);
	else if (key == serverSet[1])
		setRoot(value);
	else if (key == serverSet[2])
		setIp(value);
	else if (key == serverSet[3])
		setPort(stringToInt(value));
	else if (key == serverSet[4])
		setClientMaxBodySize(stringToInt(value));
	else if (key == serverSet[5])
		setServerIndex(value);
	else
		throw std::runtime_error("Error: Invalid server directive");
}

const std::string& Server::getServerName() const
{
	return _serverName;
}

const std::string& Server::getServerRoot() const
{
	return _serverRoot;
}

const std::string& Server::getServerIndex() const
{
	return _serverIndex;
}

const std::string& Server::getIp() const
{
	return _ip;
}

int Server::getPort() const
{
	return _port;
}

int Server::getClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

const std::vector<Location>& Server::getLocations() const
{
	return _locations;
}

const std::string& Server::getErrorPath(int code)
{
	if (_errorPages.find(code) != _errorPages.end())
		return _errorPages[code];
	throw std::runtime_error("Error: Error code not found");
}

int Server::getFd() const
{
	return _fd;
}

const std::string Server::intToString(int number)
{
    std::stringstream ss;
    ss << number;
	if (ss.fail())
		throw std::runtime_error("Error: Convert int to string failed");
    return ss.str();
}

void Server::printServerInfo(const std::vector<Server> &server)
{
	int totalWidth = 80;
	std::cout << "\033[32m" "┌" "\033[1m" "SERVER" << (server.size() == 1 ? "─" : "S") << " INFO" "\033[0m" "\033[32m" "──────────────────────────────────────────────────────────────┐" "\033[0m" "\n";
	for (std::vector<Server>::const_iterator it = server.begin(); it != server.end(); it++)
	{
		std::string serverInfo = " Server " "\033[1m" + it->getServerName() +  "\033[0m" " is running on " "\033[1m" + it->getIp() + "\033[0m" " and listening on port " "\033[1m" + Server::intToString(it->getPort()) + "\033[0m";
		std::cout << "\033[32m" "│" "\033[0m" << std::left << std::setw(totalWidth + 18) << serverInfo << "\033[32m" "│" "\033[0m" "\n";
	}
	std::cout << "\033[32m" "└──────────────────────────────────────────────────────────────────────────┘" "\033[0m" << std::endl;
}