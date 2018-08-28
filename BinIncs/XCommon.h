#ifndef __XCOMMON_H__
#define __XCOMMON_H__



//默认配置
#define _LOG_FILE_ "./run.log"

#define _IP_ "Any"
#define _PORT_ 9090
#define _LQN_ 1000

#define _XSERVER_THREAD_SIZE_ 1					//服务器线程数

#define _XCLIENT_HEART_TIME_ 10000000			//客户端心跳时间
#define _XCLIENT_SEND_TIME_ 3000000				//客户端发送时间
#define _XCLIENT_RECV_BUFFER_SIZE_ 10240		//缓冲区接收大小
#define _XCLIENT_SEND_BUFFER_SIZE_ 10240		//缓冲区发送大小

#define _XMARIADB_CONNECT_NUM_ 1000				//数据库连接池数

//系统相关宏定义
#ifdef _WIN32
	#define FD_SETSIZE 10240
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#else

#ifdef __APPLE__
	#define _DARWIN_UNLIMITED_SELECT			//苹果系统下select不限制数量。
#endif // !__APPLE__

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR    (-1)
#endif // _WIN32

//系统网络相关头文件
#ifdef _WIN32
#include <windows.h>
#include <atlsocket.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif // _WIN32

//系统头文件
#include <assert.h>
#include <stdlib.h>
#include <memory>

//内存池
#include "XMemory.h"

//对象池
#include "XObject.h"

//日志
#include "XLog.h"

//计时器
#include "XTimer.h"

//协议头
#include "MsgProtocol.h"

#endif