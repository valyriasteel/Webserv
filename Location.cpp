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
		throw std::runtime_error("Error: Location path already set");
}

void Location::setIndex(const std::string &idx)
{
	if (_index.empty() && !idx.empty())
		_index = idx;
	else
		throw std::runtime_error("Error: Location index already set");
}

void Location::setAllowMethods(const std::string &methods)
{
	if (_allowMethods.empty() && !methods.empty())
	{
		std::string method;
		std::stringstream ss(methods);
		while (std::getline(ss, method, ','))
		{
			if (validateMethod(method))
				_allowMethods.push_back(method);
		}
	}
	else
		throw std::runtime_error("Error: Location allow_methods already set");
}

bool Location::validateMethod(const std::string &method) const
{
	static const std::string validMethods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++)
	{
		if (method == validMethods[i])
			return true;
	}
	throw std::runtime_error("Error: Invalid method in allow_methods");
}

void Location::setAutoindex(const std::string &value)
{
    if (_autoindex.empty())
    {
        if (value == "on" || value == "off") // "on" ve "off" kontrolü
            _autoindex = value;
        else
            throw std::runtime_error("Error: Invalid value for autoindex (must be 'on' or 'off')");
    }
    else
        throw std::runtime_error("Error: Location autoindex already set");
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

std::string Location::getAutoindex() const
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