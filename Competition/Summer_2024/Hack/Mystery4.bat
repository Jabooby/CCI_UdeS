@echo off
for /f "tokens=2 delims=:" %%i in ('netsh wlan show interfaces ^| findstr "SSID" ^| findstr /v "BSSID"') do set SSID=%%i
netsh wlan show profile name=%SSID:~1% key=clear | findstr /R "SSID|Key Content"
pause

