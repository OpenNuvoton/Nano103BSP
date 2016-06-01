
SETLOCAL ENABLEDELAYEDEXPANSION

SET old=.ewp
SET new=
SET tmp=.log

c:\cygwin\bin\grep -n '\.out' _scripts\temp.ewp > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > out1.log


for /f "usebackq delims=" %%d in (`"dir "*IAR" /ad/b/s"`) do (
    if exist "%%d\*.ewp" (
        for /f "usebackq delims=" %%e in (`"dir "%%d\*.ewp" /b/s"`) do (    

            c:\cygwin\bin\sed -n '/\.out/p' "%%e" > out.log

            for /f %%i in (out1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' _scripts\temp.ewp > 8.log
                c:\cygwin\bin\sed '$d' 8.log > f2.log
                c:\cygwin\bin\sed '1,%%id' _scripts\temp.ewp > f3.log
            )
            copy /b f2.log+out.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/postbuild/p' "%%e" > 1.log
            c:\cygwin\bin\sed '$d' 1.log > tmp.log

c:\cygwin\bin\grep -n 'postbuild' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > 8.log
                c:\cygwin\bin\sed '$d' 8.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/CCDefines/,/option/p' "%%e" > 1.log
            c:\cygwin\bin\sed '/option/,/option/d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'CCDefines' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/ADefines/,/option/p' "%%e" > 1.log
            c:\cygwin\bin\sed '/option/,/option/d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'ADefines' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/AUserIncludes/,/option/p' "%%e" > 1.log
            c:\cygwin\bin\sed '/option/,/option/d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'AUserIncludes' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/GOutputBinary/,/OGCoreOrChip/p' "%%e" > 1.log
            c:\cygwin\bin\sed '10,$d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'GOutputBinary' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/CCDebugInfo/,/state/p' "%%e" > 1.log
            c:\cygwin\bin\sed '3,4d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'CCDebugInfo' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/Input variant/,/GOutputBinary/p' "%%e" > 1.log
            c:\cygwin\bin\sed '19,$d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'Input variant' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/OCOutputOverride/,/OOCCommandLineProducer/p' "%%e" > 1.log
            c:\cygwin\bin\sed '9,18d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > bin.log

c:\cygwin\bin\grep -n 'OCOutputOverride' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > bin1.log

            for /f %%i in (bin1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+bin.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/IlinkIcfOverride/,/IlinkIcfFileSlave/p' "%%e" > 1.log
            c:\cygwin\bin\sed '9,18d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > icf.log

c:\cygwin\bin\grep -n 'IlinkIcfOverride' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > icf1.log

            for /f %%i in (icf1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+icf.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/CCDiagSuppress/,/option/p' "%%e" > 1.log
            c:\cygwin\bin\sed '$d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'CCDiagSuppress' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/IlinkUseExtraOptions/,/option/p' "%%e" > 1.log
            c:\cygwin\bin\sed '$d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'IlinkUseExtraOptions' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > 8.log
                c:\cygwin\bin\sed '1d' 8.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/IlinkExtraOptions/,/option/p' "%%e" > 1.log
            c:\cygwin\bin\sed '$d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'IlinkExtraOptions' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > f2.log
                c:\cygwin\bin\sed '1,%%id' f4.log > 8.log
                c:\cygwin\bin\sed '1d' 8.log > f3.log
            )
            copy /b f2.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            c:\cygwin\bin\sed -n '/CCIncludePath2/,/option/p' "%%e" > 1.log
            c:\cygwin\bin\sed '$d' 1.log > 2.log
            c:\cygwin\bin\sed '1d' 2.log > tmp.log

c:\cygwin\bin\grep -n 'CCIncludePath2' f4.log > 3.log
c:\cygwin\bin\sed -n 's/\://p' 3.log > 4.log
c:\cygwin\bin\sed 's/:.*$//g' 4.log > tmp1.log

            for /f %%i in (tmp1.log) do (
                c:\cygwin\bin\sed -n '1,%%ip' f4.log > 8.log
                c:\cygwin\bin\sed '1,%%id' f4.log > f3.log
            )

            copy /b 8.log+tmp.log 10.log /y
            copy /b 10.log+f3.log f4.log /y

            SET newname=%%e
            SET newname=!newname:%old%=%new%!
            SET tmpfile=%%e
            SET tmpfile=!tmpfile:%old%=%tmp%!

            c:\cygwin\bin\sed -n '/group/,/project/p' "%%e" > "!tmpfile!"
            copy f4.log "!newname!" /y
            copy /b "!newname!"+"!tmpfile!" "%%e" /y
            c:\cygwin\bin\sed -i '$a \\\n\n' "%%e"
            del "!tmpfile!"
            del "!newname!"

        )


    ) else (
        echo Ooops, IAR project file not found in %%d >> iar.log
    )    
)


del *.log

for /f "usebackq delims=" %%d in (`"dir "*IAR" /ad/b/s"`) do (
    if exist "%%d\*.ewd" (
        for /f "usebackq delims=" %%e in (`"dir "%%d\*.ewd" /b/s"`) do (    
            copy _scripts\temp.ewd "%%e" /y
        )

    ) else (
        echo Ooops, IAR project file not found in %%d >> iar.log
    )    
)


