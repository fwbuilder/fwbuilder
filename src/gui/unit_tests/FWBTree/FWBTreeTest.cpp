#include <QDebug>
#include "FWBTreeTest.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"

using namespace libfwbuilder;
using namespace std;

void FWBTreeTest::isSystem()
{
    FWBTree fwbtree;

    FWObjectDatabase db;
    CPPUNIT_ASSERT(fwbtree.isSystem(&db) == true);

    Library *lib = db.createLibrary(4);
    CPPUNIT_ASSERT(fwbtree.isSystem(lib) == true);
    lib = db.createLibrary(7);
    CPPUNIT_ASSERT(fwbtree.isSystem(lib) == true);
    lib = db.createLibrary(1);
    CPPUNIT_ASSERT(fwbtree.isSystem(lib) == false);

    lib = Library::cast(fwbtree.createNewLibrary(&db));
    FWObject *slot = fwbtree.getStandardSlotForObject(lib, Firewall::TYPENAME);
    CPPUNIT_ASSERT(fwbtree.isSystem(slot) == true);

    Firewall fw;
    CPPUNIT_ASSERT(fwbtree.isSystem(&fw) == false);
}
