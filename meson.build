project('sunrise', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])
src = ['src/main.c']
libutilssrc = ['src/libutils/string.c', 'src/libutils/list.c', 'src/libutils/log.c']
libutils = static_library('libutils', libutilssrc)
libcfreadersrc = ['src/libcfreader/file.c', 'src/libcfreader/class.c']
libcfreader = static_library('libcfreader', libcfreadersrc)
exe = executable('sun', src, include_directories: 'include/', link_with: [libutils , libcfreader])
test('basic', exe)
