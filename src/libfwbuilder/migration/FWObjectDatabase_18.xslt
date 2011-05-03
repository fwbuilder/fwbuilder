<!--
   FWObjectDatabase_17.xslt 2011-05-03
   Author:       Vadim Kurland
   Description:  translates fwbuilder object database from v18 to 19
                 
                 
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

<xsl:template match="//fwb:PolicyRule[attribute::action='Tag']" mode="copy">

  <xsl:variable name="ipt_make_terminating"
    select="../../fwb:FirewallOptions/fwb:Option[attribute::name='classify_mark_terminating']"/>

  <xsl:variable name="pf_make_terminating"
    select="fwb:PolicyRuleOptions/fwb:Option[attribute::name='pf_tag_terminating']"/>

  <xsl:element name="PolicyRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@id"/>
    <xsl:copy-of select="@disabled"/>
    <xsl:copy-of select="@position"/>
    <xsl:copy-of select="@direction"/>
    
    <xsl:choose>
      <xsl:when test="$ipt_make_terminating = 'True'">
        <xsl:attribute name="action">Accept</xsl:attribute>
      </xsl:when>
      <xsl:when test="$pf_make_terminating = 'True'">
        <xsl:attribute name="action">Accept</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="action">Continue</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>

    <xsl:copy-of select="@log"/>
    <xsl:copy-of select="@comment"/>
    <xsl:copy-of select="@group"/>

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
    <xsl:apply-templates select="fwb:Itf" mode="copy"/>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="fwb:When" mode="copy"/>
    <xsl:text>
          </xsl:text>

    <xsl:element name="PolicyRuleOptions" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:text>
            </xsl:text>
      <xsl:for-each select="fwb:PolicyRuleOptions/fwb:Option">
        <xsl:copy-of select="."/>
        <xsl:text>
            </xsl:text>
      </xsl:for-each>

      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">tagging</xsl:attribute>True</xsl:element>
      <xsl:text>
            </xsl:text>
      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">classification</xsl:attribute>False</xsl:element>
      <xsl:text>
            </xsl:text>
      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">routing</xsl:attribute>False</xsl:element>
      <xsl:text>
          </xsl:text>
    </xsl:element>
    <xsl:text>
        </xsl:text>
  </xsl:element>
</xsl:template>


<xsl:template match="//fwb:PolicyRule[attribute::action='Classify']" mode="copy">

  <xsl:variable name="ipt_make_terminating"
    select="../../fwb:FirewallOptions/fwb:Option[attribute::name='classify_mark_terminating']"/>

  <xsl:variable name="pf_make_terminating"
    select="fwb:PolicyRuleOptions/fwb:Option[attribute::name='pf_classify_terminating']"/>

  <xsl:element name="PolicyRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@id"/>
    <xsl:copy-of select="@disabled"/>
    <xsl:copy-of select="@position"/>
    <xsl:copy-of select="@direction"/>
    
    <xsl:choose>
      <xsl:when test="$ipt_make_terminating = 'True'">
        <xsl:attribute name="action">Accept</xsl:attribute>
      </xsl:when>
      <xsl:when test="$pf_make_terminating = 'True'">
        <xsl:attribute name="action">Accept</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="action">Continue</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>

    <xsl:copy-of select="@log"/>
    <xsl:copy-of select="@comment"/>
    <xsl:copy-of select="@group"/>

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
    <xsl:apply-templates select="fwb:Itf" mode="copy"/>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="fwb:When" mode="copy"/>
    <xsl:text>
          </xsl:text>

    <xsl:element name="PolicyRuleOptions" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:text>
            </xsl:text>
      <xsl:for-each select="fwb:PolicyRuleOptions/fwb:Option">
        <xsl:copy-of select="."/>
        <xsl:text>
            </xsl:text>
      </xsl:for-each>

      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">tagging</xsl:attribute>False</xsl:element>
      <xsl:text>
            </xsl:text>
      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">classification</xsl:attribute>True</xsl:element>
      <xsl:text>
            </xsl:text>
      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">routing</xsl:attribute>False</xsl:element>
      <xsl:text>
          </xsl:text>
    </xsl:element>
    <xsl:text>
        </xsl:text>
  </xsl:element>
</xsl:template>


<xsl:template match="//fwb:PolicyRule[attribute::action='Route']" mode="copy">

  <xsl:element name="PolicyRule" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@id"/>
    <xsl:copy-of select="@disabled"/>
    <xsl:copy-of select="@position"/>
    <xsl:copy-of select="@direction"/>
    <xsl:attribute name="action">Continue</xsl:attribute>
    <xsl:copy-of select="@log"/>
    <xsl:copy-of select="@comment"/>
    <xsl:copy-of select="@group"/>

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
    <xsl:apply-templates select="fwb:Itf" mode="copy"/>
    <xsl:text>
          </xsl:text>
    <xsl:apply-templates select="fwb:When" mode="copy"/>
    <xsl:text>
          </xsl:text>

    <xsl:element name="PolicyRuleOptions" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:text>
            </xsl:text>
      <xsl:for-each select="fwb:PolicyRuleOptions/fwb:Option">
        <xsl:copy-of select="."/>
        <xsl:text>
            </xsl:text>
      </xsl:for-each>

      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">tagging</xsl:attribute>False</xsl:element>
      <xsl:text>
            </xsl:text>
      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">classification</xsl:attribute>False</xsl:element>
      <xsl:text>
            </xsl:text>
      <xsl:element name="Option" namespace="http://www.fwbuilder.org/1.0/">
        <xsl:attribute name="name">routing</xsl:attribute>True</xsl:element>
      <xsl:text>
          </xsl:text>
    </xsl:element>
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


