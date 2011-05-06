<!--
   FWObjectDatabase_19.xslt 2011-05-05
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v19 to 20

                 #2385 copy value of the option "classify_str" to
                 the option "pf_classify_str" for PF policy rules
                 with action Classify
                 
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


<xsl:template match="//fwb:PolicyRuleOptions" mode="copy">
  <xsl:variable name="classify_str">
    <xsl:value-of select="fwb:Option[@name='classify_str']"/>
  </xsl:variable>

  <xsl:copy-of select="@*"/>

  <xsl:choose>
    <xsl:when test="fwb:PolicyRuleOptions/fwb:Option[@name='pf_classify_str']">
      <xsl:apply-templates mode="copy"/>
    </xsl:when>
    <xsl:otherwise>

      <xsl:element name="PolicyRuleOptions"
                   namespace="http://www.fwbuilder.org/1.0/">
        <xsl:apply-templates mode="copy"/>
        <xsl:text>
          </xsl:text>
        <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
          <xsl:attribute name="name">pf_classify_str</xsl:attribute><xsl:value-of select="$classify_str"/></xsl:element>
        <xsl:text>
          </xsl:text>
      </xsl:element>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:text>
  </xsl:text>
</xsl:template>




<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">20</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


