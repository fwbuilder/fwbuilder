<!--
   Filename:     FWObjectDatabase_3.xslt
   Author:       Vadim Kurland
   Build date:   04/28/2008
   Last changed: 04/28/2008
   Version:      1.0.0
   Description:  translates fwbuilder object database from v4 to 5
                 Adds attribute name to Policy, NAT and Routing.
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

<xsl:template match="fwb:Policy" mode="copy">
  <xsl:element name="Policy" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:attribute name="name">Policy</xsl:attribute>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="*" mode="copy"/>
  </xsl:element>
</xsl:template>

<xsl:template match="fwb:NAT" mode="copy">
  <xsl:element name="NAT" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:attribute name="name">NAT</xsl:attribute>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="*" mode="copy"/>
  </xsl:element>
</xsl:template>

<xsl:template match="fwb:Routing" mode="copy">
  <xsl:element name="Routing" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:attribute name="name">Routing</xsl:attribute>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="*" mode="copy"/>
  </xsl:element>
</xsl:template>

<xsl:template match="fwb:PolicyRule/fwb:Policy" mode="copy">
  <xsl:variable name="branch_name"
     select="../fwb:PolicyRuleOptions/fwb:Option[@name='branch_name']"/>
  <xsl:element name="Policy" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:attribute name="name"><xsl:value-of select="$branch_name"/></xsl:attribute>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="*" mode="copy"/>
  </xsl:element>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">5</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


