@echo off
title NuMicro BSP Release Checking Script



cd ..\
if exist msg.log del msg.log

REM Command version
REM set var=
REM :LOOP
REM    if "%1"=="" goto :QUIT
REM    if "%1"=="a" goto :AS
REM    if "%1"=="c" goto :CLENAUP
REM    if "%1"=="d" goto :DOXYGEN
REM    if "%1"=="h" goto :HELP
REM    if "%1"=="i" goto :IAR
REM    if "%1"=="k" goto :KEIL
REM    if "%1"=="l" goto :LINT
REM    if "%1"=="s" goto :DEVSETTING    
REM :LOOP1
REM    set var=%var% %1
REM    shift
REM    goto :LOOP


REM Menual vertion
:LOOP1
    cls
    echo NuMicro BSP release check script v1.00
    echo    a: Astyle prettifier
    echo    c: Clean-up
    echo    d: Create .chm BSP manual by Doxygen
    echo    i: Build IAR projects
    echo    k: Build Keil projects
    echo    l: SPLint check syntax
    echo    m: Modify IAR projects from 6.5 or 6.7 to 6.0
    echo    s: Check project device setting
    echo    p: Aspell spelling check
REM    echo    x: Sequencially execute: a, d, i, k, l, s , c option
    echo    q: Quit
    set in=
    set /p in=Please enter your select:
    if "%in%"=="q" goto :QUIT
    if "%in%"=="a" goto :AST
    if "%in%"=="c" goto :CLENAUP
    if "%in%"=="d" goto :DOXYGEN
    if "%in%"=="i" goto :IAR
    if "%in%"=="k" goto :KEIL
    if "%in%"=="l" goto :LINT
    if "%in%"=="m" goto :IARPROJ
    if "%in%"=="s" goto :DEVSETTING
    if "%in%"=="p" goto :ASP
REM   if "%1"=="x" goto :ALL
    echo Unsupport option & ping -n 3 127.1>nul & goto :LOOP1

  
  
:AST
    echo Executing Astyle
    call _scripts\Astyle 1>msg.log 2>&1
    echo Done & ping -n 2 127.1>nul
    goto :LOOP1
    
:ASP
    echo Spelling checking
    call _scripts\Aspell
    echo Done & ping -n 2 127.1>nul
    goto :LOOP1
    
:CLENAUP
    echo Clean up
    call _scripts\CleanBSP 1>msg.log 2>&1
    echo Done. Do not forget to remove _script direcotry later, hit any key to continue.
    pause
    goto :LOOP1
    
:DOXYGEN
    echo Generating User's manual
    copy Library\Device\Nuvoton\Nano103\Include\Nano103.h .
    _scripts\conv.exe Library\Device\Nuvoton\Nano103\Include\Nano103.h
    move /Y temp.h Library\Device\Nuvoton\Nano103\Include\Nano103.h
    call _scripts\Doxy 1>msg.log 2>&1
    move /Y .\Nano103.h Library\Device\Nuvoton\Nano103\Include\Nano103.h
REM    call _scripts\Tex2Pdf
    echo Done & ping -n 2 127.1>nul
    goto :LOOP1
    
:IAR
    echo IAR projects test build
    call _scripts\BuildIar 1>msg.log 2>&1
    echo Done & ping -n 2 127.1>nul
    goto :LOOP1
    
:KEIL
    echo Keil projects test build
    call _scripts\BuildKeil 1>msg.log 2>&1
    echo Done & ping -n 2 127.1>nul    
    goto :LOOP1
    
:LINT
    echo spint syntax checking
    call _scripts\Lint 1>msg.log 2>&1
    echo Done & ping -n 2 127.1>nul    
    goto :LOOP1

:IARPROJ
    echo Modifying IAR projects
    call _scripts\iar 1>msg.log 2>&1
    echo Done & ping -n 2 127.1>nul
    goto :LOOP1
    
:DEVSETTING
    echo Checking project device setting 
    call _scripts\CheckPrj 1>msg.log 2>&1
    echo Done & ping -n 2 127.1>nul    
    goto :LOOP1
  
:QUIT
    echo Bye & ping -n 2 127.1>nul

