#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include "Server.hpp"
# include <sys/select.h>

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
		std::map<int, int> _client_to_server_map;
		std::map<int, std::string> _status_message;
		std::string _method;
		std::string _uri;
		std::string _request;
		Location *_matched_location;
		std::string _request_body;
	public:
		ServerManager(const std::vector<Server> &);
		void initializeSockets();
    	void run();
    	void acceptNewConnection(int);
    	void handleClientRead(int);
		void handleClientRequest(int);
		void clearClientConnections();
		bool isServerSocket(int);
		void initStatusCode();
		void directoryListing(int, const std::string &, const std::string &);
		std::string parseMethod(const std::string &);
		std::string parseUri(const std::string &);
		std::string getContentType(const std::string &);
		std::string findFilePath(const std::string &);
		std::string parseQueryString(const std::string &);
		std::string intToString(int);
		std::string handleCgiRequest(int, const std::string &);
		void handleGetRequest(int);
		void handlePostRequest(int);
		void handleCgiPostRequest(int);
		void handleDeleteRequest(int);
		void sendResponse(int, int, const std::string &, const std::string &);
		void sendAutoIndex(int, const std::string &);
		std::string getHeaderValue(const std::string &);
		bool isDirectory(const std::string &);
		bool isExecutable(const std::string &);
		bool isAutoIndexEnabled(const std::string &);
		bool isCgiScript(const std::string &);
};

#endif