REM Execute Astyle source code prettifier

REM --indent=spaces=4 --indent-switches  --indent-col1-comments --convert-tabs 
"C:\Program Files\AStyle\bin\astyle.exe"  --style=stroustrup --indent=spaces=4 --convert-tabs  --recursive --suffix=none Library\Device\Nuvoton\*.c Library\Device\Nuvoton\*.h 
"C:\Program Files\AStyle\bin\astyle.exe"  --style=stroustrup --indent=spaces=4 --convert-tabs  --recursive --suffix=none  --exclude=retarget.c  Library\StdDriver\src\*.c Library\StdDriver\inc\*.h 
"C:\Program Files\AStyle\bin\astyle.exe"  --style=stroustrup --indent=spaces=4 --convert-tabs  --recursive --suffix=none Library\LCDLib\*.c Library\LCDLib\*.h 
"C:\Program Files\AStyle\bin\astyle.exe"  --style=stroustrup --indent=spaces=4 --convert-tabs  --recursive --suffix=none Library\SmartcardLib\*.c Library\SmartcardLib\*.h 
"C:\Program Files\AStyle\bin\astyle.exe"  --style=stroustrup --indent=spaces=4 --convert-tabs  --recursive --suffix=none SampleCode\*.c SampleCode\*.h

