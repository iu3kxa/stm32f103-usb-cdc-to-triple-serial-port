This folder contain the .inf required for the correct configuration
of the pheriperials.
The files usbser.sys and serenum.sys are already part of windows.

When connecting the mcu to the usb port, 3 unknown devices on device manager
under com&Lpt ports so this inf is required.

On windows 10 the device will be recognized as generic cdc so you not really
need the inf, however using the inf will assign the correct name to the ports,
otherwise you will see 3 devices with the same name.

Windows xp
Certificate is not required for windows XP.
Double click on the device, choose to update driver and browse to the folder
containing the inf.

Windows 7.
On 32bit, the inf will install with a warning about the driver not signed, on 64bit
the installation may be refused at all.
Please go in folder "certificate" , right click on "install.bat" and choose
"run as administrator";
this will install a certificate in the Trusted "Root" and "Publisher" of windows
certification store; you can check or delete the certificates using certmgr.msc
Now the inf will be installed without any warning.
Last note: due to a bug in windows7 x64, the driver installer is not able to locate
usbser.sys and the driver installation fail even if usbser.sys is already present.
To avoid the bug i defined in the .inf a fake file to install named usbser_dummy.sys;
this file will never be used and can be delete o leaved alone in system32\drivers\.

Windows 10
As stated before, the pheriperial will be installed as generic cdc devices, you will
see 3 devices with the same name.
You need to update the driver with the inf to have the correct names, however in
windows 10 is not possible to install unsigned driver at all.
Please go in folder "certificate" , right click on "install.bat" and choose
"run as administrator";
this will install a certificate in the Trusted "Root" and "Publisher" of windows
certification store; you can check or delete the certificates using certmgr.msc
Now the inf will be installed without any warning.
