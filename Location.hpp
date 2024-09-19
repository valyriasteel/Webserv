#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <iostream>

class Location
{
	private:
		std::string _path;
		std::string _index;
		std::vector<std::string> _allowMethods;
		bool _autoindex;
	public:
		Location();
		~Location();
		void setPath(std::string &);
		void setIndex(std::string &);
		void setAllowMethods(std::string &);
		void setAutoindex(bool);
		bool validateMethod(std::string &);
		bool &getAutoindex();
		bool checkMethod(std::string &);
		std::string &getPath();
		std::string &getIndex();
		std::vector<std::string> &getAllowMethods();
};

#endif