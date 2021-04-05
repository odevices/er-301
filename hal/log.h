#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  void Log_init();
  int logCount();
  void logWrite(const char * buffer, int len);

  void _logRaw(const char *fmt, ...);
  void _logAssert(const char *filename, const char *function, int linenum, int expr);
  void _logBriefNice(const char *label, const char *fmt, ...);
  void _logCompleteNice(const char *label, const char *filename, const char *function, int linenum, const char *fmt, ...);
  void _logRude(const char *label, const char *filename, const char *function, int linenum, const char *fmt, ...);
  void _logSkipped();

#ifdef BUILDOPT_TESTING

#ifndef BUILDOPT_DEBUG_LEVEL
#define BUILDOPT_DEBUG_LEVEL 0
#endif

#define LOG_LABEL_INFO "\033[32mINFO\033[0m"
#define LOG_LABEL_WARN "\033[33mWARN\033[0m"
#define LOG_LABEL_ERROR "\033[31mERROR\033[0m"
#define LOG_LABEL_DEBUG "\033[35mDEBUG\033[0m"
#define LOG_LABEL_FATAL "\033[31;1mFATAL\033[0m"

#define logRaw(fmt, ...) _logRaw(fmt, ##__VA_ARGS__)
#define logFatal(fmt, ...) _logRude(LOG_LABEL_FATAL, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define logAssert(expr) _logAssert(__FILE__, __FUNCTION__, __LINE__, (expr) ? 1 : 0)

#ifdef BUILDOPT_VERBOSE

#define logInfo(fmt, ...) _logCompleteNice(LOG_LABEL_INFO, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define logWarn(fmt, ...) _logCompleteNice(LOG_LABEL_WARN, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define logError(fmt, ...) _logCompleteNice(LOG_LABEL_ERROR, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#if BUILDOPT_DEBUG_LEVEL
#define logDebug(level, fmt, ...)                                                                   \
  if (level <= BUILDOPT_DEBUG_LEVEL)                                                                 \
  {                                                                                                 \
    _logCompleteNice(LOG_LABEL_DEBUG #level, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__); \
  }                                                                                                 \
  else                                                                                              \
  {                                                                                                 \
    _logSkipped();                                                                                  \
  }
#else
#define logDebug(level, fmt, ...)
#endif

#else

#define logInfo(fmt, ...) _logBriefNice(LOG_LABEL_INFO, fmt, ##__VA_ARGS__)
#define logWarn(fmt, ...) _logBriefNice(LOG_LABEL_WARN, fmt, ##__VA_ARGS__)
#define logError(fmt, ...) _logBriefNice(LOG_LABEL_ERROR, fmt, ##__VA_ARGS__)

#if BUILDOPT_DEBUG_LEVEL
#define logDebug(level, fmt, ...)                              \
  if (level <= BUILDOPT_DEBUG_LEVEL)                            \
  {                                                            \
    _logBriefNice(LOG_LABEL_DEBUG #level, fmt, ##__VA_ARGS__); \
  }                                                            \
  else                                                         \
  {                                                            \
    _logSkipped();                                             \
  }
#else
#define logDebug(level, fmt, ...)
#endif

#endif

#else

#define logRaw(fmt, ...)
#define logInfo(fmt, ...)
#define logWarn(fmt, ...)
#define logError(fmt, ...)
#define logFatal(fmt, ...)
#define logAssert(expr) \
  if (expr)             \
  {                     \
  }
#define logDebug(fmt, ...)

#endif

#ifdef __cplusplus
}
#endif
