<!--
   Filename:     0.10.9/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   09/15/2002
   Last changed: 09/15/2002
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.9 to v0.10.10

-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">



<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>





<xsl:template match="*[attribute::id='root']">

  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">

  <xsl:attribute name="version">0.10.10</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>

  <xsl:for-each select="*">
    <xsl:copy-of select="."/>
  </xsl:for-each>

  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

