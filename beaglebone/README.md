Beagle Bone fspctl
==================

This beagle bone is located at the FSP power converter in the cellar.

Modbus connections
------------------

- FSP power converter with fspctl
- SDM72D smart meter with smd72dc
- Installations
- /usr/local/lib/libmodbus*
- /usr/local/bin/
  fspctl  magic-home  magicd  modbusctl  pg  sdm72dc

Automatic System Upgrade
------------------------

- /lib/systemd/system/arch-upgrade.timer
- /lib/systemd/system/arch-upgrade.service
- /home/alarm/pacman-upgrade.sh
