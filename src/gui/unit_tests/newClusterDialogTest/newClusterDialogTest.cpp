#include "newClusterDialogTest.h"

#include <QDebug>

using namespace libfwbuilder;
using namespace std;

void newClusterDialogTest::runTest()
{
    init();
    FWObjectDatabase *db = new FWObjectDatabase();
    db->load("test_data.fwb", &XMLTools::UpgradePredicate(), string(PREFIX) + "/share/libfwbuilder");
    newClusterDialog *dialog = new newClusterDialog(db);
    vector<FWObject*> fws;
    foreach(FWObject *obj, db->getByTypeDeep(Firewall::TYPENAME))
    {
        fws.push_back(obj);
    }

    dialog->setFirewallList(fws, true);
    dialog->exec();

}
