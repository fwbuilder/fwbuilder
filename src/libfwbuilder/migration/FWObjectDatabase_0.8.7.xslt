<!--
   Filename:     FWObjectDatabase.xslt
   Author:       Friedhelm Duesterhoeft
   Build date:   02/05/2001
   Last changed: 05/11/2001
   Version:      1.4.3
   Description:  translates old style fwbuilder configs according 
	         to new DTD (1.40);
   Caveates:     none known

   Changes from v1.4.1
                 added support of Interface Policies

   Changes from v1.4.2  by vadim@vk.crocodile.org
                fixed bug when InterfacePolicy objects created by 
		this script where duplicated

-->
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" version="1.0" indent="yes"
  standalone="no" encoding="utf-8"
  doctype-system="fwbuilder.dtd" />

  <xsl:template match="*[attribute::_type='INTERFACE']">
    <xsl:variable name="ifname">
      <xsl:value-of select="local-name()" />
    </xsl:variable>

    <xsl:variable name="fwname">
      <xsl:value-of select="local-name(parent::*)" />
    </xsl:variable>

    <Interface
    id="if-{../@_type}-{$fwname}-{$ifname}"
    name="{$ifname}" address="{@address}" netmask="{@netmask}"
    dyn="{translate(boolean(@dyn='1'),'truefals','TrueFals')}"
    ext="{translate(boolean(@ext='1'),'truefals','TrueFals')}">

    <xsl:choose>
      <xsl:when test="../Policy[*/intf/@val=$ifname]">
        <xsl:apply-templates select="../Policy[*/intf/@val=$ifname]" 
	     mode="local">
          <xsl:with-param name="ifname" select="$ifname"/>
        </xsl:apply-templates>
      </xsl:when>

      <xsl:otherwise>
        <InterfacePolicy id="{concat('pol-',$fwname,'-',$ifname)}"/>
      </xsl:otherwise>

    </xsl:choose>
    </Interface>
  </xsl:template>

  <xsl:template match="*[attribute::_type='REF']">
    <xsl:choose>
      <xsl:when test="contains(@ptr,'/Objects/Hosts/')">
        <ObjectRef ref="host-{local-name()}" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Objects/Networks/')">
        <ObjectRef ref="net-{local-name()}" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Objects/Groups/')">
        <ObjectRef ref="og-{local-name()}" />
      </xsl:when>

      <xsl:when
      test="contains(@ptr,'/Objects/Any') and substring-after(@ptr,'/Objects/Any')=''">

        <ObjectRef ref="sysid0" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Firewalls/')">
        <ObjectRef ref="fw-{local-name()}" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Services/ICMP/')">
        <ServiceRef ref="icmp-{local-name()}" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Services/IP/')">
        <ServiceRef ref="ip-{local-name()}" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Services/TCP/')">
        <ServiceRef ref="tcp-{local-name()}" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Services/UDP/')">
        <ServiceRef ref="udp-{local-name()}" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Services/Groups/')">
        <ServiceRef ref="sg-{local-name()}" />
      </xsl:when>

      <xsl:when
      test="contains(@ptr,'/Services/Any') and substring-after(@ptr,'/Services/Any')=''">

        <ServiceRef ref="sysid1" />
      </xsl:when>

      <xsl:when
      test="contains(@ptr,'/Time/Any') and substring-after(@ptr,'/Time/Any')=''">

        <IntervalRef ref="sysid2" />
      </xsl:when>

      <xsl:when test="contains(@ptr,'/Time/')">
        <IntervalRef ref="int-{local-name()}" />
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="Hosts">
    <ObjectGroup name="Hosts" id="stdid02">
      <xsl:for-each select="*[attribute::_type='HOST']">
        <Host id="host-{local-name()}" name="{local-name()}"
        address="{@address}" comment="{@comment}"
        snmp_read_community="{@snmp_read_community}"
        snmp_write_community="{@snmp_write_community}">
          <xsl:apply-templates
          select="*[attribute::_type='INTERFACE']" />
        </Host>
      </xsl:for-each>
    </ObjectGroup>
  </xsl:template>

  <xsl:template match="Networks">
    <ObjectGroup name="Networks" id="stdid03">
      <xsl:for-each select="*[attribute::_type='NET']">
        <Network id="net-{local-name()}" name="{local-name()}"
        address="{@address}" comment="{@comment}"
        netmask="{@netmask}" />
      </xsl:for-each>
    </ObjectGroup>
  </xsl:template>

  <xsl:template match="Groups[@_type='GROUP']" mode="Objects">
    <ObjectGroup name="Groups" id="stdid04">
      <xsl:for-each select="*[attribute::_type='GROUP']">
        <ObjectGroup id="og-{local-name()}" name="{local-name()}"
        comment="{@comment}">
          <xsl:apply-templates
          select="*[attribute::_type='REF']" />
        </ObjectGroup>
      </xsl:for-each>
    </ObjectGroup>
  </xsl:template>

  <xsl:template match="Objects[@_type='GROUP']">
    <ObjectGroup name="ScratchPad" id="sysid3" />

    <ObjectGroup name="Objects" id="stdid01">
      <xsl:apply-templates select="Groups" mode="Objects" />

      <xsl:apply-templates select="Hosts" />

      <xsl:apply-templates select="Networks" />
    </ObjectGroup>
  </xsl:template>

  <xsl:template match="ICMP[@_type='GROUP']">
    <ServiceGroup name="ICMP" id="stdid07">
      <xsl:for-each select="*[attribute::_type='ICMP']">
        <ICMPService id="icmp-{local-name()}" name="{local-name()}"
        code="{@code}" comment="{@comment}" type="{@type}" />
      </xsl:for-each>
    </ServiceGroup>
  </xsl:template>

  <xsl:template match="IP[@_type='GROUP']">
    <ServiceGroup name="IP" id="stdid06">
      <xsl:for-each select="*[attribute::_type='IP']">
        <IPService id="ip-{local-name()}" name="{local-name()}"
        comment="{@comment}"
        fragm="{translate(boolean(@fragm='1'),'truefals','TrueFals')}"
         lsrr="{translate(boolean(@lsrr='1'),'truefals','TrueFals')}"
         protocol_num="{@protocol_num}"
        rr="{translate(boolean(@rr='1'),'truefals','TrueFals')}"
        short_fragm="{translate(boolean(@short_fragm='1'),'truefals','TrueFals')}"
         ssrr="{translate(boolean(@ssrr='1'),'truefals','TrueFals')}"
         ts="{translate(boolean(@ts='1'),'truefals','TrueFals')}" />
      </xsl:for-each>
    </ServiceGroup>
  </xsl:template>

  <xsl:template match="TCP[@_type='GROUP']">
    <ServiceGroup name="TCP" id="stdid09">
      <xsl:for-each select="*[attribute::_type='TCP']">
        <TCPService id="tcp-{local-name()}" name="{local-name()}"
        comment="{comment}"
        ack_flag="{translate(boolean(@ack_flag='1'),'truefals','TrueFals')}"
         dst_range_end="{@dst_range_end}"
        dst_range_start="{@dst_range_start}"
        fin_flag="{translate(boolean(@fin_flag='1'),'truefals','TrueFals')}"
         rst_flag="{translate(boolean(@rst_flag='1'),'truefals','TrueFals')}"
         src_range_end="{@src_range_end}"
        src_range_start="{@src_range_start}"
        syn_flag="{translate(boolean(@syn_flag='1'),'truefals','TrueFals')}" />
      </xsl:for-each>
    </ServiceGroup>
  </xsl:template>

  <xsl:template match="UDP[@_type='GROUP']">
    <ServiceGroup name="UDP" id="stdid08">
      <xsl:for-each select="*[attribute::_type='UDP']">
        <UDPService id="udp-{local-name()}" name="{local-name()}"
        comment="{comment}" dst_range_end="{@dst_range_end}"
        dst_range_start="{@dst_range_start}"
        src_range_end="{@src_range_end}"
        src_range_start="{@src_range_start}" />
      </xsl:for-each>
    </ServiceGroup>
  </xsl:template>

  <xsl:template match="Groups[@_type='GROUP']" mode="Services">
    <ServiceGroup name="Groups" id="stdid10">
      <xsl:for-each select="*[attribute::_type='GROUP']">
        <ServiceGroup id="sg-{local-name()}" name="{local-name()}"
        comment="{@comment}">
          <xsl:apply-templates
          select="*[attribute::_type='REF']" />
        </ServiceGroup>
      </xsl:for-each>
    </ServiceGroup>
  </xsl:template>

  <xsl:template match="Services[@_type='GROUP']">
    <ServiceGroup name="Services" id="stdid05">
      <xsl:apply-templates select="Groups" mode="Services" />

      <xsl:apply-templates select="ICMP" />

      <xsl:apply-templates select="IP" />

      <xsl:apply-templates select="TCP" />

      <xsl:apply-templates select="UDP" />
    </ServiceGroup>
  </xsl:template>

  <xsl:template match="Time[@_type='GROUP']">
    <IntervalGroup name="Time" id="stdid11">
      <xsl:for-each select="*[attribute::_type='TIME']">
        <Interval id="int-{local-name()}" name="{local-name()}"
        comment="{@comment}" from_second="0" to_second="0"
        from_minute="{@min1}" to_minute="{@min2}"
        from_hour="{@hour1}" to_hour="{@hour2}" from_day="{@day1}"
        to_day="{@day2}" from_month="{@month1}"
        to_month="{@month2}" from_year="{@year1}"
        to_year="{@year2}" from_weekday="{@dayofweek1}"
        to_weekday="{@dayofweek2}" />
      </xsl:for-each>
    </IntervalGroup>
  </xsl:template>

  <xsl:template match="NAT[@_type='NAT']">
    <xsl:variable name="fwname">
      <xsl:value-of select="local-name(parent::*)" />
    </xsl:variable>

    <NAT id="nat-{$fwname}">
      <xsl:for-each select="*[attribute::_type='RULE']">
        <NATRule
        id="nat-{$fwname}-{substring-after(local-name(),'rule_')}"
        comment="{com/@val}"
        position="{substring-after(local-name(),'rule_')}">
          <OSrc
          neg="{translate(boolean(osrc/@neg='1'),'truefals','TrueFals')}">

            <xsl:apply-templates
            select="osrc/*[attribute::_type='REF']" />
          </OSrc>

          <ODst
          neg="{translate(boolean(odst/@neg='1'),'truefals','TrueFals')}">

            <xsl:apply-templates
            select="odst/*[attribute::_type='REF']" />
          </ODst>

          <OSrv
          neg="{translate(boolean(osrv/@neg='1'),'truefals','TrueFals')}">

            <xsl:apply-templates
            select="osrv/*[attribute::_type='REF']" />
          </OSrv>

          <TSrc
          neg="{translate(boolean(tsrc/@neg='1'),'truefals','TrueFals')}">

            <xsl:apply-templates
            select="tsrc/*[attribute::_type='REF']" />
          </TSrc>

          <TDst
          neg="{translate(boolean(tdst/@neg='1'),'truefals','TrueFals')}">

            <xsl:apply-templates
            select="tdst/*[attribute::_type='REF']" />
          </TDst>

          <TSrv
          neg="{translate(boolean(tsrv/@neg='1'),'truefals','TrueFals')}">

            <xsl:apply-templates
            select="tsrv/*[attribute::_type='REF']" />
          </TSrv>

          <xsl:if test="time/*[attribute::_type='REF']">
            <When
            neg="{translate(boolean(srv/@neg='1'),'truefals','TrueFals')}">

              <xsl:apply-templates
              select="time/*[attribute::_type='REF']" />
            </When>
          </xsl:if>

          <NATRuleOptions />
        </NATRule>
      </xsl:for-each>
    </NAT>
  </xsl:template>

  <xsl:template match="*[attribute::_type='RULE']" mode="iface">
    <xsl:variable name="poscount">
      <xsl:value-of select="position()" />
    </xsl:variable>

    <xsl:variable name="fwname">
      <xsl:value-of select="local-name(../..)" />
    </xsl:variable>

    <xsl:variable name="dir">
      <xsl:choose>
        <xsl:when test="starts-with(dir/@val,'i')">
          <xsl:value-of select="string('Inbound')" />
        </xsl:when>

        <xsl:when test="starts-with(dir/@val,'o')">
          <xsl:value-of select="string('Outbound')" />
        </xsl:when>

        <xsl:when test="starts-with(dir/@val,'b')">
          <xsl:value-of select="string('Both')" />
        </xsl:when>

        <xsl:otherwise>
          <xsl:value-of select="dir/@val" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="act">
      <xsl:choose>
   
        <xsl:when test="starts-with(act/@val,'A')">
	  <xsl:value-of select="string('Accept')"/>
	</xsl:when>

        <xsl:when test="starts-with(act/@val,'D')">
	  <xsl:value-of select="string('Deny')"/>
	</xsl:when>

        <xsl:when test="starts-with(act/@val,'R')">
	  <xsl:value-of select="string('Reject')"/>
	</xsl:when>

	<xsl:otherwise>
          <xsl:value-of select="string('Deny')" />
	</xsl:otherwise>

      </xsl:choose>
    </xsl:variable>


    <PolicyRule
    id="pol-{$fwname}-{substring-after(local-name(),'rule_')}"
    comment="{com/@val}" position="{$poscount - 1}"
    disabled="False" direction="{$dir}" action="{$act}"
    log="{translate(boolean(log/@val='Log'),'truefals','TrueFals')}">

      <Src
      neg="{translate(boolean(src/@neg='1'),'truefals','TrueFals')}">

        <xsl:apply-templates
        select="src/*[attribute::_type='REF']" />
      </Src>

      <Dst
      neg="{translate(boolean(dst/@neg='1'),'truefals','TrueFals')}">

        <xsl:apply-templates
        select="dst/*[attribute::_type='REF']" />
      </Dst>

      <Srv
      neg="{translate(boolean(srv/@neg='1'),'truefals','TrueFals')}">

        <xsl:apply-templates
        select="srv/*[attribute::_type='REF']" />
      </Srv>

      <xsl:if test="time/*[attribute::_type='REF']">
        <When
        neg="{translate(boolean(srv/@neg='1'),'truefals','TrueFals')}">

          <xsl:apply-templates
          select="time/*[attribute::_type='REF']" />
        </When>

        <PolicyRuleOptions />
      </xsl:if>
    </PolicyRule>
  </xsl:template>

  <xsl:template match="Policy[@_type='POLICY']" mode="local">
    <xsl:param name="ifname"/>

    <xsl:variable name="fwname">
      <xsl:value-of select="local-name(..)" />
    </xsl:variable>

    <xsl:variable name="intf">
      <xsl:value-of select="*/intf[1]/@val" />
    </xsl:variable>
    <InterfacePolicy id="{concat('pol-',$fwname,'-',$ifname)}">
      <xsl:apply-templates   select="*[intf/@val=$ifname]" mode="iface" />
    </InterfacePolicy>
  </xsl:template>

  <xsl:template match="*[attribute::_type='RULE']" mode="global">
    <xsl:variable name="poscount">
      <xsl:value-of select="position()" />
    </xsl:variable>

    <xsl:variable name="fwname">
      <xsl:value-of select="local-name(../..)" />
    </xsl:variable>

    <xsl:variable name="act">
      <xsl:choose>
   
        <xsl:when test="act/@val">
	  <xsl:value-of select="act/@val"/>
	</xsl:when>

	<xsl:otherwise>"Deny"</xsl:otherwise>

      </xsl:choose>
    </xsl:variable>


    <PolicyRule
    id="pol-{$fwname}-{substring-after(local-name(),'rule_')}"
    comment="{com/@val}" position="{$poscount - 1}"
    disabled="False" action="{$act}"
    log="{translate(boolean(log/@val='Log'),'truefals','TrueFals')}">

      <Src
      neg="{translate(boolean(src/@neg='1'),'truefals','TrueFals')}">

        <xsl:apply-templates
        select="src/*[attribute::_type='REF']" />
      </Src>

      <Dst
      neg="{translate(boolean(dst/@neg='1'),'truefals','TrueFals')}">

        <xsl:apply-templates
        select="dst/*[attribute::_type='REF']" />
      </Dst>

      <Srv
      neg="{translate(boolean(srv/@neg='1'),'truefals','TrueFals')}">

        <xsl:apply-templates
        select="srv/*[attribute::_type='REF']" />
      </Srv>

      <xsl:if test="time/*[attribute::_type='REF']">
        <When
        neg="{translate(boolean(srv/@neg='1'),'truefals','TrueFals')}">

          <xsl:apply-templates
          select="time/*[attribute::_type='REF']" />
        </When>

        <PolicyRuleOptions />
      </xsl:if>
    </PolicyRule>
  </xsl:template>

  <xsl:template match="Policy[@_type='POLICY']" mode="global">
    <xsl:variable name="fwname">
      <xsl:value-of select="local-name(..)" />
    </xsl:variable>

    <Policy id="pol-{$fwname}">
      <xsl:apply-templates
      select="*[attribute::_type='RULE' and intf[attribute::val='All']]"
       mode="global" />
    </Policy>
  </xsl:template>

  <xsl:template match="Firewalls[@_type='GROUP']">
    <ObjectGroup name="Firewalls" id="stdid12">
      <xsl:for-each select="*[@_type='FW']">
        <Firewall id="fw-{local-name()}" name="{local-name()}"
        comment="{@comment}" address="{@address}"
        snmp_read_community="{@snmp_read_community}"
        snmp_write_community="{@snmp_write_community}"
        platform="{@platform}" version="{@version}">
          <xsl:apply-templates select="NAT" />

          <xsl:apply-templates select="Policy" mode="global" />

          <xsl:apply-templates select="*[@_type='INTERFACE']" />

          <FirewallOptions />
        </Firewall>
      </xsl:for-each>
    </ObjectGroup>
  </xsl:template>

  <xsl:attribute-set name="FWObjectDatabase">
    <xsl:attribute name="version">0.9.0</xsl:attribute>
  </xsl:attribute-set>

  <xsl:template match="/FWObjectDatabase">
    <xsl:element name="FWObjectDatabase"
    use-attribute-sets="FWObjectDatabase">
      <AnyNetwork id="sysid0" name="Any" comment="Any Network" />

      <AnyIPService id="sysid1" name="Any"
      comment="Any IP Service" />

      <AnyInterval id="sysid2" name="Any" comment="Any Interval" />

      <xsl:apply-templates select="Objects" />

      <xsl:apply-templates select="Services" />

      <xsl:apply-templates select="Firewalls" />

      <xsl:apply-templates select="Time" />
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>

