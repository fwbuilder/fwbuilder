<!--
   Filename:     1.0.1/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   09/20/2003
   Last changed: 09/20/2003
   Version:      1.0.0
   Description:  translates fwbuilder object database from v1.0.1 to v1.0.2
                 adds some new standard objects and changes version number
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>



<xsl:template match="*[attribute::id='stdid15']" mode="copy" priority="2">
  <xsl:copy>
    <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
    <xsl:text>
</xsl:text>
    <AddressRange xmlns="http://www.fwbuilder.org/1.0/">
      <xsl:attribute name="comment"/>
      <xsl:attribute name="id">id3F6D115C</xsl:attribute>
      <xsl:attribute name="name">broadcast</xsl:attribute>
      <xsl:attribute name="library">Standard</xsl:attribute>
      <xsl:attribute name="start_address">255.255.255.255</xsl:attribute>
      <xsl:attribute name="end_address">255.255.255.255</xsl:attribute>
    </AddressRange>
    <xsl:text>
</xsl:text>
    <AddressRange xmlns="http://www.fwbuilder.org/1.0/">
      <xsl:attribute name="comment"/>
      <xsl:attribute name="id">id3F6D115D</xsl:attribute>
      <xsl:attribute name="name">old-broadcast</xsl:attribute>
      <xsl:attribute name="library">Standard</xsl:attribute>
      <xsl:attribute name="start_address">0.0.0.0</xsl:attribute>
      <xsl:attribute name="end_address">0.0.0.0</xsl:attribute>
    </AddressRange>
    <xsl:text>
</xsl:text>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*" mode="copy" priority="1">
  <xsl:copy>
    <xsl:for-each select="@*">
      <xsl:copy/>
    </xsl:for-each>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>


<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">1.0.2</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

