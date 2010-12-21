<!--
   Filename:     FWObjectDatabase_6.xslt
   Author:       Vadim Kurland
   Build date:   04/17/2009
   Last changed: 04/17/2009
   Version:      1.0.0
   Description:  translates fwbuilder object database from v10 to 11
                 Adds tree branch for the Cluster objects
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

<xsl:template match="fwb:ObjectGroup[@name='Objects']"
              mode="copy">
  <xsl:choose>
    <xsl:when test="local-name(parent::*)='Library'">

      <xsl:variable name="parent_id" select="@id"/>
      <xsl:element name="ObjectGroup" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:copy-of select="@*"/>
        <xsl:text>
      </xsl:text>

        <xsl:choose>
          <xsl:when test="fwb:ObjectGroup[@name='Clusters']">
          </xsl:when>
          <xsl:otherwise>
            <xsl:element name="ObjectGroup" namespace="http://www.fwbuilder.org/1.0/">
              <xsl:attribute name="id"><xsl:value-of select="$parent_id"/>_clusters</xsl:attribute>
              <xsl:attribute name="name">Clusters</xsl:attribute>
             </xsl:element>
            <xsl:text>
      </xsl:text>
          </xsl:otherwise>
        </xsl:choose>

        <xsl:apply-templates select="*" mode="copy"/>
        <xsl:text>
      </xsl:text>
      </xsl:element>
    </xsl:when>
    <xsl:otherwise>
      <xsl:element name="ObjectGroup" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:copy-of select="@*"/>
        <xsl:text>
        </xsl:text>
        <xsl:apply-templates select="*" mode="copy"/>
        <xsl:text>
        </xsl:text>
      </xsl:element>
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">11</xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


