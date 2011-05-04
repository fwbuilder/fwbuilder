<!--
   FWObjectDatabase_18.xslt 2011-05-04
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v18 to 19
                 finds PolicyRule elements with missing Itf
                 child element and fixes them
                 
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

<xsl:template match="//fwb:PolicyRule" mode="copy">
  <xsl:element name="PolicyRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:Src" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:Dst" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:Srv" mode="copy"/>

    <xsl:text>
            </xsl:text>
    <xsl:choose>
      <xsl:when test="fwb:Itf">
        <xsl:apply-templates select="fwb:Itf" mode="copy"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:element name="Itf" namespace="http://www.fwbuilder.org/1.0/">
          <xsl:attribute name="neg">False</xsl:attribute>
          <xsl:element name="ObjectRef" namespace="http://www.fwbuilder.org/1.0/">
            <xsl:attribute name="ref">sysid0</xsl:attribute>
          </xsl:element>
        </xsl:element>
      </xsl:otherwise>
    </xsl:choose>

    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:When" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:PolicyRuleOptions" mode="copy"/>
    <xsl:text>
          </xsl:text>
  </xsl:element>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">19</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


