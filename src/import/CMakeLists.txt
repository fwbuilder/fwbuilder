set(import_srcs
    getProtoByName.cpp
    PreImport.cpp
    serviceObjectMaker.cpp
    objectMaker.cpp
    PIXImporter.cpp
    getServByName.cpp
    PIXImporterNat.cpp
    IPTImporterRun.cpp
    objectSignature.cpp
    Importer.cpp
    IPTImporter.cpp
    PFImporter.cpp
    PFImporterRun.cpp
    IOSImporterRun.cpp
    IOSImporter.cpp
    addressObjectMaker.cpp
    QStringListOperators.cpp
    PIXImporterRun.cpp)

add_library(import STATIC ${import_srcs})

target_include_directories(import PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(import compilerdriver antlr Qt5::Core Qt5::Widgets)

target_compile_options(import PRIVATE ${CXX_DEFAULT_FLAGS})

