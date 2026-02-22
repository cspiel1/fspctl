#!/bin/bash
. /etc/profile
echo
date
paccache -r --keep 0
pacman -Syuq --noconfirm || { echo 'pacman failed!'; false; exit 1; }
paccache -r --keep 0

echo "Update successful, rebooting..."
sync
reboot
