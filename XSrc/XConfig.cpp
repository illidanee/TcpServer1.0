#include "XConfig.h"

XConfig::XConfigDelegate XConfig::_Delegate;

void XConfig::Init(int argc, char* args[])
{
	_Exe = args[0];

	for (int i = 1; i < argc; ++i)
	{
		char* pAddr = strchr(args[i], '=');

		if (pAddr)
		{
			*pAddr = 0;
			_Args.insert(std::pair<std::string, std::string>(args[i], pAddr + 1));
		}
		else
		{
			_Args.insert(std::pair<std::string, std::string>(args[i], ""));
		}	
	}
}

bool XConfig::HasKey(const char* key)
{
	std::map<std::string, std::string>::iterator it = _Args.find(key);
	if (it != _Args.end())
	{
		return true;
	}
	return false;
}

const char* XConfig::GetStringArg(const char* key, const char* def)
{
	std::map<std::string, std::string>::iterator it = _Args.find(key);
	if (it != _Args.end())
	{
		def = it->second.c_str();
	}

	return def;
}

int XConfig::GetIntArg(const char* key, int def)
{
	std::map<std::string, std::string>::iterator it = _Args.find(key);
	if (it != _Args.end())
	{
		def = atoi(it->second.c_str());
	}

	return def;
}

XConfig::XConfig()
{

}

XConfig::XConfig(const XConfig&)
{

}

XConfig& XConfig::operator=(const XConfig&)
{
	return *this;
}

XConfig::~XConfig()
{

}