#
# Set location of various cgtools
#
# These variables can be set here or on the command line.  Paths must not
# have spaces.
#
# The CCS_ARMCOMPILER and GCC_ARMCOMPILER variables, in addition to pointing to
# their respective locations, also serve as "switches" for disabling a build
# using those cgtools. To disable a build using a specific cgtool, either set
# the cgtool's variable to empty or delete/comment-out its definition:
#     CCS_ARMCOMPILER ?=
# or
#     #CCS_ARMCOMPILER ?= home/username/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_18.1.0.LTS
#
# If a cgtool's *_ARMCOMPILER variable is set (non-empty), various sub-makes
# in the installation will attempt to build with that cgtool.  This means
# that if multiple *_ARMCOMPILER cgtool variables are set, the sub-makes
# will build using each non-empty *_ARMCOMPILER cgtool.
#

XDC_INSTALL_DIR        ?= /home/username/ti/xdctools_3_61_01_25_core

FREERTOS_INSTALL_DIR   ?= /home/username/FreeRTOSv10.2.1_191129

SIMPLELINK_MSP432_SDK_INSTALL_DIR  ?= /home/username/ti/simplelink_msp432p4_sdk_3_40_01_02
SIMPLELINK_MSP432E4_SDK_INSTALL_DIR  ?= /home/username/ti/simplelink_msp432e4_sdk_4_20_00_12
SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR  ?= /home/username/ti/simplelink_cc13x2_26x2_sdk_4_20_00_35

CCS_ARMCOMPILER        ?= /home/username/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS
GCC_ARMCOMPILER        ?= /home/username/ti/ccs1010/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major

# The IAR compiler is not supported on Linux
# IAR_ARMCOMPILER      ?=

# For Linux
RM     = rm -f
RMDIR  = rm -rf
DEVNULL = /dev/null
ECHOBLANKLINE = echo
