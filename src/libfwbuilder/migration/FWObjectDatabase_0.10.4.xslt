<!--
   Filename:     0.10.4/FWObjectDatabase.xslt
   Author:       Vadim Kurland
   Build date:   01/17/2002
   Last changed: 03/01/2002
   Version:      1.0.0
   Description:  translates fwbuilder object database from v0.10.4 to v0.10.5

		 adds group "Address Ranges" (ID='stdid14')

		 adds subelement Management to Host and Firewall and moves
		 attributes snmp_read_community and snmp_write_community to
		 Management/SNMPManagement

                 moves attributes inst_script and inst_cmdline to the element
                 PolicyInstallScript

		 fixes Interface elements that have address element but it
		 is an empty string

		 adds missing TCP flags 'PSH' and 'URG' to all TCPService
		 elements

		 changes platform name 'ipfilter' -> 'ipf'

****************************************************************************
*                                                                          *
*   Files of versions 0.10.3 and older did not have namespace. Files       *
*   starting from v 0.10.4 have namespace defined in the element           *
*   FWObjectDatabase as follows:                                           *
*                                                                          *
*   <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/"                *
*	    version="0.10.4" id="root">                                    *
*                                                                          *
*   I've replaced transformation for upgrade 0.10.3->0.10.4 so the new one *
*   now adds namespace in the same way. People who upgrade from older      *
*   versions will use this new transormation and will get intermediate     *
*   data tree (of version 0.10.4) with namespace defined in it.            *
*                                                                          *
*   Therefore I could build transformation for upgrade 0.10.4->0.10.5      *
*   assuming that the namespace declaration is always there.               *
*                                                                          *
*   Transformation 0.10.3->0.10.4 does not add namespace prefixes to the   *
*   elements. It only adds namespace declaration using xmlns in the root   *
*   element.                                                               *
*                                                                          *
*   This transformation (0.10.4->0.10.5) uses its own loacally defined     *
*   prefix 'fwb' to be able to match on elements with the namespace. This  *
*   prefix is defined in xsl:stylesheet element. This prefix won't be      *
*   added to the output data, it is used only within this transformation.  *
*                                                                          *
****************************************************************************

-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fwb="http://www.fwbuilder.org/1.0/">



<xsl:output method="xml" version="1.0" 
   doctype-system="fwbuilder.dtd" indent="yes" encoding="utf-8"/>




<xsl:template match="@*" mode="copy">
<xsl:copy>
  <xsl:apply-templates select="@*" mode="copy"/>
</xsl:copy>
</xsl:template>

<xsl:template match="*" mode="copy">
<xsl:copy>
  <xsl:apply-templates select="@*" mode="copy"/>
  <xsl:apply-templates mode="copy"/>
</xsl:copy>
</xsl:template>

<!--
<xsl:template match="*">
<xsl:copy-of select="."/>
</xsl:template>
-->


<xsl:template match="*[attribute::id='sysid0']" mode="copy">
<xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="*[attribute::id='sysid1']" mode="copy">
<xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="*[attribute::id='sysid2']" mode="copy">
<xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="*[attribute::id='sysid3']" mode="copy">
<xsl:copy-of select="."/>
</xsl:template>

<!--
***************************************************************************
* This template corrects end of port range for UDP service traceroute     *
***************************************************************************
-->
<xsl:template match="fwb:UDPService[@id='id3AED0D8C']" mode="copy">
<UDPService>

<xsl:for-each select="@*">
<xsl:choose>
<xsl:when test="name()='dst_range_end'">
  <xsl:attribute name="dst_range_end">33524</xsl:attribute>
</xsl:when>
<xsl:otherwise>
  <xsl:copy-of select="."/>
</xsl:otherwise>
</xsl:choose>
</xsl:for-each>

</UDPService>
</xsl:template>

<!--
***************************************************************************
* This template adds missing TCP flags to TCPService                      *
***************************************************************************
-->
<xsl:template match="fwb:TCPService" mode="copy">
<TCPService>

<xsl:choose>
<xsl:when test="@urg_flag"></xsl:when>
<xsl:otherwise>
  <xsl:attribute name="urg_flag">False</xsl:attribute>
</xsl:otherwise>
</xsl:choose>

<xsl:choose>
<xsl:when test="@psh_flag"></xsl:when>
<xsl:otherwise>
  <xsl:attribute name="psh_flag">False</xsl:attribute>
</xsl:otherwise>
</xsl:choose>

<xsl:for-each select="@*">
  <xsl:copy-of select="."/>
</xsl:for-each>

</TCPService>
</xsl:template>

<!--
***************************************************************************
* This template fixes Interface elements that have address attribute      *
* but it is an empty string                                               *
***************************************************************************
-->
<xsl:template match="fwb:Interface" mode="copy">
<xsl:text>
</xsl:text>

<Interface>

<xsl:choose>
<xsl:when test="@address=''">
  <xsl:attribute name="address">
  <xsl:value-of select="../@address"/>
  </xsl:attribute>
</xsl:when>
<xsl:otherwise>
  <xsl:attribute name="address">
  <xsl:value-of select="@address"/>
  </xsl:attribute>
</xsl:otherwise>
</xsl:choose>

<xsl:for-each select="@*">
<xsl:choose>
<xsl:when test="name()='address'"></xsl:when>
<xsl:otherwise>
  <xsl:copy-of select="."/>
</xsl:otherwise>
</xsl:choose>
</xsl:for-each>


  <xsl:apply-templates mode="copy"/>

</Interface>

</xsl:template>


<!--
***************************************************************************
* This template works for hosts that have attributes 'snmp_read_community'*
* or 'snmp_write_community'. These attributes now move to a child element *
* Management/SNMPManagement                                               *
* Since we match only on elements that have attribute snmp_read_community *
* or snmp_write_community, and these attributes disappear in the output,  *
* this transformation can be ran multiple times on the same data          *
***************************************************************************
-->

<xsl:template match="fwb:Host[@snmp_read_community]|fwb:Host[@snmp_write_community]" mode="copy">

<xsl:variable name="snmp_read"   select="@snmp_read_community"/>
<xsl:variable name="snmp_write"  select="@snmp_write_community"/>
<xsl:variable name="address"     select="@address"/>

<Host>

<xsl:for-each select="@*">
<xsl:choose>
<xsl:when test="name()='snmp_read_community'"></xsl:when>
<xsl:when test="name()='snmp_write_community'"></xsl:when>
<xsl:otherwise>
  <xsl:copy-of select="."/>
</xsl:otherwise>
</xsl:choose>
</xsl:for-each>

<xsl:apply-templates select="fwb:Interface" mode="copy"/>

<xsl:text>
</xsl:text>
<Management>
<xsl:attribute name="address">
  <xsl:value-of select="$address"/>
</xsl:attribute>

  <xsl:text>
  </xsl:text>
  <SNMPManagement>
  <xsl:attribute name="snmp_read_community"><xsl:value-of select="$snmp_read"/></xsl:attribute>
  <xsl:attribute name="snmp_write_community"><xsl:value-of select="$snmp_write"/></xsl:attribute>
</SNMPManagement>

<xsl:text>
</xsl:text>
</Management>

<xsl:for-each select="fwb:HostOptions">
<xsl:text>
</xsl:text>
  <xsl:copy-of select="."/>
</xsl:for-each>

<xsl:text>
</xsl:text>
</Host>

</xsl:template>

<!--
***************************************************************************
* This template works for firewalls that have attributes                  *
* 'snmp_read_community' or 'snmp_write_community'. These attributes       *
* moved to a child element Management/SNMPManagement                      *
* Since we match only on elements that have attribute snmp_read_community *
* or snmp_write_community, and these attributes disappear in the output,  *
* this transformation can be ran multiple times on the same data          *
*                                                                         *
*                                                                         *
* These templates work for firewalls that have elements                   *
* FirewallOptions/Option[name="inst_cmdline"] or                          *
* FirewallOptions/Option[name="inst_script"]                              *
* Both elements moved to a child element Management/PolicyInstallScript   *
*                                                                         *
* Since we check for presence of attribute snmp_read_community or         *
* snmp_write_communityelement or FirewallOptions/Option[name="inst_cmdline"] 
* and these elements disappear in the output, this transformation can be  *
* ran multiple times on the same data                                     *
*                                                                         *
***************************************************************************
-->
<xsl:template match="fwb:Firewall" mode="copy">

<xsl:variable name="snmp_read"    select="@snmp_read_community"/>
<xsl:variable name="snmp_write"   select="@snmp_write_community"/>
<xsl:variable name="address"      select="@address"/>
<xsl:variable name="platform"     select="@platform"/>

<xsl:variable name="inst_script" >
<xsl:value-of select="fwb:FirewallOptions/fwb:Option[@name='inst_script']"/>
</xsl:variable>

<xsl:variable name="inst_cmdline">
<xsl:value-of select="fwb:FirewallOptions/fwb:Option[@name='inst_cmdline']"/>
</xsl:variable>

<Firewall>

<!-- copy attributes -->
<xsl:for-each select="@*">
<xsl:choose>
<xsl:when test="name()='snmp_read_community'"></xsl:when>
<xsl:when test="name()='snmp_write_community'"></xsl:when>
<xsl:when test="name()='platform'">
  <xsl:choose>
    <xsl:when test="$platform='ipfilter'">
	<xsl:attribute name="platform">ipf</xsl:attribute>
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy-of select="."/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:when>
<xsl:otherwise>
  <xsl:copy-of select="."/>
</xsl:otherwise>
</xsl:choose>
</xsl:for-each>

<xsl:for-each select="fwb:NAT">
  <xsl:text>
  </xsl:text>
  <xsl:copy-of select="."/>
</xsl:for-each>

<xsl:for-each select="fwb:Policy">
<xsl:text>
</xsl:text>
  <xsl:copy-of select="."/>
</xsl:for-each>

<xsl:apply-templates select="fwb:Interface" mode="copy"/>


<xsl:choose>
  <xsl:when test="Manamement">
<!-- element Management is present, add children elements is needed -->

  </xsl:when>
  <xsl:otherwise>


<xsl:text>
</xsl:text>
<Management>
<xsl:attribute name="address">
  <xsl:value-of select="$address"/>
</xsl:attribute>

<xsl:text>
</xsl:text>
<SNMPManagement>
  <xsl:attribute name="snmp_read_community"><xsl:value-of select="$snmp_read"/></xsl:attribute>
  <xsl:attribute name="snmp_write_community"><xsl:value-of select="$snmp_write"/></xsl:attribute>
</SNMPManagement>

<xsl:text>
</xsl:text>
<PolicyInstallScript>
    <xsl:choose>
      <xsl:when test="$inst_script=''">
  <xsl:attribute name="enabled">False</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
  <xsl:attribute name="enabled">True</xsl:attribute>
  <xsl:attribute name="command">  <xsl:value-of select="$inst_script"/></xsl:attribute>
  <xsl:attribute name="arguments"><xsl:value-of select="$inst_cmdline"/></xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
</PolicyInstallScript>



<xsl:text>
</xsl:text>
</Management>

  </xsl:otherwise>
</xsl:choose>

<xsl:for-each select="fwb:FirewallOptions">
<xsl:text>
</xsl:text>
  <xsl:copy-of select="."/>
</xsl:for-each>

<xsl:text>
</xsl:text>
</Firewall>
</xsl:template>


<!--
*************************************************************************
*  the following template recusrively copies ObjectGroup "Objects" and  *
*  adds child element "ObjectGroup" with id='stdid15' and name="Address *
*  Ranges" if it did not exist there before. Checking for existance of  *
*  this child element allows us to run this transformation multiple     *
*  times on the same data file                                          *
************************************************************************* 
-->
<xsl:template match="*[attribute::id='stdid01']" mode="copy">
<xsl:choose>
<xsl:when test="*[attribute::id='stdid15']">
  <ObjectGroup id="stdid01" library="Standard" name="Objects">
  <xsl:apply-templates mode="copy"/>
  </ObjectGroup>
</xsl:when>
<xsl:otherwise>
  <ObjectGroup id="stdid01" library="Standard" name="Objects">
  <xsl:apply-templates mode="copy"/>
  <ObjectGroup id="stdid15" library="Standard" name="Address Ranges"/>
  <xsl:text>
  </xsl:text>
  </ObjectGroup>
</xsl:otherwise>
</xsl:choose>

</xsl:template>

<!-- Group 'Services' -->
<xsl:template match="*[attribute::id='stdid05']" mode="copy">
<ServiceGroup id="stdid05" library="Standard" name="Services">
<xsl:apply-templates mode="copy"/>
</ServiceGroup>
</xsl:template>

<!-- Group 'UDP Services' -->
<xsl:template match="*[attribute::id='stdid08']" mode="copy">
<ServiceGroup id="stdid08" library="Standard" name="UDP">
<xsl:apply-templates mode="copy"/>
</ServiceGroup>
</xsl:template>

<!-- Group 'TCP Services' -->
<xsl:template match="*[attribute::id='stdid09']" mode="copy">
<ServiceGroup id="stdid09" library="Standard" name="TCP">
<xsl:apply-templates mode="copy"/>
</ServiceGroup>
</xsl:template>

<!-- Group 'Firewalls' -->
<xsl:template match="*[attribute::id='stdid12']" mode="copy">
<ObjectGroup id="stdid12" library="Standard" name="Firewalls">
<xsl:apply-templates mode="copy"/>
</ObjectGroup>
</xsl:template>

<xsl:template match="*[attribute::id='stdid11']" mode="copy">
<xsl:copy-of select="."/>
</xsl:template>



<xsl:template match="*[attribute::id='root']">

  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">

  <xsl:attribute name="version">0.10.5</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>

  <xsl:apply-templates select="*" mode="copy"/>

  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>

