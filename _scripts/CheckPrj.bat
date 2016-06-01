SETLOCAL ENABLEDELAYEDEXPANSION
if exist chk.log del chk.log

for /f "usebackq" %%d in (`"dir *KEIL /ad/b/s"`) do (
    if exist %%d\Nu_Link_Driver.ini (
        findstr /m "ChipName=Nano103" %%d\Nu_Link_Driver.ini
        if !errorlevel!==1  echo Ooops, %%d\Nu_Link_Driver.ini device setting error >> chk.log
    ) else (
        echo Ooops, %%d\Nu_Link_Driver.ini does not exist >> chk.log
    )    
    if exist %%d\*.uvproj (
        for /f "usebackq" %%e in (`"dir %%d\*.uvproj /b/s"`) do (    
            for /f %%N in ('find /c "NANO103SD3AN"^< "%%e"') do set /a cnt=%%N
            if not !cnt!==1  echo Ooops, device type error in %%e >> chk.log
        )
    ) else (
        echo Ooops, %%d Keil project file does not exist >> chk.log
    )    
)

for /f "usebackq" %%d in (`"dir *IAR /ad/b/s"`) do (
    if not exist "%%d\*.ewd" (
        echo Ooops, %%d\ EWD file missing >> chk.log
    ) else (
        #for /f "usebackq" %%e in (`"dir %%d\*.ewd /b/s"`) do (    
        #    for /f %%N in ('find /c "Nano112_APROM.board<"^< "%%e"') do set /a cnt=%%N
        #    if not !cnt!==1  echo Ooops, might have flash loader error in %%e >> chk.log
        #)
        for /f "usebackq" %%e in (`"dir %%d\*.ewd /b/s"`) do (    
            for /f %%N in ('find /c "$TOOLKIT_DIR$\..\..\..\Nuvoton Tools\Nu-Link_IAR\Nu-Link_IAR.dll<"^< "%%e"') do set /a cnt=%%N
            if not !cnt!==1  echo Ooops, might have third-party driver error in %%e >> chk.log
        )
    )
    if not exist "%%d\*.ewp" (
        echo Ooops, %%d\ EWP file missing >> chk.log
    ) else (
        for /f "usebackq" %%e in (`"dir %%d\*.ewp /b/s"`) do (    
            for /f %%N in ('find /c "Nuvoton Nano103AN series"^< "%%e"') do set /a cnt=%%N
            if not !cnt!==1  echo Ooops, might have wrong device error in %%e >> chk.log
        )
        for /f "usebackq" %%e in (`"dir %%d\*.ewp /b/s"`) do (    
            for /f %%N in ('find /c "Reset_Handler"^< "%%e"') do set /a cnt=%%N
            if not !cnt!==1  echo Ooops, might have wrong Reset_Handler error in %%e >> chk.log
        )
    )
    if not exist "%%d\*.eww" (
        echo Ooops, %%d\ EWW file missing >> chk.log
    )
)