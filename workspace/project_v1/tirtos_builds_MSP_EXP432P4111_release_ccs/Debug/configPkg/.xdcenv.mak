#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/source;/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages
override XDCROOT = /home/sam/ti/xdctools_3_60_02_34_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/source;/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;/home/sam/ti/xdctools_3_60_02_34_core/packages;..
HOSTOS = Linux
endif
