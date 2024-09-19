#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include <netinet/in.h>
# include "Server.hpp"

class ServerManager
{
	private:
		struct sockaddr_in _server_addr;
		int _socket_fd;
		int _max_fd;
		int _client_socket;
		socklen_t _addr_len;
		fd_set _read_fd;
		fd_set _write_fd;
		fd_set _master_fd;
		std::vector<Server> _servers;
	public:
		ServerManager(std::vector<Server> &);
		~ServerManager();
		void initializeSockets();
    	void run();
    	void acceptNewConnection(int server_socket);
    	void handleClientRequest(int client_socket);
		void handleClientWrite(int client_socket);
		bool isServerSocket(int socket);
		std::string parseMethod(std::string &request);
		std::string parseUri(std::string &request);
		void handleGetRequest(int client_socket, std::string &uri);
		void handlePostRequest(int client_socket, std::string &uri, std::string &request);
		void handleDeleteRequest(int client_socket, std::string &uri);
		void sendResponse(int client_socket, int status_code, const std::string &content);
		std::string findFilePath(const std::string &uri);
		void sendAutoIndex(int client_socket, const std::string &uri);
		bool isDirectory(const std::string &path);
		bool isAutoIndexEnabled(const std::string &path);
};

#endif