#! /usr/bin/sh

# check if root
if [ "$EUID" -ne 0 ];then
  echo "[FE] You need to run this script as root."
  exit
fi

# build
./build.sh

# install
install -m 755 -oroot bin/pcsh /usr/bin/
install -m 755 -oroot bin/libpcsh.so /usr/lib/

# add shell to /etc/shells if it isnt there already
if [ $(grep -Eic pcsh /etc/shells) != 2 ]
then
  echo "/usr/bin/pcsh" >> /etc/shells
  echo "/bin/pcsh" >> /etc/shells
fi

# done
echo "[I] Done."