<!--
   $Id$
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v13 to 14.
                 Removes ObjectRef and other reference elements that
                 are direct children of Library
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

<xsl:template match="fwb:Library" mode="copy">
  <xsl:element name="Library" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="*[local-name() != 'ObjectRef' and local-name() != 'ServiceRef' and local-name() != 'IntervalRef']" mode="copy"/>
  </xsl:element>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">14</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


