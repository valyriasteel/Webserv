#include "HttpRequest.hpp"
#include "ConfigParser.hpp"
#include <sstream>
#include <fstream>
#include <unistd.h>

HttpRequest::HttpRequest()
{}

HttpRequest::~HttpRequest()
{}

void HttpRequest::handleHttpRequest(int client_fd, const std::string &request, Server &server)
{
	HttpRequest httpRequest = HttpRequest::parseHttpRequest(request);
	std::cout << "Method: " << httpRequest._method << std::endl;
	std::cout << "URL: " << httpRequest._url << std::endl;
	std::cout << "Version: " << httpRequest._version << std::endl;

	httpRequest.parseHttpRequestUrl();

	if(isItAllowMethod(httpRequest._locationurl, server, httpRequest._method))
	{
		if (httpRequest._method == "GET")
		{
			std::string indexFile =  controlLocationUrl(httpRequest._locationurl, server);

			std::ifstream file(indexFile, std::ios::binary);
			if (file.is_open())
			{
				std::stringstream buffer;
				buffer << file.rdbuf();
				std::string content = buffer.str();
				std::string contentType = getContentType(indexFile);
			
            	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\nContent-Length: " + std::to_string(content.size()) + "\r\n\r\n" + content;
            	sendResponse(client_fd, response);
			} 
			else
			{
            	std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\nFile Not Found";
            	sendResponse(client_fd, response);
			}
		}

		else if(httpRequest._method == "POST")
			handlePostRequest(client_fd, httpRequest._body);
		else if(httpRequest._method == "DELETE")
			handleDeleteRequest(client_fd, httpRequest._locationurl, httpRequest._locationurlfile);
	}
	else
		handleUnAllowedRequest(client_fd);

	return;
}

HttpRequest HttpRequest::parseHttpRequest(const std::string &request)
{
	HttpRequest httpRequest;
	std::istringstream requestStream(request);
	std::string line;

	if (std::getline(requestStream, line))
	{
		std::istringstream lineStream(line);
		lineStream >> httpRequest._method >> httpRequest._url >> httpRequest._version;
	}

	while (std::getline(requestStream, line) && line != "\r")
	{
		std::string key, value;
		std::istringstream headerStream(line);
		if (std::getline(headerStream, key, ':'))
		{
			if (std::getline(headerStream >> std::ws, value))
				httpRequest._headers[key] = value;
		}
	}

	if (requestStream.rdbuf()->in_avail()) //.rdbuf() akışın veri tamponunu temsil eder. in_avail() nesnesi ise mevcut veri miktarını bayt cinsinden döner.Yani bu akışta okunmayı bekleyen veri miktarını gösterir.  
	{
		std::ostringstream bodyStream;
		bodyStream << requestStream.rdbuf();
		httpRequest._body = bodyStream.str();
	}

	return httpRequest;
}

void HttpRequest::parseHttpRequestUrl()
{
	std::istringstream requestlocation(_url);

	if(std::getline(requestlocation, _locationurl, '/'))
	{
		std::string remaining;
		if(std::getline(requestlocation, remaining))
			_locationurlfile = remaining;
		else
			_locationurlfile = "";
		
		_locationurl = "/" + _locationurl;
	}
}

std::string HttpRequest::controlLocationUrl(const std::string &location, Server &server)
{
    std::vector<Location>& locations = server.getLocations();
    for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); it++)
		if(it->getPath() == location)
			return it->getIndex();	
	throw std::runtime_error("Error: Location was not found.");
}

void HttpRequest::handlePostRequest(int client_fd, const std::string& body)
{
	if(!body.empty())
	{
		std::ofstream out_File("Uploaded_data.txt");
		if(out_File.is_open())
		{
			out_File << body;
			out_File.close();

		std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 14\r\n\r\nPost Received!";
		sendResponse(client_fd, response);
		}
		else
		{
			std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\n\r\nError Saving Data!";
            sendResponse(client_fd, response);
		}
	}
	else
	{
		std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nNo Data Sent!";
        sendResponse(client_fd, response);
	}
}

void HttpRequest::handleDeleteRequest(int client_fd, const std::string& url, const std::string& urlfile)
{
    std::string fileToDelete = url + urlfile;
    if (std::ifstream(fileToDelete)) // Dosya varsa
    {
        if (std::remove(fileToDelete.c_str()) == 0)
        {
            std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 15\r\n\r\nDelete Success!";
            sendResponse(client_fd, response);
        }
        else
        {
            std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 16\r\n\r\nDelete Failed!";
            sendResponse(client_fd, response);
        }
    }
    else // Dosya bulunamadıysa
    {
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\nFile Not Found";
        sendResponse(client_fd, response);
    }
}

void HttpRequest::handleUnAllowedRequest(int client_fd)
{
	std::string response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 23\r\n\r\nMethod Not Supported!";
	sendResponse(client_fd, response);
}

void HttpRequest::sendResponse(int client_fd, const std::string &response)
{
	ssize_t bytes_written = write(client_fd, response.c_str(), response.size());
	if (bytes_written == -1)
	{
		std::cout << "Failed to write to socket" << std::endl;
		close(client_fd);
	}
}

std::string HttpRequest::getContentType(const std::string &fileName)
{
    if (fileName.rfind(".html") == fileName.length() - 5) return "text/html";
    if (fileName.rfind(".css") == fileName.length() - 4) return "text/css";
    if (fileName.rfind(".js") == fileName.length() - 3) return "application/javascript";
    if ((fileName.rfind(".jpg") == fileName.length() - 4) || (fileName.rfind(".jpeg") == fileName.length() - 5)) return "image/jpeg";
    if (fileName.rfind(".png") == fileName.length() - 4) return "image/png";
    if (fileName.rfind(".gif") == fileName.length() - 4) return "image/gif";
    return "application/octet-stream";
}

bool HttpRequest::isItAllowMethod(const std::string& location, Server &server, const std::string &method)
{
    std::vector<Location>& locations = server.getLocations();
    for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		if(it->getPath() == location)
		{
			std::vector<std::string> &allowmethods = it->getAllowMethods();
			for (std::vector<std::string>::iterator it2 = allowmethods.begin(); it2 != allowmethods.end(); ++it2) 
			{
    			if(*it2 == method)
					return true;
			}
			return false;
		}		
    }
	return false;
}

std::string HttpRequest::getRootPath(const std::string &location, Server &server)
{
    std::vector<Location>& locations = server.getLocations();
    for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
		if(it->getPath() == location)
			return server.getServerRoot();
	throw std::runtime_error("Error: Location was not found.");
}