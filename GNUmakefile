ifneq ($(wildcard /ioc/tools/driver.makefile),)
PSIMAKE=YES

include /ioc/tools/driver.makefile
endif

include $(dir $(abspath $(firstword $(MAKEFILE_LIST))))/makefile

IocshDeclWrapper_VERSION=straumann_t

SOURCES+=IocshSup.cc

MODULE=Si53xxDrv

EXCLUDE_VERSIONS=3.14 
BUILDCLASSES=Linux
ARCH_FILTER=yocto%

haha:
	echo $(MAKEFILE_LIST)
