<!--
   Filename:     1.0.2/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   12/25/2003
   Last changed: 03/28/2004
   Version:      1.0.0
   Description:  translates fwbuilder object database from v1.0.2 to v2.0.0

                 removes standard object group "ScratchPad" ( id=sysid3 )

                 adds libraries and moves all user defined objects to
                 library "User"

                 and all standard objects to library "Standard"

                 makes library Standard read-only using attribute 'ro'
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/"
    exclude-result-prefixes="fwb">


<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>

<xsl:template match="fwb:ObjectRef|fwb:ServiceRef|fwb:IntervalRef|fwb:CustomServiceCommand|fwb:Option" mode="copy_std" priority="10">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="*[@library='Standard']" mode="copy_std" priority="6">
  <xsl:if test="@id!='sysid3'">
    <xsl:copy>
      <xsl:for-each select="@*">
        <xsl:if test="not (name()='library')">
          <xsl:copy-of select="."/>
        </xsl:if>
      </xsl:for-each>
      <xsl:apply-templates select="*" mode="copy_std"/>
    </xsl:copy>
  </xsl:if>
</xsl:template>

<xsl:template match="*" mode="copy_std" priority="1"/>

<!-- ###################################################################### -->

<!--
  elements that have data in the body. It is simpler to just use deep-copy 
  to copy it all.
-->
<xsl:template match="fwb:ObjectRef|fwb:ServiceRef|fwb:IntervalRef|fwb:CustomServiceCommand|fwb:Option"
  mode="copy_usr" priority="10">
  <xsl:copy-of select="."/>
</xsl:template>

<!-- special-case group /User/Objects because we need to
     add a new subgroup 'Addresses' -->

<xsl:template match="*[@id='stdid01']" mode="copy_usr" priority="8">
  <xsl:copy>
    <xsl:for-each select="@*">
      <xsl:if test="not (name()='library')">
        <xsl:if test="name()='id'">
          <xsl:attribute name="id">
            <xsl:value-of select="."/>
            <xsl:if test="starts-with(.,'stdid')">_1</xsl:if>
          </xsl:attribute>
        </xsl:if>
        <xsl:if test="not (name()='id')">
          <xsl:copy select="."/>
        </xsl:if>
      </xsl:if>
    </xsl:for-each>
    <ObjectGroup xmlns="http://www.fwbuilder.org/1.0/" id="stdid16_1" name="Addresses"/>
    <xsl:apply-templates select="*[starts-with(@id,'stdid') or not(@library)]" mode="copy_usr"/>
  </xsl:copy>
</xsl:template>


<!--
   replace Ids of the standard objects that are in the User library with
   new Ids to avoid duplicates. Generate new Ids by appending "_1" to the old ones
-->
<xsl:template match="*[(starts-with(@id,'stdid') and not(starts-with(@id,'stdid14'))) or not(@library)]"
  mode="copy_usr" priority="6">
  <xsl:copy>
    <xsl:for-each select="@*">
      <xsl:if test="not (name()='library')">
        <xsl:if test="name()='id'">
          <xsl:attribute name="id">
            <xsl:value-of select="."/>
            <xsl:if test="starts-with(.,'stdid')">_1</xsl:if>
          </xsl:attribute>
        </xsl:if>
        <xsl:if test="not (name()='id')">
          <xsl:copy-of select="."/>
        </xsl:if>
      </xsl:if>
    </xsl:for-each>
    <xsl:apply-templates select="*" mode="copy_usr"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*" mode="copy_usr" priority="1"/>

<!-- ###################################################################### -->

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">2.0.0</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>

<Library>
  <xsl:attribute name="id">syslib000</xsl:attribute>
  <xsl:attribute name="name">Standard</xsl:attribute>
  <xsl:attribute name="comment">Standard objects</xsl:attribute>
  <xsl:attribute name="color">#d4f8ff</xsl:attribute>
  <xsl:attribute name="ro">True</xsl:attribute>
  <xsl:apply-templates select="*[@library='Standard']" mode="copy_std"/>
</Library>

<Library>
  <xsl:attribute name="id">syslib001</xsl:attribute>
  <xsl:attribute name="name">User</xsl:attribute>
  <xsl:attribute name="comment">User defined objects</xsl:attribute>
  <xsl:attribute name="color">#d2ffd0</xsl:attribute>
  <xsl:apply-templates select="*[starts-with(@id,'stdid')]" mode="copy_usr"/>
</Library>


  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

