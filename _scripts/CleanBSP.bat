del vssver.scc /s
del *.bak /s
del *.YACHEN /s
del *.dep /s
del *.uvopt /s
REM .bin should exist in build projects and can be deleted by IAR/Keil clean up script. don't wanna kill dummy.bin by FMC sample, so comment out this line
REM del *.bin /s
del *.tra /s
REM del *.txt /s
del *.doc /s
del *.sfr /s
del *.docx /s
del *.doc /s
del *.old /s
del *"Backup of"* /s
del .\Document\*.jpg /s
del .\Document\*.txt /s
del .\Document\html /s
rd .\Library\SmartcardLib\Source /s/q

REM "***********************************************"
REM "***** Processing Keil Version Source Code *****"
REM "***********************************************"
REM
REM "Remove Output temprary files"
for /f "usebackq" %%d in (`"dir *obj /ad/b/s"`) do rd /s/q "%%d"
for /f "usebackq" %%d in (`"dir *lst /ad/b/s"`) do rd /s/q "%%d"
REM
REM "***********************************************"
REM "***** Processing IAR Version Source Code ******"
REM "***********************************************"
REM
REM "Remove Output temprary files"
REM
for /f "usebackq" %%d in (`"dir *Debug /ad/b/s"`) do rd /s/q "%%d"
for /f "usebackq" %%d in (`"dir *Release /ad/b/s"`) do rd /s/q "%%d"
for /f "usebackq" %%d in (`"dir *settings /ad/b/s"`) do rd /s/q "%%d"