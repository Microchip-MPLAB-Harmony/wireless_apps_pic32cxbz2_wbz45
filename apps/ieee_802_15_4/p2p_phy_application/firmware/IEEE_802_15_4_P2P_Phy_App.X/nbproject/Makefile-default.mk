#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/config/default/IEEE_802154_PHY/phy/src/phy_ed_end_cb.c ../src/config/default/initialization.c ../src/config/default/system/console/src/sys_console.c ../src/config/default/app_p2p_phy/src/app_p2p_phy.c ../src/config/default/peripheral/evsys/plib_evsys.c ../src/config/default/interrupts.c ../src/third_party/rtos/FreeRTOS/Source/stream_buffer.c ../src/config/default/IEEE_802154_PHY/resources/buffer/src/bmm.c ../src/config/default/libc_syscalls.c ../src/config/default/peripheral/clk/plib_clk.c ../src/third_party/rtos/FreeRTOS/Source/queue.c ../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F/port.c ../src/third_party/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c ../src/config/default/peripheral/nvic/plib_nvic.c ../src/config/default/app_p2p_phy/app_phy_cmd_processor/src/app_phy_cmd_processor.c ../src/third_party/rtos/FreeRTOS/Source/FreeRTOS_tasks.c ../src/config/default/system/console/src/sys_console_uart.c ../src/config/default/IEEE_802154_PHY/pal/src/pal.c ../src/config/default/peripheral/tc/plib_tc0.c ../src/config/default/IEEE_802154_PHY/resources/queue/src/qmm.c ../src/config/default/system/cache/sys_cache.c ../src/third_party/rtos/FreeRTOS/Source/timers.c ../src/config/default/system/int/src/sys_int.c ../src/config/default/peripheral/cmcc/plib_cmcc.c ../src/config/default/system/reset/sys_reset.c ../src/third_party/rtos/FreeRTOS/Source/list.c ../src/main.c ../src/config/default/osal/osal_freertos_extend.c ../src/config/default/IEEE_802154_PHY/phy/src/phy_tx_frame_done_cb.c ../src/config/default/IEEE_802154_PHY/phy/src/phy_task.c ../src/config/default/IEEE_802154_PHY/phy/src/phy_rx_frame_cb.c ../src/app_idle_task.c ../src/app_user_edits.c ../src/config/default/freertos_hooks.c ../src/config/default/peripheral/gpio/plib_gpio.c ../src/config/default/tasks.c ../src/config/default/osal/osal_freertos.c ../src/third_party/rtos/FreeRTOS/Source/event_groups.c ../src/config/default/peripheral/nvm/plib_nvm.c ../src/app.c ../src/config/default/stdio/xc32_monitor.c ../src/config/default/peripheral/sercom/usart/plib_sercom0_usart.c ../src/config/default/startup_xc32.c ../src/config/default/system/time/src/sys_time.c ../src/third_party/rtos/FreeRTOS/Source/croutine.c ../src/config/default/system/command/src/sys_command.c ../src/config/default/exceptions.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o ${OBJECTDIR}/_ext/1171490990/initialization.o ${OBJECTDIR}/_ext/1832805299/sys_console.o ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o ${OBJECTDIR}/_ext/1986646378/plib_evsys.o ${OBJECTDIR}/_ext/1171490990/interrupts.o ${OBJECTDIR}/_ext/404212886/stream_buffer.o ${OBJECTDIR}/_ext/1682558732/bmm.o ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o ${OBJECTDIR}/_ext/60165520/plib_clk.o ${OBJECTDIR}/_ext/404212886/queue.o ${OBJECTDIR}/_ext/246609638/port.o ${OBJECTDIR}/_ext/1665200909/heap_4.o ${OBJECTDIR}/_ext/1865468468/plib_nvic.o ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o ${OBJECTDIR}/_ext/794496283/pal.o ${OBJECTDIR}/_ext/829342655/plib_tc0.o ${OBJECTDIR}/_ext/637565607/qmm.o ${OBJECTDIR}/_ext/1014039709/sys_cache.o ${OBJECTDIR}/_ext/404212886/timers.o ${OBJECTDIR}/_ext/1881668453/sys_int.o ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o ${OBJECTDIR}/_ext/1000052432/sys_reset.o ${OBJECTDIR}/_ext/404212886/list.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o ${OBJECTDIR}/_ext/1006906113/phy_task.o ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o ${OBJECTDIR}/_ext/1360937237/app_idle_task.o ${OBJECTDIR}/_ext/1360937237/app_user_edits.o ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o ${OBJECTDIR}/_ext/1865254177/plib_gpio.o ${OBJECTDIR}/_ext/1171490990/tasks.o ${OBJECTDIR}/_ext/1529399856/osal_freertos.o ${OBJECTDIR}/_ext/404212886/event_groups.o ${OBJECTDIR}/_ext/60176403/plib_nvm.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/163028504/xc32_monitor.o ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o ${OBJECTDIR}/_ext/1171490990/startup_xc32.o ${OBJECTDIR}/_ext/101884895/sys_time.o ${OBJECTDIR}/_ext/404212886/croutine.o ${OBJECTDIR}/_ext/1376093119/sys_command.o ${OBJECTDIR}/_ext/1171490990/exceptions.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o.d ${OBJECTDIR}/_ext/1171490990/initialization.o.d ${OBJECTDIR}/_ext/1832805299/sys_console.o.d ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o.d ${OBJECTDIR}/_ext/1986646378/plib_evsys.o.d ${OBJECTDIR}/_ext/1171490990/interrupts.o.d ${OBJECTDIR}/_ext/404212886/stream_buffer.o.d ${OBJECTDIR}/_ext/1682558732/bmm.o.d ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o.d ${OBJECTDIR}/_ext/60165520/plib_clk.o.d ${OBJECTDIR}/_ext/404212886/queue.o.d ${OBJECTDIR}/_ext/246609638/port.o.d ${OBJECTDIR}/_ext/1665200909/heap_4.o.d ${OBJECTDIR}/_ext/1865468468/plib_nvic.o.d ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o.d ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o.d ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o.d ${OBJECTDIR}/_ext/794496283/pal.o.d ${OBJECTDIR}/_ext/829342655/plib_tc0.o.d ${OBJECTDIR}/_ext/637565607/qmm.o.d ${OBJECTDIR}/_ext/1014039709/sys_cache.o.d ${OBJECTDIR}/_ext/404212886/timers.o.d ${OBJECTDIR}/_ext/1881668453/sys_int.o.d ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o.d ${OBJECTDIR}/_ext/1000052432/sys_reset.o.d ${OBJECTDIR}/_ext/404212886/list.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o.d ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o.d ${OBJECTDIR}/_ext/1006906113/phy_task.o.d ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o.d ${OBJECTDIR}/_ext/1360937237/app_idle_task.o.d ${OBJECTDIR}/_ext/1360937237/app_user_edits.o.d ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o.d ${OBJECTDIR}/_ext/1865254177/plib_gpio.o.d ${OBJECTDIR}/_ext/1171490990/tasks.o.d ${OBJECTDIR}/_ext/1529399856/osal_freertos.o.d ${OBJECTDIR}/_ext/404212886/event_groups.o.d ${OBJECTDIR}/_ext/60176403/plib_nvm.o.d ${OBJECTDIR}/_ext/1360937237/app.o.d ${OBJECTDIR}/_ext/163028504/xc32_monitor.o.d ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o.d ${OBJECTDIR}/_ext/1171490990/startup_xc32.o.d ${OBJECTDIR}/_ext/101884895/sys_time.o.d ${OBJECTDIR}/_ext/404212886/croutine.o.d ${OBJECTDIR}/_ext/1376093119/sys_command.o.d ${OBJECTDIR}/_ext/1171490990/exceptions.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o ${OBJECTDIR}/_ext/1171490990/initialization.o ${OBJECTDIR}/_ext/1832805299/sys_console.o ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o ${OBJECTDIR}/_ext/1986646378/plib_evsys.o ${OBJECTDIR}/_ext/1171490990/interrupts.o ${OBJECTDIR}/_ext/404212886/stream_buffer.o ${OBJECTDIR}/_ext/1682558732/bmm.o ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o ${OBJECTDIR}/_ext/60165520/plib_clk.o ${OBJECTDIR}/_ext/404212886/queue.o ${OBJECTDIR}/_ext/246609638/port.o ${OBJECTDIR}/_ext/1665200909/heap_4.o ${OBJECTDIR}/_ext/1865468468/plib_nvic.o ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o ${OBJECTDIR}/_ext/794496283/pal.o ${OBJECTDIR}/_ext/829342655/plib_tc0.o ${OBJECTDIR}/_ext/637565607/qmm.o ${OBJECTDIR}/_ext/1014039709/sys_cache.o ${OBJECTDIR}/_ext/404212886/timers.o ${OBJECTDIR}/_ext/1881668453/sys_int.o ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o ${OBJECTDIR}/_ext/1000052432/sys_reset.o ${OBJECTDIR}/_ext/404212886/list.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o ${OBJECTDIR}/_ext/1006906113/phy_task.o ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o ${OBJECTDIR}/_ext/1360937237/app_idle_task.o ${OBJECTDIR}/_ext/1360937237/app_user_edits.o ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o ${OBJECTDIR}/_ext/1865254177/plib_gpio.o ${OBJECTDIR}/_ext/1171490990/tasks.o ${OBJECTDIR}/_ext/1529399856/osal_freertos.o ${OBJECTDIR}/_ext/404212886/event_groups.o ${OBJECTDIR}/_ext/60176403/plib_nvm.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/163028504/xc32_monitor.o ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o ${OBJECTDIR}/_ext/1171490990/startup_xc32.o ${OBJECTDIR}/_ext/101884895/sys_time.o ${OBJECTDIR}/_ext/404212886/croutine.o ${OBJECTDIR}/_ext/1376093119/sys_command.o ${OBJECTDIR}/_ext/1171490990/exceptions.o

# Source Files
SOURCEFILES=../src/config/default/IEEE_802154_PHY/phy/src/phy_ed_end_cb.c ../src/config/default/initialization.c ../src/config/default/system/console/src/sys_console.c ../src/config/default/app_p2p_phy/src/app_p2p_phy.c ../src/config/default/peripheral/evsys/plib_evsys.c ../src/config/default/interrupts.c ../src/third_party/rtos/FreeRTOS/Source/stream_buffer.c ../src/config/default/IEEE_802154_PHY/resources/buffer/src/bmm.c ../src/config/default/libc_syscalls.c ../src/config/default/peripheral/clk/plib_clk.c ../src/third_party/rtos/FreeRTOS/Source/queue.c ../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F/port.c ../src/third_party/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c ../src/config/default/peripheral/nvic/plib_nvic.c ../src/config/default/app_p2p_phy/app_phy_cmd_processor/src/app_phy_cmd_processor.c ../src/third_party/rtos/FreeRTOS/Source/FreeRTOS_tasks.c ../src/config/default/system/console/src/sys_console_uart.c ../src/config/default/IEEE_802154_PHY/pal/src/pal.c ../src/config/default/peripheral/tc/plib_tc0.c ../src/config/default/IEEE_802154_PHY/resources/queue/src/qmm.c ../src/config/default/system/cache/sys_cache.c ../src/third_party/rtos/FreeRTOS/Source/timers.c ../src/config/default/system/int/src/sys_int.c ../src/config/default/peripheral/cmcc/plib_cmcc.c ../src/config/default/system/reset/sys_reset.c ../src/third_party/rtos/FreeRTOS/Source/list.c ../src/main.c ../src/config/default/osal/osal_freertos_extend.c ../src/config/default/IEEE_802154_PHY/phy/src/phy_tx_frame_done_cb.c ../src/config/default/IEEE_802154_PHY/phy/src/phy_task.c ../src/config/default/IEEE_802154_PHY/phy/src/phy_rx_frame_cb.c ../src/app_idle_task.c ../src/app_user_edits.c ../src/config/default/freertos_hooks.c ../src/config/default/peripheral/gpio/plib_gpio.c ../src/config/default/tasks.c ../src/config/default/osal/osal_freertos.c ../src/third_party/rtos/FreeRTOS/Source/event_groups.c ../src/config/default/peripheral/nvm/plib_nvm.c ../src/app.c ../src/config/default/stdio/xc32_monitor.c ../src/config/default/peripheral/sercom/usart/plib_sercom0_usart.c ../src/config/default/startup_xc32.c ../src/config/default/system/time/src/sys_time.c ../src/third_party/rtos/FreeRTOS/Source/croutine.c ../src/config/default/system/command/src/sys_command.c ../src/config/default/exceptions.c

# Pack Options 
PACK_COMMON_OPTIONS=-I "${CMSIS_DIR}/CMSIS/Core/Include"



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=WBZ451
MP_LINKER_FILE_OPTION=,--script="..\src\config\default\WBZ451.ld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_ed_end_cb.c  .generated_files/flags/default/9f8025eb84a4f090775f5ec9190c38ed85ce2a2b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_ed_end_cb.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/initialization.o: ../src/config/default/initialization.c  .generated_files/flags/default/9ffc870e1a1fe25d2b0e5354badfe269ada66c7e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/initialization.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/initialization.o.d" -o ${OBJECTDIR}/_ext/1171490990/initialization.o ../src/config/default/initialization.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1832805299/sys_console.o: ../src/config/default/system/console/src/sys_console.c  .generated_files/flags/default/e44996718e34e66a832a8780fe4f6580a4205764 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1832805299" 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1832805299/sys_console.o.d" -o ${OBJECTDIR}/_ext/1832805299/sys_console.o ../src/config/default/system/console/src/sys_console.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/297566330/app_p2p_phy.o: ../src/config/default/app_p2p_phy/src/app_p2p_phy.c  .generated_files/flags/default/7241b2b578067ae326b0b76e6c8766160d72e98b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/297566330" 
	@${RM} ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o.d 
	@${RM} ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/297566330/app_p2p_phy.o.d" -o ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o ../src/config/default/app_p2p_phy/src/app_p2p_phy.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1986646378/plib_evsys.o: ../src/config/default/peripheral/evsys/plib_evsys.c  .generated_files/flags/default/1fef0c73b07366aecc96033fc21e7299190f96a8 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1986646378" 
	@${RM} ${OBJECTDIR}/_ext/1986646378/plib_evsys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986646378/plib_evsys.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1986646378/plib_evsys.o.d" -o ${OBJECTDIR}/_ext/1986646378/plib_evsys.o ../src/config/default/peripheral/evsys/plib_evsys.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/interrupts.o: ../src/config/default/interrupts.c  .generated_files/flags/default/41154c3a3dbd3b80fcaf63f97ade184ce339bb38 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/interrupts.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/interrupts.o.d" -o ${OBJECTDIR}/_ext/1171490990/interrupts.o ../src/config/default/interrupts.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/stream_buffer.o: ../src/third_party/rtos/FreeRTOS/Source/stream_buffer.c  .generated_files/flags/default/c96cc030a54e6045759675a4d09c890323dec718 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/stream_buffer.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/stream_buffer.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/stream_buffer.o.d" -o ${OBJECTDIR}/_ext/404212886/stream_buffer.o ../src/third_party/rtos/FreeRTOS/Source/stream_buffer.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1682558732/bmm.o: ../src/config/default/IEEE_802154_PHY/resources/buffer/src/bmm.c  .generated_files/flags/default/dea622306dede66b96522478afa365785802702f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1682558732" 
	@${RM} ${OBJECTDIR}/_ext/1682558732/bmm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1682558732/bmm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1682558732/bmm.o.d" -o ${OBJECTDIR}/_ext/1682558732/bmm.o ../src/config/default/IEEE_802154_PHY/resources/buffer/src/bmm.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/libc_syscalls.o: ../src/config/default/libc_syscalls.c  .generated_files/flags/default/e5b7f8b48d65fa91733b9d092bf4e82c03ff07bf .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o ../src/config/default/libc_syscalls.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/60165520/plib_clk.o: ../src/config/default/peripheral/clk/plib_clk.c  .generated_files/flags/default/c5ee4a6c0eaf8250e88cf7a8a8d6623a9d040759 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/60165520" 
	@${RM} ${OBJECTDIR}/_ext/60165520/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/60165520/plib_clk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/60165520/plib_clk.o.d" -o ${OBJECTDIR}/_ext/60165520/plib_clk.o ../src/config/default/peripheral/clk/plib_clk.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/queue.o: ../src/third_party/rtos/FreeRTOS/Source/queue.c  .generated_files/flags/default/783385a960d043e98eb6311777f2a9dbc712118c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/queue.o.d" -o ${OBJECTDIR}/_ext/404212886/queue.o ../src/third_party/rtos/FreeRTOS/Source/queue.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/246609638/port.o: ../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F/port.c  .generated_files/flags/default/cdad45177544197263f5b9fb71394a5b6b973af3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/246609638" 
	@${RM} ${OBJECTDIR}/_ext/246609638/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/246609638/port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/246609638/port.o.d" -o ${OBJECTDIR}/_ext/246609638/port.o ../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F/port.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1665200909/heap_4.o: ../src/third_party/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c  .generated_files/flags/default/c2c4236a95e18d4892645cc5cdfc3ed427e1d5a7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1665200909" 
	@${RM} ${OBJECTDIR}/_ext/1665200909/heap_4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1665200909/heap_4.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1665200909/heap_4.o.d" -o ${OBJECTDIR}/_ext/1665200909/heap_4.o ../src/third_party/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1865468468/plib_nvic.o: ../src/config/default/peripheral/nvic/plib_nvic.c  .generated_files/flags/default/79f9d135d29ba7884cae091d85233b4783d6d4df .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1865468468" 
	@${RM} ${OBJECTDIR}/_ext/1865468468/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1865468468/plib_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1865468468/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1865468468/plib_nvic.o ../src/config/default/peripheral/nvic/plib_nvic.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o: ../src/config/default/app_p2p_phy/app_phy_cmd_processor/src/app_phy_cmd_processor.c  .generated_files/flags/default/482f5f92952cd3ff77b8f4f24f7d7f8f34097322 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/767817240" 
	@${RM} ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o.d 
	@${RM} ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o.d" -o ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o ../src/config/default/app_p2p_phy/app_phy_cmd_processor/src/app_phy_cmd_processor.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o: ../src/third_party/rtos/FreeRTOS/Source/FreeRTOS_tasks.c  .generated_files/flags/default/203dae9080ae83d0397f69af6e04e17a09340c3e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o.d" -o ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o ../src/third_party/rtos/FreeRTOS/Source/FreeRTOS_tasks.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1832805299/sys_console_uart.o: ../src/config/default/system/console/src/sys_console_uart.c  .generated_files/flags/default/92e69183c94af406f648bb42a789e84e7dab8cc7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1832805299" 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1832805299/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o ../src/config/default/system/console/src/sys_console_uart.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/794496283/pal.o: ../src/config/default/IEEE_802154_PHY/pal/src/pal.c  .generated_files/flags/default/3259d8e9358788412344622f2836bb0ec4442f3c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/794496283" 
	@${RM} ${OBJECTDIR}/_ext/794496283/pal.o.d 
	@${RM} ${OBJECTDIR}/_ext/794496283/pal.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/794496283/pal.o.d" -o ${OBJECTDIR}/_ext/794496283/pal.o ../src/config/default/IEEE_802154_PHY/pal/src/pal.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/829342655/plib_tc0.o: ../src/config/default/peripheral/tc/plib_tc0.c  .generated_files/flags/default/b4c29ac85d6e6d6dfb470bdad0aeef88a729aabd .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/829342655" 
	@${RM} ${OBJECTDIR}/_ext/829342655/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/829342655/plib_tc0.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/829342655/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/829342655/plib_tc0.o ../src/config/default/peripheral/tc/plib_tc0.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/637565607/qmm.o: ../src/config/default/IEEE_802154_PHY/resources/queue/src/qmm.c  .generated_files/flags/default/f96468824ac527f8b1c5c4370059c963720b5c52 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/637565607" 
	@${RM} ${OBJECTDIR}/_ext/637565607/qmm.o.d 
	@${RM} ${OBJECTDIR}/_ext/637565607/qmm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/637565607/qmm.o.d" -o ${OBJECTDIR}/_ext/637565607/qmm.o ../src/config/default/IEEE_802154_PHY/resources/queue/src/qmm.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1014039709/sys_cache.o: ../src/config/default/system/cache/sys_cache.c  .generated_files/flags/default/8d18e020feca96ccea0d1551b036cb37a616f6b2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1014039709" 
	@${RM} ${OBJECTDIR}/_ext/1014039709/sys_cache.o.d 
	@${RM} ${OBJECTDIR}/_ext/1014039709/sys_cache.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1014039709/sys_cache.o.d" -o ${OBJECTDIR}/_ext/1014039709/sys_cache.o ../src/config/default/system/cache/sys_cache.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/timers.o: ../src/third_party/rtos/FreeRTOS/Source/timers.c  .generated_files/flags/default/1d701df2970f28e09c38ab26e96ec49b28bb246a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/timers.o.d" -o ${OBJECTDIR}/_ext/404212886/timers.o ../src/third_party/rtos/FreeRTOS/Source/timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1881668453/sys_int.o: ../src/config/default/system/int/src/sys_int.c  .generated_files/flags/default/6330830aaa73b9f91dd809586735813eff7e53d9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1881668453" 
	@${RM} ${OBJECTDIR}/_ext/1881668453/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/1881668453/sys_int.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1881668453/sys_int.o.d" -o ${OBJECTDIR}/_ext/1881668453/sys_int.o ../src/config/default/system/int/src/sys_int.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1865131932/plib_cmcc.o: ../src/config/default/peripheral/cmcc/plib_cmcc.c  .generated_files/flags/default/f47128d36dce72b20746a5d04028d1de48676925 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1865131932" 
	@${RM} ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1865131932/plib_cmcc.o.d" -o ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o ../src/config/default/peripheral/cmcc/plib_cmcc.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1000052432/sys_reset.o: ../src/config/default/system/reset/sys_reset.c  .generated_files/flags/default/49deb2a5decf214e5dea7da54906d1b32613ff9b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1000052432" 
	@${RM} ${OBJECTDIR}/_ext/1000052432/sys_reset.o.d 
	@${RM} ${OBJECTDIR}/_ext/1000052432/sys_reset.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1000052432/sys_reset.o.d" -o ${OBJECTDIR}/_ext/1000052432/sys_reset.o ../src/config/default/system/reset/sys_reset.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/list.o: ../src/third_party/rtos/FreeRTOS/Source/list.c  .generated_files/flags/default/872c070fa03d46ce8e8d3857833b1744d7872002 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/list.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/list.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/list.o.d" -o ${OBJECTDIR}/_ext/404212886/list.o ../src/third_party/rtos/FreeRTOS/Source/list.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/default/73d71bad24fce9ba417fa5cab5ecd6f1e07c2fb3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o: ../src/config/default/osal/osal_freertos_extend.c  .generated_files/flags/default/3eac1a67b26aa785c2f8c910770a64a279988f84 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1529399856" 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o.d 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o.d" -o ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o ../src/config/default/osal/osal_freertos_extend.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_tx_frame_done_cb.c  .generated_files/flags/default/701f5407103692003e0cc985979cf555bbf51452 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_tx_frame_done_cb.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1006906113/phy_task.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_task.c  .generated_files/flags/default/3193d8519a039ed532d56f4cfbf6ed8bc1563c53 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_task.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_task.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_task.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_task.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_task.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_rx_frame_cb.c  .generated_files/flags/default/c3e4bece1bec25aa28f05aa0145daa4b3a3b49ef .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_rx_frame_cb.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app_idle_task.o: ../src/app_idle_task.c  .generated_files/flags/default/45e45dbb90ae284022daccfdd255d7ab65082fe6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_idle_task.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_idle_task.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app_idle_task.o.d" -o ${OBJECTDIR}/_ext/1360937237/app_idle_task.o ../src/app_idle_task.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app_user_edits.o: ../src/app_user_edits.c  .generated_files/flags/default/31803052f7c932e8ebba35b8203ac3b344f42a5c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_user_edits.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_user_edits.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app_user_edits.o.d" -o ${OBJECTDIR}/_ext/1360937237/app_user_edits.o ../src/app_user_edits.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/freertos_hooks.o: ../src/config/default/freertos_hooks.c  .generated_files/flags/default/c1e7dbe46074c77fc7dc5e7a1292dd997a09d075 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/freertos_hooks.o.d" -o ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o ../src/config/default/freertos_hooks.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1865254177/plib_gpio.o: ../src/config/default/peripheral/gpio/plib_gpio.c  .generated_files/flags/default/c338ed91b4774fea9e4ba0ca058385ade82b757b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1865254177" 
	@${RM} ${OBJECTDIR}/_ext/1865254177/plib_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1865254177/plib_gpio.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1865254177/plib_gpio.o.d" -o ${OBJECTDIR}/_ext/1865254177/plib_gpio.o ../src/config/default/peripheral/gpio/plib_gpio.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/tasks.o: ../src/config/default/tasks.c  .generated_files/flags/default/d8063bc0b38cf43d7e9c3d7d1dab5283a7c91eaf .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/tasks.o.d" -o ${OBJECTDIR}/_ext/1171490990/tasks.o ../src/config/default/tasks.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1529399856/osal_freertos.o: ../src/config/default/osal/osal_freertos.c  .generated_files/flags/default/d0df8f8119f2ec0aabf056452602b6a09b27a1cf .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1529399856" 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1529399856/osal_freertos.o.d" -o ${OBJECTDIR}/_ext/1529399856/osal_freertos.o ../src/config/default/osal/osal_freertos.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/event_groups.o: ../src/third_party/rtos/FreeRTOS/Source/event_groups.c  .generated_files/flags/default/83c0b87a9dc1711654484da24c1681f70283d0ef .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/event_groups.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/event_groups.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/event_groups.o.d" -o ${OBJECTDIR}/_ext/404212886/event_groups.o ../src/third_party/rtos/FreeRTOS/Source/event_groups.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/60176403/plib_nvm.o: ../src/config/default/peripheral/nvm/plib_nvm.c  .generated_files/flags/default/e6422ab3797deb751ff88efd415d545426055575 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/60176403" 
	@${RM} ${OBJECTDIR}/_ext/60176403/plib_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/60176403/plib_nvm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/60176403/plib_nvm.o.d" -o ${OBJECTDIR}/_ext/60176403/plib_nvm.o ../src/config/default/peripheral/nvm/plib_nvm.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  .generated_files/flags/default/ed6d21d93bd64a160a2853a4fe865cb0bc3a540d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/163028504/xc32_monitor.o: ../src/config/default/stdio/xc32_monitor.c  .generated_files/flags/default/cc50ab3c302cd7c79d859de1618533e5379e1de2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/163028504" 
	@${RM} ${OBJECTDIR}/_ext/163028504/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/163028504/xc32_monitor.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/163028504/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/163028504/xc32_monitor.o ../src/config/default/stdio/xc32_monitor.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o: ../src/config/default/peripheral/sercom/usart/plib_sercom0_usart.c  .generated_files/flags/default/6e82cb671b25c0717bd86f1443d31dd241a5a129 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/504274921" 
	@${RM} ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o.d 
	@${RM} ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o.d" -o ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o ../src/config/default/peripheral/sercom/usart/plib_sercom0_usart.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/startup_xc32.o: ../src/config/default/startup_xc32.c  .generated_files/flags/default/31025624b7e1403bfb0539621a04fb3a3b281a93 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/startup_xc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/startup_xc32.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/startup_xc32.o.d" -o ${OBJECTDIR}/_ext/1171490990/startup_xc32.o ../src/config/default/startup_xc32.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/101884895/sys_time.o: ../src/config/default/system/time/src/sys_time.c  .generated_files/flags/default/3a713c6cf1b0fc10c3ebb0c12121a0d9b637ed71 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/101884895" 
	@${RM} ${OBJECTDIR}/_ext/101884895/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/101884895/sys_time.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/101884895/sys_time.o.d" -o ${OBJECTDIR}/_ext/101884895/sys_time.o ../src/config/default/system/time/src/sys_time.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/croutine.o: ../src/third_party/rtos/FreeRTOS/Source/croutine.c  .generated_files/flags/default/c6f2a0d766adc8c01b7a75b92b9d0d5907556ad9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/croutine.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/croutine.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/croutine.o.d" -o ${OBJECTDIR}/_ext/404212886/croutine.o ../src/third_party/rtos/FreeRTOS/Source/croutine.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1376093119/sys_command.o: ../src/config/default/system/command/src/sys_command.c  .generated_files/flags/default/28df6e15d6e0d9a0fc4f4c41f2629c7f8d1c0156 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1376093119" 
	@${RM} ${OBJECTDIR}/_ext/1376093119/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/1376093119/sys_command.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1376093119/sys_command.o.d" -o ${OBJECTDIR}/_ext/1376093119/sys_command.o ../src/config/default/system/command/src/sys_command.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/exceptions.o: ../src/config/default/exceptions.c  .generated_files/flags/default/a9b2fa8cfe3e91a6d93cbf29a8f6d5d2d17c70fe .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/exceptions.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/exceptions.o.d" -o ${OBJECTDIR}/_ext/1171490990/exceptions.o ../src/config/default/exceptions.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
else
${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_ed_end_cb.c  .generated_files/flags/default/bfe93ec27998f315ab2e986e22590db018ff902d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_ed_end_cb.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_ed_end_cb.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/initialization.o: ../src/config/default/initialization.c  .generated_files/flags/default/d78cacff5cf6f4ca229071969900c534cc9fe5cf .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/initialization.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/initialization.o.d" -o ${OBJECTDIR}/_ext/1171490990/initialization.o ../src/config/default/initialization.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1832805299/sys_console.o: ../src/config/default/system/console/src/sys_console.c  .generated_files/flags/default/ec352ab442609726b2901e14ddb1b42cf09e36 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1832805299" 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1832805299/sys_console.o.d" -o ${OBJECTDIR}/_ext/1832805299/sys_console.o ../src/config/default/system/console/src/sys_console.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/297566330/app_p2p_phy.o: ../src/config/default/app_p2p_phy/src/app_p2p_phy.c  .generated_files/flags/default/b038d95c3f0e529bfda792c1587cd9516b7753be .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/297566330" 
	@${RM} ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o.d 
	@${RM} ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/297566330/app_p2p_phy.o.d" -o ${OBJECTDIR}/_ext/297566330/app_p2p_phy.o ../src/config/default/app_p2p_phy/src/app_p2p_phy.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1986646378/plib_evsys.o: ../src/config/default/peripheral/evsys/plib_evsys.c  .generated_files/flags/default/e3b2cafed6857cc14b673bb9a5e582e3c5f9e47a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1986646378" 
	@${RM} ${OBJECTDIR}/_ext/1986646378/plib_evsys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986646378/plib_evsys.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1986646378/plib_evsys.o.d" -o ${OBJECTDIR}/_ext/1986646378/plib_evsys.o ../src/config/default/peripheral/evsys/plib_evsys.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/interrupts.o: ../src/config/default/interrupts.c  .generated_files/flags/default/a10212c9fee9cb31d19628d2f1b0a80f651bf82a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/interrupts.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/interrupts.o.d" -o ${OBJECTDIR}/_ext/1171490990/interrupts.o ../src/config/default/interrupts.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/stream_buffer.o: ../src/third_party/rtos/FreeRTOS/Source/stream_buffer.c  .generated_files/flags/default/659ac8f174a7ea23405f3a1b7d765796969a0753 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/stream_buffer.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/stream_buffer.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/stream_buffer.o.d" -o ${OBJECTDIR}/_ext/404212886/stream_buffer.o ../src/third_party/rtos/FreeRTOS/Source/stream_buffer.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1682558732/bmm.o: ../src/config/default/IEEE_802154_PHY/resources/buffer/src/bmm.c  .generated_files/flags/default/fc9359caa1768abb64a406338f4ad908d9754644 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1682558732" 
	@${RM} ${OBJECTDIR}/_ext/1682558732/bmm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1682558732/bmm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1682558732/bmm.o.d" -o ${OBJECTDIR}/_ext/1682558732/bmm.o ../src/config/default/IEEE_802154_PHY/resources/buffer/src/bmm.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/libc_syscalls.o: ../src/config/default/libc_syscalls.c  .generated_files/flags/default/34f221b775c6234fafa26f9abd5eb6a9264459ac .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1171490990/libc_syscalls.o ../src/config/default/libc_syscalls.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/60165520/plib_clk.o: ../src/config/default/peripheral/clk/plib_clk.c  .generated_files/flags/default/226ae794da00970f514f53d9758aeda418999eb .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/60165520" 
	@${RM} ${OBJECTDIR}/_ext/60165520/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/60165520/plib_clk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/60165520/plib_clk.o.d" -o ${OBJECTDIR}/_ext/60165520/plib_clk.o ../src/config/default/peripheral/clk/plib_clk.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/queue.o: ../src/third_party/rtos/FreeRTOS/Source/queue.c  .generated_files/flags/default/3cbfe3b3387f5c2f0e52ff7c91a36af844909515 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/queue.o.d" -o ${OBJECTDIR}/_ext/404212886/queue.o ../src/third_party/rtos/FreeRTOS/Source/queue.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/246609638/port.o: ../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F/port.c  .generated_files/flags/default/985502386d3810aaf8d9728ce86f1cbd818164f1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/246609638" 
	@${RM} ${OBJECTDIR}/_ext/246609638/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/246609638/port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/246609638/port.o.d" -o ${OBJECTDIR}/_ext/246609638/port.o ../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F/port.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1665200909/heap_4.o: ../src/third_party/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c  .generated_files/flags/default/56b11cc5e754a8d2b63bf49bbc0f40ddd5a2b4f3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1665200909" 
	@${RM} ${OBJECTDIR}/_ext/1665200909/heap_4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1665200909/heap_4.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1665200909/heap_4.o.d" -o ${OBJECTDIR}/_ext/1665200909/heap_4.o ../src/third_party/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1865468468/plib_nvic.o: ../src/config/default/peripheral/nvic/plib_nvic.c  .generated_files/flags/default/f5f91f9f09800d0b0ef857c4c36f32f72a8ddb34 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1865468468" 
	@${RM} ${OBJECTDIR}/_ext/1865468468/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1865468468/plib_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1865468468/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1865468468/plib_nvic.o ../src/config/default/peripheral/nvic/plib_nvic.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o: ../src/config/default/app_p2p_phy/app_phy_cmd_processor/src/app_phy_cmd_processor.c  .generated_files/flags/default/b0664b68ee7babc0e3aed6568beb70ebaa1600db .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/767817240" 
	@${RM} ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o.d 
	@${RM} ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o.d" -o ${OBJECTDIR}/_ext/767817240/app_phy_cmd_processor.o ../src/config/default/app_p2p_phy/app_phy_cmd_processor/src/app_phy_cmd_processor.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o: ../src/third_party/rtos/FreeRTOS/Source/FreeRTOS_tasks.c  .generated_files/flags/default/cdac242da155d9bae40d4eb6796e9890600d7c0e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o.d" -o ${OBJECTDIR}/_ext/404212886/FreeRTOS_tasks.o ../src/third_party/rtos/FreeRTOS/Source/FreeRTOS_tasks.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1832805299/sys_console_uart.o: ../src/config/default/system/console/src/sys_console_uart.c  .generated_files/flags/default/de4badcc2d74da33fe32814c5723099e99429835 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1832805299" 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1832805299/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/1832805299/sys_console_uart.o ../src/config/default/system/console/src/sys_console_uart.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/794496283/pal.o: ../src/config/default/IEEE_802154_PHY/pal/src/pal.c  .generated_files/flags/default/e043c9cba33b912dd68af10abbcdee163cdf04e6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/794496283" 
	@${RM} ${OBJECTDIR}/_ext/794496283/pal.o.d 
	@${RM} ${OBJECTDIR}/_ext/794496283/pal.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/794496283/pal.o.d" -o ${OBJECTDIR}/_ext/794496283/pal.o ../src/config/default/IEEE_802154_PHY/pal/src/pal.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/829342655/plib_tc0.o: ../src/config/default/peripheral/tc/plib_tc0.c  .generated_files/flags/default/3f5c5dc6b84a2a016417d64015063333b17e797a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/829342655" 
	@${RM} ${OBJECTDIR}/_ext/829342655/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/829342655/plib_tc0.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/829342655/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/829342655/plib_tc0.o ../src/config/default/peripheral/tc/plib_tc0.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/637565607/qmm.o: ../src/config/default/IEEE_802154_PHY/resources/queue/src/qmm.c  .generated_files/flags/default/3fdd031e8b3993c001fab6359475711822279c0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/637565607" 
	@${RM} ${OBJECTDIR}/_ext/637565607/qmm.o.d 
	@${RM} ${OBJECTDIR}/_ext/637565607/qmm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/637565607/qmm.o.d" -o ${OBJECTDIR}/_ext/637565607/qmm.o ../src/config/default/IEEE_802154_PHY/resources/queue/src/qmm.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1014039709/sys_cache.o: ../src/config/default/system/cache/sys_cache.c  .generated_files/flags/default/ee700513b03dd8bfc1b7575026571f96d3ce7743 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1014039709" 
	@${RM} ${OBJECTDIR}/_ext/1014039709/sys_cache.o.d 
	@${RM} ${OBJECTDIR}/_ext/1014039709/sys_cache.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1014039709/sys_cache.o.d" -o ${OBJECTDIR}/_ext/1014039709/sys_cache.o ../src/config/default/system/cache/sys_cache.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/timers.o: ../src/third_party/rtos/FreeRTOS/Source/timers.c  .generated_files/flags/default/918e69e36a011abc1fa719c844f9d773b7dbc4a9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/timers.o.d" -o ${OBJECTDIR}/_ext/404212886/timers.o ../src/third_party/rtos/FreeRTOS/Source/timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1881668453/sys_int.o: ../src/config/default/system/int/src/sys_int.c  .generated_files/flags/default/aea189044bb41e20d7fc21b4775e642d3ff12ad0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1881668453" 
	@${RM} ${OBJECTDIR}/_ext/1881668453/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/1881668453/sys_int.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1881668453/sys_int.o.d" -o ${OBJECTDIR}/_ext/1881668453/sys_int.o ../src/config/default/system/int/src/sys_int.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1865131932/plib_cmcc.o: ../src/config/default/peripheral/cmcc/plib_cmcc.c  .generated_files/flags/default/85d6e69cb9f828cbe125105cf787a7840299f8e6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1865131932" 
	@${RM} ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1865131932/plib_cmcc.o.d" -o ${OBJECTDIR}/_ext/1865131932/plib_cmcc.o ../src/config/default/peripheral/cmcc/plib_cmcc.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1000052432/sys_reset.o: ../src/config/default/system/reset/sys_reset.c  .generated_files/flags/default/a630dc1dccacb1134c96225cc0497f942f90187c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1000052432" 
	@${RM} ${OBJECTDIR}/_ext/1000052432/sys_reset.o.d 
	@${RM} ${OBJECTDIR}/_ext/1000052432/sys_reset.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1000052432/sys_reset.o.d" -o ${OBJECTDIR}/_ext/1000052432/sys_reset.o ../src/config/default/system/reset/sys_reset.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/list.o: ../src/third_party/rtos/FreeRTOS/Source/list.c  .generated_files/flags/default/d3e081007d29038b4ce1b0fd71504daf6a0c8afb .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/list.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/list.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/list.o.d" -o ${OBJECTDIR}/_ext/404212886/list.o ../src/third_party/rtos/FreeRTOS/Source/list.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/default/b8d9944a3cdc5684943a46aec0e316dadeaa1814 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o: ../src/config/default/osal/osal_freertos_extend.c  .generated_files/flags/default/d215485cccc6e15a8b913025578905c4b93ac4b1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1529399856" 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o.d 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o.d" -o ${OBJECTDIR}/_ext/1529399856/osal_freertos_extend.o ../src/config/default/osal/osal_freertos_extend.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_tx_frame_done_cb.c  .generated_files/flags/default/e47dfb8485fb40bac17bd421e9b2990946049b05 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_tx_frame_done_cb.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_tx_frame_done_cb.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1006906113/phy_task.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_task.c  .generated_files/flags/default/ed7a2da403f9ad1b48e50bae99a93dff2803b60a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_task.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_task.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_task.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_task.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_task.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o: ../src/config/default/IEEE_802154_PHY/phy/src/phy_rx_frame_cb.c  .generated_files/flags/default/73456f5de515f349fba89e8a40d03ae05e947acc .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1006906113" 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o.d" -o ${OBJECTDIR}/_ext/1006906113/phy_rx_frame_cb.o ../src/config/default/IEEE_802154_PHY/phy/src/phy_rx_frame_cb.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app_idle_task.o: ../src/app_idle_task.c  .generated_files/flags/default/fab52ac419662f284567b78a16467e05fc878f6f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_idle_task.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_idle_task.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app_idle_task.o.d" -o ${OBJECTDIR}/_ext/1360937237/app_idle_task.o ../src/app_idle_task.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app_user_edits.o: ../src/app_user_edits.c  .generated_files/flags/default/95d47c46684adc2a6cd7ab85353799bb907bb4bb .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_user_edits.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_user_edits.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app_user_edits.o.d" -o ${OBJECTDIR}/_ext/1360937237/app_user_edits.o ../src/app_user_edits.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/freertos_hooks.o: ../src/config/default/freertos_hooks.c  .generated_files/flags/default/8f415767cdfb3e94604871be3fb222e174dfc085 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/freertos_hooks.o.d" -o ${OBJECTDIR}/_ext/1171490990/freertos_hooks.o ../src/config/default/freertos_hooks.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1865254177/plib_gpio.o: ../src/config/default/peripheral/gpio/plib_gpio.c  .generated_files/flags/default/23647399d4bb202f4d026bf82643537f69592276 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1865254177" 
	@${RM} ${OBJECTDIR}/_ext/1865254177/plib_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1865254177/plib_gpio.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1865254177/plib_gpio.o.d" -o ${OBJECTDIR}/_ext/1865254177/plib_gpio.o ../src/config/default/peripheral/gpio/plib_gpio.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/tasks.o: ../src/config/default/tasks.c  .generated_files/flags/default/222e53579cbbd73f3fe012b0a261f447d8a4ed96 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/tasks.o.d" -o ${OBJECTDIR}/_ext/1171490990/tasks.o ../src/config/default/tasks.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1529399856/osal_freertos.o: ../src/config/default/osal/osal_freertos.c  .generated_files/flags/default/c6fd2746e49d9339b44210a0a7632632b62f1f13 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1529399856" 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1529399856/osal_freertos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1529399856/osal_freertos.o.d" -o ${OBJECTDIR}/_ext/1529399856/osal_freertos.o ../src/config/default/osal/osal_freertos.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/event_groups.o: ../src/third_party/rtos/FreeRTOS/Source/event_groups.c  .generated_files/flags/default/2b5d571d0b370666191cff315167a3df7b7eae71 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/event_groups.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/event_groups.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/event_groups.o.d" -o ${OBJECTDIR}/_ext/404212886/event_groups.o ../src/third_party/rtos/FreeRTOS/Source/event_groups.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/60176403/plib_nvm.o: ../src/config/default/peripheral/nvm/plib_nvm.c  .generated_files/flags/default/76c2072746abe7ff7fda126b159a62e793543d68 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/60176403" 
	@${RM} ${OBJECTDIR}/_ext/60176403/plib_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/60176403/plib_nvm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/60176403/plib_nvm.o.d" -o ${OBJECTDIR}/_ext/60176403/plib_nvm.o ../src/config/default/peripheral/nvm/plib_nvm.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  .generated_files/flags/default/b7e26f0dd3e3c3e279acdb83063a50c462682593 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/163028504/xc32_monitor.o: ../src/config/default/stdio/xc32_monitor.c  .generated_files/flags/default/bf805317fb47405adf45cb595e058d42394b65ed .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/163028504" 
	@${RM} ${OBJECTDIR}/_ext/163028504/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/163028504/xc32_monitor.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/163028504/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/163028504/xc32_monitor.o ../src/config/default/stdio/xc32_monitor.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o: ../src/config/default/peripheral/sercom/usart/plib_sercom0_usart.c  .generated_files/flags/default/7110cf0c70dd70997c5d3266848bb7d57aaf86f9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/504274921" 
	@${RM} ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o.d 
	@${RM} ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o.d" -o ${OBJECTDIR}/_ext/504274921/plib_sercom0_usart.o ../src/config/default/peripheral/sercom/usart/plib_sercom0_usart.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/startup_xc32.o: ../src/config/default/startup_xc32.c  .generated_files/flags/default/28309938c54e46168532022429907b4ce3ad73c6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/startup_xc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/startup_xc32.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/startup_xc32.o.d" -o ${OBJECTDIR}/_ext/1171490990/startup_xc32.o ../src/config/default/startup_xc32.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/101884895/sys_time.o: ../src/config/default/system/time/src/sys_time.c  .generated_files/flags/default/571c3ac908c904cab1848ba2168b9f9238ae4f27 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/101884895" 
	@${RM} ${OBJECTDIR}/_ext/101884895/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/101884895/sys_time.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/101884895/sys_time.o.d" -o ${OBJECTDIR}/_ext/101884895/sys_time.o ../src/config/default/system/time/src/sys_time.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/404212886/croutine.o: ../src/third_party/rtos/FreeRTOS/Source/croutine.c  .generated_files/flags/default/f154d109af8d48f2cd891a77b0afdf4342f1bee2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/404212886" 
	@${RM} ${OBJECTDIR}/_ext/404212886/croutine.o.d 
	@${RM} ${OBJECTDIR}/_ext/404212886/croutine.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/404212886/croutine.o.d" -o ${OBJECTDIR}/_ext/404212886/croutine.o ../src/third_party/rtos/FreeRTOS/Source/croutine.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1376093119/sys_command.o: ../src/config/default/system/command/src/sys_command.c  .generated_files/flags/default/c6a00749cb8b17924169bbedc3b4767a7fbc7869 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1376093119" 
	@${RM} ${OBJECTDIR}/_ext/1376093119/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/1376093119/sys_command.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1376093119/sys_command.o.d" -o ${OBJECTDIR}/_ext/1376093119/sys_command.o ../src/config/default/system/command/src/sys_command.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1171490990/exceptions.o: ../src/config/default/exceptions.c  .generated_files/flags/default/543909997ea030afd120c1fc4f5e0f075c4f8c32 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1171490990" 
	@${RM} ${OBJECTDIR}/_ext/1171490990/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1171490990/exceptions.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O2 -fcommon -DENABLE_LARGE_BUFFER -DENABLE_QUEUE_CAPACITY -I"../src" -I"../src/config/default" -I"../src/config/default/IEEE_802154_PHY/pal/inc" -I"../src/config/default/IEEE_802154_PHY/phy/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/buffer/inc/" -I"../src/config/default/IEEE_802154_PHY/resources/queue/inc/" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/WBZ451_DFP" -I"../src/third_party/rtos/FreeRTOS/Source/include" -I"../src/third_party/rtos/FreeRTOS/Source/portable/GCC/SAM/ARM_CM4F" -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1171490990/exceptions.o.d" -o ${OBJECTDIR}/_ext/1171490990/exceptions.o ../src/config/default/exceptions.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/WBZ451" ${PACK_COMMON_OPTIONS} 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../src/config/default/lib/lib-ieee802154_phy_pic32cxbz-v1.0.0.a ../src/config/default/driver/device_support/pic32cx_bz2_device_support.a ../src/config/default/driver/pds/pds.a  ../src/config/default/WBZ451.ld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o ${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\src\config\default\lib\lib-ieee802154_phy_pic32cxbz-v1.0.0.a ..\src\config\default\driver\device_support\pic32cx_bz2_device_support.a ..\src\config\default\driver\pds\pds.a      -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=1024,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}/WBZ451"
	
else
${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../src/config/default/lib/lib-ieee802154_phy_pic32cxbz-v1.0.0.a ../src/config/default/driver/device_support/pic32cx_bz2_device_support.a ../src/config/default/driver/pds/pds.a ../src/config/default/WBZ451.ld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o ${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\src\config\default\lib\lib-ieee802154_phy_pic32cxbz-v1.0.0.a ..\src\config\default\driver\device_support\pic32cx_bz2_device_support.a ..\src\config\default\driver\pds\pds.a      -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=1024,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}/WBZ451"
	${MP_CC_DIR}\\xc32-bin2hex ${DISTDIR}/IEEE_802_15_4_P2P_Phy_App.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
