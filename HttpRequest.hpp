#ifndef HTTPREQUEST_HPP
# define HTPPREQUEST_HPP

#include <iostream>
#include <map>

class HttpRequest
{
    private:
        std::string _method;
        std::string _url;
        std::string _version;
        std::string _body;
        std::map<std::string, std::string> _headers;
        std::string _locationurl;
        std::string _locationurlfile;
    public:
        HttpRequest();
        ~HttpRequest();
        static void handleHttpRequest(int client_fd, const std::string &request); 
        static HttpRequest parseHttpRequest(const std::string &request);
        static void parseHttpRequestUrl(std::string &httprequesturl);
        static bool controlLocationUrl(std::string &location, std::vector<Server> &);

};

#endif