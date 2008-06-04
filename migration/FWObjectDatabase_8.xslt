<!--
   Filename:     FWObjectDatabase_6.xslt
   Author:       Vadim Kurland
   Build date:   06/04/2008
   Last changed: 06/04/2008
   Version:      1.0.0
   Description:  translates fwbuilder object database from v8 to 9
                 Replaces option tagvalue with tagobject_id pointing to
                 TagService objects found in the TagServices service
                 group.
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>


<!-- **************************************************************** -->
<xsl:template match="*" mode="copy">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
  <xsl:text>
</xsl:text>
</xsl:template>


<!-- **************************************************************** -->
<xsl:template
    match="//fwb:PolicyRule[@action='Tag']//fwb:Option[@name='tagvalue']"
    mode="copy">

  <xsl:variable name="library_name" select="../../../../../../@name"/>
  <xsl:variable name="tagvalue" select="."/>

  <xsl:for-each select="//fwb:Library[@name=$library_name]//fwb:ServiceGroup[@name='TagServices']/fwb:TagService[@tagcode=$tagvalue]">
    
    <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:attribute name="name">tagobject_id</xsl:attribute>
      <xsl:value-of select="@id"/>
    </xsl:element>
    <xsl:text>
      </xsl:text>
  </xsl:for-each>
  <xsl:text>
      </xsl:text>
</xsl:template>

<!-- **************************************************************** -->
<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">9</xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>
