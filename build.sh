#!/usr/bin/bash

set -e
jdk="/usr/bin"
meson compile -C build
if [[ "$0" == "full-build.sh" ]]; then
	$jdk/javac syslib/Make.java                                  
	$jdk/java syslib/Make syslib/VMObj.class
	for i in syslib/*.java; do
        javac $i                                           
        done
	rm syslib/Make.class
	jar cf syslib.jar syslib/*.class
fi
mv build/sun .
./sun -h
echo "Successfully built Sunrise!"


