<!--
   Filename:     FWObjectDatabase_11.xslt
   Author:       Vadim Kurland
   Build date:   07/16/2009
   Last changed: 07/16/2009
   Version:      1.0.0
   Description:  translates fwbuilder object database from v11 to 12
                 Moves group Clusters from Objects/ to the top of the tree
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

    <xsl:text>
    </xsl:text>

    <xsl:for-each select="fwb:ObjectGroup[@name='Objects']/fwb:ObjectGroup[@name='Clusters']">
      <xsl:element name="ObjectGroup" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">Clusters</xsl:attribute>
        <xsl:copy-of select="@*"/>
        <xsl:text>
        </xsl:text>
        <xsl:apply-templates select="*" mode="copy"/>
        <xsl:text>
        </xsl:text>
      </xsl:element>
    </xsl:for-each>

    <xsl:apply-templates select="*" mode="copy"/>

  </xsl:element>
</xsl:template>

<!-- skip object group Clusters while copying the rest of the tree, we
     copy it explicitly above
  -->
<xsl:template match="fwb:ObjectGroup[@name='Clusters']" mode="copy">
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">12</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


