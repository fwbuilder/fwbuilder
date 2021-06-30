[![Linux](https://github.com/fwbuilder/fwbuilder/workflows/Linux/badge.svg)](https://github.com/fwbuilder/fwbuilder/actions?query=workflow%3ALinux)
[![macOS](https://github.com/fwbuilder/fwbuilder/workflows/macOS/badge.svg)](https://github.com/fwbuilder/fwbuilder/actions?query=workflow%3AmacOS)
[![w32-mxe](https://github.com/fwbuilder/fwbuilder/workflows/w32-mxe/badge.svg)](https://github.com/fwbuilder/fwbuilder/actions?query=workflow%3Aw32-mxe)

fwbuilder
=========

Firewall Builder is a GUI firewall management application for iptables, PF, Cisco ASA/PIX/FWSM, Cisco router ACL and more. Firewall configuration data is stored in a central file that can scale to hundreds of firewalls managed from a single UI.


Installation instructions
=========================


Ubuntu
---------
```
 sudo apt install git cmake libxml2-dev libxslt-dev libsnmp-dev qt5-default qttools5-dev-tools
 git clone https://github.com/fwbuilder/fwbuilder.git
 mkdir build
 cd build
 cmake ../fwbuilder
 make
 sudo make install
```
Note: default destination is /usr/local. This is configurable:
```
 cmake ../fwbuilder -DCMAKE_INSTALL_PREFIX=/usr
```

Create deb package
---------
```
debuild -us -uc --lintian-opts --profile debian
```
