fwb_ipf(1)               Firewall Builder              fwb_ipf(1)



NNAAMMEE
       fwb_ipf - Policy compiler for ipfilter

SSYYNNOOPPSSIISS
       ffwwbb__iippff [[--vvVVxx]] [[--dd wwddiirr]] --ff ddaattaa__ffiillee..xxmmll object_name


DDEESSCCRRIIPPTTIIOONN
       ffwwbb__iippff  is  a firewall policy compiler component of Fire­
       wall Builder (see fwbuilder(1)). This  compiler  generates
       code  for ipfilter. Compiler reads objects definitions and
       firewall description from the  data  file  specified  with
       "-f" option and generates ipfilter configuration files and
       firewall activation script.

       All generated files have names that start with the name of
       the firewall object. Firewall activation script has exten­
       sion ".fw" and is simple shell script that flushes current
       policy,  loads new filter and nat rules and then activates
       ipfilter.  IPFilter configuration file  name  starts  with
       the  name  of  the firewall object, plus "-ipf.conf".  NAT
       configuration file name also starts with the name  of  the
       firewall  object,  plus "-nat.conf". For example, if fire­
       wall object has name "myfirewall", then compiler will cre­
       ate  three  files:  "myfirewall.fw", "myfirewall-pf.conf",
       "myfirewall-nat.conf".

       The data file and the name of the firewall objects must be
       specified  on the command line. Other command line parame­
       ters are optional.



OOPPTTIIOONNSS
       -f FILE
              Specify the name of the data file to be  processed.


       -d wdir
              Specify   working   directory.   Compiler   creates
              firewall activation script and ipfilter  configura­
              tion files in this directory.  If this parameter is
              missing, then all files will be placed in the  cur­
              rent working directory.


       -v     Be  verbose:  compiler  prints  diagnostic messages
              when it works.


       -V     Print version number and quit.


       -x     Generate debugging information while working.  This
              option  is intended for debugging only and may pro­
              duce lots of cryptic messages.


NNOOTTEESS
       Support for ipf returned  in  version  1.0.1  of  Firewall
       Builder

       Supported features:



       o      both ipf.conf and nat.conf files are generated


       o      negation in policy rules


       o      stateful  inspection  in  individual  rule  can  be
              turned off in rule options dialog. By default  com­
              piler adds "keep state" or "modulate state" to each
              rule with action 'pass'


       o      rule options dialog provides a choice  of  icmp  or
              tcp rst replies for rules with action "Reject"


       o      compiler  adds  flag  "allow-opts"  if  match on ip
              options is needed


       o      compiler can generate rules matching on TCP flags


       o      compiler can generate script adding ip aliases  for
              NAT rules using addresses that do not belong to any
              interface of the firewall


       o      compiler always adds rule "block quick all" at  the
              very  bottom  of the script to ensure "block all by
              default" policy even if the policy is empty.


       o      Address ranges in both policy and NAT




       Features that are not supported (yet)


       o      negation in NAT


       o      custom services




       Features that won't be supported  (at  least  not  anytime
       soon)


       o      policy routing




UURRLL
       Firewall  Builder  home  page  is located at the following
       URL: hhttttpp::////wwwwww..ffwwbbuuiillddeerr..oorrgg//


BBUUGGSS
       Please report bugs using bug tracking  system  on  Source­
       Forge:

       hhttttpp::////ssoouurrcceeffoorrggee..nneett//ttrraacckkeerr//??ggrroouupp__iidd==55331144&&aattiidd==110055331144



SSEEEE AALLSSOO
       ffwwbbuuiillddeerr((11)),, ffwwbb__iipptt((11)),, ffwwbb__ppff((11))





FWB                                                    fwb_ipf(1)
