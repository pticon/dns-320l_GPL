#0     to restore default color
color_default="\E[0m"
#1     for brighter colors
color_bright="\E[1m"
#4     for underlined text
color_uline="\E[4m"
#5     for flashing text
color_flash="\E[5m"
#30     for black foreground
fcolor_black="\E[30m"
#31     for red foreground
fcolor_red="\E[31m"
#32     for green foreground
fcolor_green="\E[32m"
#33     for yellow (or brown) foreground
fcolor_yellow="\E[33m"
#34     for blue foreground
fcolor_blue="\E[34m"
#35     for purple foreground
fcolor_purple="\E[35m"
#36     for cyan foreground
fcolor_cyan="\E[36m"
#37     for white (or gray) foreground
fcolor_white="\E[37m"
#40     for black background
bcolor_black="\E[40m"
#41     for red background
bcolor_red="\E[41m"
#42     for green background
bcolor_green="\E[42m"
#43     for yellow (or brown) background
bcolor_yellow="\E[43m"
#44     for blue background
bcolor_blue="\E[44m"
#45     for purple background
bcolor_purple="\E[45m"
#46     for cyan background
bcolor_cyan="\E[46m"
#47     for white (or gray) background
bcolor_white="\E[47m"


#### 說明

#echo -e "\E[32m"
#echo wow
#echo -e "\E[0m"
#echo wow

#第一行除了 32 外其它字串 echo -e \E[  m 皆是必備的

#32 代表前景顯示綠色
#0 代表回復預設顏色
#
#搭配前景背景 → echo -e "\E[41;32m"，使用分號分隔，前後順序沒有差別
#
#ANSI 顏色碼，從 man 查來的 → man 5 dir_colors

