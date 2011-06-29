#include "stdafx.h"
#include <stdlib.h>
#include "CommandLineParser.h"
#include <string>
#include <cerrno>


namespace DAVA
{
	
CommandLineParser * CommandLineParser::instance = 0;

CommandLineParser::CommandLineParser(int argc, char * argv[])
{
	if (instance == 0)
		instance = this;
	
	for (int i = 0; i < argc; ++i)
	{
		char * arg = argv[i];
		size_t argLen = strlen(arg);
		if ((argLen >= 1) && (arg[0] == '-'))
			flags.push_back(String(arg));
		else params.push_back(String(arg));
	}
}
	
CommandLineParser::CommandLineParser(const Vector<String> & commandLine)
{
	if (instance == 0)
		instance = this;

	for (size_t in = 0; in < commandLine.size(); ++in)
	{
		if ((commandLine[in].length() >= 1) && (commandLine[in][0] == '-'))
			flags.push_back(commandLine[in]);
		else params.push_back(commandLine[in]);
	}
}


CommandLineParser::~CommandLineParser()
{


}

bool CommandLineParser::IsFlagSet(const String & s)
{
	for (uint32 k = 0; k < flags.size(); ++k)
		if (flags[k] == s)return true;
	return false;
}

String CommandLineParser::GetParam(int index)
{
	return params[index];
}

void CommandLineParser::SplitFilePath( const String & filePath, String & path, String & filename )
{
	String fullPath(filePath);
	std::replace(fullPath.begin(),fullPath.end(),'\\','/');
	// now only Unix style slashes
	String::size_type lastSlashPos = fullPath.find_last_of('/');
	
	if (lastSlashPos==std::string::npos)
	{
		path = String("");
		filename = fullPath;
	}
	else
	{
		path = fullPath.substr(0,lastSlashPos);
		filename = fullPath.substr(lastSlashPos + 1, fullPath.size() - lastSlashPos - 1);
	}
}



//! \brief Return canonical path name of \a path.
//
// realpath expands all symbolic links and resolves references to '/./', '/../' and extra '/' characters in
// the string named by path and returns the canonicalized absolute pathname.
// The resulting path will have no symbolic link, '/./' or '/../' components, also no trailing ones.
// Nor will it  end on a slash: if the result is the root then the returned path is empty,
// and unless the result is empty, it will always start with a slash.
//

String realpath(String const& _path)
{

	String path = (_path);
	std::replace(path.begin(), path.end(),'\\','/');
	
	const String & delims="/";

	// Skip delims at beginning, find start of first token
	String::size_type lastPos = path.find_first_not_of(delims, 0);
	// Find next delimiter @ end of token
	String::size_type pos     = path.find_first_of(delims, lastPos);

	// output vector
	Vector<String> tokens;
	
	
	while (String::npos != pos || String::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(path.substr(lastPos, pos - lastPos));
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = path.find_first_not_of(delims, pos);
		// Find next delimiter at end of token.
		pos     = path.find_first_of(delims, lastPos);
	}

	String result;
	
	for (int i = 0; i < (int)tokens.size(); ++i)
	{
		if (tokens[i] == String("."))
		{		
			for (int k = i + 1; k < (int)tokens.size(); ++k)
			{
				tokens[k - 1] = tokens[k];
			}
			i--;
			tokens.pop_back();
		}
		if (tokens[i] == String(".."))
		{		
			for (int k = i + 1; k < (int)tokens.size(); ++k)
			{
				tokens[k - 2] = tokens[k];
			}
			i-=2;
			tokens.pop_back();
			tokens.pop_back();
		}	
	}
#ifndef _WIN32
	result = String("/");
#endif
	for (int k = 0; k < (int)tokens.size(); ++k)
	{
		result += tokens[k];
		if (k + 1 != (int)tokens.size())
			result += String("/");
	}
	return result;
}


String CommandLineParser::RealPath( String path )
{
	return realpath(path);
	//char tmp[1024];
	//_realpath(path.c_str(), tmp);
	//return String(tmp);
}
	
	
void replace(std::string & repString,const std::string & needle, const std::string & s)
{
	std::string::size_type lastpos = 0, thispos;
	while ((thispos = repString.find(needle, lastpos)) != std::string::npos)
	{
		repString.replace(thispos, needle.length(), s);
		lastpos = thispos + 1;
	}
}

void	CommandLineParser::RemoveFromPath(String & path, const String & removePart)
{
	replace(path, removePart, std::string(""));
}
	
String CommandLineParser::ReplaceExtension(const String & filename, const String & newExt)
{
	String result = filename.substr(0, filename.length() - 4) + newExt;
	return result;
}	
	
	
};