<!--
   Filename:     FWObjectDatabase_2.0.3.xslt
   Author:       Vadim Kurland
   Build date:   10/09/2004
   Last changed: 10/09/2004
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.0.3 to v2.0.99

                 only changes version number
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>


<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">2.0.99</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:copy-of select="*"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

