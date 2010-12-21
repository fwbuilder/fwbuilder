<!--
   Filename:     FWObjectDatabase_2.1.11.xslt
   Author:       Vadim Kurland
   Build date:   05/06/2007
   Last changed: 05/06/2007
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.1.11 to v2.1.12
                 only changes version number
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

<xsl:template match="fwb:Option[attribute::name='stateless']" mode="optcopy">
<Option xmlns="http://www.fwbuilder.org/1.0/" name="stateless">True</Option>
</xsl:template>

<xsl:template match="fwb:Option" mode="optcopy">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="fwb:PolicyRule[attribute::action='Accept']|fwb:PolicyRule[attribute::action='Tag']" mode="copy">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="fwb:PolicyRule" mode="copy">
  <xsl:element name="PolicyRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:for-each select="fwb:*">
      <xsl:choose>
        <xsl:when test="local-name()='PolicyRuleOptions'">
          <xsl:text>
            </xsl:text>
          <xsl:element name="PolicyRuleOptions"
                       namespace="http://www.fwbuilder.org/1.0/">
            <xsl:copy-of select="@*"/> 
            <xsl:text>
              </xsl:text>
            <xsl:apply-templates select="*" mode="optcopy"/>
            <xsl:text>
              </xsl:text>
<Option xmlns="http://www.fwbuilder.org/1.0/" name="stateless">True</Option>
            <xsl:text>
            </xsl:text>
          </xsl:element>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>
            </xsl:text>
          <xsl:copy>
            <xsl:copy-of select="@*"/>
            <xsl:apply-templates mode="copy"/>
          </xsl:copy>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    <xsl:text>
          </xsl:text>
  </xsl:element>
</xsl:template>




<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">2.1.12</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


