project('sunrise', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])
src = ['src/main.c', 'src/runtime/loader.c', 'src/runtime/frame.c', 'src/runtime/exec.c']
libutilssrc = ['src/libutils/string.c', 'src/libutils/list.c', 'src/libutils/log.c']
libutils = static_library('libutils', libutilssrc)
libcfreadersrc = ['src/libcfreader/file.c', 'src/libcfreader/class.c', 'src/libcfreader/cpool.c', 'src/libcfreader/attrs.c']
libcfreader = static_library('libcfreader', libcfreadersrc)
exe = executable('sun', src, include_directories: 'include/', link_with: [libutils , libcfreader], link_args: '-lexecinfo')
test('basic', exe)
