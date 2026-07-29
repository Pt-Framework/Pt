@echo off
setlocal
set OPENSSL=C:\_Dev\OpenSSL-3.4.1\bin\openssl.exe
set OPENSSL_CONF=C:\_Dev\OpenSSL-3.4.1\ssl\openssl.cnf
cd /d %~dp0

%OPENSSL% req -newkey rsa:2048 -nodes -keyout root_ca.key -x509 -days 3650 -subj "/CN=Root CA" -out root_ca.cer
if %ERRORLEVEL% neq 0 (echo FAILED root_ca & exit /b 1)

%OPENSSL% req -newkey rsa:2048 -nodes -keyout ca_1.key -subj "/CN=Intermediate CA 1" -out ca_1.req
if %ERRORLEVEL% neq 0 (echo FAILED ca_1_req & exit /b 1)

%OPENSSL% x509 -req -in ca_1.req -CA root_ca.cer -CAkey root_ca.key -days 3650 -out ca_1.cer -extfile %OPENSSL_CONF% -extensions v3_ca
if %ERRORLEVEL% neq 0 (echo FAILED ca_1_sign & exit /b 1)

%OPENSSL% req -newkey rsa:2048 -nodes -keyout ca_2.key -subj "/CN=Intermediate CA 2" -out ca_2.req
if %ERRORLEVEL% neq 0 (echo FAILED ca_2_req & exit /b 1)

%OPENSSL% x509 -req -in ca_2.req -CA ca_1.cer -CAkey ca_1.key -days 3650 -out ca_2.cer -extfile %OPENSSL_CONF% -extensions v3_ca
if %ERRORLEVEL% neq 0 (echo FAILED ca_2_sign & exit /b 1)

%OPENSSL% req -newkey rsa:2048 -nodes -keyout server.key -subj "/CN=Server" -out server.req
if %ERRORLEVEL% neq 0 (echo FAILED server_req & exit /b 1)

%OPENSSL% x509 -req -in server.req -CA ca_2.cer -CAkey ca_2.key -days 3650 -out server.cer
if %ERRORLEVEL% neq 0 (echo FAILED server_sign & exit /b 1)

copy /b root_ca.cer+ca_1.cer+ca_2.cer ca_chain.cer
%OPENSSL% pkcs12 -export -inkey server.key -in server.cer -chain -CAfile ca_chain.cer -passout pass:123 -out server_chain-with-password.p12
if %ERRORLEVEL% neq 0 (echo FAILED pkcs12 & exit /b 1)

echo SUCCESS
