goto :SPLINT
REM ========================== PC-Lint ===============================
REM PC-Lint related actions here..
:PCLINT
REM Create file list for PC-Lint to check
dir Library\Device\*.c /b /s > list.txt
dir Library\Device\*.h /b /s >> list.txt
dir Library\StdDriver\*.c /b /s >> list.txt
dir Library\StdDriver\*.h /b /s >> list.txt
dir SampleCode\*.c /b /s >> list.txt
dir SampleCode\*.h /b /s >> list.txt

REM ============================ splint ==========================
REM Parameter for splint
:SPLINT
if exist lint.log del lint.log

set SPLINT_PATH="c:\splint-3.1.2\bin\splint.exe"
REM Don't known why can't I pass following variables as parameter to splint.
REM set NUC400_INC="-IC:\work_zone\nano112\Nano1x2BSP\Library\StdDriver\inc  -IC:\work_zone\nano112\Nano1x2BSP\Library\Device\Nuvoton\nano112Series\Include"
REM set KEIL_INC="-IC:\Keil\ARM\RV31\INC"
REM set CMSIS_INC="-IC:\work_zone\nano112\Nano1x2BSP\Library\CMSIS\Include"
REM set PARAM="-weak -unrecogcomments -fcnuse +trytorecover"

for /f "usebackq" %%d in (`"dir Library\Device\*.c /b/s"`) do (
    %SPLINT_PATH% -IC:\work_zone\nano112\Nano1x2BSP\Library\StdDriver\inc -IC:\work_zone\nano112\Nano1x2BSP\Library\Device\Nuvoton\Nano103\Include -IC:\Keil\ARM\RV31\INC -IC:\work_zone\nano112\Nano1x2BSP\Library\CMSIS\Include -weak -unrecogcomments -fcnuse +trytorecover -D__int64="long long" -D__STATIC_INLINE="static inline" -D__pure=" " -D__value_in_regs=" " -D__INLINE=inline "%%d" >> lint.log
)

for /f "usebackq" %%d in (`"dir Library\StdDriver\*.c /b/s"`) do (
    %SPLINT_PATH% -IC:\work_zone\nano112\Nano1x2BSP\Library\StdDriver\inc -IC:\work_zone\nano112\Nano1x2BSP\Library\Device\Nuvoton\Nano103\Include -IC:\Keil\ARM\RV31\INC -IC:\work_zone\nano112\Nano1x2BSP\Library\CMSIS\Include -weak -unrecogcomments -fcnuse +trytorecover  -D__int64="long long" -D__STATIC_INLINE="static inline" -D__pure=" " -D__value_in_regs=" " -D__INLINE=inline "%%d" >> lint.log
)

for /f "usebackq" %%d in (`"dir SampleCode\*.c /b/s"`) do (
    %SPLINT_PATH% -IC:\work_zone\nano112\Nano1x2BSP\Library\StdDriver\inc -IC:\work_zone\nano112\Nano1x2BSP\Library\Device\Nuvoton\Nano103\Include -IC:\Keil\ARM\RV31\INC -IC:\work_zone\nano112\Nano1x2BSP\Library\CMSIS\Include -weak -unrecogcomments -fcnuse +trytorecover  -D__int64="long long" -D__STATIC_INLINE="static inline" -D__pure=" " -D__value_in_regs=" " -D__INLINE=inline "%%d" >> lint.log
)
