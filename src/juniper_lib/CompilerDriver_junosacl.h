#ifndef __COMPILER_DRIVER_JUNOSACL_HH__
#define __COMPILER_DRIVER_JUNOSACL_HH__

#include "CompilerDriver.h"

#include <string>
#include <sstream>

#include <QTextStream>

namespace libfwbuilder {
    class FWObjectDatabase;
    class Cluster;
    class ClusterGroup;
    class Firewall;
    class RuleSet;
    class Interface;
};

namespace fwcompiler {

    class NamedObjectsManager;

    class CompilerDriver_junosacl : public CompilerDriver
    {
protected:
    std::string system_configuration_script;
    std::string policy_script;
    std::string comment_symbol;

    void printProlog(QTextStream &file, const std::string &prolog_code);

    virtual QString assembleManifest(libfwbuilder::Cluster *cluster,
                                     libfwbuilder::Firewall *fw,
                                     bool cluster_member);
    virtual QString printActivationCommands(libfwbuilder::Firewall *fw);
    virtual QString assembleFwScript(libfwbuilder::Cluster *cluster,
                                     libfwbuilder::Firewall* fw,
                                     bool cluster_member,
                                     OSConfigurator *oscnf);

public:

    CompilerDriver_junosacl(libfwbuilder::FWObjectDatabase *db);

    // create a copy of itself, including objdb
    virtual CompilerDriver* clone();

    virtual QString run(const std::string &cluster_id,
                        const std::string &firewall_id,
                        const std::string &single_rule_id);

    };
}

#endif // __COMPILER_DRIVER_JUNOSACL_HH__
