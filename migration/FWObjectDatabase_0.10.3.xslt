<!--
   Filename:     0.10.3/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   12/25/2001
   Last changed: 02/28/2002
   Version:      1.1.0
   Description:  translates fwbuilder object database from v0.10.3 to v0.10.4
                 This transformation changes version number

	v1.1.0	 this translformation also adds namespace to all elements
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--
    xmlns:fwbuilder="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwbuilder"
-->


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>



<xsl:template match="*">
<xsl:element name="{name()}"  namespace="http://www.fwbuilder.org/1.0/">
  <xsl:apply-templates select="@*" />
  <xsl:apply-templates />
</xsl:element>
</xsl:template>

<xsl:template match="@*" >
<xsl:copy select="."/>
</xsl:template>

<xsl:template match="FWObjectDatabase">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">

  <xsl:attribute name="version">0.10.4</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>

  <xsl:apply-templates select="*" />

  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

