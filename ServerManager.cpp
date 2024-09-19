#include "ServerManager.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/dir.h>
#include <sys/stat.h>

ServerManager::ServerManager(std::vector<Server> &servers)
{
	_servers = servers;
	_socket_fd = -1;
	_max_fd = -1;
	_client_socket = -1;
	FD_ZERO(&_master_fd);
	FD_ZERO(&_read_fd);
	FD_ZERO(&_write_fd); 
	_addr_len = 0;
}

ServerManager::~ServerManager()
{
	for (size_t i = 0; i < _servers.size(); i++)
		close(_servers[i].getFd());
}

void ServerManager::initializeSockets()
{
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket_fd == -1)
			throw std::runtime_error("Error: Failed to create socket");
		int opt = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			close(_socket_fd);
			throw std::runtime_error("Error: Failed to set socket options");
		}
		std::memset(&_server_addr, 0, sizeof(_server_addr));
		_server_addr.sin_family = AF_INET;
		_server_addr.sin_port = htons(it->getPort());
		_server_addr.sin_addr.s_addr = inet_addr(it->getIp().c_str());
		if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		{
			close(_socket_fd);
			throw std::runtime_error("Error: Failed to set non-blocking mode");
		}
		_addr_len = sizeof(_server_addr);
		if (bind(_socket_fd, (struct sockaddr *)&_server_addr, _addr_len) == -1)
		{
			close(_socket_fd);
			throw std::runtime_error("Error: Failed to bind socket");
		}
		if (listen(_socket_fd, SOMAXCONN) == -1)
		{
			close(_socket_fd);
			throw std::runtime_error("Error: Failed to listen on socket");
		}
		it->setFd(_socket_fd);
		FD_SET(_socket_fd, &_master_fd);
		if (_socket_fd > _max_fd)
			_max_fd = _socket_fd;
	}
}

void ServerManager::run()
{
	while (true)
	{
		_read_fd = _master_fd;
		_write_fd = _master_fd;
		int activity = select(_max_fd + 1, &_read_fd, &_write_fd, NULL, NULL);
		if (activity == -1 || activity > FD_SETSIZE)
			throw std::runtime_error("Error: Failed to select");
		for (size_t i = 0; i < _servers.size(); i++)
		{
			if (FD_ISSET(_servers[i].getFd(), &_read_fd))
				acceptNewConnection(_servers[i].getFd());
		}
		for (int i = 3; i <= _max_fd; i++)
		{
			if (FD_ISSET(i, &_read_fd) && !isServerSocket(i))
				handleClientRequest(i);
			if (FD_ISSET(i, &_write_fd) && !isServerSocket(i))
				handleClientWrite(i);
		}
	}
}

void ServerManager::acceptNewConnection(int server_socket)
{
	_client_socket = accept(server_socket, (struct sockaddr *)&_server_addr, &_addr_len);
	if (_client_socket == -1)
		throw std::runtime_error("Error: Failed to accept connection");
	if (fcntl(_client_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		close(_client_socket);
		throw std::runtime_error("Error: Failed to set non-blocking mode");
	}
	FD_SET(_client_socket, &_master_fd);
	if (_client_socket > _max_fd)
		_max_fd = _client_socket;
}

void ServerManager::handleClientRequest(int client_socket)
{
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));

	int bytes_received = read(client_socket, buffer, sizeof(buffer));
	if (bytes_received <= 0)
	{
		if (bytes_received == -1)
			std::cout << "Error: Failed to read from socket" << std::endl;
		else
			std::cout << "Error: Client disconnected" << std::endl;
		close(client_socket);
		FD_CLR(client_socket, &_master_fd);
		return;
	}
	std::string request(buffer, bytes_received);
	std::string method = parseMethod(request);
	std::string uri = parseUri(request);
	if (method.empty() || uri.empty())
	{
		std::cout << "Error: Invalid request" << std::endl;
		close(client_socket);
		FD_CLR(client_socket, &_master_fd);
		return;
	}
	if (method == "GET")
		handleGetRequest(client_socket, uri);
	else if (method == "POST")
		handlePostRequest(client_socket, uri, request);
	else if (method == "DELETE")
		handleDeleteRequest(client_socket, uri);
	else
		sendResponse(client_socket, 405, "Method Not Allowed");
}

void ServerManager::handleClientWrite(int client_socket)
{
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello World!";
	int bytes_sent = write(client_socket, response.c_str(), response.size());
	if (bytes_sent == -1)
	{
		close(client_socket);
		FD_CLR(client_socket, &_master_fd);
		throw std::runtime_error("Error: Failed to write to socket");
	}
	close(client_socket);
	FD_CLR(client_socket, &_master_fd);
}

bool ServerManager::isServerSocket(int socket)
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i].getFd() == socket)
			return true;
	}
	return false;
}

std::string ServerManager::parseMethod(std::string &request)
{
	size_t pos = request.find(' ');
	if (pos == std::string::npos)
		return "";
	return request.substr(0, pos);
}

std::string ServerManager::parseUri(std::string &request)
{
	size_t pos1 = request.find(' ');
	if (pos1 == std::string::npos)
		return "";
	size_t pos2 = request.find(' ', pos1 + 1);
	if (pos2 == std::string::npos)
		return "";
	return request.substr(pos1 + 1, pos2 - pos1 - 1);
}

void ServerManager::handleGetRequest(int client_socket, std::string &uri)
{
	std::string file_path = findFilePath(uri);

	if (isDirectory(file_path))
	{
		if (isAutoIndexEnabled(file_path))
			sendAutoIndex(client_socket, file_path);
		else
			sendResponse(client_socket, 403, "Forbidden");
	}
	else
	{
		std::ifstream file(file_path.c_str());
		if (!file.is_open())
		{
			sendResponse(client_socket, 404, "Not Found");
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		sendResponse(client_socket, 200, buffer.str());
	}
}

void ServerManager::handlePostRequest(int client_socket, std::string &uri, std::string &request)
{
	size_t pos = request.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		sendResponse(client_socket, 400, "Bad Request");
		return;
	}
	std::string content = request.substr(pos + 4);
	std::string upload_dir = "/uploads" + uri;
	std::ofstream file(upload_dir.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		sendResponse(client_socket, 500, "Internal Server Error");
		return;
	}
	file << content;
	file.close();
	sendResponse(client_socket, 200, "File uploaded successfully");
}

void ServerManager::handleDeleteRequest(int client_socket, std::string &uri)
{
	std::string file_path = findFilePath(uri);
	if (remove(file_path.c_str()) == 0)
		sendResponse(client_socket, 200, "File deleted successfully");
	else
		sendResponse(client_socket, 404, "Not Found");
}

void ServerManager::sendResponse(int client_socket, int status_code, const std::string &content)
{
	std::string status_message;

	if (status_code == 200)
		status_message = "OK";
	else if (status_code == 400)
		status_message = "Bad Request";
	else if (status_code == 403)
		status_message = "Forbidden";
	else if (status_code == 404)
		status_message = "Not Found";
	else if (status_code == 405)
		status_message = "Method Not Allowed";
	else if (status_code == 500)
		status_message = "Internal Server Error";
	else
		status_message = "Unknown";
	
	std::string response = "HTTP/1.1 " + std::to_string(status_code) + " " + status_message + "\r\n";
    response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
    response += "Content-Type: text/html\r\n";
    response += "\r\n";
    response += content;

	write(client_socket, response.c_str(), response.size());
}

std::string ServerManager::findFilePath(const std::string &uri)
{
    std::string root = "www";
    return root + uri;
}

void ServerManager::sendAutoIndex(int client_socket, const std::string &uri)
{
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	response += "<html><head><title>Index of " + uri + "</title></head><body>";
	response += "<h1>Index of " + uri + "</h1>";
	response += "<ul>";
	DIR *dir = opendir(uri.c_str());
	if (dir == NULL)
	{
		sendResponse(client_socket, 404, "Not Found");
		return;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		response += "<li><a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></li>";
	}
	response += "</ul></body></html>";
	sendResponse(client_socket, 200, response);
}

bool ServerManager::isDirectory(const std::string &path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
	return S_ISDIR(info.st_mode);
}

bool ServerManager::isAutoIndexEnabled(const std::string &path)
{
	std::string index_file = path + "/index.html";
	std::ifstream file(index_file.c_str());
	return file.is_open();
}