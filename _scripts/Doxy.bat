REM We need m4 logo at the same direction doxygen exxcutes... cannot figure out how to modify configure file to fix this
copy _scripts\*.jpg .
copy _scripts\*.tex .
doxygen _scripts\DoxyFile.txt >> doxy.log
copy Document\html\*.chm Document\
REM Recycling temp files
del /Q *.jpg
del /Q *.tex
rd Document\html /Q /S
