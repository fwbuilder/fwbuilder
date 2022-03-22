This is a directory supporting RPM builds on Red Hat releases.  It has
been tested on Fedora 34, but should be close for Fedora 35+ and
(possibly) current versions of CentOS.

Installation instructions
=========================

Create the spec file and tarball generation script with the proper
version number:
```
 ./mkfiles.sh
```
Manually edit the release number in the generated spec file if
necessary.  Then run:
```
 ./mktarball.sh
 rpmbuild -bs --build-in-place fwbuilder.spec
```
This will create a source RPM in your rpmbuild/SRPMS directory.  From
there, follow the usual rpmbuild steps to create the binary packages.

By default the mktarball.sh script creates the tarball from the master
branch.  To create the tarball from another branch, use the -b option:
```
 ./mktarball.sh -b anotherbranch
```
To create a tarball from the current working tree, use the -c option:
```
 ./mktarball.sh -c
```
