REM http://peterdn.com/category/WinRT.aspx

MakeAppx.exe pack /d .\appx /p Pt-test.appx

REM create private and public key
REM makecert.exe -n "CN=pt-framework.org" -r -a sha1 -sv pt-framework.org.pvk pt-framework.org.cer –ss root

REM create client ceritfictate for signinig
REM installs certificate to root store, remove later with certmgr.exe or certutil.exe from trusted root CAss
REM makecert -a sha1 -sk "pt-framework.org" -iv pt-framework.org.pvk -n "CN=pt-framework.org" -ic pt-framework.org.cer -sr currentuser -ss My

REM determine Thumbprint
dir cert:\CurrentUser\My

REM use thumbprint to sign package
signtool.exe sign /fd sha256 /sha1 0a241ba3a94080f0df32d90dc60358368a4ebce0 Pt-test.appx

REM install package
Add-AppxPackage .\MyApp.appx