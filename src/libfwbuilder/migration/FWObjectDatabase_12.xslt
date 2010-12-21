<!--
   Filename:     FWObjectDatabase_12.xslt
   Author:       Vadim Kurland
   Build date:   10/19/2009
   Last changed: 10/19/2009
   Version:      1.0.0
   Description:  translates fwbuilder object database from v12 to 13
                 Adds required attribute "action" to NATRule elements
                 Attribute is added with value "Translate" since this is
                 the only NAT rule action supported so far.
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

<xsl:template match="fwb:NATRule" mode="copy">
  <xsl:element name="NATRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="action">Translate</xsl:attribute>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:element>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">13</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


