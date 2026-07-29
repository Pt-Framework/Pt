@echo off
setlocal
set OPENSSL=C:\_Dev\OpenSSL-3.4.1\bin\openssl.exe
set OPENSSL_CONF=C:\_Dev\Pt\src\Pt-Ssl\tests\cert\openssl.cnf
cd /d %~dp0

rem --- SGC Certificate Authority (self-signed CA) ---
%OPENSSL% req -newkey rsa:2048 -nodes -keyout sgc_ca.key -x509 -days 3650 -subj "/CN=SGC Certificate Authority" -addext "basicConstraints=CA:TRUE,pathlen:0" -out sgc_ca.cer 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED sgc_ca & exit /b 1)
echo sgc_ca.cer

rem --- SGC Mainframe (server cert) ---
%OPENSSL% req -newkey rsa:2048 -nodes -keyout sgc_mainframe.key -subj "/CN=SGC Mainframe" -out sgc_mainframe.req 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED sgc_mainframe_req & exit /b 1)
%OPENSSL% x509 -req -in sgc_mainframe.req -CA sgc_ca.cer -CAkey sgc_ca.key -days 3650 -out sgc_mainframe.cer 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED sgc_mainframe_sign & exit /b 1)
echo sgc_mainframe.cer

rem --- Atlantis Mainframe (client cert) ---
%OPENSSL% req -newkey rsa:2048 -nodes -keyout atlantis.key -subj "/CN=Atlantis Mainframe" -out atlantis.req 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED atlantis_req & exit /b 1)
%OPENSSL% x509 -req -in atlantis.req -CA sgc_ca.cer -CAkey sgc_ca.key -days 3650 -out atlantis.cer 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED atlantis_sign & exit /b 1)
echo atlantis.cer

rem --- PKCS12: server (SGC Mainframe + CA chain) ---
%OPENSSL% pkcs12 -export -inkey sgc_mainframe.key -in sgc_mainframe.cer -CAfile sgc_ca.cer -chain -passout pass:123 -out sgc_server.p12 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED sgc_server.p12 & exit /b 1)
echo sgc_server.p12

rem --- PKCS12: client (Atlantis Mainframe + CA chain) ---
%OPENSSL% pkcs12 -export -inkey atlantis.key -in atlantis.cer -CAfile sgc_ca.cer -chain -passout pass:123 -out sgc_client.p12 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED sgc_client.p12 & exit /b 1)
echo sgc_client.p12

rem --- PKCS12: CA only ---
%OPENSSL% pkcs12 -export -inkey sgc_ca.key -in sgc_ca.cer -passout pass:123 -out sgc_ca.p12 2>nul
if %ERRORLEVEL% neq 0 (echo FAILED sgc_ca.p12 & exit /b 1)
echo sgc_ca.p12

echo SUCCESS
