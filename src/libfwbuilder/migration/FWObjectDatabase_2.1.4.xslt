<!--
   Filename:     FWObjectDatabase_2.1.4.xslt
   Author:       Vadim Kurland
   Build date:   02/15/2006
   Last changed: 02/15/2006
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.1.4 to v2.1.5
                 only changes version number

                 09/05/2006: changed target version to 2.1.6
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>


<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">2.1.6</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:copy-of select="*"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


