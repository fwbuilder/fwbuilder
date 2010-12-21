<!--
   Filename:     0.10.12/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   12/24/2002
   Last changed: 12/25/2002
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.12 to v0.10.13
                 adds attribute "unnum" to element Interface
                 replaces attribute "physAddress" in element Interface with
                 new child element physAddress
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template name="copy-element">
  <xsl:copy>
    <xsl:for-each select="@*">
      <xsl:copy/>
    </xsl:for-each>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>



<xsl:template name="interface">
<xsl:copy select=".">

  <xsl:variable name="iid"><xsl:value-of select="@id"/></xsl:variable>
  <xsl:variable name="iname"><xsl:value-of select="@name"/></xsl:variable>
  <xsl:variable name="pa"><xsl:value-of select="@physAddress"/></xsl:variable>

  <xsl:if test="not (@unnum)">
    <xsl:attribute name="unnum">False</xsl:attribute>
  </xsl:if>

  <xsl:for-each select="@*">
    <xsl:if test="not (name()='physAddress')">
      <xsl:copy-of select="."/>
    </xsl:if>
  </xsl:for-each>

<!-- order is important: IPv4*, physAddress?, InterfacePolicy? -->

<xsl:text>
          </xsl:text><xsl:copy-of select="fwb:IPv4|IPv4"/>

  <xsl:if test="$pa!=''">
<xsl:text>
          </xsl:text><physAddress>
    <xsl:attribute name="id"><xsl:value-of select="$iid"/>-pa</xsl:attribute>
    <xsl:attribute name="name"><xsl:value-of select="$iname"/>(MAC)</xsl:attribute>
    <xsl:attribute name="address"><xsl:value-of select="$pa"/></xsl:attribute>
  </physAddress>
  </xsl:if>

<xsl:text>
          </xsl:text><xsl:copy-of select="fwb:InterfacePolicy|InterfacePolicy"/>

<xsl:text>
        </xsl:text>
</xsl:copy>
</xsl:template>





<xsl:template match="fwb:*" mode="copy">
  <xsl:choose>
    <xsl:when test="name()='Interface'">
      <xsl:call-template name="interface"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="copy-element"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*" mode="copy">
  <xsl:choose>
    <xsl:when test="name()='Interface'">
      <xsl:call-template name="interface"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="copy-element"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>



<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">0.10.13</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

