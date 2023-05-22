#include "logger.h"
#include <time.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <stdarg.h>

using namespace std;
using namespace melon::utility;

const char* Logger::s_level[LEVEL_COUNT] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

Logger * Logger::m_instance = nullptr;

Logger::Logger() : m_max(0), m_len(0), m_level(DEBUG){
}

Logger::~Logger(){
    close();
}

Logger* Logger::instance(){
    if(m_instance == nullptr)                   // 实例不存在就new一个
        m_instance = new Logger();
    return m_instance;
}

void Logger::open(const string& filename){
    m_filename = filename;                      // 更新类内文件名
    m_fout.open(filename, ios::app);            // 以追加的方式打开文件
    if(m_fout.fail())
        throw logic_error("open file failed " + filename);
    m_fout.seekp(0, ios::end);                               
    m_len = m_fout.tellp();                     // 获取当前流的长度
}

void Logger::close(){
    m_fout.close();
}

void Logger::log(Level level, const char* file, int line, const char* format, ...){
    if(m_level > level) return;

    if(m_fout.fail())
        throw logic_error("open file failed " + m_filename);

    time_t ticks = time(NULL);              // 获取当前的时间戳
    struct tm* ptm = localtime(&ticks);     // 用tick初始化时间结构体（年月日时分秒）
    char timestamp[32];                     // 存储转换为字符形式的时间
    memset(timestamp, 0, sizeof timestamp);
    strftime(timestamp, sizeof(timestamp),  // 将时间转换为字符串传入字符数组timestamp
        "%Y-%m-%d %H:%M:%S", ptm);          
    const char *fmt = "%s %s %s:%d ";       // 定义输出格式（时间，级别，文件名，行号）
    
    // 获取相关内容的size，（输出目标，输出大小，输出格式，内容1，内容2, .....）
    int size = snprintf(NULL, 0, fmt, timestamp, s_level[level], file, line);
    if(size > 0){
        char* buffer = new char[size + 1];
        // 以fmt的格式输出相关信息到buffer
        snprintf(buffer, size + 1, fmt, timestamp, s_level[level], file, line);
        buffer[size] = '\0';                // 文件结束符
        cout << buffer << endl;
        m_fout << buffer;                   // 记录到文件流
        m_len += size;
        delete buffer;
    }

    va_list arg_ptr;                        // 指向可变参数
    va_start(arg_ptr, format);              // 初始化arg_ptr
    size = vsnprintf(NULL, 0, format, arg_ptr); // 获取可变参数size
    va_end(arg_ptr);                        // 释放arg_ptr 
    if(size > 0){
        char* content = new char[size + 1]; // 保存可变参数
        va_start(arg_ptr, format);
        vsnprintf(content, size + 1, format, arg_ptr);  // 将可变参数存于content
        va_end(arg_ptr);
        cout << content << endl;
        m_fout << content;
        m_len += size;
        delete content;
    }
    m_fout << "\n";
    m_fout.flush();                         // 内容写入硬盘

    if(m_len >= m_max && m_max > 0)         // 内存超出且设置了上限
        rotate();
}

// 每次都往原文件中写，当超出内存，则重命名原文件，然后再生成原文件
void Logger::rotate(){
    close();
    time_t ticks = time(NULL);
    struct tm* ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof timestamp);
    strftime(timestamp, sizeof (timestamp), ".%Y-%m-%d_%H-%M-%S", ptm);
    string filename = m_filename + timestamp;               // 新文件名 = 当前文件名 + 时间戳
    if(rename(m_filename.c_str(), filename.c_str()) != 0)   // 重命名文件，成功返回0
        throw logic_error("rename log file failed: ");
    open(m_filename); 
}