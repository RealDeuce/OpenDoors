@echo off
echo.>>C:\TCBUILD.LOG
echo ==== EX_VOTE.C + EX_VOTE_IO.C ====>>C:\TCBUILD.LOG
echo Building example EX_VOTE.C
cd \SRC
TCC -ml -O -G -Z -d -w -DMULTINODE_AWARE -eC:\OUT\EX_VOTE.EXE EX_VOTE.C EX_VOTE_IO.C C:\OUT\ODOORL.LIB >>C:\TCBUILD.LOG
if not errorlevel 1 goto test
echo EXAMPLE:EX_VOTE.C>>C:\TCFAIL.TXT
goto done
:test
echo.>>C:\TCBUILD.LOG
echo ==== VOTETEST.C + EX_VOTE_IO.C ====>>C:\TCBUILD.LOG
echo Building and running Vote portable I/O test
TCC -ml -O -G -Z -d -w -DMULTINODE_AWARE -eC:\OUT\ODVTEST.EXE VOTETEST.C EX_VOTE_IO.C >>C:\TCBUILD.LOG
if errorlevel 1 goto testfail
C:\OUT\ODVTEST.EXE
if not errorlevel 1 goto done
echo RUN:ODVTEST.EXE>>C:\TCFAIL.TXT
goto done
:testfail
echo TEST:VOTETEST.C>>C:\TCFAIL.TXT
:done
cd \OBJ
