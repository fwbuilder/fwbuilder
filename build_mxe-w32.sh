#!/bin/bash
JOBS=$(nproc --all)
export PATH=/usr/lib/mxe/usr/bin:$PATH
export TOOLCHAIN_PREFIX=/usr/lib/mxe/usr/i686-w64-mingw32.shared
export CXXFLAGS="-Wno-maybe-uninitialized -Wno-attributes"

main()
{
    if [ $# -eq 0 ]; then
        usage
        exit
    fi

    while [ "$1" != "" ]; do
        case $1 in
        -h | --help | help | usage )
            usage
            exit
            ;;
        all )
            configure
            compile
            package
            exit
            ;;
        configure )
            configure
            ;;
        compile )
            compile
            ;;
        package )
            package
            ;;
        * )
            usage
            exit 1
            ;;
        esac
        shift
    done
}

usage()
{
    echo "Usage: $0 [ all | configure | compile | package ]"
}

configure()
{
    echo "==> Configuring"
    ./autogen.sh \
      --with-xml2-config=/usr/lib/mxe/usr/i686-w64-mingw32.shared/bin/xml2-config \
      --with-xslt-config=/usr/lib/mxe/usr/i686-w64-mingw32.shared/bin/xslt-config \
      --with-qtdir=/usr/lib/mxe/usr/i686-w64-mingw32.shared/qt5 \
      --host=i686-w64-mingw32.shared
    if [ $? -eq 0 ]; then
        echo "==> Done configuring"
    fi
}

compile()
{
    echo "==> Compiling"
    make -j${JOBS}
    if [ $? -eq 0 ]; then
        echo "==> Done compiling"
    fi
}

package()
{
    echo "==> Packaging"
    makensis -nocd packaging/fwbuilder.nsi
    if [ $? -eq 0 ]; then
        echo "==> Done packaging"
    fi
}

main "$@"
