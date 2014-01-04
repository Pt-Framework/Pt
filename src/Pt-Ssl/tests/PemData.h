/*
 * Copyright (C) 2006 - 2012 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

const char clientCertPemData [] = "\
-----BEGIN CERTIFICATE-----\n\
MIIDdDCCAlygAwIBAgIBZTANBgkqhkiG9w0BAQUFADBEMQswCQYDVQQGEwJVUzEa\n\
MBgGA1UEBxMRQ2hleWVubmUgTW91bnRhaW4xGTAXBgNVBAoTEFN0YXJnYXRlIENv\n\
bW1hbmQwIBcNMTIwNjI4MDAwOTQzWhgPMjExMjAyMjUwMDA5NDNaMEQxCzAJBgNV\n\
BAYTAlVTMRowGAYDVQQHExFDaGV5ZW5uZSBNb3VudGFpbjEZMBcGA1UEChMQU3Rh\n\
cmdhdGUgQ29tbWFuZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN2H\n\
OvGTyA0jIUlqTHYMjrCwX4aha5hp91CuqNpQZwkN7awgk97edBb50EcQNrlfFr/J\n\
LEDP06mHpp9Fv3aW5mQQKakw1s6/ST2YOlqp9fRqsaTO/C4w5bvOQTN6LZm+0tqj\n\
J0cvZ6w1x9dSjLt1M1VUnTP0fQ0LkeBScY0tJpR663MTa/c4B0kGdgIGKALf0xzl\n\
MJVaucSv+tbhfGLUW+qA225WUgJN3H0UdK6bSWSw2YKJ/aD8AMZU+TBJm9V/O7DO\n\
4D4uOAv99JkgtJjkN8iC5wDLYJeXg/11ewSXU1xe7NJt3FN+/TOWNv2VkwSWHw8x\n\
COYcIY66xxSBsGCmIwECAwEAAaNvMG0wCQYDVR0TBAIwADALBgNVHQ8EBAMCBLAw\n\
EwYDVR0lBAwwCgYIKwYBBQUHAwIwEQYJYIZIAYb4QgEBBAQDAgeAMCsGA1UdHwQk\n\
MCIwIKAeoByGGmh0dHA6Ly90ZXN0Y2EubG9jYWwvY2EuY3JsMA0GCSqGSIb3DQEB\n\
BQUAA4IBAQA3zExU/mMi4mPbfrMJJlso0IAwWKBwVYNzY0+PDYRDwasJ/nGDRpYg\n\
3t51RMy+RDq1D9o66NYT02EjOt2+FKIPa1rqSqlOOSp4JhwYI2snyNjwM1uyEfwu\n\
5JDGqR4K8RUPGz2idI9ueYKkInfZkWtWLscX25Zt3NHybdp8pn91+/EfhRkzDQs5\n\
PAQOP2+DjFI9cIbxsM+ww7LaD2TJCNuoL5L+r9bRN/2iLAQG6ds88NdPmsP/zoiL\n\
Q597Co441X3otxORYviKnMisdq80Z9EPH64CsIVLycmwfRgZskoj4kO3SnAVuiI5\n\
HzDpmPLQ57zce4ZMKHaq1zYQRSM35aNh\n\
-----END CERTIFICATE-----";

const char serverCertPemData [] = "\
-----BEGIN CERTIFICATE-----\n\
MIIDdDCCAlygAwIBAgIBZDANBgkqhkiG9w0BAQUFADBEMQswCQYDVQQGEwJVUzEa\n\
MBgGA1UEBxMRQ2hleWVubmUgTW91bnRhaW4xGTAXBgNVBAoTEFN0YXJnYXRlIENv\n\
bW1hbmQwIBcNMTIwNjI4MDAwOTE4WhgPMjExMjAyMjUwMDA5MThaMEQxCzAJBgNV\n\
BAYTAlVTMRowGAYDVQQHExFDaGV5ZW5uZSBNb3VudGFpbjEZMBcGA1UEChMQU3Rh\n\
cmdhdGUgQ29tbWFuZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANEH\n\
yT6cGxfqzdG/zgFM9/ip2BuaiL+XfKv50Yk52EcjY9pDdtqR7O1XVg9G8TlZOfKy\n\
RVjHSBwM1KQdT7UAy/eV0Zco/8xCDuxI5HU+UhvJWSQKbzTeO6TIdWhCSFROCfvN\n\
HvaXUWZHv792lYwDIwb9UulFai32ZC4HtERaobtT/meG8ZNbJBtzkUQAvcjMDpPl\n\
A5ntVuB9LwZeYUyU5L48F4oj7qUFLdWZ4WLu52F/0CKNlEouzEPTj8dHH0GNsas7\n\
z+We3jmWLjidQ7b4YtZrOa6fnQawOWdIr14egHxyrYX1xIf6EOlrAj6txz5sY5zX\n\
m1dzGQLy9vgz58M/AFECAwEAAaNvMG0wCQYDVR0TBAIwADALBgNVHQ8EBAMCBLAw\n\
EwYDVR0lBAwwCgYIKwYBBQUHAwEwEQYJYIZIAYb4QgEBBAQDAgZAMCsGA1UdHwQk\n\
MCIwIKAeoByGGmh0dHA6Ly90ZXN0Y2EubG9jYWwvY2EuY3JsMA0GCSqGSIb3DQEB\n\
BQUAA4IBAQBEQ/SR5/8I67zG2blmqgRfTF4So501dAxYp2YF8xLtyVKsZpB+i2sQ\n\
7mhXVEHH/AxWCzyOWwJ8yF9I0Oxv2hFW9FtJIOwV1P1543HzTG2IwKKvQckulGO8\n\
vKZrb/dYdPe8NrXd58Ucrm/21KlFoSgfdsZvD6Cl1VqTSYzX0OxtQhhF2RksKmjB\n\
l+gxvMCTMY3sjWAXbIZ71yGPAuyrRbwPW4Xz1987BXeRWu2vUaZ6WgHZAfGo24gV\n\
seJESU0RtDD59CsT3F9K7sGwEpq1DT0VtnVF5QsmlYYL/ofYOLeS+vx+96ZdmBND\n\
ef7W3tSD4nkfHpCrkcDwq/BzEnEE6c2t\n\
-----END CERTIFICATE-----";

const char caPemData [] = "\
-----BEGIN CERTIFICATE-----\n\
MIID4TCCAsmgAwIBAgIJAPBNTh+CLvbDMA0GCSqGSIb3DQEBBQUAMEQxCzAJBgNV\n\
BAYTAlVTMRowGAYDVQQHExFDaGV5ZW5uZSBNb3VudGFpbjEZMBcGA1UEChMQU3Rh\n\
cmdhdGUgQ29tbWFuZDAgFw0xMjA2MjgwMDA4MzJaGA8yMTEyMDYwNDAwMDgzMlow\n\
RDELMAkGA1UEBhMCVVMxGjAYBgNVBAcTEUNoZXllbm5lIE1vdW50YWluMRkwFwYD\n\
VQQKExBTdGFyZ2F0ZSBDb21tYW5kMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n\
CgKCAQEAyAFtY68tpFwhaUtZ9XSeGHl9LFlqIZvU/6caKr85Yuo8ZuGcBEI18xLI\n\
ElOpr4Z0PDjzR3Ycn7dZZu+f5qAEUej6HV5Ss1q5jBsQ45ugX9iBubnxue3J8Ut3\n\
wLgzwm+TH2NvXo6u06rFcQN9riLYZY7fIdxM/g2Xzd01nzpDAU4ZjLgwKJCw85qR\n\
VR6mF4TMCis+924fLnPIkcsIef0fJ+UBYUxc8/fyDQnINAAwfQmDctFl4ag2uTkp\n\
R/FVTsUC2C5Za3cgQik/JChByv+yT7Bzz2g7zHaFf10kRpPr6jlsVLAI8g82mq/9\n\
uf3nuHsIxrOya1cee4KKIqqzc10dKwIDAQABo4HTMIHQMB0GA1UdDgQWBBS+Nyt3\n\
3Kp1dV4gjoPz5o3/V08ZejB0BgNVHSMEbTBrgBS+Nyt33Kp1dV4gjoPz5o3/V08Z\n\
eqFIpEYwRDELMAkGA1UEBhMCVVMxGjAYBgNVBAcTEUNoZXllbm5lIE1vdW50YWlu\n\
MRkwFwYDVQQKExBTdGFyZ2F0ZSBDb21tYW5kggkA8E1OH4Iu9sMwDAYDVR0TBAUw\n\
AwEB/zArBgNVHR8EJDAiMCCgHqAchhpodHRwOi8vdGVzdGNhLmxvY2FsL2NhLmNy\n\
bDANBgkqhkiG9w0BAQUFAAOCAQEAowVOjxvkRPTvzFmCrRkYRTXrcBadg/RvVQU+\n\
DDbGyu6Fopw9lvjSL5jnfA1BOZwGK2/7/9T7tKi74j+VhrM9JiJCtToAwGnvDqRE\n\
FWvh7ULOqEzHLoRhzz9XaakkXtXcRXNM2ypVhWKuaNdRYhC4hAWYtxmlkWi1PI42\n\
sLbCpPN3OmKL1rM7kGeZckhWqnTsYRtw7aHc+Rlvbqh0D5gp4CPMy9FPh+pYerwV\n\
lTFliktKeuJf+8bTp4i9Dt4Que2uq4pEsh31UwWA08nkUcne8BY0QS8Ax07SZK1i\n\
VgE7ix12WW5QeQymud9cqkv4+LBtjvBuzByjydJd46oecqc76A==\n\
-----END CERTIFICATE-----";

const char clientKeyData [] = "\
-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpAIBAAKCAQEA3Yc68ZPIDSMhSWpMdgyOsLBfhqFrmGn3UK6o2lBnCQ3trCCT\n\
3t50FvnQRxA2uV8Wv8ksQM/TqYemn0W/dpbmZBApqTDWzr9JPZg6Wqn19GqxpM78\n\
LjDlu85BM3otmb7S2qMnRy9nrDXH11KMu3UzVVSdM/R9DQuR4FJxjS0mlHrrcxNr\n\
9zgHSQZ2AgYoAt/THOUwlVq5xK/61uF8YtRb6oDbblZSAk3cfRR0rptJZLDZgon9\n\
oPwAxlT5MEmb1X87sM7gPi44C/30mSC0mOQ3yILnAMtgl5eD/XV7BJdTXF7s0m3c\n\
U379M5Y2/ZWTBJYfDzEI5hwhjrrHFIGwYKYjAQIDAQABAoIBAQDPZGn3fl38hpYE\n\
MEMzBWCU5BoaZYbgotwJqeqksD82B9cpnToynVpbpDasQO6qru4SUNlgN/E1tqx0\n\
Ug0EjhNVPJh0MCf6WLTNuHVxKcr8ruJuo+UNrXN2VotOESUd2bt3OuVrzGKno5Xa\n\
7QqZ+Qg868FcwZhNHTbNpwFtpQaIW6mkX1V86IOTP539oeymiyneN9hawuw7BCEc\n\
mvAS2UsyOQYuDCwxIT0ooW6STGWylpBNFT1ohPtiKtbXXvOCIQ63qBmCpcEhhiuW\n\
YyxkfQdXqjmQ+DMg4nRHfp8KZVCd1NzK+UB82042tOA+/dQdtxsQwTFl/5FI3Wi1\n\
NAwXso+RAoGBAPCaZg1BxWqFGumXcSdYnyOs4cGmId8qBdcforE39mrguGxeMuXQ\n\
hf1IMIVU1kt7iWSIidI6bYCy/PDK/73uXkFNXdh5xamZ4f9i6FN1AhdLYIzSMPlD\n\
E7B3CUchpL2UrMPXEvWrG9ahoBn9sGE44NZDsVPu+wr8ZQSbPIMN+vpVAoGBAOu0\n\
V/58tfuTnoDf0/N1E2Inb14mf1W1Qyz6zzP9BMdHPfcxV1bcCQtQYmAu0Yv2JWEo\n\
sTIbUl8mdPbmVaibom/ea7bF2/zluvxZODrsCrNOQjZJJjAzCGnGW3xUDXbA0Vix\n\
wuwSuQgcfoxN22ScDtaczuUdT7bIe2WXuMGs0Mn9AoGANEOVv3N2Crl3Kx62ebGK\n\
gBlhwMrxuKRMOHqqwIxDftvps8CSldFCCc7Qm2XeVu8y3R+7sGr4tmgrAbNW8k8B\n\
MnY32Ho7Mag3q1400UwtdBnvkydlc//aGw8AeO8Z9gSBstjZSXwtw+W//WVaFwRf\n\
8qspre3PoodI+kMuVdmHlzECgYAd1i5tYIwI1J7z4WGLJ/7UJfnLCUSFLPTb9MPV\n\
6Ol58GK/r9dGf4QFkp9hXYvZE8eXjPchSKQct1mHB5FATrM5RgaMlG9MjCgnWYPs\n\
sYSJPhbTmF7lKxCdkyZxjpJFvhLCD36FM7mIdVLvn8tt6PNX53GpPxSgJrC4I4SX\n\
v7COYQKBgQCimn2yWFrdlbuIx1+CccKvmXnsxWGelS1wSGeUyUWmt0Vsn361eKJ2\n\
nqDCuduZkrmyR71prIjUbZmJH9ccbzNCKgTAmUgl8BLdWWYkGQRs8tkU66IXRhro\n\
FvTQbzqYTKK5NNKVVp7VMEotMFyIewjVPrwKlklkMBo/LJCawEtHEg==\n\
-----END RSA PRIVATE KEY-----";

const char serverKeyData [] = "\
-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpAIBAAKCAQEA0QfJPpwbF+rN0b/OAUz3+KnYG5qIv5d8q/nRiTnYRyNj2kN2\n\
2pHs7VdWD0bxOVk58rJFWMdIHAzUpB1PtQDL95XRlyj/zEIO7EjkdT5SG8lZJApv\n\
NN47pMh1aEJIVE4J+80e9pdRZke/v3aVjAMjBv1S6UVqLfZkLge0RFqhu1P+Z4bx\n\
k1skG3ORRAC9yMwOk+UDme1W4H0vBl5hTJTkvjwXiiPupQUt1ZnhYu7nYX/QIo2U\n\
Si7MQ9OPx0cfQY2xqzvP5Z7eOZYuOJ1Dtvhi1ms5rp+dBrA5Z0ivXh6AfHKthfXE\n\
h/oQ6WsCPq3HPmxjnNebV3MZAvL2+DPnwz8AUQIDAQABAoIBAE/BtsnK6pOPpgS4\n\
OLRKV/trYuf1aOQhuxIregXwKlPKbxki0MDJGSvePRoLKZOGNhQibU9Yim0ZgDE2\n\
delMaocHhTc2D5QySFtj3LfXlaDAjOCxkokLmppfWlzznYF9XVXUiFHynr519fXq\n\
G1mXANBz7CTRtZfgAY2GpwPFyRweWbuxPVu2zRPWEiAOfRRkz9Y6w9y4OoOOhXIb\n\
e6OdqJ06p+o2X5mGleuUypGa93Ov7G6FIO/5AGrleQJpOuEd0KfgMK7rJF/xJSJp\n\
jmdYZ6rzg484V0GDVoZR7jUftc0u+h+ZIgl/VRGrNlqAqSf0+/+BoR90Ug+ukivL\n\
kY4xVfkCgYEA711Rw79RSjGThWLdd1DadoA+fgvq27kjTmfZXp+ievRIpwnHYGOB\n\
0RlvvifqrDfSMvE67H3qjj2id9tf2v5UajQLvWmRse/uA+A682GGYDEtxo4FYMZw\n\
g5BD5zmIPFQqsCg2mfEijLXq+m+O7i0+ACu3b52NafW35lua8hC0d6cCgYEA347G\n\
IcI5OPPQdxstqjwOIkscf4pSwFIc6a7pHaFIh3K8CupmKRGuco9L4QfQJvJ71n91\n\
1lOWSC5Nt/S5xzz4w7YpFaBbBsln51B5PM/RENdfFcYfxNy73u/ZrSVdqeOPENpD\n\
ay6B3QMIX7YmpwUJvQ5rSZaVWDJDgIuV2qw5x0cCgYEAxZ468ixS1blmfPpps6da\n\
JfmtHm2CYhNQ1duhN6D+CRsDnHzTUX314IdqnQjhk3umnA5TQoqWiwbRyrigQ/I0\n\
CVac1q2A7vIShLqskOFUirgvfb0lLYfHvqbtmMoD8yLd93/LzH6rspZCwK43x+Ys\n\
SQHOuWTPKssGIDZDD1TBWgMCgYAeXODSEvP8pTiqBJn2JgPxamwrclj83+onXucw\n\
bAhSxSV/AgjwCQO9cXLn1EzUDVj+b/9MJbwif490rZtR0RkkU6hO1KGoUVB4XTiq\n\
JruJYwb6CRelgClQBRl2Z73J5is3Kb3AX+epYdEQVsf3JTsGX1/49afA+8qpOcwX\n\
kA6k3wKBgQChbTCvBcFs7eTxZnZRN13QW8QXec3LTrfvWlpfMfZklkORDAXGHGKt\n\
CMr4gN7eYmkXyjhGXym58j+DX+MArJ+OJK7dplzHk+XhqLwyMsSQl5I2WkzaIogH\n\
wsqSHAaOI8SZgJH6L4eXodm6sF5O2xKZlw16lVfuBB+XeoHjByTt4A==\n\
-----END RSA PRIVATE KEY-----";
