<!--
   Filename:     0.10.8/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   08/09/2002
   Last changed: 09/04/2002
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.8 to v0.10.9

                 adds child element IPv4 to each Interface, assigns
                 address and netmask attribute values to IPv4 and
                 removes these attrbites from Interface

                 removes attribute "address" from elements Host and Firewall
                 and, if element does not have child element Interface,
                 creates one with its own child element IPv4 and assigns
                 value of attribute "address" to it.

                 Sets name for the new IPv4 objects using name of the parent
                 host or firewall object
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/">



<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>


<!--  Host or Firewall -->
<xsl:template name="Host" mode="copy">
<xsl:text>
</xsl:text>

<xsl:copy select=".">

<xsl:variable name="hid"   select="@id"/>
<xsl:variable name="hname" select="@name"/>
<xsl:variable name="addr"  select="@address"/>

<xsl:for-each select="@*">
<xsl:choose>
<xsl:when test="name()='address'"></xsl:when>
<xsl:when test="name()='netmask'"></xsl:when>
<xsl:otherwise>
  <xsl:copy-of select="."/>
</xsl:otherwise>
</xsl:choose>
</xsl:for-each>

<xsl:choose>
  <xsl:when test="fwb:Interface">
  </xsl:when>
  <xsl:otherwise>

<xsl:text>
</xsl:text>

<Interface>
<xsl:attribute name="id"><xsl:value-of select="$hid"/>-i</xsl:attribute>
<xsl:attribute name="name">interface-1</xsl:attribute>
<xsl:attribute name="dyn">False</xsl:attribute>
<xsl:attribute name="security_level">0</xsl:attribute>

<xsl:text>
</xsl:text>
    <IPv4>
    <xsl:attribute name="id"><xsl:value-of select="$hid"/>-i-1-addr</xsl:attribute>
    <xsl:attribute name="name"><xsl:value-of select="$hname"/></xsl:attribute>
    <xsl:attribute name="address">
      <xsl:value-of select="$addr"/>
    </xsl:attribute>
    <xsl:attribute name="netmask">255.255.255.255</xsl:attribute>
    </IPv4>
<xsl:text>
</xsl:text>
</Interface>

  </xsl:otherwise>
</xsl:choose>

<xsl:text>
</xsl:text>

  <xsl:apply-templates mode="copy"/>

<!--  </Host> -->
</xsl:copy>
</xsl:template>



<!-- Interface -->
<xsl:template name="Interface" mode="copy">
<xsl:text>
</xsl:text>
  <Interface>

<xsl:variable name="addr"    select="@address"/>
<xsl:variable name="netm"    select="@netmask"/>
<xsl:variable name="ifaceid" select="@id"/>
<xsl:variable name="hname"   select="../@name"/>

<xsl:for-each select="@*">
<xsl:choose>
<xsl:when test="name()='address'"></xsl:when>
<xsl:when test="name()='netmask'"></xsl:when>
<xsl:otherwise>
  <xsl:copy-of select="."/>
</xsl:otherwise>
</xsl:choose>
</xsl:for-each>

<xsl:choose>
  <xsl:when test="fwb:IPv4">
  </xsl:when>
  <xsl:otherwise>
<xsl:text>
</xsl:text>

<xsl:variable name="ipv4id" select="concat($ifaceid,'-ipv4')"/>

    <IPv4>
    <xsl:attribute name="id">     <xsl:value-of select="$ipv4id"/></xsl:attribute>
    <xsl:attribute name="name">   <xsl:value-of select="$hname"/> </xsl:attribute>
    <xsl:attribute name="address"><xsl:value-of select="$addr"/>  </xsl:attribute>
    <xsl:attribute name="netmask"><xsl:value-of select="$netm"/>  </xsl:attribute>
    </IPv4>
  </xsl:otherwise>
</xsl:choose>

<xsl:text>
</xsl:text>

  <xsl:apply-templates mode="copy"/>

  </Interface>

</xsl:template>


<xsl:template name="copy-element">
  <xsl:copy>
    <xsl:for-each select="@*">
      <xsl:copy/>
    </xsl:for-each>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="fwb:*" mode="copy">
  <xsl:choose>
    <xsl:when test="name()='Host'">
      <xsl:call-template name="Host"/>
    </xsl:when>
    <xsl:when test="name()='Firewall'">
      <xsl:call-template name="Host"/>
    </xsl:when>
    <xsl:when test="name()='Interface'">
      <xsl:call-template name="Interface"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="copy-element"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>



<xsl:template match="*[attribute::id='root']">

  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">

  <xsl:attribute name="version">0.10.9</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>

  <xsl:apply-templates select="*" mode="copy"/>

  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

