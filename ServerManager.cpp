#include "ServerManager.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/dir.h>
#include <sys/stat.h>

ServerManager::ServerManager(const std::vector<Server> &servers)
{
	_servers = servers;
	_max_fd = -1;
	_client_socket = -1;
	FD_ZERO(&_master_fd);
	FD_ZERO(&_read_fd);
	FD_ZERO(&_write_fd); 
	_current_server = NULL;
	_matched_location = NULL;
	_matched_location = NULL;
}

void ServerManager::initializeSockets()
{
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		it->setFd(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
		if (it->getFd() == -1)
			throw std::runtime_error("Error: Failed to create socket");
		int opt = 1;
		if (setsockopt(it->getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			close(it->getFd());
			throw std::runtime_error("Error: Failed to set socket options");
		}
		if (fcntl(it->getFd(), F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
		{
			close(it->getFd());
			throw std::runtime_error("Error: Failed to set non-blocking mode");
		}
		struct sockaddr_in server_addr;
		socklen_t addr_len = 0;
		std::memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(it->getPort());
		server_addr.sin_addr.s_addr = inet_addr(it->getIp().c_str());
		addr_len = sizeof(server_addr);
		if (bind(it->getFd(), (struct sockaddr *)&server_addr, addr_len) == -1)
		{
			close(it->getFd());
			throw std::runtime_error("Error: Failed to bind socket");
		}
		if (listen(it->getFd(), SOMAXCONN) == -1)
		{
			close(it->getFd());
			throw std::runtime_error("Error: Failed to listen on socket");
		}
		FD_SET(it->getFd(), &_master_fd);
		if (it->getFd() > _max_fd)
			_max_fd = it->getFd();
	}
	initStatusCode();
}

void ServerManager::run()
{
	while (true)
	{
		_read_fd = _master_fd;
		FD_ZERO(&_write_fd);//
		int activity = select(_max_fd + 1, &_read_fd, &_write_fd, NULL, NULL);
		if (activity == -1)
		{
			clearClientConnections();// !!!!sıkıntı!!!!
			throw std::runtime_error("Error: Failed to select");
		}

		for (int i = 3; i <= _max_fd; i++)
		{
			if(FD_ISSET(i, &_read_fd))
			{
				if(isServerSocket(i))
					acceptNewConnection(i);
				else
					handleClientRead(i);
			}
			if (FD_ISSET(i, &_write_fd))
				handleClientRequest(i);
		}
	}
}

void ServerManager::acceptNewConnection(int server_socket)
{
	_client_socket = accept(server_socket, NULL, NULL);
	if (_client_socket == -1)
		throw std::runtime_error("Error: Failed to accept connection");
	if (fcntl(_client_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
	{
		close(_client_socket);
		throw std::runtime_error("Error: Failed to set non-blocking mode");
	}
	FD_SET(_client_socket, &_master_fd);
	if (_client_socket > _max_fd)
		_max_fd = _client_socket;
	_client_to_server_map[_client_socket] = server_socket;//
}

void ServerManager::handleClientRead(int client_socket)
{
	int server_socket = _client_to_server_map[client_socket];//

	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->getFd() == server_socket)
		{
			_current_server = &(*it);//
			break;
		}
	}
	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));

	int bytes_received = read(client_socket, buffer, sizeof(buffer));
	if (bytes_received <= 0)
	{
		if (bytes_received == -1)
			throw std::runtime_error("Error: Failed to read from socket");
		FD_CLR(client_socket, &_master_fd);
		_client_to_server_map.erase(client_socket);//
		return;
	}
	std::string request(buffer, bytes_received);
	_request = request;
	FD_SET(client_socket, &_write_fd);
}

void ServerManager::handleClientRequest(int client_socket)
{
	_method = parseMethod(_request);
	_uri = parseUri(_request);
	if (_method.empty() || _uri.empty())
	{
		close(client_socket);
		FD_CLR(client_socket, &_master_fd);
		throw std::runtime_error("Error: Failed to parse request");
	}
	const std::vector<Location> &locations = _current_server->getLocations();
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); it++)
	{
		if (_uri == it->getPath() || (_uri.find(it->getPath()) == 0 && _uri[it->getPath().size()] == '/'))
		{
			_matched_location = const_cast<Location *>(&(*it));
			break;
		}
	}
	if (_matched_location == NULL)
	{
		sendResponse(client_socket, 404, "Not Found", _uri);
		return;
	}
	if (!_matched_location->checkMethod(_method))
	{
		sendResponse(client_socket, 405, "Method Not Allowed", _uri);
		return;
	}
	if (_method == "GET")
	{
		std::string index = _matched_location->getIndex();
		std::string autoindex = _matched_location->getAutoindex();
		if (!index.empty() || !autoindex.empty())
			handleGetRequest(client_socket, _uri);
		else
			sendResponse(client_socket, 404, "Not Found", _uri);
	}
	else if (_method == "POST")
		handlePostRequest(client_socket, _uri, _request);
	else if (_method == "DELETE")
		handleDeleteRequest(client_socket, _uri);
	else
		sendResponse(client_socket, 405, "Method Not Allowed", _uri);
	close(client_socket);
	FD_CLR(client_socket, &_master_fd);
	_client_to_server_map.erase(client_socket);
}

bool ServerManager::isServerSocket(int socket)
{
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
		if (it->getFd() == socket)
			return true;
	return false;
}

std::string ServerManager::parseMethod(const std::string &request)
{
	size_t pos = request.find(' ');
	if (pos == std::string::npos)
		return "";
	return request.substr(0, pos);
}

std::string ServerManager::parseUri(const std::string &request)
{
	size_t pos1 = request.find(' ');
	if (pos1 == std::string::npos)
		return "";
	size_t pos2 = request.find(' ', pos1 + 1);
	if (pos2 == std::string::npos)
		return "";
	return request.substr(pos1 + 1, pos2 - pos1 - 1);
}

void ServerManager::handleGetRequest(int client_socket, const std::string &uri)
{
	std::string file_path = findFilePath(uri);
	if (isDirectory(file_path))
		directoryListing(client_socket, uri, file_path);
	else
	{
		std::ifstream file(file_path.c_str());
		if (!file.is_open())
		{
			std::string error_file = _current_server->getErrorPages().find(404)->second;
			error_file = _current_server->getServerRoot() + error_file;
			std::ifstream file(error_file.c_str());
			if (file.is_open())
			{
				std::stringstream buffer;
				buffer << file.rdbuf();
				sendResponse(client_socket, 404, buffer.str(), error_file);
			}
			else
				sendResponse(client_socket, 404, "Not Found", file_path);
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		sendResponse(client_socket, 200, buffer.str(), file_path);
	}
}

void ServerManager::handlePostRequest(int client_socket, const std::string &uri, const std::string &request)
{
	size_t pos = request.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		sendResponse(client_socket, 400, "Bad Request", uri);
		return;
	}
	std::string content = request.substr(pos + 4);
	std::string upload_dir = "/uploads" + uri;
	std::ofstream file(upload_dir.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		sendResponse(client_socket, 500, "Internal Server Error", upload_dir);
		return;
	}
	file << content;
	file.close();
	sendResponse(client_socket, 200, "File uploaded successfully", upload_dir);
}

void ServerManager::handleDeleteRequest(int client_socket, const std::string &uri)
{
	std::string file_path = findFilePath(uri);
	if (remove(file_path.c_str()) == 0)
		sendResponse(client_socket, 200, "File deleted successfully", file_path);
	else
		sendResponse(client_socket, 404, "Not Found", file_path);
}

void ServerManager::sendResponse(int client_socket, int status_code, const std::string &content, const std::string &file_path)
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

		std::string content_type = (status_code == 200 && file_path.find("error_pages") != std::string::npos) 
                               ? "text/html" : getContentType(file_path);

	std::string response = "HTTP/1.1 " + intToString(status_code) + " " + status_message + "\r\n";
    response += "Content-Length: " + intToString(content.size()) + "\r\n";
    response += "Content-Type: " + content_type + "\r\n";
    response += "\r\n";
    response += content;

	ssize_t total_sent = 0;
	ssize_t response_size = response.size();

	while (total_sent < response_size)
	{
		ssize_t bytes_sent = write(client_socket, response.c_str() + total_sent, response_size - total_sent);
		if (bytes_sent == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				FD_SET(client_socket, &_write_fd);
				continue;
			}
			else
			{
				close(client_socket);
				FD_CLR(client_socket, &_master_fd);
				return;
			}
		}
		total_sent += bytes_sent;
	}
}

std::string ServerManager::findFilePath(const std::string &uri)
{
    std::string root = _current_server->getServerRoot();
	if (uri.find("//") != std::string::npos)
		return "";
	if (uri == "/" || checkIndexFileInPath(root, uri))
		return root + uri + "/" + _matched_location->getIndex();
    return root + uri;
}

void ServerManager::sendAutoIndex(int client_socket, const std::string &uri)
{
	std::string response = "<html><head><title>Index of " + uri + "</title></head><body>";
	response += "<h1>Index of " + uri + "</h1>";
	response += "<ul>";
	
	std::string dir_path = findFilePath(uri);
	DIR *dir = opendir(dir_path.c_str());
	if (dir == NULL)
	{
		sendResponse(client_socket, 404, "Not Found", uri);
		return;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
			continue;
		response += "<li><a href=\"" + uri + "/" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></li>";
	}
	response += "</ul></body></html>";
	sendResponse(client_socket, 200, response, uri);
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
	if (_matched_location->getPath() == path)
		return _matched_location->getAutoindex() == "on";
	return false;
}

void ServerManager::clearClientConnections()
{
    for (int i = 3; i <= _max_fd; i++)
    {
        if (FD_ISSET(i, &_read_fd))
        {
            if (_current_server == NULL || i != _current_server->getFd())
            {
                FD_CLR(i, &_master_fd);
                FD_CLR(i, &_read_fd);
                close(i);
				_client_to_server_map.erase(i);
            }
        }
        if (FD_ISSET(i, &_write_fd))
            if (_current_server == NULL || i != _current_server->getFd())
			{
				FD_CLR(i, &_write_fd);
				_client_to_server_map.erase(i);
			}
    }
}

std::string ServerManager::getContentType(const std::string &file_path)
{
    if (file_path.find(".html") != std::string::npos)
        return "text/html";
    else if (file_path.find(".png") != std::string::npos)
        return "image/png";
    else if (file_path.find(".jpg") != std::string::npos || file_path.find(".jpeg") != std::string::npos)
        return "image/jpeg";
    else if (file_path.find(".gif") != std::string::npos)
        return "image/gif";
    else if (file_path.find(".ico") != std::string::npos)
        return "image/x-icon";
    return "application/octet-stream";
}

std::string ServerManager::intToString(int number)
{
    std::stringstream ss;
    ss << number;
	if (ss.fail())
		throw std::runtime_error("Error: Convert int to string failed");
    return ss.str();
}

void ServerManager::initStatusCode()
{
	_status_code.insert(std::make_pair(200, "OK"));
	_status_code.insert(std::make_pair(400, "Bad Request"));
	_status_code.insert(std::make_pair(403, "Forbidden"));
	_status_code.insert(std::make_pair(404, "Not Found"));
	_status_code.insert(std::make_pair(405, "Method Not Allowed"));
	_status_code.insert(std::make_pair(413, "Payload Too Large"));
	_status_code.insert(std::make_pair(500, "Internal Server Error"));
	_status_code.insert(std::make_pair(501, "Not Implemented"));
	_status_code.insert(std::make_pair(502, "Bad Gateway"));
}

void ServerManager::directoryListing(int client_socket, const std::string &uri, const std::string &file_path)
{
	std::string index_file = file_path + _matched_location->getIndex();
	std::ifstream file(index_file.c_str());
	if (!file.is_open() || index_file == file_path)
	{
		if (isAutoIndexEnabled(uri))
			sendAutoIndex(client_socket, uri);
		else
			sendResponse(client_socket, 403, "Forbidden", index_file); //
	}
	else
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		sendResponse(client_socket, 200, buffer.str(), index_file); //
	}
}

bool ServerManager::checkIndexFileInPath(const std::string& path, const std::string& uri)
{
	std::string indexFilePath = path + uri + "/" + _matched_location->getIndex();
    std::ifstream file(indexFilePath.c_str());
    if (file.is_open())
        return true;
    return false;
}