<!--
   Filename:     0.10.6/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   06/02/2002
   Last changed: 06/02/2002
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.6 to v0.10.7


****************************************************************************
*                                                                          *
*   This transformation (0.10.6->0.10.7) defines its own namespace         *
*   prefix 'fwb' to be able to match on elements with the namespace. This  *
*   prefix is defined in xsl:stylesheet element. This prefix won't be      *
*   added to the output data, it is used only within this transformation.  *
*                                                                          *
****************************************************************************

-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/">



<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>





<xsl:template match="*[attribute::id='root']">

  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">

  <xsl:attribute name="version">0.10.7</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>

  <xsl:for-each select="*">
    <xsl:copy-of select="."/>
  </xsl:for-each>

  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

