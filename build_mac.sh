#!/bin/bash
export QMAKE_MAC_SDK="macosx"
export QMAKESPEC="macx-clang"
export QMAKE_MACOSX_DEPLOYMENT_TARGET=10.9

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
            bundle
            exit
            ;;
        configure )
            configure
            ;;
        compile )
            compile
            ;;
        bundle )
            bundle
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
    echo "Usage: $0 [ all | configure | compile | bundle ]"
}

configure()
{
    echo "==> Configuring"
    qbs setup-toolchains --detect
    qbs setup-qt $(which qmake) qt
    if [ $? -eq 0 ]; then
        echo "==> Done configuring"
    else
        exit 1
    fi
}

compile()
{
    echo "==> Compiling"
    qbs config:release profile:qt
    if [ $? -eq 0 ]; then
        echo "==> Done compiling"
    else
        exit 1
    fi
}

bundle()
{
    BUNDLE=fwbuilder.app
    BIN=${BUNDLE}/Contents/MacOS

    echo "==> Bundling"
    qbs install --install-root . config:release profile:qt

    echo "==> Stripping binaries"
    strip ${BIN}/*

    echo "==> Running macdeployqt"
    macdeployqt ${BUNDLE} -executable=${BIN}/fwbedit -executable=${BIN}/fwb_ipt -executable=${BIN}/fwb_pf -executable=${BIN}/fwb_ipf -executable=${BIN}/fwb_ipfw -executable=${BIN}/fwb_iosacl -executable=${BIN}/fwb_nxosacl -executable=${BIN}/fwb_pix -executable=${BIN}/fwb_procurve_acl -executable=${BIN}/fwb_junosacl

    if [ $? -eq 0 ]; then
        echo "==> Done bundling"
        echo "    To open bundle:"
        echo "    # open ${BUNDLE}/"
    else
        exit 1
    fi
}

main "$@"
