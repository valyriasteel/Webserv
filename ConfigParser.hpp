#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Server.hpp"
# include <vector>

class ConfigParser
{
	private:
		std::vector<Server> _servers;
		bool _inServer;
		bool _inLocation;
		bool _inError;
		Server *_currentServer;
	public:
		ConfigParser();
		~ConfigParser();
		void checkArgument(std::string &);
		std::vector<Server> configFileParser(std::string &);
		bool serverBlockStart();
		bool isErrorPage();
		bool isLocation();
		void handleDirective(std::string &);
		void validateServers();
};

#endif