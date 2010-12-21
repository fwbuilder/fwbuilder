<!--  -*- mode: xml; tab-width: 2; -*-
   Filename:     FWObjectDatabase_2.1.0.xslt
   Author:       Vadim Kurland
   Build date:   09/27/2005
   Last changed: 09/27/2005
   Version:      1.0.0
   Description:  translates fwbuilder object database from v2.1.1 to v2.1.2

                 changes version number
                 merges InterfacePolicy and Policy
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


<xsl:template match="fwb:Interface" mode="copy">
  <xsl:text>
        </xsl:text>
  <xsl:element name="Interface" namespace="http://www.fwbuilder.org/1.0/">
    <xsl:copy-of select="@*"/> 
    <xsl:text>
          </xsl:text>
    <xsl:for-each select="fwb:*">
      <xsl:choose>
        <xsl:when test="local-name()='InterfacePolicy'">
        </xsl:when>
        <xsl:otherwise>
          <xsl:copy-of select="." />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    <xsl:text>
        </xsl:text>
  </xsl:element>
</xsl:template>

<xsl:template match="fwb:Firewall" mode="copy">
   
  <xsl:variable name="fwid" select="@id"/>
  <xsl:variable name="nInterfaces" select="count(fwb:Interface)" />
  <xsl:variable name="nInterfacePolicyRules"
                select="count(fwb:Interface/fwb:InterfacePolicy/fwb:PolicyRule)"/>
   
  <Firewall xmlns="http://www.fwbuilder.org/1.0/">

    <xsl:copy-of select="@*"/> 
    <xsl:text>
        </xsl:text>    

    <xsl:comment> num. of interfaces: <xsl:value-of
                                         select="$nInterfaces"/> </xsl:comment>
    <xsl:text>
        </xsl:text>    
    <xsl:comment> num. of interface policy rules: <xsl:value-of
                              select="$nInterfacePolicyRules" /> </xsl:comment>
    <xsl:text>
        </xsl:text>    

    <xsl:copy-of select="fwb:NAT" />

    <xsl:text>
        </xsl:text>    
    <xsl:element name="Policy">
      <xsl:copy-of select="fwb:Policy/@*"/>
      <xsl:text>
          </xsl:text>    
<!-- first copy interface policies *** NEED TO FIX POSITIONS! *** -->

      <xsl:for-each select="fwb:Interface">
        <xsl:variable name="interfaceIdx" select="position()" />
        <xsl:variable name="itfid" select="@id"/>
        <xsl:variable name="nPrecedingInterfacePolicyRules"
  select="count(preceding::fwb:Interface/fwb:InterfacePolicy/fwb:PolicyRule)" />

        <xsl:comment> interface  <xsl:value-of
            select="@name"/> id=<xsl:value-of select="$itfid"/> </xsl:comment>
        <xsl:text>
          </xsl:text>
        <xsl:comment> num. of preceding interface policy rules: <xsl:value-of
            select="$nPrecedingInterfacePolicyRules" /> </xsl:comment>
        <xsl:text>
          </xsl:text>    


        <xsl:for-each select="fwb:InterfacePolicy/fwb:PolicyRule">
          <!-- local-name() here is "PolicyRule"  -->

          <xsl:variable name="pos" select="@position"/>
          <xsl:variable name="newPos">
            <xsl:value-of select="$nPrecedingInterfacePolicyRules + $pos" />
          </xsl:variable>
          <xsl:comment> interface policy rule pos <xsl:value-of
                                                     select="$pos"/> </xsl:comment>
          <xsl:text>
          </xsl:text>    
          <xsl:comment> new position <xsl:value-of select="$newPos" /></xsl:comment>
          <xsl:text>
          </xsl:text>    


          <!-- start element PolicyRule -->
          <xsl:element name="PolicyRule">
            <xsl:for-each select="@*">
              <xsl:choose>
                <xsl:when test="local-name()='position'">
                  <xsl:attribute name="position">
                    <xsl:value-of select="$newPos" />
                  </xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:copy-of select="."/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:for-each>
            <!-- iterate over rule elements -->
            <xsl:for-each select="*">
              <xsl:text>
              </xsl:text>    
              <xsl:copy-of select="."/>
              <xsl:if test="local-name()='Srv'">
                <xsl:text>
              </xsl:text>
                <!-- insert new element Itf after Srv  -->
                <xsl:element name="Itf">
                  <xsl:attribute name="neg">False</xsl:attribute>
                  <xsl:text>
                </xsl:text>    
                  <xsl:element name="ObjectRef">
                    <xsl:attribute name="ref">
                      <xsl:value-of select="$itfid"/>
                    </xsl:attribute>
                  </xsl:element>
                <!-- end element Itf  -->
                  <xsl:text>
              </xsl:text>    
                </xsl:element>
              </xsl:if>
            </xsl:for-each>
            <!-- end iteration over rule elements -->
            <xsl:text>
          </xsl:text>    
          </xsl:element>
          <!-- end PolicyRule -->
          <xsl:text>
            </xsl:text>    
        </xsl:for-each>
        <!-- end for-each select="fwb:InterfacePolicy/Policyrule" -->
      </xsl:for-each>
      <!-- end for-each select="fwb:Interface" -->

<!-- then copy original policy rules -->
      <xsl:for-each select="fwb:Policy/*">

        <xsl:variable name="pos" select="@position"/>
        <xsl:variable name="newPos">
          <xsl:value-of select="$nInterfacePolicyRules + $pos" />
        </xsl:variable>

        <xsl:comment> global policy rule <xsl:value-of select="$pos"/></xsl:comment>
        <xsl:text>
          </xsl:text>    
        <xsl:comment> new position <xsl:value-of select="$newPos" /></xsl:comment>
        <xsl:text>
          </xsl:text>    

          <!-- start element PolicyRule  -->
          <xsl:element name="PolicyRule">
            <xsl:for-each select="@*">
              <xsl:choose>
                <xsl:when test="local-name()='position'">
                  <xsl:attribute name="position">
                    <xsl:value-of select="$newPos" />
                  </xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:copy-of select="."/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:for-each>
            <!-- iterate over rule elements -->
            <xsl:for-each select="*">
              <xsl:text>
              </xsl:text>    
              <xsl:copy-of select="."/>
              <xsl:if test="local-name()='Srv'">
                <xsl:text>
            </xsl:text>
                <!-- insert new element Itf after Srv  -->
                <xsl:element name="Itf">
                  <xsl:attribute name="neg">False</xsl:attribute>
                  <xsl:text>
              </xsl:text>    
                  <xsl:element name="ObjectRef">
                    <xsl:attribute name="ref">sysid0</xsl:attribute>
                  </xsl:element>
                <!-- end element Itf  -->
                  <xsl:text>
            </xsl:text>    
                </xsl:element>
              </xsl:if>
            </xsl:for-each>
            <!-- end iteration over rule elements -->
            <xsl:text>
          </xsl:text>    
          </xsl:element>
          <!-- end PolicyRule -->
        <xsl:text>
        </xsl:text>    
      </xsl:for-each>  <!-- for-each Policy  -->

<!-- global policy has been copied, now close Policy element and copy
the rest under Firewall
  -->

    </xsl:element>     <!-- Policy -->
    <xsl:text>
        </xsl:text>    
    <xsl:copy-of select="fwb:Routing" />

<!-- this copies interfaces, skipping InterfacePolicy elements  -->
    <xsl:apply-templates select="fwb:Interface" mode="copy"/>

    <xsl:text>
        </xsl:text>    
    <xsl:copy-of select="fwb:Management" />
    <xsl:text>
        </xsl:text>    
    <xsl:copy-of select="fwb:FirewallOptions" />

    <xsl:text>
      </xsl:text>    
  </Firewall>
</xsl:template>

<xsl:template match="*[attribute::id='root']">
  <FWObjectDatabase xmlns="http://www.fwbuilder.org/1.0/">
  <xsl:attribute name="version">2.1.2</xsl:attribute>
  <xsl:attribute name="id">root</xsl:attribute>
  <xsl:apply-templates select="*" mode="copy"/>
  </FWObjectDatabase>
</xsl:template>

</xsl:stylesheet>
