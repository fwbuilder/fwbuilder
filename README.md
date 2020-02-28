[![Build Status](https://travis-ci.org/fwbuilder/fwbuilder.svg?branch=master)](https://travis-ci.org/fwbuilder/fwbuilder)

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

CentOS-8
--------

Add the **Repository** to the configuration directory under CentOS 8 by creating a file under the path `/etc/yum.repos.d` with path and filename `/etc/yum.repos.d/tachtler-fwbuilder-epel-8.repo` and the content:
```
[copr:copr.fedorainfracloud.org:tachtler:fwbuilder]
name=Copr repo for fwbuilder owned by tachtler
baseurl=https://download.copr.fedorainfracloud.org/results/tachtler/fwbuilder/epel-8-$basearch/
type=rpm-md
skip_if_unavailable=True
gpgcheck=1
gpgkey=https://download.copr.fedorainfracloud.org/results/tachtler/fwbuilder/pubkey.gpg
repo_gpgcheck=0
enabled=1
enabled_metadata=1
```

Compare the **GPG-Key - FINGERPRINT** for the **Repository**, using following command  
`# wget -qO- https://download.copr.fedorainfracloud.org/results/tachtler/fwbuilder/pubkey.gpg | gpg --import-options show-only --import --with-fingerprint`   
the **output** shows the correct **FINGERPRINT**
```
pub   rsa2048 2020-02-18 [SCEA] [expires: 2025-02-16]
      4FD0 EFC8 4C70 F79D FFC4  6126 C0DA 0AFE DED4 668B
uid                      tachtler_fwbuilder (None) <tachtler#fwbuilder@copr.fedorahosted.org>
```
Install the **GPG-Key** for the **Repository only, if the FINGERPRINT matches**, with following command  
`rpm --import https://download.copr.fedorainfracloud.org/results/tachtler/fwbuilder/pubkey.gpg`

Install the **base** RPM-Package with following command:  
`# dnf install fwbuilder`  
and the **required module** RPM-Package, for **example** the module for **iptables** with  
`# dnf install fwbuilder-ipt`  
