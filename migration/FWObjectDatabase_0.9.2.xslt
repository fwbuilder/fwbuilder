<!--
   Filename:     0.9.2/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   07/12/2001
   Last changed: 07/29/2001
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.9.2 to v0.9.3
                 This transformation only changes version number.
-->

<xsl:stylesheet version="1.0"
       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template match="FWObjectDatabase">
  <FWObjectDatabase version="0.9.3">
  <xsl:copy-of select="*"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

