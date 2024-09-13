#include "Location.hpp"
#include <sstream>

Location::Location()
{
	_path = "";
	_index = "";
	_autoindex = false;
	_allowMethods = std::vector<std::string>();
}

Location::~Location()
{
	
}

void Location::setPath(std::string &p)
{
	if (_path.empty() && !p.empty())
		_path = p;
	else
		throw std::runtime_error("Error: Location path empty or already set");
}

void Location::setIndex(std::string &idx)
{
	if (_index.empty() && !idx.empty())
		_index = idx;
	else
		throw std::runtime_error("Error: Location index empty or already set");
}

void Location::setAllowMethods(std::string &methods)
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
		throw std::runtime_error("Error: Location allow_methods empty or already set");
}

void Location::setAutoindex(bool value)
{
	if (!_autoindex)
		_autoindex = value;
	else
		throw std::runtime_error("Error: Location autoindex already set");
}

std::string& Location::getPath()
{
	return _path;
}

std::string& Location::getIndex()
{
	return _index;
}

std::vector<std::string>& Location::getAllowMethods()
{
	return _allowMethods;
}

bool& Location::getAutoindex()
{
	return _autoindex;
}

bool Location::validateMethod(std::string &method)
{
	std::string validMethods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++)
	{
		if (method == validMethods[i])
			return true;
	}
	throw std::runtime_error("Error: Invalid method in allow_methods");
}