#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

// Macros para organizar os níveis de severidade dos logs
#define LOG_INFO  1
#define LOG_ERROR 2
#define LOG_DEBUG 3

#define SERIAL_COM1_BASE 0x3F8

void serial_configure(unsigned short com);
void log_message(int level, char *msg);

#endif /* INCLUDE_SERIAL_H */