REM Aspell spelling check in C/C++ mode


for /f "usebackq" %%d in (`"dir Library\Device\*.c /b/s"`) do (
    C:\cygwin\bin\aspell.exe check --mode=ccpp --dont-backup "%%d"
)

for /f "usebackq" %%d in (`"dir Library\Device\*.h /b/s"`) do (
    C:\cygwin\bin\aspell.exe check --mode=ccpp --dont-backup "%%d"
)

for /f "usebackq" %%d in (`"dir Library\StdDriver\*.c /b/s"`) do (
    C:\cygwin\bin\aspell.exe check --mode=ccpp --dont-backup "%%d"
)

for /f "usebackq" %%d in (`"dir Library\StdDriver\*.h /b/s"`) do (
    C:\cygwin\bin\aspell.exe check --mode=ccpp --dont-backup "%%d"
)

for /f "usebackq" %%d in (`"dir SampleCode\StdDriver\*.c /b/s"`) do (
    C:\cygwin\bin\aspell.exe check --mode=ccpp --dont-backup "%%d"
)

for /f "usebackq" %%d in (`"dir SampleCode\StdDriver\*.h /b/s"`) do (
    C:\cygwin\bin\aspell.exe check --mode=ccpp --dont-backup "%%d"
)