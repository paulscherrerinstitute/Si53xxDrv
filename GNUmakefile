ifneq ($(wildcard /ioc/tools/driver.makefile),)
PSIMAKE=YES

include /ioc/tools/driver.makefile
endif

include $(dir $(abspath $(firstword $(MAKEFILE_LIST))))/makefile

IocshDeclWrapper_VERSION=1.6

SOURCES+=IocshSup.cc
HEADERS+=Si53xx.h
HEADERS+=Si53xxIocshSup.h
HEADERS+=Si5395.h

MODULE=Si53xxDrv

EXCLUDE_VERSIONS=3.14 
BUILDCLASSES=Linux
ARCH_FILTER=yocto%
