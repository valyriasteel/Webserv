#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>

void ConfigParser::checkArgument(const std::string &configFile)
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
        throw std::runtime_error("Error: Config file " + configFile + " is empty");
}

bool ConfigParser::isWhitespaceOrControl(char c)
{
    return (c >= 9 && c <= 13);
}

bool ConfigParser::isLineEmptyOrWhitespace(const std::string &line)
{
    for (size_t i = 0; i < line.size(); ++i)
        if (!isWhitespaceOrControl(line[i]))
            return false;
    return true;
}
std::vector<Server> ConfigParser::configFileParser(const std::string &configFile)
{
    std::ifstream file(configFile.c_str());
    std::string line;
    ParserState state(_servers);

    while (std::getline(file, line))
    {
        std::istringstream iss(line.c_str());
        std::string key;
        iss >> key;

        if (isLineEmptyOrWhitespace(line))
            continue;
        if (key == "server")
            handleServerBlock(iss, state, key);
        else if (key == "{")
            handleOpeningBrace(state);
        else if (key == "location")
            handleLocationBlock(state, iss);
        else if (key == "}")
            handleClosingBrace(state, _servers);
        else
            handleDirective(state, key, iss);
    }

    if (state.braceCount != 0)
        throw std::runtime_error("Error: Unmatched opening brace");
    if (state.inServerBlock)
    {
        if (state.inLocationBlock)
        {
            if (state.currentLocation.getRoot().empty())
                throw std::runtime_error("Error: 'root' directive is missing in location block " + state.currentLocationName);
            state.currentServer.addLocation(state.currentLocationName, state.currentLocation);
        }
        if (!state.listenSet)
            throw std::runtime_error("Error: 'listen' directive is missing in server block");
        _servers.push_back(state.currentServer);
    }
    return _servers;
}

void ConfigParser::handleServerBlock(std::istringstream &iss, ParserState &state, std::string &key)
{
    std::string extra;
    if (iss >> extra)
        throw std::runtime_error("Error: Unexpected extra characters after '" + key + "'. Found: '" + extra + "'");
    if (state.inServerBlock)
        throw std::runtime_error("Error: New 'server' block started before the previous one was closed.");
    state.inServerBlock = true;
    state.inLocationBlock = false;
    state.listenSet = false;
}

void ConfigParser::handleOpeningBrace(ParserState &state)
{
    if (!state.inServerBlock && !state.inLocationBlock)
            throw std::runtime_error("Error: Unmatched opening brace '{'. No 'server' or 'location' block expected.");
    state.braceCount++;
}

void ConfigParser::handleLocationBlock(ParserState &state, std::istringstream &iss)
{
    if (!state.inServerBlock)
        throw std::runtime_error("Error: 'location' directive found outside of server block");
    if (state.inLocationBlock)
        throw std::runtime_error("Error: New 'location' block started before the previous one was closed.");
    state.inLocationBlock = true;
    iss >> state.currentLocationName;
}

void ConfigParser::handleClosingBrace(ParserState &state, std::vector<Server> &_servers)
{
    state.braceCount--;
    if (state.braceCount < 0)
        throw std::runtime_error("Error: Unmatched closing brace '}'");
    if (state.inLocationBlock)
    {
        if (state.currentLocation.getRoot().empty())
            throw std::runtime_error("Error: 'root' directive is missing in location block " + state.currentLocationName);
        state.currentServer.addLocation(state.currentLocationName, state.currentLocation);
        state.currentLocation = Location();
        state.inLocationBlock = false;
    }
    else if (state.inServerBlock)
    {
        if (!state.listenSet)
            throw std::runtime_error("Error: 'listen' directive is missing in server block");
        _servers.push_back(state.currentServer);
        state.currentServer = Server();
        state.inServerBlock = false;
    }
}

void ConfigParser::handleDirective(ParserState &state, std::string &key, std::istringstream &iss)
{
    if (!state.inServerBlock)
        throw std::runtime_error("Error: Directive found outside of server block");
    if (key == "listen")
    {
        std::string listen;
        iss >> listen;
        size_t pos = listen.find(':');
        if (pos == std::string::npos)
            throw std::runtime_error("Error: Invalid 'listen' directive");
        std::string host = listen.substr(0, pos);
        int port = std::atoi(listen.substr(pos + 1).c_str());
        std::map<std::string, int> listenInfo;
        listenInfo[host] = port;
        state.currentServer.setListen(listenInfo);
        state.listenSet = true;
    }
    else if (key == "root")
    {
        std::string root;
        iss >> root;
        if (root.back() == ';')
            root.pop_back();
        state.currentLocation.setRoot(root);
    }
    else if (key == "index")
    {
        std::string index;
        iss >> index;
        if (index.back() == ';')
            index.pop_back();
        state.currentLocation.setIndex(index);
    }
    else if (key == "cgi_extension")
    {
        std::string ext;
        iss >> ext;
        if (ext.back() == ';')
            ext.pop_back();
        state.currentLocation.setCgiExtension(ext);
    }
    else if (key == "cgi_path")
    {
        std::string path;
        iss >> path;
        if (path.back() == ';')
            path.pop_back();
        state.currentLocation.setCgiPath(path);
    }
    else if (key == "autoindex")
    {
        std::string value;
        iss >> value;
        if (value.back() == ';')
            value.pop_back();
        state.currentLocation.setAutoindex(value == "on");
    }
    else if (key == "error_page")
    {
        int error_code;
        std::string path;
        iss >> error_code >> path;
        if (path.back() == ';')
            path.pop_back();
        state.currentServer.setErrorPage(error_code, path);
    }
    else if (key == "client_max_body_size")
    {
        int size;
        if (!(iss >> size) || size <= 0 || size > std::numeric_limits<int>::max())
            throw std::runtime_error("Error: Invalid 'client_max_body_size' value");
        state.currentServer.setClientMaxBodySize(size);
    }
    else if (key == "allow_methods")
    {
        std::vector<std::string> methods;
        std::string method;
        while (iss >> method)
            methods.push_back(method);
        state.currentLocation.setAllowMethods(methods);
    }
    else
        throw std::runtime_error("Error: Unknown directive '" + key + "'");
}