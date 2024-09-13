#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <netinet/in.h>
# include "Location.hpp"

class Server
{
	private:
		std::string _ip;
		std::string _serverName;
		std::string _serverRoot;
		std::string _serverIndex;
		int _clientMaxBodySize;
		int _port;
		std::vector<Location> _locations;
		std::map<int, std::string> _errorPages;
		struct sockaddr_in _server_addr;
		Location *_currentLocation;

		std::map<int, std::string> _client_requests; //her istemci için gelen isteği saklamak için
		int _socket_fd;
		fd_set _read_fd;
		fd_set _write_fd;
		fd_set _master_fd;
	public:
		Server();
		~Server();
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
		static void serverInfo(std::vector<Server> &);
		bool isServerValid();
		bool operator==(const Server &);
		int stringToInt(std::string &);
		int &getPort();
		int &getClientMaxBodySize();
		std::string &getServerName();
		std::string &getServerRoot();
		std::string &getServerIndex();
		std::string &getIp();
		std::string &getErrorPath(int);
		std::vector<Location> &getLocations();
		std::map<int, std::string> &getErrorPages();
		Location *getCurrentLocation();	

		bool initSocket();
		bool bindSocket();
		bool listenSocket();
		bool acceptSocket();
		bool serverRun();
		bool readRequest(int client_fd);
		bool requestNewConnection(int &);
		bool closeSocket();
		Server(int port);
};

#endif