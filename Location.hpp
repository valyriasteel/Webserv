#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <iostream>

class Location
{
	private:
		std::vector<std::string> _allow_methods;
		std::string _root;
		std::string _index;
		std::string _cgi_path;
		std::string _cgi_extension;
		bool _autoindex;
	public:
		Location();
		void setAllowMethods(const std::vector<std::string> &);
		void setRoot(const std::string &);
		void setIndex(const std::string &);
		void setCgiPath(const std::string &);
		void setCgiExtension(const std::string &);
		void setAutoindex(bool value);
		const std::vector<std::string> &getAllowMethods() const;
		const std::string &getRoot() const;
		const std::string &getIndex() const;
		const std::string &getCgiPath() const;
		const std::string &getCgiExtension() const;
		bool isAutoindex() const;
};

#endif