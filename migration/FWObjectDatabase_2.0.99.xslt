<!--
   Filename:     FWObjectDatabase_2.0.99.xslt
   Author:       Vadim Kurland
   Build date:   04/13/2006
   Last changed: 04/13/2006
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.0.99 to v2.1.0

                 v2.0.99 is an intermediate version used to simplify
                 migration chains. Migration scripts for previous versions
                 of 2.0.x will convert directly to 2.0.99 to avoid having
                 to go through multiple conversion stages that only change
                 version number

                 v2.1.0 is the base for 2.1 series, it does not include
                 any new features and is created to work out and test
                 build infrastructure and code and make sure v2.1 can
                 coexist with 2.0.X

                 adds a new group for DNS Names.
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
      <xsl:attribute name="id"><xsl:value-of select="@id"/>_og_dnsn_1</xsl:attribute>
      <xsl:attribute name="name">DNS Names</xsl:attribute>
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
  <xsl:attribute name="version">2.1.0</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>



