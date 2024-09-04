#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include "Location.hpp"

class Server
{
	private:
		std::string _listen;
		std::map<int, std::string> _error_pages;
		int client_max_body_size;
		std::map<std::string, Location> _locations;
	public:
		Server();
		void setListen(const std::string &);
		void setErrorPage(int code, const std::string &);
		void setClientMaxBodySize(int size);
		void addLocation(const std::string &, const Location &);
		const std::string &getListen() const;
		const std::map<int, std::string> &getErrorPages() const;
		int getClientMaxBodySize() const;
		const std::map<std::string, Location> &getLocations() const;
};

#endif