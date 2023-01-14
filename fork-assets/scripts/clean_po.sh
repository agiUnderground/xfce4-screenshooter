#!/bin/sh
filename=$1
sed -i ".bak" '/imgur-dialog.c:128/d' $filename
sed -i ".bak" '/imgur-dialog.c:130/d' $filename
sed -i ".bak" '/screenshooter-dialogs.c:1318/,/^$/d' $filename
sed -i ".bak" '/screenshooter-imgur.c:133/,/^$/d' $filename
sed -i ".bak" '/screenshooter-imgur.c:166/,/^$/d' $filename
sed -i ".bak" '/screenshooter-imgur.c:231/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:1/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:2/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:3/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:4/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:5/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:6/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:7/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:8/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:9/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:10/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:11/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:12/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:13/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:14/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:15/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:16/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:17/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:18/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:19/,/^$/d' $filename
sed -i ".bak" '/imgur-dialog.ui.h:20/,/^$/d' $filename
sed -i ".bak" '/host it on imgur, a free online image hosting service./d' $filename
sed -i ".bak" 's/using another application, or /using another application./g' $filename
sed -i ".bak" '/screenshooter-dialogs.c:1302/,/^$/d' $filename
sed -i ".bak" '/screenshooter-dialogs.c:1306/,/^$/d' $filename
sed -i ".bak" '/main.c:98/,/^$/d' $filename
sed -i ".bak" '/job-callbacks.c:172/d' $filename
sed -i ".bak" '/job-callbacks.c:173/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:73/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:170/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:213/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:221/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:227/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:234/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:240/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:248/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:254/,/^$/d' $filename
sed -i ".bak" '/screenshooter-job-callbacks.c:262/,/^$/d' $filename
