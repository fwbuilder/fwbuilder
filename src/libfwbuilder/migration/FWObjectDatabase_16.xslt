<!--
   $Id: FWObjectDatabase_16.xslt 507 2010-07-20 21:27:42Z vadim $
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v16 to 17
                 Make sure Standard objects library is read-only
                 
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

<xsl:template match="fwb:Library[@id='syslib000']" mode="copy">
  <xsl:element name="Library" namespace="http://www.fwbuilder.org/1.0/">

  <xsl:for-each select="@*">
    <xsl:choose>
      <xsl:when test="local-name()='ro'">
        <xsl:attribute name="ro">True</xsl:attribute>
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
    <xsl:attribute name="version">17</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


