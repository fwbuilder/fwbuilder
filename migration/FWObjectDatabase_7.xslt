<!--
   Filename:     FWObjectDatabase_6.xslt
   Author:       Vadim Kurland
   Build date:   06/04/2008
   Last changed: 06/04/2008
   Version:      1.0.0
   Description:  translates fwbuilder object database from v7 to 8
                 Creates missing TagService objects for all Tag actions.
                 Checks if TagService element with required tagvalue attribute
                 already exists in the same library and does not create
                 duplicates. Also suppresses duplicates if many rules
                 use the same tag value. Duplicate TagService objects are
                 not created inside of each library, but TagService objects
                 with the same codes will be created if firewalls in different
                 libraries need them.
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<!-- ****************************************************************
     declare keys for all TagService elements we might create. Some of
     these elements are going to be duplicates so we won't create them all,
     but to find duplicates we need keys. 

     Use concatenation of the library name and tagvalue option value as a key
     to make sure we create new TagService objects in each library.
-->
<xsl:key name="tag_codes" match="//fwb:PolicyRule[@action='Tag']//fwb:Option[@name='tagvalue']" use="concat(../../../../../../@name, '_', .)"/>


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
<xsl:template name="add_tag_service">
  <xsl:param name="tagvalue"/>
  <xsl:param name="rule_id"/>
  <xsl:param name="rule_num"/>
  <xsl:param name="fw_name"/>
  <xsl:param name="library_name"/>

  <!-- check if TagService object with the same tagcode attribute exists -->

  <xsl:if test="not (//fwb:Library[@name=$library_name]//fwb:ServiceGroup[@name='TagServices']/fwb:TagService[@tagcode=$tagvalue])">

    <xsl:text>
    </xsl:text>

    <xsl:element name="TagService" namespace="http://www.fwbuilder.org/1.0/">
      <xsl:attribute name="comment">Automatically created for <xsl:value-of select="$fw_name"/> rule <xsl:value-of select="$rule_num"/></xsl:attribute>
      <xsl:attribute name="id"><xsl:value-of select="generate-id()"/></xsl:attribute>
      <xsl:attribute name="name">Tag <xsl:value-of select="$tagvalue"/></xsl:attribute>
      <xsl:attribute name="tagcode"><xsl:value-of select="$tagvalue"/></xsl:attribute>
    </xsl:element>
  </xsl:if>
  <xsl:text>
      </xsl:text>
</xsl:template>



<!-- **************************************************************** -->
<xsl:template
    match="fwb:ServiceGroup/fwb:ServiceGroup[@name='TagServices']" mode="copy">

  <xsl:variable name="library_name" select="../../@name"/>
  <xsl:variable name="context_node_id" select="@id"/>

  <xsl:comment>  In library "<xsl:value-of select="$library_name"/>"  </xsl:comment>
  <xsl:text>
      </xsl:text>

  <xsl:element name="ServiceGroup" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/>
    <xsl:text>
        </xsl:text>
    <xsl:apply-templates select="*" mode="copy"/>


    <!-- process rules that use unique tag codes, create TagService
    elements
     -->
    <xsl:for-each select="//fwb:Library[@name=$library_name]//fwb:ObjectGroup[@name='Firewalls']//fwb:PolicyRule[@action='Tag']//fwb:Option[@name='tagvalue' and  count(key('tag_codes', concat($library_name, '_', .))[2]) = 0]">

      <xsl:call-template name="add_tag_service">
        <xsl:with-param name="tagvalue" select="."/>
        <xsl:with-param name="rule_id" select="../../@id"/>
        <xsl:with-param name="rule_num" select="../../@position"/>
        <xsl:with-param name="fw_name" select="../../../../@name"/>
        <xsl:with-param name="library_name" select="$library_name"/>
      </xsl:call-template>

    </xsl:for-each>
    <xsl:text>
      </xsl:text>

    <!-- process rules that use the same tag code, create only one TagService
    element for all of them. Expression
       and key('tag_codes', .)[2] and count(.|key('tag_codes', .)[1]) = 1
    removes duplicates.
     -->

    <xsl:for-each select="//fwb:Library[@name=$library_name]//fwb:ObjectGroup[@name='Firewalls']//fwb:PolicyRule[@action='Tag']//fwb:Option[@name='tagvalue' and key('tag_codes', concat($library_name, '_', .))[2] and count(.|key('tag_codes', concat($library_name, '_', .))[1]) = 1]">

      <xsl:call-template name="add_tag_service">
        <xsl:with-param name="tagvalue" select="."/>
        <xsl:with-param name="rule_id" select="../../@id"/>
        <xsl:with-param name="rule_num" select="../../@position"/>
        <xsl:with-param name="fw_name" select="../../../../@name"/>
        <xsl:with-param name="library_name" select="$library_name"/>
      </xsl:call-template>

    </xsl:for-each>
    <xsl:text>
      </xsl:text>

  </xsl:element>
</xsl:template>

<!-- **************************************************************** -->
<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
    <xsl:attribute name="version">8</xsl:attribute>
    <xsl:attribute name="id">root</xsl:attribute>
    <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>
