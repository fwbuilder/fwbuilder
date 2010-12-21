<!--
   Filename:     0.10.0/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   11/12/2001
   Last changed: 11/12/2001
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.0 to v0.10.1
                 This transformation changes version number,
		 adds child element Interface for each Host
		 changes attribute "ext" to "security_level" in Interface

   01/18/02      added exclude-result-prefixes attribute to stylesheet def.

-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwbuilder="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwbuilder"
>


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<!-- <xsl:strip-space elements="*"/> -->

<xsl:template match="@*|node()" mode="copy">
<xsl:copy>
  <xsl:apply-templates select="@*" mode="copy"/>
  <xsl:apply-templates mode="copy"/>
</xsl:copy>
</xsl:template>


<xsl:template match="Host">
<xsl:variable name="hid"  select="@id"/>
<xsl:variable name="addr" select="@address"/>
<xsl:copy>
  <xsl:apply-templates select="@*" mode="copy"/>
  <xsl:text>
  </xsl:text>
  <xsl:choose>
    <xsl:when test="Interface">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <Interface name="unknown" dyn="False" security_level="0" netmask="255.255.255.255">
        <xsl:attribute name="address"><xsl:value-of select="$addr"/></xsl:attribute>
        <xsl:attribute name="id"><xsl:value-of select="$hid"/>-i</xsl:attribute>
      </Interface>
    </xsl:otherwise>
  </xsl:choose>
<!--  <xsl:apply-templates mode="copy"/> -->
</xsl:copy>
</xsl:template>

<xsl:template match="Interface[attribute::ext]">
<xsl:copy>
  <xsl:copy-of select="@address"    />
  <xsl:copy-of select="@dyn"        />
  <xsl:copy-of select="@name"       />
  <xsl:copy-of select="@netmask"    />
  <xsl:copy-of select="@id"         />
  <xsl:copy-of select="@physAddress"/>
  <xsl:choose>
    <xsl:when test="@ext='True'">
      <xsl:attribute name="security_level">0</xsl:attribute>
    </xsl:when>
    <xsl:otherwise>
      <xsl:attribute name="security_level">100</xsl:attribute>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates mode="copy"/>
</xsl:copy>
</xsl:template>

<xsl:template match="FirewallOptions">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
<xsl:text>
</xsl:text>
    <xsl:choose>
      <xsl:when test="Option[attribute::name='accept_established']">
        <xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
<Option name="accept_established">True</Option>
        <xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

<xsl:template match="Option[attribute::name='manage_arp']">
  <xsl:variable name='val' select='node()'/>
  <Option name="manage_virtual_addr">
  <xsl:value-of select='$val'/>
  </Option>
</xsl:template>

<xsl:template match="ObjectGroup[attribute::name='Hosts']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="ObjectGroup[attribute::name='Firewalls']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="Firewall">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="ObjectGroup[attribute::name='Objects']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="ServiceGroup[attribute::name='Services']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:choose>
      <xsl:when test="CustomService[attribute::id='stdid14']">
        <xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
<xsl:text>
</xsl:text>
<CustomService comment="This service matches all packets which are part of network connections established through the firewall, or connections 'related' to those established through the firewall. Term 'established' refers to the state tracking mechanism which exists inside iptables and other stateful firewalls and does not mean any particular combination of packet header options. Packet is considered to correspond to the state 'ESTABLISHED' if it belongs to the network session, for which proper initiation has been seen by the firewall, so its stateful inspection module made appropriate record in the state table. Usually statefule firewalls keep track of network connections using not only tcp protocol, but also udp and sometimes even icmp protocols. 'RELATED' describes packet belonging to a separate network connection, related to the session firewall is keeping track of. One example is FTP command and FTP data sessions." id="stdid14" library="Standard"  name="ESTABLISHED">
<CustomServiceCommand platform="Undefined"></CustomServiceCommand>
<CustomServiceCommand platform="ipfilter"></CustomServiceCommand>
<CustomServiceCommand platform="iptables">-m state --state ESTABLISHED,RELATED</CustomServiceCommand>
</CustomService>
<xsl:text>
</xsl:text>
        <xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
<xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="FWObjectDatabase">
  <FWObjectDatabase version="0.10.1" id="root">
  <xsl:apply-templates/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

