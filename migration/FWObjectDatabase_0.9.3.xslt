<!--
   Filename:     0.9.3/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   07/29/2001
   Last changed: 07/29/2001
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.9.3 to v0.9.4
                 This transformation only changes version number.
-->

<xsl:stylesheet version="1.0"
       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template match="FWObjectDatabase">
  <FWObjectDatabase version="0.9.4">
  <xsl:apply-templates/>
  </FWObjectDatabase>
</xsl:template>

<xsl:template match="ServiceGroup[attribute::id='stdid05']">
  <ServiceGroup id="stdid05" name="Services">
    <xsl:apply-templates/>
    <ServiceGroup id="stdid13" name="Custom_Services">
<CustomService comment="works in iptables and requires patch-o-matic" id="id3B64EEA8" name="rpc">
<CustomServiceCommand platform="Undefined"></CustomServiceCommand>
<CustomServiceCommand platform="ipfilter"></CustomServiceCommand>
<CustomServiceCommand platform="iptables">-m record_rpc</CustomServiceCommand>
</CustomService>
<CustomService comment="IRC connection tracker, supports DCC. Works with iptables and requires patch-o-matic" id="id3B64EF4E" name="irc-conn">
<CustomServiceCommand platform="Undefined"></CustomServiceCommand>
<CustomServiceCommand platform="ipfilter"></CustomServiceCommand>
<CustomServiceCommand platform="iptables">-m irc</CustomServiceCommand>
</CustomService>
<CustomService comment="Port scan detector, works only with iptables and  requires patch-o-matic " id="id3B64EF50" name="psd">
<CustomServiceCommand platform="Undefined"></CustomServiceCommand>
<CustomServiceCommand platform="ipfilter"></CustomServiceCommand>
<CustomServiceCommand platform="iptables">-m psd --psd-weight-threshold 5 --psd-delay-threshold 10000</CustomServiceCommand>
</CustomService>
<CustomService comment="Matches a string in a whole packet, works with iptables and requires patch-o-matic" id="id3B64EF52" name="string">
<CustomServiceCommand platform="Undefined"></CustomServiceCommand>
<CustomServiceCommand platform="ipfilter"></CustomServiceCommand>
<CustomServiceCommand platform="iptables">-m string --string test_pattern</CustomServiceCommand>
</CustomService>
<CustomService comment="Talk protocol support. Works with iptables and requires patch-o-matic" id="id3B64EF54" name="talk">
<CustomServiceCommand platform="Undefined"></CustomServiceCommand>
<CustomServiceCommand platform="ipfilter"></CustomServiceCommand>
<CustomServiceCommand platform="iptables">-m talk</CustomServiceCommand>
</CustomService>
    </ServiceGroup>
  </ServiceGroup>
</xsl:template>

<xsl:template match="*">
<xsl:copy-of select="."/>
</xsl:template>


</xsl:stylesheet>

