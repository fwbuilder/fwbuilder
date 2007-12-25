<!--
   Filename:     FWObjectDatabase_2.1.3.xslt
   Author:       Vadim Kurland
   Build date:   09/20/2005
   Last changed: 09/26/2005
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.1.3 to v2.1.4

                 changes version number
                 adds a new group for Address Tables.
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>


<xsl:template match="*[attribute::name='Objects']" mode="copy" priority="2">
  <xsl:copy>
    <xsl:for-each select="@*"><xsl:copy/></xsl:for-each>
    <xsl:text>
        </xsl:text>
    <ObjectGroup xmlns="http://www.fwbuilder.org/1.0/">
      <xsl:attribute name="id"><xsl:value-of select="@id"/>_og_ats_1</xsl:attribute>
      <xsl:attribute name="name">Address Tables</xsl:attribute>
    </ObjectGroup>
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
  <xsl:attribute name="version">2.1.3</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>



