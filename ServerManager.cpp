#include "ServerManager.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/dir.h>
#include <sys/stat.h>
#include <cstring>
#include <sys/wait.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <string>

ServerManager::ServerManager(const std::vector<Server> &servers)
{
	_servers = servers;
	_max_fd = -1;
	_client_socket = -1;
	FD_ZERO(&_master_fd);
	FD_ZERO(&_read_fd);
	FD_ZERO(&_write_fd); 
	_current_server = NULL;//
	_matched_location = NULL;//
	initStatusCode();
}

void ServerManager::initializeSockets()
{
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		it->setFd(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
		if (it->getFd() < 0)
			throw std::runtime_error("Error: Failed to create socket");
		int opt = 1;
		if (setsockopt(it->getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			close(it->getFd());
			throw std::runtime_error("Error: Failed to set socket options");
		}
		if (fcntl(it->getFd(), F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0)
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
		if (server_addr.sin_addr.s_addr == INADDR_NONE)
		{
			close(it->getFd());
			throw std::runtime_error("Error: Invalid IP address");
		}
		addr_len = sizeof(server_addr);
		if (bind(it->getFd(), (struct sockaddr *)&server_addr, addr_len) < 0)
		{
			close(it->getFd());
			if (errno == EADDRINUSE)
				throw std::runtime_error("Error: Port is already in use");
			else
				throw std::runtime_error("Error: Failed to bind socket");
		}
		if (listen(it->getFd(), 255) < 0)
		{
			close(it->getFd());
			throw std::runtime_error("Error: Failed to listen on socket");
		}
		FD_SET(it->getFd(), &_master_fd);
		_max_fd = std::max(_max_fd, it->getFd());
	}
}

void ServerManager::run()
{
	while (true)
	{
		_read_fd = _master_fd;
		FD_ZERO(&_write_fd);//
		if (select(_max_fd + 1, &_read_fd, &_write_fd, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				clearClientConnections();//
				throw std::runtime_error("Error: Failed to select");
			}
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
	if (_client_socket < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;
		else
			throw std::runtime_error("Error: Failed to accept connection");
	}
	if (fcntl(_client_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0)
	{
		close(_client_socket);
		throw std::runtime_error("Error: Failed to set non-blocking mode");
	}
	FD_SET(_client_socket, &_master_fd);
	_max_fd = std::max(_max_fd, _client_socket);
	_client_to_server_map[_client_socket] = server_socket;
}

void ServerManager::handleClientRead(int client_socket)
{
    int server_socket = _client_to_server_map[client_socket];
    // Sunucuyu eşleştirme
    for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        if (it->getFd() == server_socket)
        {
            _current_server = &(*it);
            break;
        }
    }
    
    char buffer[512];
    std::memset(buffer, 0, sizeof(buffer));
    int bytes_received = 0;
    std::string request;
    bool headers_complete = false;
    size_t content_length = 0;
    size_t total_bytes_received = 0;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
    {
        request.append(buffer, bytes_received);
        total_bytes_received += bytes_received;
        
        // İstek başlıklarının tamamlandığını kontrol edin
        if (!headers_complete && request.find("\r\n\r\n") != std::string::npos)
        {
            headers_complete = true;

            // POST isteği için Content-Length bulun ve gövde boyutunu belirleyin
            size_t content_length_pos = request.find("Content-Length: ");
            if (content_length_pos != std::string::npos)
            {
                content_length = strtoul(request.substr(content_length_pos + 16).c_str(), NULL, 10);
                std::cerr << "Content-Length: " << content_length << std::endl;
            }
            // Gövdeyi aldıysanız döngüyü sonlandırın
            if (total_bytes_received >= (request.find("\r\n\r\n") + 4 + content_length))
            {
                std::cerr << "Full request received" << std::endl;
                break;
            }
            else
            {
                std::cerr << "Incomplete request: received " << total_bytes_received << " bytes out of " << content_length << "." << std::endl;
            }
        }
        else if (headers_complete)
        {
            if (total_bytes_received >= (request.find("\r\n\r\n") + 4 + content_length))
                break;
        }
    }

    // Eğer bağlantı kapatıldıysa
    if (bytes_received == 0)
    {
        close(client_socket);
        FD_CLR(client_socket, &_master_fd);
        _client_to_server_map.erase(client_socket);
        return;
    }

    // Gövdeyi ayrıştırın ve POST verilerini CGI'ye aktaracak fonksiyonu çağırın
    size_t body_pos = request.find("\r\n\r\n");
    std::string request_body = request.substr(body_pos + 4, content_length);  // Gövdeyi ayır

    _request = request;  // İstek verisini kaydedin
    _request_body = request_body;  // POST gövdesini kaydedin

    //std::cout << "Request: " << _request << std::endl;
    FD_SET(client_socket, &_write_fd);  // Yazma işlemine hazır hale getiriyoruz
}


void ServerManager::handleClientRequest(int client_socket)
{
	_method = parseMethod(_request);
	_uri = parseUri(_request);
	const std::vector<Location> &locations = _current_server->getLocations();
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); it++)
	{
		if (!it->getPath().empty() && (_uri == it->getPath() || (_uri.find(it->getPath()) == 0 && _uri[it->getPath().size()] == '/')))
		{
			_matched_location = const_cast<Location *>(&(*it));
			break;
		}
	}
	if (_matched_location == NULL)
	{
		sendResponse(client_socket, 404, _status_message[404], _uri);
		close(client_socket);
		FD_CLR(client_socket, &_master_fd);
		_client_to_server_map.erase(client_socket);
		return;
	}
	if (!_matched_location->checkMethod(_method))
	{
		if (_matched_location->getAllowMethods().empty() || !_matched_location->checkMethod(_method))
			sendResponse(client_socket, 405, _status_message[405], _uri);
		else
			sendResponse(client_socket, 404, _status_message[404], _uri);
		close(client_socket);
		FD_CLR(client_socket, &_master_fd);
		_client_to_server_map.erase(client_socket);
		return;
	}
	if (_method == "GET")
	{
		if (!_matched_location->getIndex().empty() || !_matched_location->getAutoindex().empty())
			handleGetRequest(client_socket);
		else
			sendResponse(client_socket, 404, _status_message[404], _uri);
	}
	else if (_method == "POST")
		handlePostRequest(client_socket);
	else if (_method == "DELETE")
		handleDeleteRequest(client_socket);
	else
		sendResponse(client_socket, 405, _status_message[405], _uri);
	close(client_socket);
	FD_CLR(client_socket, &_master_fd);
	FD_CLR(client_socket, &_write_fd);
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
	return request.substr(0, pos);
}

std::string ServerManager::parseUri(const std::string &request)
{
	size_t pos1 = request.find(' ');
	size_t pos2 = request.find(' ', pos1 + 1);
	return request.substr(pos1 + 1, pos2 - pos1 - 1);
}

void ServerManager::handleGetRequest(int client_socket)
{
    std::string file_path = findFilePath(_uri);

    if (isDirectory(file_path))
    {
        directoryListing(client_socket, _uri, file_path);
    }
    else
    {
        if (isCgiScript(file_path))
        {
            std::cout << "CGI script found: " << file_path << std::endl;
            std::string cgi_response = handleCgiRequest(client_socket, file_path);
            std::cout << "----CGI response----\n" << cgi_response << std::endl;
            if (!cgi_response.empty())
            {
                std::cout << "Sending CGI response" << std::endl;
                sendResponse(client_socket, 200, cgi_response, file_path);
            } 
            else
            {
                std::cout << "Failed to execute CGI script" << std::endl;
                sendResponse(client_socket, 500, _status_message[500], file_path);
            }
        }
        else
        {
            std::ifstream file(file_path.c_str());
            if (!file.is_open())
            {
                std::string error_file = _current_server->getErrorPath(404);
                error_file = _current_server->getServerRoot() + error_file;
                std::ifstream error_stream(error_file.c_str());
                if (error_stream.is_open())
                {
                    std::stringstream buffer;
                    buffer << error_stream.rdbuf();
                    sendResponse(client_socket, 404, buffer.str(), error_file);
                }
                else
                {
                    sendResponse(client_socket, 404, _status_message[404], file_path);
                }
                return;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            sendResponse(client_socket, 200, buffer.str(), file_path);
        }
    }
}


void ServerManager::handlePostRequest(int client_socket)
{
    size_t pos = _request.find("\r\n\r\n");
    if (pos == std::string::npos)
    {
        sendResponse(client_socket, 400, _status_message[400], _uri);
        return;
    }
    
    std::string body = _request_body; // The body should already be set by handleClientRead
    std::string content_type = getHeaderValue("Content-Type");

    if (content_type.find("multipart/form-data") != std::string::npos || content_type.find("application/x-www-form-urlencoded") != std::string::npos)
    {
        if (content_type.find("multipart/form-data") != std::string::npos)
        {
            // Extract boundary
            std::string boundary;
            size_t boundary_pos = content_type.find("boundary=");
            if (boundary_pos != std::string::npos)
            {
                boundary = "--" + content_type.substr(boundary_pos + 9);
            }
            else
            {
                sendResponse(client_socket, 400, "Bad Request: Boundary not found", _uri);
                return;
            }

            // Parse multipart content
            size_t part_start = body.find(boundary);
            while (part_start != std::string::npos)
            {
                size_t part_end = body.find(boundary, part_start + boundary.length());
                if (part_end == std::string::npos)
                    part_end = body.length();

                std::string part = body.substr(part_start + boundary.length() + 2, part_end - part_start - boundary.length() - 4);

                size_t header_end = part.find("\r\n\r\n");
                if (header_end == std::string::npos)
                {
                    part_start = body.find(boundary, part_end + boundary.length());
                    continue;  // Skip malformed part
                }

                std::string part_headers = part.substr(0, header_end);
                std::string part_body = part.substr(header_end + 4);

                // Extract filename
                std::string filename;
                size_t filename_pos = part_headers.find("filename=\"");
                if (filename_pos != std::string::npos)
                {
                    filename_pos += 10;
                    size_t filename_end = part_headers.find("\"", filename_pos);
                    filename = part_headers.substr(filename_pos, filename_end - filename_pos);
                }
                else
                {
                    part_start = body.find(boundary, part_end + boundary.length());
                    continue;  // Skip parts that are not files
                }

/*                 // Save the file
                std::string upload_dir = _current_server->getServerRoot() + "/uploads";
                struct stat st;
                if (stat(upload_dir.c_str(), &st) == -1)
                {
                    mkdir(upload_dir.c_str(), 0755);
                }

                std::string file_path = upload_dir + "/" + filename;
                std::ofstream file(file_path.c_str(), std::ios::binary);
                if (!file.is_open())
                {
                    sendResponse(client_socket, 500, _status_message[500], file_path);
                    return;
                }

                // Writing the file content
                file.write(part_body.data(), part_body.size());
                file.close(); */

                // Move to the next part
                part_start = body.find(boundary, part_end + boundary.length());
            }
        }
        // CGI script'ini çalıştır ve sonucunu döndür
        std::string file_path = findFilePath(_uri);
        std::cout << "File path: " << file_path << std::endl;
        std::string cgi_response = handleCgiRequest(client_socket, file_path);
        std::cout << "CGI response: " << cgi_response << std::endl;
        if (!cgi_response.empty())
            sendResponse(client_socket, 200, cgi_response, file_path);
        else
            sendResponse(client_socket, 500, _status_message[500], _uri);
    }
    else
    {
        sendResponse(client_socket, 400, "Bad Request: Unsupported Content-Type", _uri);
    }
}




void ServerManager::handleDeleteRequest(int client_socket)
{
	std::string file_path = findFilePath(_uri);
	if (remove(file_path.c_str()) == 0)
		sendResponse(client_socket, 200, _status_message[200], file_path);
	else
		sendResponse(client_socket, 404, _status_message[404], file_path);
}

void ServerManager::sendResponse(int client_socket, int status_code, const std::string &content, const std::string &file_path)
{
	std::string content_type = getContentType(file_path);

	std::string response = "HTTP/1.1 " + intToString(status_code) + " " + _status_message[status_code] + "\r\n";
    response += "Content-Length: " + intToString(content.size()) + "\r\n";
    response += "Content-Type: " + content_type + "\r\n";
    response += "\r\n";
    response += content;

	ssize_t total_sent = 0;
	ssize_t response_size = response.size();
	int send_test = 0;

	while (total_sent < response_size)
	{
		ssize_t bytes_sent = write(client_socket, response.c_str() + total_sent, 512);

		if (bytes_sent > 0)
        {
            total_sent += bytes_sent;
			send_test = 0;
        }
		else
		{
			send_test++;
			if (send_test == 1000) {
				break;
			}
		}
	}
}

std::string ServerManager::findFilePath(const std::string &uri)
{
    std::string root = _current_server->getServerRoot();
	if (uri.find("//") != std::string::npos)
		return "";
	if (uri == "/" || uri[uri.size() - 1] == '/')
		return root + uri + _matched_location->getIndex();		
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
		sendResponse(client_socket, 404, _status_message[404], uri);
		return;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
			continue;
		if (uri[uri.size() - 1] == '/')
			response += "<li><a href=\"" + uri + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></li>";
		else
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
	if (_matched_location->getPath() == path || path.find(_matched_location->getPath() + "/") == 0)
		return _matched_location->getAutoindex() == "on";
	return false;
}

void ServerManager::clearClientConnections()
{
	for (std::map<int, int>::iterator it = _client_to_server_map.begin(); it != _client_to_server_map.end();)
	{
		if (FD_ISSET(it->first, &_read_fd))
		{
			close(it->first);
			FD_CLR(it->first, &_master_fd);
			it = _client_to_server_map.erase(it);
		}
		else if (FD_ISSET(it->first, &_write_fd))
		{
			FD_CLR(it->first, &_write_fd);
			it = _client_to_server_map.erase(it);
		}
		else
			it++;
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
    else if (file_path.find(".py") != std::string::npos)
        return "text/html";
	else if (file_path.find(".php") != std::string::npos)
		return "text/html";
    else if (file_path.find(".ico") != std::string::npos)
        return "image/x-icon";
	else if (file_path.find("/error_pages") != std::string::npos)
		return "text/html";
	return "text/plain";
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
	_status_message.insert(std::make_pair(200, "OK"));
	_status_message.insert(std::make_pair(400, "Bad Request"));
	_status_message.insert(std::make_pair(403, "Forbidden"));
	_status_message.insert(std::make_pair(404, "Not Found"));
	_status_message.insert(std::make_pair(405, "Method Not Allowed"));
	_status_message.insert(std::make_pair(413, "Payload Too Large"));
	_status_message.insert(std::make_pair(500, "Internal Server Error"));
	_status_message.insert(std::make_pair(501, "Not Implemented"));
	_status_message.insert(std::make_pair(502, "Bad Gateway"));
}

void ServerManager::directoryListing(int client_socket, const std::string &uri, const std::string &file_path)
{
    std::string index_file;
    if (file_path[file_path.length() - 1] == '/')
        index_file = file_path;
    else
	{
        index_file = file_path + "/" + _matched_location->getIndex();
	}
	if (isAutoIndexEnabled(uri))
		sendAutoIndex(client_socket, uri);
	else
	{
		if (_matched_location->getIndex().empty())
		{
			sendResponse(client_socket, 403, _status_message[403], index_file);
			return;
		}
		std::ifstream file(index_file.c_str());
		std::stringstream buffer;
		buffer << file.rdbuf();
		sendResponse(client_socket, 200, buffer.str(), index_file);
	}
}

std::string ServerManager::handleCgiRequest(int client_socket, const std::string &file_path)
{
    // Determine the interpreter based on the file extension
    std::string interpreter;
    if (file_path.find(".php") != std::string::npos)
    {
        interpreter = "/usr/bin/php-cgi";  // Adjust path if necessary
    }
    else if (file_path.find(".py") != std::string::npos)
    {
        interpreter = "/usr/bin/python3";  // Adjust path if necessary
    }
    else
    {
        // Unsupported script type
        sendResponse(client_socket, 500, "Unsupported script type", file_path);
        return "";
    }
    // Check if interpreter is executable
    if (!isExecutable(interpreter))
    {
        sendResponse(client_socket, 500, "Interpreter not executable", interpreter);
        return "";
    }

    pid_t pid;
    int cgi_output[2];
    int cgi_input[2];

    if (pipe(cgi_output) == -1 || pipe(cgi_input) == -1)
    {
        perror("pipe");
        sendResponse(client_socket, 500, _status_message[500], _uri);
        return "";
    }

    if ((pid = fork()) < 0)
    {
        perror("fork");
        sendResponse(client_socket, 500, _status_message[500], _uri);
        return "";
    }

    if (pid == 0)  // Child process
    {
        // Close unnecessary pipe ends
        close(cgi_output[0]); // Close read end of output pipe
        close(cgi_input[1]);  // Close write end of input pipe

        // Redirect stdout to the write end of the output pipe
        dup2(cgi_output[1], STDOUT_FILENO);
        close(cgi_output[1]);

        // Redirect stdin from the read end of the input pipe
        dup2(cgi_input[0], STDIN_FILENO);
        close(cgi_input[0]);

        // Prepare arguments
        char *argv[] = {strdup(interpreter.c_str()), strdup(file_path.c_str()), NULL};

        // Prepare environment variables
        std::stringstream ss;
        ss << _request_body.size();
        std::string script_filename = "SCRIPT_FILENAME=" + file_path;
        std::string request_method = "REQUEST_METHOD=" + _method;
        std::string query_string = "QUERY_STRING=" + parseQueryString(_uri);
        std::string content_length = "CONTENT_LENGTH=" + ss.str();
        std::string content_type_env = "CONTENT_TYPE=" + getHeaderValue("Content-Type");

        char *envp[] = {
            strdup("REDIRECT_STATUS=200"),
            strdup(script_filename.c_str()),
            strdup(request_method.c_str()),
            strdup(query_string.c_str()),
            strdup(content_length.c_str()),
            strdup(content_type_env.c_str()),
            NULL
        };

        // Execute the interpreter
        execve(argv[0], argv, envp);
        perror("execve");  // If execve returns, it's an error
        exit(1);
    }
    else  // Parent process
    {
        // Close unnecessary pipe ends
        close(cgi_output[1]); // Close write end of output pipe
        close(cgi_input[0]);  // Close read end of input pipe

        // Write the request body to the child's stdin
        write(cgi_input[1], _request_body.c_str(), _request_body.size());
        close(cgi_input[1]); // Close write end after writing

        // Read CGI output
        std::string response;
        char buffer[10240];
        ssize_t bytes_read;

        while ((bytes_read = read(cgi_output[0], buffer, sizeof(buffer))) > 0)
        {
            response.append(buffer, bytes_read);
        }

        close(cgi_output[0]);

        int status;
        waitpid(pid, &status, 0);

        return response;
    }
}



void ServerManager::handleCgiPostRequest(int client_socket)
{
    std::string file_path = findFilePath(_uri);  // İstenen dosyanın yolunu bulun
    if (!isExecutable(file_path))  // Dosya yürütülebilir mi kontrol edin
    {
        sendResponse(client_socket, 403, _status_message[403], file_path);  // Eğer yürütülebilir değilse 403 Forbidden döndürün
        return;
    }

    pid_t pid;
    int cgi_output[2];
    int cgi_input[2];

    if (pipe(cgi_output) == -1 || pipe(cgi_input) == -1)
    {
        sendResponse(client_socket, 500, _status_message[500], _uri);
        return;
    }

    if ((pid = fork()) < 0)
    {
        sendResponse(client_socket, 500, _status_message[500], _uri);
        return;
    }

    if (pid == 0)  // Çocuk işlem
    {
        char *argv[] = {strdup(file_path.c_str()), NULL};  // CGI programı için argüman

        // URI'den sorgu parametrelerini ayıkla
        std::string query_string = parseQueryString(_uri);
        std::stringstream ss;
        ss << _request_body.size();

        // Ortam değişkenlerini oluşturuyoruz
        char *envp[] = {
            strdup(("REQUEST_METHOD=" + _method).c_str()),
            strdup(("CONTENT_LENGTH=" + ss.str()).c_str()),  // POST isteğinde content-length önemlidir
            strdup(("QUERY_STRING=" + query_string).c_str()),  // URI'den sorgu parametrelerini alıyoruz
            strdup(("SCRIPT_NAME=" + _uri).c_str()),
            NULL
        };

        close(cgi_output[0]);
        dup2(cgi_output[1], STDOUT_FILENO);  // CGI'nin çıktısını standard output'a yönlendirin

        close(cgi_input[1]);
        dup2(cgi_input[0], STDIN_FILENO);  // CGI'nin girdisini standard input'a yönlendirin

        execve(argv[0], argv, envp);  // CGI programını çalıştırın
        exit(0);
    }
    else  // Ebeveyn işlem
    {
        close(cgi_output[1]);  // Çıkışın yazma ucunu kapatın
        close(cgi_input[0]);   // Girişin okuma ucunu kapatın

        // POST verisini CGI programına gönderin
        write(cgi_input[1], _request_body.c_str(), _request_body.size());
        close(cgi_input[1]);  // Girişin yazma ucunu kapatın

        std::string response;
        char buffer[1024];
        int bytes_read;

        while ((bytes_read = read(cgi_output[0], buffer, sizeof(buffer))) > 0)  // CGI çıktısını okuyun
        {
            response.append(buffer, bytes_read);
        }

        close(cgi_output[0]);  // Çıkışın okuma ucunu kapatın
        sendResponse(client_socket, 200, response, _uri);  // CGI çıktısını istemciye gönderin
    }
}


bool ServerManager::isExecutable(const std::string &file_path)
{
    struct stat info;
    if (stat(file_path.c_str(), &info) != 0)
        return false;
    return (info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0;
}

std::string ServerManager::parseQueryString(const std::string &uri)
{
    size_t query_pos = uri.find('?');
    if (query_pos != std::string::npos)
        return uri.substr(query_pos + 1);  // '?' sonrası kısmı al
    return "";  // Sorgu parametresi yoksa boş döndür
}

bool ServerManager::isCgiScript(const std::string &file_path)
{
    size_t pos = file_path.rfind('.');
    if (pos != std::string::npos)
    {
        std::string ext = file_path.substr(pos);
        return (ext == ".php" || ext == ".py");
    }
    return false;
}

std::string ServerManager::getHeaderValue(const std::string &header_name)
{
    // Construct the header line to search for
    std::string header_line = header_name + ": ";
    
    // Find the position where the header line starts
    size_t pos = _request.find(header_line);
    if (pos != std::string::npos)
    {
        // Calculate the start position of the header value
        size_t start = pos + header_line.length();
        
        // Find the end of the header line (position of the next CRLF)
        size_t end = _request.find("\r\n", start);
        
        // Extract and return the header value
        return _request.substr(start, end - start);
    }
    // Return an empty string if the header is not found
    return "";
}
