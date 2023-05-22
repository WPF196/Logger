#ifndef __LOGGER__
#define __LOGGER__

#include <string>
#include <fstream>

using namespace std;

namespace melon{
namespace utility{

// 宏定义，简化函数
#define debug(format, ...) \
    Logger::instance()->log(DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__);
#define info(format, ...) \
    Logger::instance()->log(INFO, __FILE__, __LINE__, format, ##__VA_ARGS__);
#define warn(format, ...) \
    Logger::instance()->log(WARN, __FILE__, __LINE__, format, ##__VA_ARGS__);
#define error(format, ...) \
    Logger::instance()->log(ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__);
#define fatal(format, ...) \
    Logger::instance()->log(FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__);

// 日志级别
enum Level{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL,
    LEVEL_COUNT
};

class Logger{
public:
    static Logger* instance();          // 返回单例
    void open(const string& filename);  // 打开文件
    void close();                       // 关闭文件

    // 将日志写入文件（日志等级，日志所在的文件，日志函数所在的行号，日志内容...）
    void log(Level level, const char* file, int line, const char* format, ...);
    
    void set_Level(Level level) { m_level = level; };   // 设置日志等级
    void set_Max(int bytes) { m_max = bytes; };         // 设置最大字节数

private:
    Logger();
    ~Logger();

    void rotate();                  // 文件翻滚

private:
    string m_filename;              // 日志需要写入的文件名
    ofstream m_fout;                // 文件输出流（write）
    static const char* s_level[LEVEL_COUNT]; // 等级映射
    static Logger* m_instance;      // 全局唯一的实例（单例）
    Level m_level;                  // 日志级别
    int m_max;                      // 日志上限大小
    int m_len;                      // 日志当前大小
};


}
}



#endif