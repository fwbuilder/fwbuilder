<!--
   Filename:     0.9.1/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   05/29/2001
   Last changed: 06/02/2001
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.9.1 to v0.9.2
-->

<xsl:stylesheet version="1.0"
       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes"/>

<xsl:strip-space elements="*"/>

<xsl:template match="FirewallOptions" mode="fw">
  <FirewallOptions>
    <Option name="firewall_is_part_of_any_and_networks">True</Option>
    <xsl:copy-of select="*"/>
  </FirewallOptions>
</xsl:template>

<xsl:template match="Firewall" mode="fw">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="fw"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="ObjectGroup[@name='Firewalls']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="fw"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*" mode="fw">
<xsl:copy-of select="."/>
</xsl:template>


<xsl:template match="*">
<xsl:copy-of select="."/>
</xsl:template>


<xsl:template match="FWObjectDatabase">
  <FWObjectDatabase version="0.9.2">
    <xsl:apply-templates/>
  </FWObjectDatabase>
</xsl:template>


</xsl:stylesheet>

