<!--
   $Id$
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v15 to 16.
                 Change host_OS attribute from "linksys" to "sveasoft"
                 
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template match="*" mode="copy">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="fwb:Firewall[@host_OS='linksys']" mode="copy">
  <xsl:element name="Firewall" namespace="http://www.fwbuilder.org/1.0/">

  <xsl:for-each select="@*">
    <xsl:choose>
      <xsl:when test="local-name()='host_OS'">
        <xsl:attribute name="host_OS">sveasoft</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy-of select="."/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
  <xsl:text>
  </xsl:text>
  <xsl:apply-templates select="*" mode="copy"/>
  </xsl:element>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">16</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


