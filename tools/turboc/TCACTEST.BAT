@echo off
echo.>>C:\TCBUILD.LOG
echo ==== TRANSPRT.C ====>>C:\TCBUILD.LOG
echo Building public transport acceptance doors
TCC -ml -O -G -Z -d -w -IC:\SRC;C:\TC\INCLUDE -LC:\OUT;C:\TC\LIB -eC:\OUT\TCUART.EXE C:\SRC\TRANSPRT.C C:\OUT\ODOORL.LIB >>C:\TCBUILD.LOG
if errorlevel 1 goto uartfail
TCC -ml -O -G -Z -d -w -DOD_ACCEPTANCE_DOS_FOSSIL=1 -IC:\SRC;C:\TC\INCLUDE -LC:\OUT;C:\TC\LIB -eC:\OUT\TCFOSS.EXE C:\SRC\TRANSPRT.C C:\OUT\ODOORL.LIB >>C:\TCBUILD.LOG
if errorlevel 1 goto fossilfail
goto end
:uartfail
echo ACCEPTANCE:TRANSPRT.C:UART>>C:\TCFAIL.TXT
goto end
:fossilfail
echo ACCEPTANCE:TRANSPRT.C:FOSSIL>>C:\TCFAIL.TXT
:end
