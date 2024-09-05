#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include "Location.hpp"
# include <netinet/in.h>

class Server
{
	private:
		std::map<std::string, int> _listen;
		std::map<int, std::string> _error_pages;
		int client_max_body_size;
		std::map<std::string, Location> _locations;
		int _port;
		std::string _ip;
	public:
		Server();
		void setListen(const std::map<std::string, int> &);
		void setErrorPage(int code, const std::string &);
		void setClientMaxBodySize(int size);
		void addLocation(const std::string &, const Location &);
		const std::string &getListen() const;
		const std::map<int, std::string> &getErrorPages() const;
		int getClientMaxBodySize() const;
		const std::map<std::string, Location> &getLocations() const;
		int getPort() const;
		std::string getIp() const;
};

#endif