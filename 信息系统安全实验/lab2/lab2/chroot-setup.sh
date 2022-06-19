#!/bin/sh -x
if id | grep -qv uid=0; then
    echo "Must run setup as root"
    exit 1
fi

create_socket_dir() {
    local dirname="$1"
    local ownergroup="$2"
    local perms="$3"

    mkdir -p $dirname
    chown $ownergroup $dirname
    chmod $perms $dirname
}

set_perms() {
    local ownergroup="$1"
    local perms="$2"
    local pn="$3"

    chown $ownergroup $pn
    chmod $perms $pn
}

rm -rf /jail
mkdir -p /jail
cp -p index.html /jail

./chroot-copy.sh touchstone /jail

./chroot-copy.sh httpd /jail

./chroot-copy.sh filesv /jail
./chroot-copy.sh banksv /jail

./chroot-copy.sh /bin/bash /jail

./chroot-copy.sh /usr/bin/env /jail

# to bring in the crypto libraries
./chroot-copy.sh /usr/bin/openssl /jail

mkdir -p /jail/usr/lib /jail/usr/lib/i386-linux-gnu /jail/lib /jail/lib/i386-linux-gnu


mkdir -p /jail/usr/local/lib

cp /lib/i386-linux-gnu/libnss_dns.so.2 /jail/lib/i386-linux-gnu
cp /lib/i386-linux-gnu/libresolv.so.2 /jail/lib/i386-linux-gnu

#extra_files :  etc
mkdir -p /jail/etc

#extra_files
cp /etc/localtime /jail/etc/
cp /etc/timezone /jail/etc/
cp /etc/resolv.conf /jail/etc/

mkdir -p /jail/usr/share/zoneinfo
#extra_files
cp -r /usr/share/zoneinfo/America /jail/usr/share/zoneinfo/


#extra_files : tmp
mkdir -p /jail/tmp
chmod a+rwxt /jail/tmp

#extra_files : dev 
mkdir -p /jail/dev
mknod /jail/dev/urandom c 1 9

#copy directory: sql_lite3 , db
cp -r sql_lite3 /jail/
cp -r db        /jail/

#copy some static files , a.txt b.txt
#cp a.txt /jail/
#cp b.txt /jail/

#fill in your commands here
#using chgrp, chmod, chown and etc..
