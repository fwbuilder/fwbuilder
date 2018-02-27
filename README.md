[![Build Status](https://travis-ci.org/fwbuilder/fwbuilder.svg?branch=master)](https://travis-ci.org/fwbuilder/fwbuilder)

**Note**: This is the 5.3 branch of Firewall Builder. This branch will keep C++98 compatibility, while the master branch will require newer C++ standards. All development is done on the master branch, and deprecated language and library features are removed to clean up code. Fixes and new features may be backported to this branch when possible. Testing, however, will mainly be done on the master branch.

fwbuilder
=========

Firewall Builder is a GUI firewall management application for iptables, PF, Cisco ASA/PIX/FWSM, Cisco router ACL and more. Firewall configuration data is stored in a central file that can scale to hundreds of firewalls managed from a single UI.


Installation instructions
=========================


Ubuntu
---------

The instructions are inspired by the well-meaning but out of date instructions available from the fwbuilder.org website: http://www.fwbuilder.org/4.0/docs/users_guide5/compile_from_source.shtml

run the following apt-get install command to install the compile requirements:

```
 sudo apt-get install git automake autoconf libtool libxml2-dev libxslt-dev libsnmp-dev qt5-default qttools5-dev-tools
 git clone https://github.com/fwbuilder/fwbuilder.git
 cd fwbuilder
 ./autogen.sh
 make
 sudo make install
```
Then, if you are lucky, you should be able to type
```
 fwbuilder &
```
At the prompt and get it to run. There is also a GUI icon you can use to start the program. 


