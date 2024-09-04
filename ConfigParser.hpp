#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Server.hpp"
# include "ParserState.hpp"

class ConfigParser
{
	private:
		std::vector<Server> _servers;
	public:
		static void checkArgument(const std::string &);
		static bool isWhitespaceOrControl(char c);
		static bool isLineEmptyOrWhitespace(const std::string &);
		std::vector<Server>configFileParser(const std::string &);
		void handleServerBlock(std::istringstream &, ParserState &, std::string &);
		void handleOpeningBrace(ParserState &);
		void handleLocationBlock(ParserState &, std::istringstream &);
		void handleClosingBrace(ParserState &, std::vector<Server> &);
		void handleDirective(ParserState &, std::string &, std::istringstream &);
};

#endif