project('sunrise', ['c'],
  version : '0.1',
  default_options : ['warning_level=3'])
src = ['src/main.c', 'src/runtime/loader.c', 'src/runtime/frame.c', 'src/runtime/exec.c', 'src/runtime/ref.c', 'src/runtime/native.c', 'src/runtime/common.c']
thirdparty = ['third-party/argparse.c', 'third-party/zip.c']
libutilssrc = ['src/libutils/string.c', 'src/libutils/list.c', 'src/libutils/log.c']
libutils = static_library('libutils', libutilssrc)
libcfreadersrc = ['src/libcfreader/file.c', 'src/libcfreader/class.c', 'src/libcfreader/cpool.c', 'src/libcfreader/attrs.c']
libcfreader = static_library('libcfreader', libcfreadersrc)
c = meson.get_compiler('c')
gc = c.find_library('gc', required : true)
math = c.find_library('m', required : true)
dl = c.find_library('dl', required : true)
exe = executable('sun', [src, thirdparty], include_directories: 'include/', link_with: [libutils , libcfreader], dependencies: [gc, math, dl])
test('basic', exe)

