cmake_minimum_required(VERSION 2.8)
project(uhr)

# download and extract recognition core
set(UNIHAN2 unihan-wtpen_2.1.99-0.5.rays1.tar.gz)
file(DOWNLOAD http://rays.openrays.org/rays/pool/main/u/unihan-wtpen/${UNIHAN2} ${CMAKE_CURRENT_BINARY_DIR}/${UNIHAN2} SHOW_PROGRESS EXPECTED_MD5 b5155a565f52461f490791b9409a7363)

add_custom_command(OUTPUT RecoCore.rig WTPDA_GB2312.lib
		COMMAND tar xzf ${UNIHAN2} --strip=2 unihan-wtpen-2.1.99/data/RecoCore.rig unihan-wtpen-2.1.99/src/WTPDA_GB2312.lib)

add_custom_command(OUTPUT WTPDA_GB2312.o
		COMMAND objcopy -I binary -O elf32-i386 -B i386 WTPDA_GB2312.lib WTPDA_GB2312.o DEPENDS WTPDA_GB2312.lib)

#since cmake won't link a .rig
add_custom_command(OUTPUT RecoCore.o COMMAND mv RecoCore.rig RecoCore.o DEPENDS RecoCore.rig)

add_definitions(-std=c99 -Wall -Wextra -Werror)
add_executable(uhr uhr.c RecoCore.o WTPDA_GB2312.o)

