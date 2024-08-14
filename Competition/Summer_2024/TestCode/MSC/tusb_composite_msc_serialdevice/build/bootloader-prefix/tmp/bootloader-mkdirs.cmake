# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/gratt/esp/v5.2.2/esp-idf/components/bootloader/subproject"
  "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader"
  "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader-prefix"
  "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader-prefix/tmp"
  "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader-prefix/src"
  "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/gratt/Documents/GitHub/CCI_UdeS/Competition/Summer_2024/TestCode/MSC/tusb_composite_msc_serialdevice/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
