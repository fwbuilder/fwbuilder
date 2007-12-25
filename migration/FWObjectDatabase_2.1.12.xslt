<!--
   Filename:     FWObjectDatabase_2.1.12.xslt
   Author:       Vadim Kurland
   Build date:   07/05/2007
   Last changed: 07/05/2007
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.1.12 to v2.1.13
                 only changes version number

                 Modified 10/21/07, target version is 2.1.99
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>


<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">2.1.99</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:copy-of select="*"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


