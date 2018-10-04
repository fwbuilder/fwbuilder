#!/bin/bash
export PATH=/usr/lib/mxe/usr/bin:$PATH

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
    qbs setup-toolchains /usr/lib/mxe/usr/bin/i686-w64-mingw32.shared-g++ mingw32
    qbs setup-qt /usr/lib/mxe/usr/i686-w64-mingw32.shared/qt5/bin/qmake qt
    if [ $? -eq 0 ]; then
        echo "==> Done configuring"
    else
        exit 1
    fi
}

compile()
{
    echo "==> Compiling"
    qbs release profile:qt
    if [ $? -eq 0 ]; then
        echo "==> Done compiling"
    else
        exit 1
    fi
}

package()
{
    echo "==> Packaging"
    makensis release/install-root/fwbuilder.nsi
    if [ $? -eq 0 ]; then
        echo "==> Done packaging"
    else
        exit 1
    fi
}

main "$@"
