#include "Server.hpp"
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

bool Server::initSocket()
{
	_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(_socket_fd == -1)
		return false;
	int opt = 1;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cout << "Error: Failed to set socket options" << std::endl;
		return false;
	}
	if(fcntl(_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		return false;
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htonl(_port);
	return true;
}

bool Server::bindSocket()
{
    if(bind(_socket_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1)
        return false;
    return true;
}

bool Server::listenSocket()
{
    if(listen(_socket_fd, 5) == -1)
        return false;
    return true;
}

bool Server::acceptSocket()
{
    fd_set write_fd, read_fd, master_fd;
    FD_ZERO(&read_fd);
    FD_ZERO(&write_fd);
    FD_ZERO(&master_fd);
    FD_SET(_socket_fd, &master_fd);
    int max_fd = _socket_fd;

    while(true)
    {
        read_fd = master_fd;
        FD_ZERO(&write_fd);
        int activity = select(max_fd + 1, &read_fd, &write_fd, NULL, NULL);
        if(activity == -1)
            throw std::runtime_error("Error: Selecting socket failed");
        for(int i = 3; i <= max_fd; i++)
        {
            if(FD_ISSET(i, &read_fd))
            {
                struct sockaddr_in _client_addr;
                socklen_t _client_addr_len = sizeof(_client_addr);
                if(i == _socket_fd)
                {
                    int _client_fd = accept(_socket_fd, (struct sockaddr *)&_client_addr, &_client_addr_len);
                    if(_client_fd == -1)
                    {
                        throw std::runtime_error("Error: Accepting socket failed");
                        continue;
                    }
                    FD_SET(_client_fd, &master_fd);
                    if(_client_fd > max_fd)
                        max_fd = _client_fd;
                }
                else
                {
                    readRequest(i, write_fd, master_fd);
                }
            }
        }
        
    }
}

void Server::readRequest(int client_fd, fd_set& write_fds, fd_set& master_fds)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    size_t byte_read = read(client_fd, buffer, sizeof(buffer));
    if(byte_read == -1)
    {
        throw std::runtime_error("Reading request failed");
        close(client_fd);
        FD_CLR(client_fd, &master_fds);
    }
    else if(byte_read == 0)
    {
        close(client_fd);
        FD_CLR(client_fd, &master_fds);
    }
    else
    {
        std::string request;
        
        

    }
}