REM Need these for rename pdf file
SETLOCAL ENABLEDELAYEDEXPANSION
SET old=chm
SET new=pdf
REM Copy necessary files to latex directory
copy _scripts\*.sty Document\latex\
copy _scripts\*.jpg Document\latex\
copy _scripts\*.png Document\latex\
REM Generate PDF file from Latex files
cd Document\latex
call make.bat
cd ..\..
copy Document\latex\refman.pdf Document\
REM Recycling temp files
rd Document\latex /Q /S
REM Since Doxygne cannot set Letex/pdf output file name, find chm file name and rename pdf file
for /f "delims=" %%f in ('dir Document\*.chm /b/s') do (
    SET newname=%%f
    SET newname=!newname:%old%=%new%!
    MOVE /Y Document\refman.pdf "!newname!"
)