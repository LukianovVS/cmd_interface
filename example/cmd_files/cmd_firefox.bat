@echo off

set argC=0
FOR %%x in (%*) DO ( Set /A argC+=1 )



IF %argC%==0 (
    set goto_web=https://www.google.com/
) ELSE (
    set goto_web=https://%1/
)

start firefox %goto_web%