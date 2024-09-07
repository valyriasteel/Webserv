#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <netinet/in.h>
# include "Location.hpp"

class Server
{
	private:
		std::string _listen;
		std::map<int, std::string> _error_pages;
		int client_max_body_size;
		std::map<std::string, Location> _locations;
		int _port;
		int _socket_fd;
		struct sockaddr_in _server_addr;
		fd_set _read_fd;
		fd_set _write_fd;
		fd_set _master_fd;
		std::map<int, std::string> _client_requests; //her istemci için gelen isteği saklamak için
	public:
		Server();
		Server(int port);
		void setListen(const std::string &);
		void setErrorPage(int code, const std::string &);
		void setClientMaxBodySize(int size);
		void addLocation(const std::string &, const Location &);
		const std::string &getListen() const;
		const std::map<int, std::string> &getErrorPages() const;
		int getClientMaxBodySize() const;
		const std::map<std::string, Location> &getLocations() const;
		bool initSocket();
		bool bindSocket();
		bool listenSocket();
		bool acceptSocket();
		bool readRequest(int client_fd);
		bool requestNewConnection(int max_fd);
		bool closeSocket();
};

extern std::vector<Server> servers;
#endif