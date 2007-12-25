<!--
   Filename:     0.10.11/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   11/23/2002
   Last changed: 11/23/2002
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.11 to v0.10.12
                 adds few new attributes to element TCPService
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template name="copy-element">
  <xsl:copy>
    <xsl:for-each select="@*">
      <xsl:copy/>
    </xsl:for-each>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>

<!--
   need to add the following attribites to TCPService:


 urg_flag_mask   %BOOLEAN; #REQUIRED
 ack_flag_mask   %BOOLEAN; #REQUIRED
 psh_flag_mask   %BOOLEAN; #REQUIRED
 rst_flag_mask   %BOOLEAN; #REQUIRED
 syn_flag_mask   %BOOLEAN; #REQUIRED
 fin_flag_mask   %BOOLEAN; #REQUIRED


-->
<xsl:template name="tcpservice">
<xsl:text>
</xsl:text>

<xsl:copy select=".">

  <xsl:variable name="mask">
    <xsl:if test="@urg_flag='True'">1</xsl:if>
    <xsl:if test="@ack_flag='True'">1</xsl:if>
    <xsl:if test="@psh_flag='True'">1</xsl:if>
    <xsl:if test="@rst_flag='True'">1</xsl:if>
    <xsl:if test="@syn_flag='True'">1</xsl:if>
    <xsl:if test="@fin_flag='True'">1</xsl:if>
  </xsl:variable>
  <xsl:variable name="m">
    <xsl:choose>
      <xsl:when test="$mask=''">False</xsl:when>
      <xsl:otherwise>True</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:if test="not (@urg_flag_mask)">
    <xsl:attribute name="urg_flag_mask"><xsl:value-of select="$m"/></xsl:attribute>
  </xsl:if>
  <xsl:if test="not (@ack_flag_mask)">
    <xsl:attribute name="ack_flag_mask"><xsl:value-of select="$m"/></xsl:attribute>
  </xsl:if>
  <xsl:if test="not (@psh_flag_mask)">
    <xsl:attribute name="psh_flag_mask"><xsl:value-of select="$m"/></xsl:attribute>
  </xsl:if>
  <xsl:if test="not (@rst_flag_mask)">
    <xsl:attribute name="rst_flag_mask"><xsl:value-of select="$m"/></xsl:attribute>
  </xsl:if>
  <xsl:if test="not (@syn_flag_mask)">
    <xsl:attribute name="syn_flag_mask"><xsl:value-of select="$m"/></xsl:attribute>
  </xsl:if>
  <xsl:if test="not (@fin_flag_mask)">
    <xsl:attribute name="fin_flag_mask"><xsl:value-of select="$m"/></xsl:attribute>
  </xsl:if>

  <xsl:for-each select="@*">
    <xsl:copy-of select="."/>
  </xsl:for-each>

  <xsl:apply-templates mode="copy"/>

</xsl:copy>
</xsl:template>





<xsl:template match="fwb:*" mode="copy">
  <xsl:choose>
    <xsl:when test="name()='TCPService'">
      <xsl:call-template name="tcpservice"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="copy-element"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*" mode="copy">
  <xsl:choose>
    <xsl:when test="name()='TCPService'">
      <xsl:call-template name="tcpservice"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="copy-element"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>



<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">0.10.12</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

