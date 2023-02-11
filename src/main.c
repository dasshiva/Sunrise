#include <stdio.h>
#include <include/rt.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include <argparse.h>
#include <include/config.h>

#define APP "Sunrise - A Java Virtual Machine Implementation"
#define COPYRIGHT "Copyright (C) 2023 Shivashish Das"

config* app;

static void default_init(config* app) {
  if (!app->syslib)
    app->syslib = "./syslib.jar";
  if (app->debug == -1)
    app->debug = 0;
}
void handler(int signum) {
  fprintf(stderr, "Segmentation fault ocurred");
  exit(1);
}

int main(int argc, char* argv[]) {
    GC_INIT();
    signal(SIGSEGV, &handler);
    app = GC_MALLOC(sizeof(config));
    struct argparse_option opts[] = {
      OPT_HELP(),
      OPT_STRING('s', "syslib", &app->syslib, "Specify syslib.jar location (Default is current directory)", NULL, 0, 0),
      OPT_BOOLEAN('d', "debug", &app->debug, "Output debug logs", NULL, 0, 0),
      OPT_END()
    };
    const char *const usages[] = {
      "sun [options] main-class [program arguments]",
      NULL
    };
    struct argparse parser;
    argparse_init(&parser, opts, usages, 0);
    argparse_describe(&parser, APP, COPYRIGHT);
    argc = argparse_parse(&parser, argc, argv);
    if (argc == 0) {
      argparse_usage(&parser);
      err("Main class name needed");
    }
    default_init(app);
    load_jar(app->syslib);
    class* c = get_class(argv[argc - 1]);
    method* m = get_method(c, "main","()V");
    if (m == NULL) 
      err ("Method main not found");
    frame* f = new_frame(m, c->cp, c->this_class);
    exec(f);
    return 0;
}
