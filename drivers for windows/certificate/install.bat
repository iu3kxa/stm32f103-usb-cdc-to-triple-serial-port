certmgr.exe /del iu3kxa.cer /s /r localMachine ROOT
certmgr.exe /del iu3kxa.cer /s /r localMachine TRUSTEDPUBLISHER
certmgr.exe /add iu3kxa.cer /s /r localMachine ROOT
certmgr.exe /add iu3kxa.cer /s /r localMachine TRUSTEDPUBLISHER
certutil -p "" -importpfx iu3kxa.pfx
pause
