#include "ConfigParser.hpp"
#include <fstream>
#include <algorithm>

ConfigParser::ConfigParser()
{
    _inServer = false;
    _inLocation = false;
    _inError = false;
    _currentServer = NULL;
}

void ConfigParser::checkArgument(const std::string &configFile) const
{
    const std::string &confExtension = ".conf";
    size_t extensionPos = configFile.rfind(confExtension);

    if (extensionPos == std::string::npos || extensionPos + confExtension.length() != configFile.length())
        throw std::invalid_argument("Error: Config file must have a .conf extension.");

    if (extensionPos == 0)
        throw std::length_error("Error: Config file name must have at least one character before the .conf extension.");

    std::ifstream file(configFile.c_str());
    if (!file.is_open())
        throw std::runtime_error("Error: Cannot open config file " + configFile);
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        file.close();
        throw std::runtime_error("Error: Config file " + configFile + " is empty");
    }
    file.close();
}

std::vector<Server> ConfigParser::configFileParser(const std::string &configFile)
{
    checkArgument(configFile);
    std::ifstream file(configFile.c_str());
    std::string line;

    while (std::getline(file, line))
    {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line[0] == '#')
            continue;
        if (line == "server:")
            if (serverBlockStart())
                continue;
        if (line == "error_page:")
            if (isErrorPage())
                continue;
        if (line == "location:")
            if (isLocation())
                continue;
        handleDirective(line);
    }
    if (_servers.empty())
        throw std::runtime_error("Error: No server blocks found in the config file");
    validateServers();
    file.close();
    return _servers;
}

bool ConfigParser::serverBlockStart()
{
    _inLocation = false;
    _inError = false;
    _inServer = true;
    _servers.push_back(Server());
    _currentServer = &_servers.back();
    return true;
}

bool ConfigParser::isErrorPage()
{
    if (!_inServer || _inLocation || _inError)
        throw std::runtime_error("Error: 'error_page' directive found outside of server block");
    _inError = true;
    return true;
}

bool ConfigParser::isLocation()
{
    if (_inServer)
    {
        _inError = false;
        _inLocation = true;
        _currentServer->addLocation();
    }
    else
        throw std::runtime_error("Error: 'location' directive found outside of server block");
    return true;
}

void ConfigParser::handleDirective(std::string &line)
{
    std::string key;
    std::string value;
    size_t pos;

    line = line.substr(0, line.find('#'));
    pos = line.find(':');
    if (pos == std::string::npos)
        throw std::runtime_error("Error: Missing ':' in directive");
    key = line.substr(0, pos);
    value = line.substr(pos + 1);
    if (_inError)
        _currentServer->setErrorPage(key, value, _inServer);
    else if (_inLocation)
        _currentServer->locationDirective(key, value, _inServer);
    else
        _currentServer->serverDirective(key, value, _inServer);
}

void ConfigParser::validateServers()
{
    for (std::vector<Server>::iterator it = _servers.begin(); it != (_servers.end() -1); it++)
        for (std::vector<Server>::iterator it2 = it + 1; it2 != _servers.end(); it2++)
            if (*it == *it2)
                throw std::runtime_error("Error: Duplicate server block found");
    for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
        if (it->isServerValid() == false)
            throw std::runtime_error("Error: Invalid server block found");
}