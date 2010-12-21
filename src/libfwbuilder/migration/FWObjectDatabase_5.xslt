<!--
   Filename:     FWObjectDatabase_5.xslt
   Author:       Vadim Kurland
   Build date:   05/11/2008
   Last changed: 05/11/2008
   Version:      1.0.0
   Description:  translates fwbuilder object database from v5 to 6
                 Move branch rule sets from under PolicyRule to under
                 Firewall.

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
  <xsl:element name="Firewall" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:text>
        </xsl:text>
    <xsl:apply-templates select="fwb:NAT" mode="copy"/>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="fwb:Policy" mode="copy"/>
    <xsl:text>
          </xsl:text>


    <xsl:for-each select="fwb:Policy/fwb:PolicyRule/fwb:Policy/fwb:PolicyRule/fwb:Policy">
      <xsl:element name="Policy" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:copy-of select="@*"/> 
        <xsl:text>
            </xsl:text>
        <xsl:apply-templates select="*" mode="copy"/>
      </xsl:element>
    <xsl:text>
          </xsl:text>
    </xsl:for-each>
    <xsl:text>
          </xsl:text>


    <xsl:for-each select="fwb:Policy/fwb:PolicyRule/fwb:Policy">
      <xsl:element name="Policy" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:copy-of select="@*"/> 
        <xsl:text>
            </xsl:text>
        <xsl:apply-templates select="*" mode="copy"/>
      </xsl:element>
    <xsl:text>
          </xsl:text>
    </xsl:for-each>
    <xsl:text>
          </xsl:text>


    <xsl:apply-templates select="fwb:Routing" mode="copy"/>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="fwb:Interface" mode="copy"/>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="fwb:Management" mode="copy"/>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="fwb:FirewallOptions" mode="copy"/>
    <xsl:text>
          </xsl:text>
  </xsl:element>
</xsl:template>

<xsl:template match="fwb:PolicyRule" mode="copy">
  <xsl:element name="PolicyRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:text>
            </xsl:text>
    <xsl:apply-templates select="fwb:Src|fwb:Dst|fwb:Srv|fwb:Itf|fwb:When" mode="copy"/>
    <xsl:text>
            </xsl:text>
    <xsl:for-each select="fwb:PolicyRuleOptions">
      <xsl:element name="PolicyRuleOptions" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:copy-of select="@*"/> 
        <xsl:apply-templates mode="copy"/>
      </xsl:element>
      <xsl:text>
          </xsl:text>
    </xsl:for-each>
  </xsl:element>
  <xsl:text>
          </xsl:text>
</xsl:template>

<xsl:template match="fwb:Policy" mode="copy">
  <xsl:element name="Policy" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:attribute name="name">Policy</xsl:attribute>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="*" mode="copy"/>
  </xsl:element>
</xsl:template>


<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">6</xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


