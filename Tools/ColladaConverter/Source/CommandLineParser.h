#ifndef __DAVAENGINE_COMMANDLINEPARSER_H__
#define __DAVAENGINE_COMMANDLINEPARSER_H__

#include <assert.h>
#include "Base/BaseTypes.h"

namespace DAVA
{

class CommandLineParser
{
public:
	CommandLineParser(int argc, char * argv[]);
	CommandLineParser(const Vector<String> & commandLine);
	~CommandLineParser();
	
	
	bool	IsFlagSet(const String & s);
	uint32	GetFlagCount() { return flags.size(); };
	String	GetParam(int index);
	uint32	GetParamCount() { return params.size(); };
	
	static CommandLineParser * Instance() { return instance; };	


	static void		SplitFilePath(const String & filepath, String & path, String & filename);
	static void		RemoveFromPath(String & path, const String & removePart);	
	static String	ReplaceExtension(const String & filename, const String & nexExt);
	
	static String RealPath(String path);

private:
	Vector<String>	params;
	Vector<String>	flags;
	static CommandLineParser * instance;
};


};

#endif // __DAVAENGINE_COMMANDLINEPARSER_H__