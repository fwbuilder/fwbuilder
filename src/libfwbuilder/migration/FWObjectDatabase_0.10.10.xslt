<!--
   Filename:     0.10.10/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   10/13/2002
   Last changed: 10/20/2002
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.10 to v0.10.11
                 adds FirewallOptions/Option with name "check_shading" and value 'True'
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

<xsl:template name="fwoptions">
  <xsl:copy select=".">
    <xsl:text>
    </xsl:text>
    <xsl:apply-templates select="*" mode="copy"/>
    <xsl:text>
    </xsl:text>
    <Option name="check_shading">true</Option>
    <xsl:text>
    </xsl:text>
  </xsl:copy>
</xsl:template>

<xsl:template match="fwb:*" mode="copy">
  <xsl:choose>
    <xsl:when test="name()='FirewallOptions'">
      <xsl:call-template name="fwoptions"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="copy-element"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*" mode="copy">
  <xsl:choose>
    <xsl:when test="name()='FirewallOptions'">
      <xsl:call-template name="fwoptions"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="copy-element"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">0.10.11</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

