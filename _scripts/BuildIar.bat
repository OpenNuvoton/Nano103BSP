REM IAR test build for all projects

if exist iar.log del iar.log
for /f "usebackq" %%d in (`"dir *IAR /ad/b/s"`) do (
    if exist "%%d\*.ewp" (
        for /f "usebackq" %%e in (`"dir %%d\*.ewp /b/s"`) do (    
        	echo Check %%d ... >> iar.log
             "C:\Program Files\IAR Systems\Embedded Workbench 6.5\common\bin\iarbuild.exe" "%%e" -build Release -log warnings >> iar.log
        )

    ) else (
        echo Ooops, Keil project file not found in %%d >> iar.log
    )    
)