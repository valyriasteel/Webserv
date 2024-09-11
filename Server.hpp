#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include "Location.hpp"

class Server
{
	private:
		Location *_currentLocation;
		std::vector<Location> _locations;
		std::map<int, std::string> _errorPages;
		int _clientMaxBodySize;
		int _port;
		std::string _ip;
		std::string _serverName;
		std::string _serverRoot;
		std::string _serverIndex;
	public:
		Server();
		~Server();
		int stringToInt(std::string &);
		void setErrorPage(std::string &, std::string &);
		void locationDirective(std::string &, std::string &);
		void addLocation();
		void serverDirective(std::string &, std::string &);
		void setName(std::string &);
		void setRoot(std::string &);
		void setIndex(std::string &);
		void setIp(std::string &);
		void setPort(int);
		void setClientMaxBodySize(int);
		bool isServerValid();
		bool operator==(const Server &);
		std::string &getServerName();
		std::string &getServerRoot();
		std::string &getServerIndex();
		std::string &getIp();
		int &getPort();
		int &getClientMaxBodySize();
		std::vector<Location> &getLocations();
		std::map<int, std::string> &getErrorPages();
		std::string &getErrorPath(int);
		Location *getCurrentLocation();
};

#endif