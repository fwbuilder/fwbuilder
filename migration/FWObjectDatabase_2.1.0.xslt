<!--
   Filename:     FWObjectDatabase_2.1.0.xslt
   Author:       Tidei Maurizio     <fwbuilder-routing at compal.de> 
   Build date:   09/27/2005
   Last changed: 09/27/2005
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.1.0 to v2.1.1

                 changes version number
                 adds Routing element if necessary
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

<xsl:template match="fwb:Firewall" mode="copy">
   
  <xsl:variable name="fwid" select="@id"/>
   
  <Firewall xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
      <xsl:text>
        </xsl:text>    
    <xsl:for-each select="*">
      <xsl:copy-of select="."/>
      <xsl:text>
        </xsl:text>      
      <xsl:if test="local-name()='Policy'">
        <xsl:choose>
          <xsl:when test="following::*[local-name()='Routing']">
          </xsl:when>
          <xsl:otherwise>
       <xsl:element name="Routing">
         <xsl:attribute name="id"><xsl:value-of select="$fwid"/>-routing</xsl:attribute>
       </xsl:element>
       <xsl:text>
        </xsl:text>    
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </xsl:for-each>
  </Firewall>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">2.1.1</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>
