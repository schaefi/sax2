#!/bin/sh

################
# FILE         : getPrimary
################
# PROJECT      : SaX2 ( SuSE advanced X11 configuration )
#              :
# DESCRIPTION  : figure out primary VGA card and print out
#              : SaX2 ChipID as seen in 'sax2 -p' output
#              :
# AUTHOR       : Stefan Dirsch <sndirsch@suse.de>
#              :
# STATUS       : quick & dirty :-)
################

# last resort: let's assume first SaX2 ChipID is the primary device
fallback() {
  echo 0
  exit
}

bus_slot_function=""
sax_id=""

# "Luc told me how to figure out the primary gfx card. It's marked in 
#  the PCI config space. You can see it in a "lspci -v -v" output. Check  
#  for "Control I/O+" instead of "Control I/O-" for the VGA compatible
#  devices."
for busid in $(/sbin/lspci -n | grep 0300 | cut -d " " -f 1); do
  /sbin/lspci -s $busid -vv | grep -q "Control: I/O+"
  if test $? -eq 0; then
    bus_slot_function=$busid
    break;
  fi
done

test "$bus_slot_function" == "" && fallback

# search for SaX2 ChipID ("sysp -c" is the same as "sax2 -p")
sax_id=$(/usr/sbin/sysp -c | \
         grep $bus_slot_function | \
         awk '{print $2}')
test "$sax_id" == "" && fallback

# SaX2 ChipID for primary device
echo $sax_id
