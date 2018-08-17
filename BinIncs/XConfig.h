#ifndef __XCONFIG_H__
#define __XCONFIG_H__

#include <string.h>
#include <string>
#include <map>

class XConfig
{
public:
	class XConfigDelegate
	{
	public:
		XConfigDelegate()
		{
			XConfig::Instance();
		}
	};

private:
	static XConfigDelegate _Delegate;

public:
	static XConfig& Instance()
	{
		static XConfig _Ins;
		return _Ins;
	}

	void Init(int argc, char* args[]);

	bool HasKey(const char* key);
	const char* GetStringArg(const char* key, const char* def);
	int GetIntArg(const char* key, int def);

private:
	XConfig();
	XConfig(const XConfig&);
	XConfig& operator=(const XConfig&);
	~XConfig();

private:
	std::string _Exe;
	std::map<std::string, std::string> _Args;
};

#endif // ifndef
