#include "XCommon.h"
#include "XConfig.h"
#include "MyServer.h"

#ifndef _WIN32
#include <signal.h>
#endif

int main(int argc, char* args[])
{

//关闭中断信号，禁止中断服务器进程。
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

	//解析参数。
	XConfig::Instance().Init(argc, args);

	//日志信息
	XLog::SetFileName(_LOG_FILE_, "w");

	XInfo("---------------------------------------------------------------------------------------------------------------------------------------\n");
	XInfo("                                                             C++ TCP Server                                                            \n");
	XInfo("                                                                                                        Designed by Org.illidan        \n");
	XInfo("---------------------------------------------------------------------------------------------------------------------------------------\n");

	//创建服务器
	MyServer* server = new MyServer;
	//初始化服务器
	server->Init();
	//开启服务器
	server->Start();

	while (true)
	{
		char buffer[32] = {};
		scanf("%s", buffer);
		if (0 == strcmp(buffer, "exit"))
		{
			server->Stop();
			break;
		}
		else if (0 == strcmp(buffer, "Exit"))
		{
			server->Stop();
			break;
		}
	}

	//销毁服务器
	server->Done();
	return 0;
}
