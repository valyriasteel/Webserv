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
		void setErrorPage(const std::string &, const std::string &);
		void setName(const std::string &);
		void setRoot(const std::string &);
		void setIndex(const std::string &);
		void setIp(const std::string &);
		void setPort(int);
		void setClientMaxBodySize(int);
		void locationDirective(const std::string &, const std::string &);
		void serverDirective(const std::string &, const std::string &);
		void addLocation();
		static void printServerInfo(const std::vector<Server> &);
		bool isServerValid() const;
		bool operator==(const Server &) const;
		int stringToInt(const std::string &);
		int getPort() const;
		int getClientMaxBodySize() const;
		int getFd() const;
		const std::string &getServerName() const;
		const std::string &getServerRoot() const;
		const std::string &getServerIndex() const;
		const std::string &getIp() const;
		const std::string &getErrorPath(int);
		const std::vector<Location> &getLocations() const;
		const std::map<int, std::string> &getErrorPages() const;
		Location *getCurrentLocation();
};

#endif