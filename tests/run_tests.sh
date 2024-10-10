#!/usr/bin/bash

# Reset
Color_Off='\033[0m'       # Text Reset

# Regular Colors
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

# Bold
BBlack='\033[1;30m'       # Black
BRed='\033[1;31m'         # Red
BGreen='\033[1;32m'       # Green
BYellow='\033[1;33m'      # Yellow
BBlue='\033[1;34m'        # Blue
BPurple='\033[1;35m'      # Purple
BCyan='\033[1;36m'        # Cyan
BWhite='\033[1;37m'       # White

echo -e "$BYellow ======================================="
echo -e "           WEBSERV TEST SUITE"
echo -e " =======================================\n\n\n\n"

hurl_bin="tests/hurl/bin/hurl"


echo -e "$BPurple (+) TEST : Deux serveurs ecoutent sur le meme port$Color_Off"
./webserv tests/01_test-2-servers/conf/test-2-servers.conf  > /dev/null &
bg_pid=$!
echo "starting server pid : $bg_pid"
# sleep 1
$hurl_bin --test tests/01_test-2-servers/test-2-servers.hurl
kill $bg_pid
echo
echo


echo -e "$BPurple (+) TEST : Deux serveurs deux ports differents$Color_Off"
./webserv tests/02_test_deux_ports/conf/test-2-ports.conf > /dev/null &
bg_pid=$!
echo "starting server pid : $bg_pid"
# sleep 1
$hurl_bin --test tests/02_test_deux_ports/test-2-ports.hurl
kill $bg_pid
echo
echo

echo -e "$BPurple (+) TEST : Series de tests simple sur un seul server $Color_Off"

touch tests/03_server_simple/server-files/file_forbidden.html
chmod 0 tests/03_server_simple/server-files/file_forbidden.html

./webserv tests/03_server_simple/conf/server_simple.conf > /dev/null &
bg_pid=$!
echo "starting server pid : $bg_pid"
# sleep 1
$hurl_bin --test tests/03_server_simple/test-server_simple.hurl
kill $bg_pid

rm -f tests/03_server_simple/server-files/file_forbidden.html
echo
echo

# tests a implementer:
# serveur simple
# 404 403 requet GET simple requete POST simple meme serveur sur deux ports differents
