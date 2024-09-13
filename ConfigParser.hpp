#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Server.hpp"
# include <vector>

class ConfigParser
{
	private:
		bool _inServer;
		bool _inLocation;
		bool _inError;
		std::vector<Server> _servers;
		Server *_currentServer;
	public:
		ConfigParser();
		~ConfigParser();
		void validateServers();
		void handleDirective(std::string &);
		bool isLocation();
		bool isErrorPage();
		bool serverBlockStart();
		void checkArgument(const std::string &);
		std::vector<Server> configFileParser(const std::string &);
};

#endif