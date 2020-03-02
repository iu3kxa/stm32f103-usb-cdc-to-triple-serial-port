Install the certificate to avoid the unsigned driver alert, in windows 10 installing the certificate is mandatory, in windows 7 x64 i'm not sure if id mandatory, just install it.

Due to a bug in windows7 x64, the driver installer is not able to locate usbser.sys and the driver installation fail even if usbser.sys is already present.
To avoid the bug, i defined in the .inf a dummy file to install named usbser_dummy.sys; this file will never be used and can be delete o leaved alone in system32\drivers\ if copied from the installer.
