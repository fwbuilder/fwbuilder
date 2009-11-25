<!--
   $Id$
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v14 to 15.
                 See ticket #650, #709. Gets rid of FirewallOptions option
                 ipt_mangle_only_rulesets, adds RuleSetOptions child
                 element to Policy, NAT and Routing rule set objects
                 and sets option "mangle_only" to True in the
                 RuleSetOptions child of Policy if the name of the
                 ruleset appears in the old  ipt_mangle_only_rulesets
                 option.

                 This script has a limitation: it will only mark one
                 policy ruleset as mangle-only using new Options
                 element.  If there are more, they used to be saved as
                 space-separated list in the old
                 "ipt_mangle_only_rulesets" option. Unfortunately xslt
                 1.0 does not have split() function so we can't
                 separate the names without compicated xslt hacks.
                 This script will find and mark appropriately only the
                 first rule set found in "ipt_mangle_only_rulesets"
                 option
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


<xsl:template match="fwb:Policy" mode="copy">
  <xsl:element name="Policy" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/>
    <xsl:variable name="ruleset_name" select="@name"/>
    <xsl:variable
       name="mangle_rulesets_raw"
       select="normalize-space(../fwb:FirewallOptions/fwb:Option[@name='ipt_mangle_only_rulesets'])"/>

    <xsl:variable name="mangle_ruleset_name">
      <xsl:choose>
        <xsl:when test="contains($mangle_rulesets_raw, ' ')">
          <xsl:copy-of select="substring-before($mangle_rulesets_raw, ' ')"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:copy-of select="$mangle_rulesets_raw"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:text>
        </xsl:text>

    <xsl:apply-templates select="*" mode="copy"/>

    <xsl:element name="RuleSetOptions" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:if test="$ruleset_name = $mangle_ruleset_name">
        <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
          <xsl:attribute name="name">mangle_only_rule_set</xsl:attribute>
          True
        </xsl:element>
      </xsl:if>
    </xsl:element>

  </xsl:element>
</xsl:template>


<xsl:template match="fwb:NAT|fwb:Routing" mode="copy">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
    <xsl:element name="RuleSetOptions" namespace="http://www.fwbuilder.org/1.0/"/>
    <xsl:text>
        </xsl:text>
  </xsl:copy>
  <xsl:text>
        </xsl:text>
</xsl:template>




<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">15</xsl:attribute>
    <xsl:attribute name="lastModified"><xsl:value-of select="@lastModified"/></xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>


