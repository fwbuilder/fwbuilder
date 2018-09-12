FROM ubuntu:18.04

# We need this to use apt-key
RUN apt-get update && apt-get install -y \
    dirmngr

# Add mxe repos
RUN echo "deb http://pkg.mxe.cc/repos/apt/debian wheezy main" > \
    /etc/apt/sources.list.d/mxeapt.list && \
    apt-key adv --keyserver keyserver.ubuntu.com --recv-keys D43A795B73B16ABE9643FE1AFD8FFF16DB45C6AB

RUN apt-get update && apt-get install -y \
    autoconf \
    automake \
    autopoint \
    bash \
    bison \
    bzip2 \
    flex \
    gettext \
    git \
    g++ \
    g++-multilib \
    gperf \
    intltool \
    libc6-dev-i386 \
    libffi-dev \
    libgdk-pixbuf2.0-dev \
    libtool \
    libltdl-dev \
    libssl-dev \
    libxml-parser-perl \
    make \
    openssl \
    p7zip-full \
    patch \
    perl \
    pkg-config \
    python \
    ruby \
    scons \
    sed \
    unzip \
    wget \
    xz-utils \
    nsis \
    qbs \
    mxe-i686-w64-mingw32.shared-qtbase \
    mxe-i686-w64-mingw32.shared-libxml2 \
    mxe-i686-w64-mingw32.shared-libxslt

WORKDIR /root
