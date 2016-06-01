REM Keil test build for all projects
SETLOCAL ENABLEDELAYEDEXPANSION
rem C:\Keil_v5\UV4\Uv4.exe -b -j0 All_Samples.uvmpw -z -o "keil.log"

if exist keil.log del keil.log
REM set DIRECTORY=%cd%
for /f "usebackq" %%d in (`"dir *keil /ad/b/s"`) do (
    if exist "%%d\*.uvproj" (
        for /f "usebackq" %%e in (`"dir %%d\*.uvproj /b/s"`) do (    
             C:\Keil_v5\UV4\Uv4.exe -b -j0 -z "%%e"
        )

        echo Check %%d ... >> keil.log

        if not !errorlevel!==0 (
            REM C:\Keil_v5\UV4\Uv4.exe -b -j0 -z -o %DIRECTORY%/keil.log  "%%e"
            echo Ooops, Check build Keil project %%d has error or warning >> keil.log
        )
        
    ) else (
        echo Ooops, Keil project file not found in %%d >> keil.log
    )    
)