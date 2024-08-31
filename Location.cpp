#include "Location.hpp"

Location::Location()
{
	_autoindex = false;
}

void Location::setAllowMethods(const std::vector<std::string> &methods)
{
	_allow_methods = methods;
}

void Location::setRoot(const std::string &r)
{
	_root = r;
}

void Location::setIndex(const std::string &idx)
{
	_index = idx;
}

void Location::setCgiPath(const std::string &path)
{
	_cgi_path = path;
}

void Location::setCgiExtension(const std::string &ext)
{
	_cgi_extension = ext;
}

void Location::setAutoindex(bool value)
{
	_autoindex = value;
}

const std::vector<std::string> &Location::getAllowMethods() const
{
	return _allow_methods;
}

const std::string &Location::getRoot() const
{
	return _root;
}

const std::string &Location::getIndex() const
{
	return _index;
}

const std::string &Location::getCgiPath() const
{
	return _cgi_path;
}

const std::string &Location::getCgiExtension() const
{
	return _cgi_extension;
}

bool Location::isAutoindex() const
{
	return _autoindex;
}