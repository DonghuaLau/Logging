#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include "log.h"
#include "common.h"

static FILE *flog = NULL;
static char slogfile[256];
static int slog_level = 0;
static pthread_mutex_t log_mutex;

int OpenLog(const char *logfile, int log_level)
{
	flog = fopen(logfile, "a");
	if(flog == NULL){
		fprintf(stderr, "Log file open failed, file: %s, errno: %d\n", logfile, errno);
		return -1;
	}
	slog_level = log_level;
	pthread_mutex_init(&log_mutex, NULL);
	return 0;
}

int CloseLog()
{
	if(flog) fclose(flog);
	pthread_mutex_destroy(&log_mutex);
}

int Logging(int level, const char *format, ...)
{
	if(flog == NULL){
		fprintf(stderr, "Can't write log, the log file have not open.\n");
		return -1;
	}
	
	if(level > slog_level) return 0;
	if(level < E_LOG_ERROR) return -1;

	pthread_mutex_lock(&log_mutex);

	va_list arglst;
	va_start(arglst, format);

	int nb = 0;
	char logbuf[2048] = {0};
	char timestr[24];
	switch(level){
	case E_LOG_ERROR:
		snprintf(logbuf, 2048, "%s -ERROR- %s\n", gettime(timestr), format);
		break;
	case E_LOG_WARNNING:
		snprintf(logbuf, 2048, "%s -WARNNING- %s\n", gettime(timestr), format);
		break;
	case E_LOG_INFO:
		snprintf(logbuf, 2048, "%s -INFO- %s\n", gettime(timestr), format);
		break;
	case E_LOG_DEBUG:
		snprintf(logbuf, 2048, "%s -DEBUG- %s\n", gettime(timestr), format);
		break;
	default:
		break;
	}

	nb = vfprintf(flog, logbuf, arglst);

	pthread_mutex_unlock(&log_mutex);

	return 0;
}


