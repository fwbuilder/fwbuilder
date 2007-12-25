<!--
   Filename:     0.10.1/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   12/05/2001
   Last changed: 12/05/2001
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.1 to v0.10.2
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

<!-- <xsl:strip-space elements="*"/> -->

<xsl:template match="@*|node()" mode="copy">
<xsl:copy>
  <xsl:apply-templates select="@*" mode="copy"/>
  <xsl:apply-templates mode="copy"/>
</xsl:copy>
</xsl:template>


<xsl:template match="Host">
<xsl:copy>

  <xsl:apply-templates select="@*" mode="copy"/>

  <xsl:text>
  </xsl:text>

  <xsl:choose>
    <xsl:when test="HostOptions">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
      <HostOptions>
        <xsl:text>
        </xsl:text>
        <Option name="use_mac_addr_filter">false</Option>
	<xsl:text>
	</xsl:text>
      </HostOptions>
      <xsl:text>
      </xsl:text>
    </xsl:otherwise>
  </xsl:choose>

</xsl:copy>
</xsl:template>

<xsl:template match="ObjectGroup[attribute::name='Hosts']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="ObjectGroup[attribute::name='Objects']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
<xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="FWObjectDatabase">
  <FWObjectDatabase version="0.10.2" id="root">
  <xsl:apply-templates/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

