#include "Server.hpp"
#include "HttpRequest.hpp"
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
	_server_addr.sin_addr.s_addr = inet_addr(this->getIp().c_str());
	_server_addr.sin_port = htons(this->getPort());
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

bool Server::serverRun()
{
    acceptSocket();
    return true;
}

bool Server::acceptSocket()
{
    FD_ZERO(&_read_fd);
    FD_ZERO(&_write_fd);
    FD_ZERO(&_master_fd);
    FD_SET(_socket_fd, &_master_fd);
    int max_fd = _socket_fd;

    while(true)
    {
        FD_ZERO(&_read_fd); // bu iki satırı bir düşün
        FD_ZERO(&_write_fd);
        _read_fd = _master_fd; // `select()` için `read_fds` kümesini güncelle
        _write_fd = _master_fd; // `select()` için `write_fds` kümesini güncelle
        int activity = select(max_fd + 1, &_read_fd, &_write_fd, NULL, NULL);
        if(activity == -1)
            throw std::runtime_error("Error: Failed to select on socket");
        for(int i = 3; i <= max_fd; i++)
        {
            if(FD_ISSET(i, &_read_fd))
            {
                if(i == _socket_fd)
                {
                    if(!requestNewConnection(max_fd))
                    {
                        std::cout << "Error: Failed to handle new connection" << std::endl;
                        continue;
                    }
                }
                else
                {
                    if(!readRequest(i))
                    {
                        std::cout << "Error: Failed to read request" << std::endl;
                        continue;
                    }
                        FD_SET(i, &_write_fd);//yazma işlemi için dosya tanımlayıcısı işaretledik yani ayarladık.
                }
            }
            if(FD_ISSET(i, &_write_fd))
            {
                std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
                ssize_t bytes_written = write(i, response.c_str(), response.size());
                if (bytes_written == -1)
                {
                    std::cout << "Failed to write to socket" << std::endl;
                }
                close(i); // Bağlantıyı kapat
                FD_CLR(i, &_master_fd); // Dosya tanımlayıcısını sil
            }
        }
    }
    closeSocket();
    return true;
}

bool Server::readRequest(int client_fd)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t byte_read = read(client_fd, buffer, sizeof(buffer));
    if(byte_read <= 0)
    {
        if(byte_read == -1)
            std::cout << "Error: Failed to read from socket" << std::endl;
        else
            std::cout << "Error: Client disconnected" << std::endl;
        close(client_fd);
        FD_CLR(client_fd, &_master_fd);
        _client_requests.erase(client_fd); //daha önceden veri kaldıysa onu sil.
        return false;
    }
    _client_requests[client_fd] += std::string(buffer, byte_read); //gelen veriyi sakla

    if (_client_requests[client_fd].find("\r\n\r\n") != std::string::npos) //// Tam bir HTTP isteği alındıysa işle
    {
        HttpRequest::handleHttpRequest(client_fd, _client_requests[client_fd], *this);
        _client_requests.erase(client_fd);
        return true;
    }
    return false;
}

bool Server::requestNewConnection(int &max_fd)
{
    struct sockaddr_in _client_addr;
    socklen_t _client_addr_len = sizeof(_client_addr);
    int _client_fd = accept(_socket_fd, (struct sockaddr *)&_client_addr, &_client_addr_len);
    if(_client_fd == -1)
    {
        std::cout << "Error: Accepting socket failed" << std::endl;
        return false;
    }
    if(fcntl(_client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
       std::cout << "Error: Failed to set non-blocking mode for client socket" << std::endl;
        close(_client_fd);
        return false;
    }
    FD_SET(_client_fd, &_master_fd);
    if(_client_fd > max_fd)
        max_fd = _client_fd;
    
    return true;
}

bool Server::closeSocket()
{
    if(close(_socket_fd) == -1)
        return false;
    return true;
}