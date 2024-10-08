#include "Location.hpp"
#include <sstream>

Location::Location()
{
	_path = "";
	_index = "";
	_autoindex = "";
}

void Location::setPath(const std::string &p)
{
	if (_path.empty() && !p.empty())
		_path = p;
	else
		throw std::runtime_error("Error: Location path empty or duplicate");
}

void Location::setIndex(const std::string &idx)
{
	if (_index.empty() && !idx.empty())
		_index = idx;
	else
		throw std::runtime_error("Error: Location index empty or duplicate");
}

void Location::setAllowMethods(const std::string &methods)
{
	if (!methods.empty())
	{
		std::string method;
		std::stringstream ss(methods);
		while (std::getline(ss, method, ','))
		{
			if (!validateMethod(method) || std::find(_allowMethods.begin(), _allowMethods.end(), method) != _allowMethods.end())
				throw std::runtime_error("Error: Invalid method in allow_methods or duplicate");	
			_allowMethods.push_back(method);
		}
	}
}

bool Location::validateMethod(const std::string &method) const
{
	static const std::string validMethods[9] = {"GET", "POST", "DELETE", "PUT", "HEAD", "CONNECT", "OPTIONS", "TRACE", "PATCH"};
	for (int i = 0; i < 9; i++)
	{
		if (method == validMethods[i])
			return true;
	}
	throw std::runtime_error("Error: Invalid method in allow_methods");
}

void Location::setAutoindex(const std::string &value)
{
    if (_autoindex.empty() && !value.empty())
    {
        if (value == "on" || value == "off")
            _autoindex = value;
        else
            throw std::runtime_error("Error: Invalid value for autoindex (must be 'on' or 'off')");
    }
    else
        throw std::runtime_error("Error: Location autoindex empty or duplicate");
}


const std::string& Location::getPath() const
{
	return _path;
}

const std::string& Location::getIndex() const
{
	return _index;
}

const std::vector<std::string>& Location::getAllowMethods() const
{
	return _allowMethods;
}

const std::string& Location::getAutoindex() const
{
	return _autoindex;
}

bool Location::checkMethod(const std::string &method) const
{
	for (std::vector<std::string>::const_iterator it = _allowMethods.begin(); it != _allowMethods.end(); it++)
	{
		if (*it == method)
			return true;
	}
	return false;
}