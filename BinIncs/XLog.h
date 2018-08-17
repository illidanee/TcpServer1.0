#ifndef __XLOG_H__
#define __XLOG_H__

#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <assert.h>
#include <stdio.h>
#include <mutex>
#include <condition_variable>
#include <list>
#include <functional>
#include <thread>
#include <chrono>
#include <ctime>

enum XLogType
{
	Info,
	Warn,
	Error
};

class XLog
{
public:
	static XLog& GetInstance();									//log必须使用类型，因为构造函数被私有。

	static void SetFileName(const char* pFileName, const char* pMode);

	template <typename... Args>
	static void Info(const char* pFormat, Args... args)
	{
		Echo(XLogType::Info, pFormat, args...);
	}

	template <typename... Args>
	static void Warn(const char* pFormat, Args... args)
	{
		Echo(XLogType::Warn, pFormat, args...);
	}
	
	template <typename... Args>
	static void Error(const char* pFormat, Args... args)
	{
		Echo(XLogType::Error, pFormat, args...);
	}

	template <typename... Args>
	static void Echo(XLogType type, const char* pFormat, Args... args)
	{
		//输出到文件
		XLog& log = GetInstance();								//log必须使用类型，因为构造函数被私有。

		//log 必须使用引用。否则会阻塞住。
		log.AddTask([&log, type, pFormat, args...]() {				//log必须使用类型，因为构造函数被私有。

			//计算当前时间
			std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
			time_t tt = std::chrono::system_clock::to_time_t(t);
			std::tm* ttt = std::localtime(&tt);
			
			switch (type)
			{
			case XLogType::Info:
			{
				//输出到终端
				printf("=>%-5s|%s| [%04d-%02d-%02d_%02d:%02d:%02d] ", "Info", "   ", ttt->tm_year + 1900, ttt->tm_mon + 1, ttt->tm_mday, ttt->tm_hour, ttt->tm_min, ttt->tm_sec);
				printf(pFormat, args...);

				//输出到文件
				fprintf(log._File, "=>%-5s|%s| [%04d-%02d-%02d_%02d:%02d:%02d] ", "Info", "   ", ttt->tm_year + 1990, ttt->tm_mon + 1, ttt->tm_mday, ttt->tm_hour, ttt->tm_min, ttt->tm_sec);
				fprintf(log._File, pFormat, args...);
				fflush(log._File);
				break;
			}
			case XLogType::Warn:
			{
				//输出到终端
				printf("=>%-5s|%s| [%04d-%02d-%02d_%02d:%02d:%02d] ", "Warn", " ! ", ttt->tm_year + 1900, ttt->tm_mon + 1, ttt->tm_mday, ttt->tm_hour, ttt->tm_min, ttt->tm_sec);
				printf(pFormat, args...);

				//输出到文件
				fprintf(log._File, "=>%-5s|%s| [%04d-%02d-%02d_%02d:%02d:%02d] ", "Warn", " ! ", ttt->tm_year + 1990, ttt->tm_mon + 1, ttt->tm_mday, ttt->tm_hour, ttt->tm_min, ttt->tm_sec);
				fprintf(log._File, pFormat, args...);
				fflush(log._File);
				break;
			}
			case XLogType::Error:
			{
				//输出到终端
				printf("=>%-5s|%s| [%04d-%02d-%02d_%02d:%02d:%02d] ", "Error", " X ", ttt->tm_year + 1900, ttt->tm_mon + 1, ttt->tm_mday, ttt->tm_hour, ttt->tm_min, ttt->tm_sec);
				printf(pFormat, args...);

				//输出到文件
				fprintf(log._File, "=>%-5s|%s| [%04d-%02d-%02d_%02d:%02d:%02d] ", "Error", " X ", ttt->tm_year + 1990, ttt->tm_mon + 1, ttt->tm_mday, ttt->tm_hour, ttt->tm_min, ttt->tm_sec);
				fprintf(log._File, pFormat, args...);
				fflush(log._File);
				break;
			}
			}
		});
	}

private:
	//私有化构造析构函数。
	XLog();
	XLog(const XLog& that);
	XLog& operator=(const XLog& that);
	~XLog();

	void Start();
	void Stop();
	void Run();

	void AddTask(std::function<void()> pTask);

private:
	FILE* _File;												//日志文件

	bool _Run;													//当前线程是否正在运行
	std::mutex _ThreadlMutex;									//线程锁

	std::condition_variable _CV;								//条件变量
	std::mutex _SignalMutex;									//锁
	int _WaitNum;												//等待计数
	int _WakeNum;												//唤醒计数

	std::list<std::function<void()>> _Tasks;					//任务列表
	std::list<std::function<void()>> _TasksCache;				//任务缓冲区
	std::mutex _TasksCacheMutex;								//任务缓冲区锁
};

//调试宏
#ifndef XInfo
#define XInfo(...) XLog::Info(__VA_ARGS__)
#endif

#ifndef XWarn
#define XWarn(...) XLog::Warn(__VA_ARGS__)
#endif

#ifndef XError
#define XError(...) XLog::Error(__VA_ARGS__)
#endif

#ifndef XDebug
#ifdef _DEBUG
#define XDebug(...) XLog::XDebug(__VA_ARGS__)
#else
#define XDebug(...)
#endif
#endif



#endif