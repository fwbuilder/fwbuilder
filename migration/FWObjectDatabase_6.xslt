<!--
   Filename:     FWObjectDatabase_6.xslt
   Author:       Vadim Kurland
   Build date:   05/11/2008
   Last changed: 05/11/2008
   Version:      1.0.0
   Description:  translates fwbuilder object database from v6 to 7
                 Adds tree branch for the UserService objects
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

<xsl:template match="fwb:ServiceGroup[local-name(parent::*)='Library']"
              mode="copy">
  <xsl:variable name="parent_id" select="@id"/>
  <xsl:element name="ServiceGroup" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/>
    <xsl:text>
      </xsl:text>

    <xsl:choose>
      <xsl:when test="fwb:ServiceGroup[@name='Users']">
      </xsl:when>
      <xsl:otherwise>
        <xsl:element name="ServiceGroup" namespace="http://www.fwbuilder.org/1.0/">
          <xsl:attribute name="id"><xsl:value-of select="$parent_id"/>_userservices</xsl:attribute>
          <xsl:attribute name="name">Users</xsl:attribute>
        </xsl:element>
        <xsl:text>
        </xsl:text>
      </xsl:otherwise>
    </xsl:choose>

    <xsl:apply-templates select="*" mode="copy"/>
    <xsl:text>
      </xsl:text>
  </xsl:element>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">7</xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


