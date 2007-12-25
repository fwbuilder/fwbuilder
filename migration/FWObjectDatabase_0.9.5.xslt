<!--
   Filename:     0.9.5/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   08/23/2001
   Last changed: 09/19/2001
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.9.5 to v0.1.0
                 This transformation does the following:
		 1. changes version number
		 2. adds "host_OS" attribute to the Firewall element
		 3. changes name of the option "pkt_fwd" to "linux24_ip_forward"
		 4. adds option "accept_new_tcp_with_no_syn"
		 5. adds attribute "library" to all standard elements

   Precautions are made so this transformation can be applied twice.

   01/18/02      added namespace "fwbuilder" to template matching on
		 FWObjectDatabase
		 added exclude-result-prefixes attribute to stylesheet def.

-->

<xsl:stylesheet version="1.0"
       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwbuilder="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwbuilder"
>


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template match="@*|node()" mode="copy">
<xsl:copy>
  <xsl:apply-templates select="@*" mode="copy"/>
  <xsl:apply-templates mode="copy"/>
</xsl:copy>
</xsl:template>

<!--  Adding missing PolicyRuleOptions element ( bug #451490 ) -->

<xsl:template match="PolicyRule" mode="copy">
<xsl:copy>
  <xsl:apply-templates select="@*" mode="copy"/>
  <xsl:apply-templates mode="copy"/>
  <xsl:choose>
    <xsl:when test="PolicyRuleOptions">
    </xsl:when>
    <xsl:otherwise>
      <PolicyRuleOptions/>
<xsl:text>
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:copy>
</xsl:template>


<xsl:template match="FirewallOptions" mode="copy">
<xsl:choose>
  <xsl:when test="Option[@name='accept_new_tcp_with_no_syn']">
    <xsl:copy>
      <xsl:apply-templates mode="copy"/>
    </xsl:copy>
  </xsl:when>
  <xsl:otherwise>
    <xsl:copy>
<xsl:text>
</xsl:text>
      <Option name="accept_new_tcp_with_no_syn">True</Option>
      <xsl:apply-templates mode="copy"/>
    </xsl:copy>
  </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="Option[@name='pkt_fwd']" mode="copy">
<Option name="linux24_ip_forward">
<xsl:choose>
  <xsl:when test="node()='True'">1</xsl:when>
  <xsl:otherwise>0</xsl:otherwise>
</xsl:choose>
</Option>
</xsl:template>

<xsl:template match="Firewall" mode="copy">
<xsl:choose>
  <xsl:when test="@host_OS">
    <xsl:copy>
      <xsl:apply-templates select="@*" mode="copy"/>
      <xsl:apply-templates mode="copy"/>
    </xsl:copy>
  </xsl:when>
  <xsl:otherwise>
    <xsl:copy>
      <xsl:attribute name="host_OS">linux24</xsl:attribute>
      <xsl:apply-templates select="@*" mode="copy"/>
      <xsl:apply-templates mode="copy"/>
    </xsl:copy>
  </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="FWObjectDatabase">
<xsl:copy>
  <xsl:attribute name="version">0.10.0</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates mode="copy"/>
</xsl:copy>
</xsl:template>

<!--  adding attribute 'library' to all known standard elements -->

<xsl:template name="add_library">
<xsl:choose>
  <xsl:when test="@library">
    <xsl:copy>
      <xsl:apply-templates select="@*" mode="copy"/>
      <xsl:apply-templates mode="copy"/>
    </xsl:copy>
  </xsl:when>
  <xsl:otherwise>
    <xsl:copy>
      <xsl:attribute name="library">Standard</xsl:attribute>
      <xsl:apply-templates select="@*" mode="copy"/>
      <xsl:apply-templates mode="copy"/>
    </xsl:copy>
  </xsl:otherwise>
</xsl:choose>
</xsl:template>


<xsl:template match="*[@id='sysid0']"  mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='sysid1']"  mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='sysid2']"  mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='sysid3']"  mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid01']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid04']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid02']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid03']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid05']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid06']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid07']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid08']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid09']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid10']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid11']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid12']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='stdid13']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>

<xsl:template match="*[@id='sg-DHCP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='sg-NETBIOS']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='sg-Useful_ICMP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEDD9']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEFFA']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FF35E']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='icmp-Unreachables']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='icmp-Host_unreach']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='icmp-ping_reply']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='icmp-ping_request']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='icmp-Port_unreach']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='icmp-Time_exceeded']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='icmp-Time_exceeded_in_transit']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='ip-IPSEC']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='ip-RR']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='ip-SRR']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='ip-IP_Fragments']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-ALL_TCP_Masqueraded']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-All_TCP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AEDBEAC']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-Auth']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AEDBE6E']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-DNS_zone_transf']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEDA3']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF774']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-FTP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-FTP_data']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-HTTP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FED69']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF776']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FED9F']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FF13C']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEE21']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEE23']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF778']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FF000']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AED0D6D']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEEEE']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEE7A']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-NNTP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEE1D']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FF0EA']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF782']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEF7C']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF77A']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF77C']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF77E']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEF34']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-SMTP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FF04C']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEE76']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FF09A']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-SSH']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AEDBE00']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-TCP-SYN']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-Telnet']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='tcp-uucp']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FF1B8']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-ALL_UDP_Masqueraded']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-All_UDP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AED0D6B']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEDA1']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-bootpc']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-bootps']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AEDBE70']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-DNS']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEDA5']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEDA9']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEDA7']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEDAB']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-netbios-dgm']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-netbios-ns']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-netbios-ssn']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEE78']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-ntp']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEF7E']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B4FEE1F']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='udp-SNMP']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AED0D69']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AEDBE19']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AECF780']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AED0D67']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3AED0D8C']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B64EEA8']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B64EF4E']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B64EF50']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B64EF52']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='id3B64EF54']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='int-workhours']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='int-weekends']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>
<xsl:template match="*[@id='int-afterhours']" mode="copy"><xsl:call-template name="add_library"/></xsl:template>

</xsl:stylesheet>

