1. chmod -R 777 *

2. ./alpha_config.sh

3. add -lmipc in /src/Makefile

3. make

2. 若使用DNS-327L的toolchain編譯會使得在mt-daapd.conf裡的設定mp3_tag_codepage指定非utf-8的編碼時可能造成mt-daapd重新執行時掛掉，目前暫時的解法是使用DNS-320L的toolchain來編譯，則不的有這個問題