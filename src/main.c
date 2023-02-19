#include <include/rt.h>
#include <signal.h>
#include <stdlib.h>
#include <include/config.h>

static char* usage = "Sunrise - A Java Virtual Machine Implementation\n\
Copyright (C) 2023 Shivashish Das \n\n\
Usage : \n\
-h - Print this help menu \n\
-d - Output debug logs \n\
-s - Specify syslib.jar location (By default searches in current directory\n\
Use -- to separate arguments to the vm and the program\n";

config* app = NULL;
list* stack = NULL;
void prepare_args(frame* f, int argc, char* argv[], int total);

static void default_init(config* app) {
  if (!app->syslib)
    app->syslib = "./syslib.jar";
  if (app->debug == -1)
    app->debug = 0;
}

void handler(int signum) {
  switch (signum) {
    case SIGSEGV: fputs("Segmentation fault ocurred", stderr); break;
    case SIGINT: fputs("Exiting due to interrupt signal", stderr);
    break;
  }
  exit(1);
}

int main(int argc, char* argv[]) {
    GC_INIT();
    signal(SIGSEGV, &handler);
    signal(SIGINT, &handler);
    char* actual_argv = argv;
    int total = argc;
    app = GC_MALLOC(sizeof(config));
    app->main = NULL;
    app->syslib = NULL;
    for (int i = 1; i < argc; i++) {
      switch(argv[i][0]) {
        case '-': {
          switch (argv[i][1]) {
            case 'h': fprintf(stdout, "%s", usage); exit(0);
            case 'd': app->debug = 1; break;
            case 's': {
              if (i + 1 == argc) 
                err("Option -s needs an argument");
              app->syslib = argv[++i];
              break;
            }
            case '-': argc = i + 1; break;
            default: 
            fprintf(stdout, "%s", usage);
            err("Unrecognised option %s", argv[i]);
          }
          break;
        }
        default: {
          if (!app->main) 
            app->main = argv[i];
          else 
            err("Unexpected value %s", argv[i]);
        }
      }
    }
    if (!app->main) {
      err("Main class name needed");
    }
    default_init(app);
    load_jar(app->syslib);
    class* sys = get_class("java/lang/System");
    field* out = get_field(sys, "out");
    out->stat_val.refer = new_inst(get_class("java/io/PrintStream"));
    field* in = get_field(sys, "in");
    in->stat_val.refer = new_inst(get_class("java/io/InputStream"));
    class* c = get_class(app->main);
    method* m = get_method(c, "main","([Ljava/lang/String;)V");
    stack = new_list();
    frame* f = new_frame(m, c->cp, c->this_class);
    prepare_args(f, argc, argv, total);
    exec(f);
    return 0;
}

void prepare_args(frame* f, int argc, char* argv[], int total) {
  array* ret = new_array(total - argc, 12);
  for (int i = argc, j = 0; i < total; i++, j++) {
    string* str = new_str(argv[i]);
    class* c = get_class("java/lang/String");
    method* init = get_method(c, "<init>", "([C)V");
    elem* self = GC_MALLOC(sizeof(elem));
    self->t = REF;
    self->data.ref = new_inst(c);
    elem* arg = GC_MALLOC(sizeof(elem));
    arg->t = ARRAY;
    arg->data.arr = new_array(str->len, 5);
    for (u2 i = 0; i < str->len; i++) {
      elem* el = get(arg->data.arr->data, i);
      el->data.integer = at(str, i);
    }
    frame* ctr = new_frame(init, c->cp, new_str("java/lang/String"));
    set(ctr->lvarray, 0, self);
    set(ctr->lvarray, 1, arg);
    exec(ctr);
    set(ret->data, j, self);
  }
  elem* e = GC_MALLOC(sizeof(elem));
  e->t = ARRAY;
  e->data.arr = ret;
  set(f->lvarray, 0, e);
}