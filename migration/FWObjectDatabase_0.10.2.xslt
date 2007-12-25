<!--
   Filename:     0.10.2/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   12/10/2001
   Last changed: 12/10/2001
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.2 to v0.10.3
                 This transformation changes version number

   01/18/02      added exclude-result-prefixes attribute to stylesheet def.

-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwbuilder="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwbuilder"
>


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template match="FWObjectDatabase">
  <FWObjectDatabase version="0.10.3" id="root">
  <xsl:copy-of select="*"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

