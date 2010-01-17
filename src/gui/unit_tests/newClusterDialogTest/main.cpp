#include "newClusterDialogTest.h"
#include "FWWindow.h"
#include "FWBSettings.h"
#include "common/commoninit.h"

#include <iostream>

int fwbdebug = 0;
FWWindow *mw = NULL;
FWBSettings *st = NULL;
char** args;
QApplication *app = NULL;

using namespace libfwbuilder;
using namespace std;


int main( int argc, char** argv)
{
    app = new QApplication(argc, argv, 0);
    app->setOrganizationName(QLatin1String("NetCitadel"));
    app->setApplicationName(QLatin1String("Firewall Builder"));
    st = new FWBSettings();
    st->init();
    init(argv);
    respath = std::string(PREFIX) + "/share/libfwbuilder";
    std::string full_res_path = std::string(PREFIX) + "/resources/resources.xml";
    try
    {
        new Resources(full_res_path);
    } catch (FWException &e)
    {
        cerr << "Can not read resource file from " << full_res_path << endl;
    }
    newClusterDialogTest tst;
    QTest::qExec(&tst);
}

//QTEST_MAIN(newClusterDialogTest)
