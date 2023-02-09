#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct {
  char* syslib;
  char* main;
  int debug;
} config;

extern config* app;
#endif