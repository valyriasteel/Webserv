#include "Server.hpp"
#include <fstream>
#include <sstream>

bool isWhitespaceOrControl(char c)
{
    return (c >= 9 && c <= 13);
}

bool isLineEmptyOrWhitespace(const std::string &line)
{
    for (size_t i = 0; i < line.size(); ++i)
        if (!isWhitespaceOrControl(line[i]))
            return false;
    return true;
}

Server::Server()
{
	client_max_body_size = 0;
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

void Server::parseConfigFile(const std::string &filename, std::vector<Server> &servers)
{
    std::ifstream file(filename.c_str());
    std::string line;
    Server currentServer;
    Location currentLocation;
    bool inServerBlock = false;
    bool inLocationBlock = false;
    bool listenSet = false;
    std::string currentLocationName;
    int braceCount = 0;

    while (std::getline(file, line))
    {
        std::istringstream iss(line.c_str());
        std::string key;
        iss >> key;

        if (isLineEmptyOrWhitespace(line))
            continue;

        if (key == "server")
        {
            std::string extra;
            if (iss >> extra)
                throw std::runtime_error("Error: Unexpected extra characters after '" + key + "'. Found: '" + extra + "'");
            if (inServerBlock)
                throw std::runtime_error("Error: New 'server' block started before the previous one was closed.");
            inServerBlock = true;
            inLocationBlock = false;
            listenSet = false;
        }
        else if (key == "{")
        {
            if (!inServerBlock && !inLocationBlock)
                throw std::runtime_error("Error: Unmatched opening brace '{'. No 'server' or 'location' block expected.");
            braceCount++;
        }
        else if (key == "location")
        {
            if (!inServerBlock)
                throw std::runtime_error("Error: 'location' directive found outside of server block");
            if (inLocationBlock)
                throw std::runtime_error("Error: New 'location' block started before the previous one was closed.");
            inLocationBlock = true;
            iss >> currentLocationName;
        }
        else if (key == "}")
        {
            braceCount--;
            if (braceCount < 0)
                throw std::runtime_error("Error: Unmatched closing brace '}'");
            if (inLocationBlock)
            {
                if (currentLocation.getRoot().empty())
                    throw std::runtime_error("Error: 'root' directive is missing in location block " + currentLocationName);
                currentServer.addLocation(currentLocationName, currentLocation);
                currentLocation = Location();
                inLocationBlock = false;
            }
            else if (inServerBlock)
            {
                if (!listenSet)
                    throw std::runtime_error("Error: 'listen' directive is missing in server block");
                servers.push_back(currentServer);
                currentServer = Server();
                inServerBlock = false;
            }
        }
        else
        {
            if (!inServerBlock)
                throw std::runtime_error("Error: Directive found outside of server block");
            if (key == "listen")
            {
                std::string listen;
                iss >> listen;
                if (listen.back() == ';')
                    listen.pop_back();
                currentServer.setListen(listen);
                listenSet = true;
            }
            else if (key == "root")
            {
                std::string root;
                iss >> root;
                if (root.back() == ';')
                    root.pop_back();
                currentLocation.setRoot(root);
            }
            else if (key == "index")
            {
                std::string index;
                iss >> index;
                if (index.back() == ';')
                    index.pop_back();
                currentLocation.setIndex(index);
            }
            else if (key == "cgi_extension")
            {
                std::string ext;
                iss >> ext;
                if (ext.back() == ';')
                    ext.pop_back();
                currentLocation.setCgiExtension(ext);
            }
            else if (key == "cgi_path")
            {
                std::string path;
                iss >> path;
                if (path.back() == ';')
                    path.pop_back();
                currentLocation.setCgiPath(path);
            }
            else if (key == "autoindex")
            {
                std::string value;
                iss >> value;
                if (value.back() == ';')
                    value.pop_back();
                currentLocation.setAutoindex(value == "on");
            }
            else if (key == "error_page")
            {
                int error_code;
                std::string path;
                iss >> error_code >> path;
                if (path.back() == ';')
                    path.pop_back();
                currentServer.setErrorPage(error_code, path);
            }
            else if (key == "client_max_body_size")
            {
                int size;
                if (!(iss >> size) || size <= 0 || size > std::numeric_limits<int>::max())
                    throw std::runtime_error("Error: Invalid 'client_max_body_size' value");
                currentServer.setClientMaxBodySize(size);
            }
            else if (key == "allow_methods")
            {
                std::vector<std::string> methods;
                std::string method;
                while (iss >> method)
                    methods.push_back(method);
                    
                currentLocation.setAllowMethods(methods);
            }
            else
                throw std::runtime_error("Error: Unknown directive '" + key + "'");
        }
    }

    if (braceCount != 0)
        throw std::runtime_error("Error: Unmatched opening brace");
    if (inServerBlock)
    {
        if (inLocationBlock)
        {
            if (currentLocation.getRoot().empty())
                throw std::runtime_error("Error: 'root' directive is missing in location block " + currentLocationName);
            currentServer.addLocation(currentLocationName, currentLocation);
        }
        if (!listenSet)
            throw std::runtime_error("Error: 'listen' directive is missing in server block");
        servers.push_back(currentServer);
    }
}

void Server::checkArg(const std::string &filename)
{
    const std::string confExtension = ".conf";
    size_t extensionPos = filename.rfind(confExtension);

    if (extensionPos == std::string::npos || extensionPos + confExtension.length() != filename.length())
        throw std::invalid_argument("Error: Config file must have a .conf extension.");

    if (extensionPos == 0)
        throw std::length_error("Error: Config file name must have at least one character before the .conf extension.");

    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw std::runtime_error("Error: Cannot open config file " + filename);
    if (file.peek() == std::ifstream::traits_type::eof())
        throw std::runtime_error("Error: Config file " + filename + " is empty");
}