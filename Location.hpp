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
		void setPath(const std::string &);
		void setIndex(const std::string &);
		void setAllowMethods(const std::string &);
		void setAutoindex(bool);
		bool validateMethod(const std::string &);
		const bool &getAutoindex() const;
		bool checkMethod(const std::string &) const;// bu duruma göre silinebilir
		const std::string &getPath() const;	
		const std::string &getIndex() const;
		const std::vector<std::string> &getAllowMethods() const;
};

#endif