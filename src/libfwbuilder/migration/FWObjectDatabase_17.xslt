<!--
   FWObjectDatabase_17.xslt 2011-02-15 
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v17 to 18
                 Adds NAT rule elements ItfInb and ItfOutb
                 
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

<xsl:template match="//fwb:NATRule" mode="copy">
  <xsl:element name="NATRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:OSrc" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:ODst" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:OSrv" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:TSrc" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:TDst" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:TSrv" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:element name="ItfInb" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:attribute name="neg">False</xsl:attribute>
      <xsl:element name="ObjectRef" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="ref">sysid0</xsl:attribute>
      </xsl:element>
    </xsl:element>
    <xsl:text>
            </xsl:text>
    <xsl:element name="ItfOutb" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:attribute name="neg">False</xsl:attribute>
      <xsl:element name="ObjectRef" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="ref">sysid0</xsl:attribute>
      </xsl:element>
    </xsl:element>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:When" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:NATRuleOptions" mode="copy"/>
    <xsl:text>
          </xsl:text>
  </xsl:element>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">18</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


