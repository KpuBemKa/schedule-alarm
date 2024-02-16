# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/dev/tools/idf-5.1.2/v5.1.2/esp-idf/components/bootloader/subproject"
  "E:/dev/projects/school-alarm/build/bootloader"
  "E:/dev/projects/school-alarm/build/bootloader-prefix"
  "E:/dev/projects/school-alarm/build/bootloader-prefix/tmp"
  "E:/dev/projects/school-alarm/build/bootloader-prefix/src/bootloader-stamp"
  "E:/dev/projects/school-alarm/build/bootloader-prefix/src"
  "E:/dev/projects/school-alarm/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/dev/projects/school-alarm/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/dev/projects/school-alarm/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
