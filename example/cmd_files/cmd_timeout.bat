@echo off

set argC=0
FOR %%x in (%*) DO ( Set /A argC+=1 )


IF %argC% NEQ 2 ( 
    exit /b 1
)


set /a timeToPause=%1

IF "%2"=="sec" (
    goto goto_prcSEC
)
IF "%2"=="m" (
    goto goto_prcM
)
IF "%2"=="h" (
    goto goto_prcH
)

exit /b 2




:goto_prcSEC
    echo pause %timeToPause% sec
    timeout %timeToPause%
exit /b 0


:goto_prcM
    echo pause %timeToPause% m
    set /a timeToPause*=60
    timeout %timeToPause%
exit /b 0


:goto_prcH
    echo pause %timeToPause% h
    set /a timeToPause*=3600
    timeout %timeToPause%
exit /b 0
