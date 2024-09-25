#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include "Server.hpp"

class ServerManager
{
	private:
		int _max_fd;
		int _client_socket;
		fd_set _read_fd;
		fd_set _write_fd;
		fd_set _master_fd;
		std::vector<Server> _servers;
		Server *_current_server;
	public:
		ServerManager(std::vector<Server> &);
		void initializeSockets();
    	void run();
    	void acceptNewConnection(int);
    	void handleClientRequest(int);
		void handleClientWrite(int);
		void clearClientConnections();
		bool isServerSocket(int);
		std::string parseMethod(std::string &);
		std::string parseUri(std::string &);
		std::string getContentType(const std::string &);
		std::string findFilePath(const std::string &);
		std::string intToString(int);
		void handleGetRequest(int, std::string &);
		void handlePostRequest(int, std::string &, std::string &);
		void handleDeleteRequest(int, std::string &);
		void sendResponse(int, int, const std::string &, const std::string &);
		void sendAutoIndex(int, const std::string &);
		bool isDirectory(const std::string &);
		bool isAutoIndexEnabled(const std::string &);

};

#endif