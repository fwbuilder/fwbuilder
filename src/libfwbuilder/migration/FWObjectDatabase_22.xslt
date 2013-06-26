<!--
   Filename:     FWObjectDatabase_22.xslt
   Author:       Sirius Bakke
   Build date:   2013-06-26
   Last changed: 2013-06-26
   Version:      1.0.0
   Description:  translates fwbuilder object database from v22 to 23

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

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">23</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


