
#include <string>

#include "CompilerDriver_junosacl.h"

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

#include <QString>

#ifdef _WIN32
string fs_separator = "\\";
#else
string fs_separator = "/";
#endif

CompilerDriver_junosacl::CompilerDriver_junosacl(FWObjectDatabase *db) :
    CompilerDriver(db)
{
    comment_symbol = "#";
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_junosacl::clone()
{
    CompilerDriver_junosacl* new_cd = new CompilerDriver_junosacl(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

void CompilerDriver_junosacl::printProlog(QTextStream &file, const string &prolog_code)
{
    file << endl;
    file << "/*" << endl;
    file << " * Prolog script" << endl;
    file << " */" << endl;
    file << prolog_code << endl;
    file << "/*" << endl;
    file << " * End of prolog script" << endl;
    file << " */" << endl;
}
