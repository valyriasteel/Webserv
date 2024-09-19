#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include "Location.hpp"

class Server
{
	private:
		std::string _ip;
		std::string _serverName;
		std::string _serverRoot;
		std::string _serverIndex;
		int _fd;
		int _clientMaxBodySize;
		int _port;
		std::vector<Location> _locations;
		std::map<int, std::string> _errorPages;
		Location *_currentLocation;
	public:
		Server();
		~Server();
		void setFd(int);
		void setErrorPage(std::string &, std::string &);
		void setName(std::string &);
		void setRoot(std::string &);
		void setIndex(std::string &);
		void setIp(std::string &);
		void setPort(int);
		void setClientMaxBodySize(int);
		void locationDirective(std::string &, std::string &);
		void serverDirective(std::string &, std::string &);
		void addLocation();
		static void printServerInfo(std::vector<Server> &);
		bool isServerValid();
		bool operator==(const Server &);
		int stringToInt(std::string &);
		int getPort();
		int getClientMaxBodySize();
		int getFd();
		std::string &getServerName();
		std::string &getServerRoot();
		std::string &getServerIndex();
		std::string &getIp();
		std::string &getErrorPath(int);
		std::vector<Location> &getLocations();
		std::map<int, std::string> &getErrorPages();
		Location *getCurrentLocation();
};

#endif