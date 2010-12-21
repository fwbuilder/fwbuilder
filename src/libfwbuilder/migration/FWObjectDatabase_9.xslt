<!--
   Filename:     FWObjectDatabase_9.xslt
   Author:       Vadim Kurland
   Build date:   07/07/2008
   Last changed: 07/07/2008
   Version:      1.0.0
   Description:  translates fwbuilder object database from v9 to 10
                 Adds attribute top_rule_set to Policy, NAT and Routing
                 elements if their name is "Policy", "NAT" and "Routing"
                 respectively.
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
<xsl:template match="//fwb:Policy[@name='Policy']" mode="copy">
  <xsl:element name="Policy" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="top_rule_set">True</xsl:attribute>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:element>
  <xsl:text>
    </xsl:text>
</xsl:template>

<xsl:template match="//fwb:NAT[@name='NAT']" mode="copy">
  <xsl:element name="NAT" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="top_rule_set">True</xsl:attribute>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:element>
  <xsl:text>
    </xsl:text>
</xsl:template>

<xsl:template match="//fwb:Routing[@name='Routing']" mode="copy">
  <xsl:element name="Routing" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="top_rule_set">True</xsl:attribute>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:element>
  <xsl:text>
    </xsl:text>
</xsl:template>


<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">10</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


