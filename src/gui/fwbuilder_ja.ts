<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS><TS version="1.1">
<context>
    <name>AboutDialog_q</name>
    <message>
        <location filename="aboutdialog_q.ui" line="404"/>
        <source>Firewall Builder</source>
        <translation>ファイアウォールビルダー</translation>
    </message>
    <message>
        <location filename="aboutdialog_q.ui" line="423"/>
        <source>Revision: </source>
        <translation>リビジョン: </translation>
    </message>
    <message>
        <location filename="aboutdialog_q.ui" line="442"/>
        <source>Using libfwbuilder API v</source>
        <translation>使用中の libfwbuilder API バージョン</translation>
    </message>
    <message>
        <location filename="aboutdialog_q.ui" line="477"/>
        <source>Copyright 2002-2008  NetCitadel, LLC</source>
        <translation>Copyright 2002-2008 NetCitadel, LLC</translation>
    </message>
    <message>
        <location filename="aboutdialog_q.ui" line="523"/>
        <source>&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:12pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p align=&quot;center&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&lt;a href=&quot;http://www.fwbuilder.org/&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#0000ff;&quot;&gt;http://www.fwbuilder.org&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="aboutdialog_q.ui" line="595"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
</context>
<context>
    <name>ActionsDialog</name>
    <message>
        <location filename="ActionsDialog.cpp" line="138"/>
        <source>&apos;Chabge inbound interface&apos;, &apos;Continue packet inspection&apos; and &apos;Make a copy&apos; options are mutually exclusive</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ActionsDialog.cpp" line="164"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="ActionsDialog.cpp" line="163"/>
        <source>Rule name for accounting is converted to the iptables
chain name and therefore may not contain white space
and special characters.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ActionsDialog.cpp" line="291"/>
        <source>Emulation of terminating behavior for MARK and CLASSIFY targets is currently ON, rule will be terminating</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ActionsDialog.cpp" line="296"/>
        <source>Emulation of terminating behavior for MARK and CLASSIFY targets is currently OFF, rule will not be terminating</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ActionsDialog_q</name>
    <message>
        <location filename="actionsdialog_q.ui" line="13"/>
        <source>Actions Dialog</source>
        <translation>操作ダイアログ</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="82"/>
        <source>fw/rule num/action</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="288"/>
        <source>Tag service object:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="193"/>
        <source>If rule action is &apos;Reject&apos;, this option defines firewall&apos;s reaction to the packet matching the rule</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="271"/>
        <source>This action has no parameters.</source>
        <translation>この操作はパラメーターがありません。.</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="335"/>
        <source>Requires CONNMARK target</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="338"/>
        <source>Mark connections created by packets that match this rule</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="345"/>
        <source>Note: this action translates into MARK target for iptables.
Normally this target is non-terminating, that is, other rules
with Classify or Tag actions below this one will process
the same packet. However, Firewall Builder can emulate
terminating behavior for this action. Option in the &quot;compiler&quot;
tab of the firewall object properties dialog activates emulation.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="353"/>
        <source>Emulation of terminating behavior for MARK target is currently ON, the rule will be terminating</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="389"/>
        <source>Rule name for accounting. (white spaces and special characters are not allowed)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="431"/>
        <source>Packet classification can be implemented in different ways:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="456"/>
        <source>use dummynet(4) &apos;pipe&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="463"/>
        <source>use dummynet(4) &apos;queue&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="473"/>
        <source>Pipe or queue number:</source>
        <translation>パイプもしくはキュー番号:</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="538"/>
        <source>Custom string:</source>
        <translation>カスタム文字列:</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="574"/>
        <source>Classify string:</source>
        <translation>分類文字列:</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="603"/>
        <source>Note:  CLASSIFY target in iptables is non-terminating,
that is other rules with Classify or Mark
target below this will process the same packet.
However, Firewall Builder can emulate terminating
behavior for this action. Emulation is activated
by an option in the &quot;compiler&quot; tab of the firewall
object properties dialog.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="612"/>
        <source>Emulation is currently ON, rule will be terminating</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="632"/>
        <source>Divert socket port number:</source>
        <translation type="unfinished">プロトコル番号:</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="691"/>
        <source>Policy ruleset object:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="722"/>
        <source>In addition to &apos;filter&apos;, create branching rule in &apos;mangle&apos; table as well</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="762"/>
        <source>Policy rule set object:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="904"/>
        <source>Route through</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="909"/>
        <source>Route reply through</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="914"/>
        <source>Route a copy through</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="922"/>
        <source>interface</source>
        <translation>インターフェース</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="935"/>
        <source>next hop</source>
        <translation>次のホップ</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="950"/>
        <source>Fastroute</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="973"/>
        <source>Load Balancing:</source>
        <translation>ロードバランシング:</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="981"/>
        <source>None</source>
        <translation>なし</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="986"/>
        <source>Bitmask</source>
        <translation>ビットマスク</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="991"/>
        <source>Random</source>
        <translation>ランダム</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="996"/>
        <source>Source Hash</source>
        <translation>ソースハッシュ</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="1001"/>
        <source>Round Robin</source>
        <translation>ラウンドロビン</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="1048"/>
        <source>Change inbound interface to</source>
        <translation type="unfinished">インターフェンス管理</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="1058"/>
        <source>Route through gateway</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="1071"/>
        <source>Change outbound interface to</source>
        <translation type="unfinished">インターフェンス管理</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="1103"/>
        <source>Continue packet inspection</source>
        <translation>パケット監査を続ける</translation>
    </message>
    <message>
        <location filename="actionsdialog_q.ui" line="1110"/>
        <source>Make a copy</source>
        <translation>コピーを作る</translation>
    </message>
</context>
<context>
    <name>AddressRangeDialog</name>
    <message>
        <location filename="AddressRangeDialog.cpp" line="128"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation>不当な IP アドレス &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="AddressRangeDialog.cpp" line="129"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>AddressRangeDialog_q</name>
    <message>
        <location filename="addressrangedialog_q.ui" line="100"/>
        <source>Address Range</source>
        <translation>アドレスの範囲</translation>
    </message>
    <message>
        <location filename="addressrangedialog_q.ui" line="131"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="addressrangedialog_q.ui" line="157"/>
        <source>Range Start:</source>
        <translation>範囲の開始:</translation>
    </message>
    <message>
        <location filename="addressrangedialog_q.ui" line="177"/>
        <source>Range End:</source>
        <translation>範囲の終了:</translation>
    </message>
    <message>
        <location filename="addressrangedialog_q.ui" line="244"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>AddressTableDialog_q</name>
    <message>
        <location filename="addresstabledialog_q.ui" line="75"/>
        <source>Address Table</source>
        <translation>アドレステーブル</translation>
    </message>
    <message>
        <location filename="addresstabledialog_q.ui" line="137"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="addresstabledialog_q.ui" line="169"/>
        <source>Compile Time</source>
        <translation>コンパイル時間</translation>
    </message>
    <message>
        <location filename="addresstabledialog_q.ui" line="182"/>
        <source>Run Time</source>
        <translation>ランタイム</translation>
    </message>
    <message>
        <location filename="addresstabledialog_q.ui" line="189"/>
        <source>File name:</source>
        <translation>ファイル名:</translation>
    </message>
    <message>
        <location filename="addresstabledialog_q.ui" line="232"/>
        <source>Browse</source>
        <translation>参照</translation>
    </message>
    <message>
        <location filename="addresstabledialog_q.ui" line="239"/>
        <source>Preview</source>
        <translation>プレビュー</translation>
    </message>
    <message>
        <location filename="addresstabledialog_q.ui" line="289"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>ColorLabelMenuItem</name>
    <message>
        <location filename="ColorLabelMenuItem.cpp" line="51"/>
        <source>no color</source>
        <translation>色なし</translation>
    </message>
</context>
<context>
    <name>CommentEditorPanel</name>
    <message>
        <location filename="CommentEditorPanel.cpp" line="83"/>
        <source>Warning: loading from file discards current contents of the script.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="CommentEditorPanel.cpp" line="87"/>
        <source>Choose file that contains PIX commands</source>
        <translation>PIX コマンドを含むファイルを選択してください</translation>
    </message>
    <message>
        <location filename="CommentEditorPanel.cpp" line="95"/>
        <source>Could not open file %1</source>
        <translation>ファイル %1 を開くことが出来ません</translation>
    </message>
</context>
<context>
    <name>CommentEditorPanel_q</name>
    <message>
        <location filename="commenteditorpanel_q.ui" line="16"/>
        <source>Comment Editor Panel</source>
        <translation>コメントエディターパネル</translation>
    </message>
    <message>
        <location filename="commenteditorpanel_q.ui" line="102"/>
        <source>fw/rule num</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="commenteditorpanel_q.ui" line="161"/>
        <source>Import from file ...</source>
        <translation>ファイルからインポート ...</translation>
    </message>
</context>
<context>
    <name>CompareObjectsDialog</name>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="300"/>
        <source>Next</source>
        <translation>次へ</translation>
    </message>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="302"/>
        <source>The following two objects have the same internal ID but different attributes:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="303"/>
        <source>Skip the rest but build report</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ConfirmDeleteObjectDialog</name>
    <message>
        <location filename="" line="0"/>
        <source>NAT</source>
        <translation type="obsolete">NAT</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Policy</source>
        <translation type="obsolete">ポリシー</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Routing</source>
        <translation type="obsolete">ルーティング</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Unknown rule set</source>
        <translation type="obsolete">不明なルールセット</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>/Rule%1</source>
        <translation type="obsolete">/ルール %1</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Type: </source>
        <translation type="obsolete">種類: </translation>
    </message>
    <message>
        <location filename="ConfirmDeleteObjectDialog.cpp" line="135"/>
        <source>Not used anywhere</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ConfirmDeleteObjectDialog_q</name>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="19"/>
        <source>Firewall Builder</source>
        <translation>ファイアウォールビルダー</translation>
    </message>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="31"/>
        <source>Groups and firewall policy rules shown in the list below reference objects you are about to delete. If you delete objects, they will be removed from these groups and rules.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="50"/>
        <source>Deleted objects are moved to the &quot;Deleted objects&quot; library. You can recover them later by moving back to the user&apos;s library. However if you delete an object already located in the &quot;Deleted objects&quot; library, it is destroyed and can not be restored.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="103"/>
        <source>Object</source>
        <translation>オブジェクト</translation>
    </message>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="108"/>
        <source>Parent</source>
        <translation>親</translation>
    </message>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="113"/>
        <source>Details</source>
        <translation>詳細</translation>
    </message>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="126"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="confirmdeleteobjectdialog_q.ui" line="133"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
</context>
<context>
    <name>CustomServiceDialog_q</name>
    <message>
        <location filename="customservicedialog_q.ui" line="69"/>
        <source>Custom Service</source>
        <translation>カスタムサービス</translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="116"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="154"/>
        <source>Platform:</source>
        <translation>プラットフォーム:</translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="178"/>
        <source>Code String:</source>
        <translation>コード文字列:</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Protocol String:</source>
        <translation type="obsolete">プロトコル文字列:</translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="310"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="200"/>
        <source>Custom service object has separate code string for each
supported firewall platform.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="217"/>
        <source>Protocol Name:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="224"/>
        <source>Choose one of the standard protocols from the menu or enter custom
protocol string. Policy compilers can use this information to add correct
protocol-specific parameters to the generated configuration. For example,
iptables compiler is allowed to add &quot;--reject-with tcp-reset&quot; to the target
REJECT only if service object belongs to protocol &quot;tcp&quot;.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="239"/>
        <source>Address Family:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="252"/>
        <source>Like with protocol, setting address family helps
policy compiler generate more accurate configuration
using this custom service object. For example, if
address family is set to ipv6, this custom service
will only be used in ipv6 policies.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="278"/>
        <source>IPv4</source>
        <translation type="unfinished">IPv4</translation>
    </message>
    <message>
        <location filename="customservicedialog_q.ui" line="288"/>
        <source>IPv6</source>
        <translation type="unfinished">IPv6</translation>
    </message>
</context>
<context>
    <name>DNSNameDialog_q</name>
    <message>
        <location filename="dnsnamedialog_q.ui" line="69"/>
        <source>DNS Name</source>
        <translation>DNS 名</translation>
    </message>
    <message>
        <location filename="dnsnamedialog_q.ui" line="122"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="dnsnamedialog_q.ui" line="148"/>
        <source>DNS Record:</source>
        <translation>DNS レコード:</translation>
    </message>
    <message>
        <location filename="dnsnamedialog_q.ui" line="174"/>
        <source>Compile Time</source>
        <translation>コンパイル時間</translation>
    </message>
    <message>
        <location filename="dnsnamedialog_q.ui" line="181"/>
        <source>Run Time</source>
        <translation>ランタイム</translation>
    </message>
    <message>
        <location filename="dnsnamedialog_q.ui" line="197"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>DiscoveryDruid</name>
    <message>
        <location filename="DiscoveryDruid.cpp" line="641"/>
        <source>Hosts file parsing ...</source>
        <translation>hosts ファイルを解析しています ...</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="650"/>
        <source>DNS zone transfer ...</source>
        <translation>DNS ゾーン転送 ...</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="660"/>
        <source>Network discovery using SNMP ...</source>
        <translation>SNMP を用いたネットワーク検出 ...</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="670"/>
        <source>Import configuration from file ...</source>
        <translation>ファイルから構成のインポート ...</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="1734"/>
        <source>Discovery error</source>
        <translation>検出エラー</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="770"/>
        <source>Could not open file %1</source>
        <translation>ファイル %1 を開くことが出来ません</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="1084"/>
        <source>Adding objects ...</source>
        <translation>オブジェクトを追加しています ...</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="1563"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="1270"/>
        <source>Prepare objects ...</source>
        <translation>オブジェクトの準備 ...</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="1562"/>
        <source>Copying results ...</source>
        <translation>結果をコピーしています ...</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="1885"/>
        <source>Incomlete network specification.</source>
        <translation>ネットワークの指定が完了していません。</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="1973"/>
        <source>Empty community string</source>
        <translation>コミュニティ文字列が空です</translation>
    </message>
</context>
<context>
    <name>DiscoveryDruid_q</name>
    <message>
        <location filename="discoverydruid_q.ui" line="43"/>
        <source>TextLabel</source>
        <translation>テキストラベル</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="75"/>
        <source>Choose discovery method used to collect information about network objects from the list below and click &apos;Next&apos; to continue.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="104"/>
        <source>Discovery method:</source>
        <translation>検出方法:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="119"/>
        <source>Read file in hosts format</source>
        <translation>hosts 形式のファイルを読み込む</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="129"/>
        <source>Import DNS zone</source>
        <translation>DNS ゾーンをインポートをする</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="136"/>
        <source>Perform network discovery using SNMP</source>
        <translation>SNMP を用いてネットワークの検出を実行する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="143"/>
        <source>Import configuration of a firewall or a router</source>
        <translation>ファイアウォールもしくはルーターの設定のインポート</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="163"/>
        <source>Enter full path and file name below or click &quot;Browse&quot; to find it:</source>
        <translation>下にフルパスとファイル名を入力し、「参照」ボタンをクリックして検索してください:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="192"/>
        <source>File in hosts format</source>
        <translation>hosts 形式のファイル</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="231"/>
        <source>Browse ...</source>
        <translation>参照 ...</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="285"/>
        <source>All objects created during import will be placed in the library currently opened in the tree.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="314"/>
        <source>Policy import tries to parse given configuration file and preserve its logic as close as possible. However, very often target firewall configuration allows for more commands, options and their combinations than importer can understand. Rules that importer could not parse exactly are colored red in the rule sets it creates. Always inspect firewall policy created by the importer and compare it with the original. Manual changes and corrections may be required. Comments in the rules that could not be parsed show fragments of the original configuration parser did not understand.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="327"/>
        <source>Import from file: </source>
        <translation>ファイルからインポート:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="346"/>
        <source>Browse...</source>
        <translation>参照...</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="354"/>
        <source>Cisco IOS</source>
        <translation>Cisco IOS</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="359"/>
        <source>iptables</source>
        <translation>iptables</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="367"/>
        <source>Platform:</source>
        <translation>プラットフォーム:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="399"/>
        <source>textLabel1</source>
        <translation>テキストラベル 1</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="470"/>
        <source>This discovery method creates objects for all &apos;A&apos; records found in DNS domain. You will later have a chance to accept only those objects you wish and ignore others.
Please enter the domain name below:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="500"/>
        <source>Domain name</source>
        <translation>ドメイン名</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="566"/>
        <source>Objects created using this method may have long or short names. long name consists of the host name and full domain name (this is called &lt;i&gt;Fully Qualified Domain Name&lt;/i&gt;). Short name consists of only host name. Check in the box below if you wish to use long name, then click next to continue:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="579"/>
        <source>Use long names</source>
        <translation>長い名前を使う</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="596"/>
        <source>DNS zone information has to be transferred from the name server authoritative for the domain. Pick the name server:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="609"/>
        <source>Name server</source>
        <translation>ネームサーバー</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="621"/>
        <source>choose name server from the list below</source>
        <translation>下の一覧からネームサーバーを選択する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="634"/>
        <source>server name or its IP address here if you wish to use different one:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="705"/>
        <source>DNS Query options</source>
        <translation>DNS 問い合わせオプション</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="717"/>
        <source>Timeout (sec)</source>
        <translation>タイムアウト (秒)</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="727"/>
        <source>Retries</source>
        <translation>再実行</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="818"/>
        <source>This discovery method scans networks looking for hosts or gateways responding to SNMP queries. It pulls host&apos;s ARP table and uses all the entries found in it to create objects. Scan starts from the host called &quot;seed&quot;. Enter &quot;seed&quot; host name or address below:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="831"/>
        <source>&apos;Seed&apos; host</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="893"/>
        <source>Enter a valid host name or address.</source>
        <translation>正しいホスト名かアドレスを入力してください。</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="943"/>
        <source>The scanner process can be confined to a certain network, so it won&apos;t discover hosts on adjacent networks. If you leave these fields blank, scanner will visit all networks it can find:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="956"/>
        <source>Confine scan to this network:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1006"/>
        <source>Netmask:</source>
        <translation>ネットマスク:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1016"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1065"/>
        <source>The scanner process can repeat its algorithm recursively using each new host it finds as a new &quot;seed&quot;. This allows it to find as many objects on your network as possible. On the other hand, it takes more time and may find some objects you do not really need. You can turn recursive scanning on below:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1078"/>
        <source>Run network scan recursively</source>
        <translation>ネットワークの再起スキャンを実行する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1098"/>
        <source>The scanner process can find nodes beyond the boundaries of your network by following point-to-point links connecting it to the Internet or other parts of WAN.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1111"/>
        <source>Follow point-to-point links</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1131"/>
        <source>The scanner process can distinguish virtual IP addresses created on hosts as static &quot;published&quot; ARP entries or as secondary addresses on interfaces.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1144"/>
        <source>Include virtual addresses</source>
        <translation>バーチャルアドレスを含む</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1164"/>
        <source>Analysis of ARP table yields IP addresses for hosts on your network. In order to determine their names, scanner can run reverse name lookup queries using your name servers (DNS):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1177"/>
        <source>Run reverse name lookup DNS queries to determine host names</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1197"/>
        <source>Enter parameters for SNMP and DNS reverse lookup queries below. (If unsure, just leave default values):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1210"/>
        <source>SNMP query parameters:</source>
        <translation>SNMP 問い合わせパラメーター:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1222"/>
        <source>SNMP &apos;read&apos; community string:</source>
        <translation>SNMP &apos;read&apos; コミュニティ文字列:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1358"/>
        <source>number of retries:</source>
        <translation>再実行の回数:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1242"/>
        <source>timeout (sec):</source>
        <translation>タイムアウト (秒):</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1272"/>
        <source>public</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1330"/>
        <source>DNS parameters:</source>
        <translation>DNS パラメーター:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1368"/>
        <source>timeout (sec) :</source>
        <translation>タイムアウト (秒) :</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1378"/>
        <source>Number of threads:</source>
        <translation>スレッドの数:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1479"/>
        <source>Process name</source>
        <translation>処理名</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1511"/>
        <source>Stop</source>
        <translation>停止</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1518"/>
        <source>Save scan log to file</source>
        <translation>スキャンログをファイルに保存する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1544"/>
        <source>Process log:</source>
        <translation>処理ログ:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1576"/>
        <source>These are the networks found by the scanner process. Choose the ones you wish to use from the list below, then click &apos;Next&apos;:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1975"/>
        <source>Select All</source>
        <translation>すべて選択する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1952"/>
        <source>Filter ...</source>
        <translation>フィルター ...</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1938"/>
        <source>Unselect All</source>
        <translation>すべて選択を解除する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1945"/>
        <source>Remove Filter</source>
        <translation>フィルターを削除する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1829"/>
        <source>-&gt;</source>
        <translation>→</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1836"/>
        <source>&lt;-</source>
        <translation>←</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1745"/>
        <source>Choose objects you wish to use, then click &apos;Next&apos;:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1982"/>
        <source>Change type of selected objects:</source>
        <translation>選択されたオブジェクトの種類の変更:</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="1994"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2001"/>
        <source>Host</source>
        <translation>ホスト</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2008"/>
        <source>Firewall</source>
        <translation>ファイアウォール</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2027"/>
        <source>Object</source>
        <translation>オブジェクト</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2032"/>
        <source>Interfaces</source>
        <translation>インターフェース</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2037"/>
        <source>Type</source>
        <translation>種類</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2045"/>
        <source>Here you can change type of the objects to be created for each address discovered by the scanner. By default, an &quot;Address&quot; object is created for the host with just one interface with single IP address and &quot;Host&quot; object is created for the host with multiple interfaces, however you can change their types on this page.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2068"/>
        <source>Select target library</source>
        <translation>ターゲットライブラリーを選択する</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2135"/>
        <source>Adding new objects to library ...</source>
        <translation>ライブラリーに新しいオブジェクトを追加しています ...</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2215"/>
        <source>&lt; &amp;Back</source>
        <translation>&lt; 戻る(&amp;B)</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2222"/>
        <source>&amp;Next &gt;</source>
        <translation>次へ(&amp;N) &gt;</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2232"/>
        <source>&amp;Finish</source>
        <translation>終了(&amp;F)</translation>
    </message>
    <message>
        <location filename="discoverydruid_q.ui" line="2242"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>FWBAboutDialog</name>
    <message>
        <location filename="FWBAboutDialog.h" line="28"/>
        <source>Revision: %1 ( Build: %2 )</source>
        <translation>リビジョン: %1 ( ビルド: %2 )</translation>
    </message>
    <message>
        <location filename="FWBAboutDialog.h" line="31"/>
        <source>Using Firewall Builder API %1</source>
        <translation>使用中のファイアウォールビルダー API %1</translation>
    </message>
    <message>
        <location filename="FWBAboutDialog.h" line="35"/>
        <source>Registered</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWBAboutDialog.h" line="36"/>
        <source>Unregistered</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FWBMainWindow_q</name>
    <message>
        <location filename="FWBMainWindow_q.ui" line="28"/>
        <source>Firewall Builder</source>
        <translation>ファイアウォールビルダー</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="73"/>
        <source>&amp;Edit</source>
        <translation>編集(&amp;E)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="88"/>
        <source>Object</source>
        <translation>オブジェクト</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="98"/>
        <source>Tools</source>
        <translation>ツール</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="105"/>
        <source>&amp;Help</source>
        <translation>ヘルプ(&amp;H)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="113"/>
        <source>Rules</source>
        <translation>ルール</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="130"/>
        <source>&amp;File</source>
        <translation>ファイル(&amp;F)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="161"/>
        <source>Window</source>
        <translation>ウィンドウ</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="182"/>
        <source>&amp;New Object File</source>
        <translation>新規オブジェクトファイル(&amp;N)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="730"/>
        <source>New Object File</source>
        <translation>新規オブジェクトファイル</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="195"/>
        <source>&amp;Open...</source>
        <translation>開く(&amp;O)...</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="385"/>
        <source>Open</source>
        <translation>開く</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="201"/>
        <source>Ctrl+O</source>
        <translation>Ctrl+O</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="211"/>
        <source>&amp;Save</source>
        <translation>保存(&amp;S)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="397"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="217"/>
        <source>Ctrl+S</source>
        <translation>Ctrl+S</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="222"/>
        <source>Save &amp;As...</source>
        <translation>名前を付けて保存(&amp;A)...</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="225"/>
        <source>Save As</source>
        <translation>名前を付けて保存</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="236"/>
        <source>&amp;Print...</source>
        <translation>印刷(&amp;P)...</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="239"/>
        <source>Print</source>
        <translation>印刷</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="242"/>
        <source>Ctrl+P</source>
        <translation>Ctrl+P</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="247"/>
        <source>E&amp;xit</source>
        <translation>終了(&amp;X)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="250"/>
        <source>Exit</source>
        <translation>終了</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="261"/>
        <source>&amp;Undo</source>
        <translation>取り消し(&amp;U)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="264"/>
        <source>Undo</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="267"/>
        <source>Ctrl+Z</source>
        <translation>Ctrl+Z</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="275"/>
        <source>&amp;Redo</source>
        <translation>やり直し(&amp;R)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="278"/>
        <source>Redo</source>
        <translation>やり直し</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="281"/>
        <source>Ctrl+Y</source>
        <translation>Ctrl+Y</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="286"/>
        <source>&amp;Cut</source>
        <translation>切り取り(&amp;C)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="289"/>
        <source>Cut</source>
        <translation>切り取り</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="292"/>
        <source>Ctrl+X</source>
        <translation>Ctrl+X</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="300"/>
        <source>C&amp;opy</source>
        <translation>コピー(&amp;O)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="303"/>
        <source>Copy</source>
        <translation>コピー</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="306"/>
        <source>Ctrl+C</source>
        <translation>Ctrl+C</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="311"/>
        <source>&amp;Paste</source>
        <translation>貼り付け(&amp;P)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="314"/>
        <source>Paste</source>
        <translation>貼り付け</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="317"/>
        <source>Ctrl+V</source>
        <translation>Ctrl+V</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="496"/>
        <source>Ctrl+F</source>
        <translation>Ctrl+F</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="333"/>
        <source>&amp;Contents...</source>
        <translation>内容(&amp;C)...</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="336"/>
        <source>Contents</source>
        <translation>内容</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="344"/>
        <source>&amp;Index...</source>
        <translation>インデックス(&amp;I)...</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="347"/>
        <source>Index</source>
        <translation>インデックス</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="355"/>
        <source>&amp;About</source>
        <translation>ファイアウォールビルダーについて(&amp;A)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="358"/>
        <source>About</source>
        <translation>ファイアウォールビルダーについて</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="373"/>
        <source>New</source>
        <translation>新規</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="402"/>
        <source>&amp;Close</source>
        <translation>閉じる(&amp;C)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="405"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="417"/>
        <source>Compile</source>
        <translation>コンパイル</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="420"/>
        <source>Compile rules</source>
        <translation>コンパイルルール</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="432"/>
        <source>Install</source>
        <translation>インストール</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="435"/>
        <source>Install firewall policy</source>
        <translation>ファイアウォールポリシーのインストール</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="450"/>
        <source>Back</source>
        <translation>戻る</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="456"/>
        <source>Move back to the previous object</source>
        <translation type="unfinished">前のオブジェクトに戻る</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="466"/>
        <source>&amp;New Object</source>
        <translation>新規オブジェクト(&amp;N)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="469"/>
        <source>New Object</source>
        <translation>新規オブジェクト</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="472"/>
        <source>Create New Object</source>
        <translation>新規オブジェクト作成</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="475"/>
        <source>Ctrl+N</source>
        <translation>Ctrl+N</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="484"/>
        <source>&amp;Find Object</source>
        <translation>オブジェクト検索(&amp;F)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="487"/>
        <source>Find Object</source>
        <translation>オブジェクト検索</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="493"/>
        <source>Find object in the tree</source>
        <translation>ツリー中のオブジェクトを検索する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="501"/>
        <source>P&amp;references...</source>
        <translation>設定(&amp;E)...</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="504"/>
        <source>Preferences...</source>
        <translation>設定...</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="507"/>
        <source>Edit Preferences</source>
        <translation>設定の編集</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="515"/>
        <source>Insert Rule</source>
        <translation>ルール挿入</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="523"/>
        <source>Move Rule Up</source>
        <translation>ルールを上に移動する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="531"/>
        <source>Move Rule Down</source>
        <translation>ルールを下に移動</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="539"/>
        <source>Add Rule Below</source>
        <translation>下にルールを追加する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="547"/>
        <source>Remove Rule</source>
        <translation>ルール削除</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="550"/>
        <source>Ctrl+Del</source>
        <translation>Ctrl+Del</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="558"/>
        <source>Copy Rule</source>
        <translation>ルールのコピー</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="566"/>
        <source>Cut Rule</source>
        <translation>ルール切り取り</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="574"/>
        <source>Paste Rule Above</source>
        <translation>上にルールを貼り付け</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="582"/>
        <source>Paste Rule Below</source>
        <translation>下にルールを貼り付け</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="587"/>
        <source>Add File to &amp;RCS</source>
        <translation>RCS にファイルを追加する(&amp;R)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="590"/>
        <source>Add File to RCS</source>
        <translation>RCS にファイルを追加する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="598"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="603"/>
        <source>&amp;Export Library</source>
        <translation>ライブラリーのエクスポート(&amp;E)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="606"/>
        <source>Export Library To a File</source>
        <translation>ファイルへライブラリーをエクスポートする</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="611"/>
        <source>Import &amp;Library</source>
        <translation>ライブラリーをインポートする(&amp;L)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="614"/>
        <source>Import Library From a File</source>
        <translation>ファイルからライブラリーをインポート</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="619"/>
        <source>&amp;Debug</source>
        <translation>デバッグ(&amp;D)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="622"/>
        <source>Debug</source>
        <translation>デバッグ</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="630"/>
        <source>Propert&amp;ies</source>
        <translation>プロパティ(&amp;I)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="633"/>
        <source>Show File Properties</source>
        <translation>ファイルのプロパティの表示</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="636"/>
        <source>Ctrl+I</source>
        <translation>Ctrl+I</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="644"/>
        <source>Move Selected Rules</source>
        <translation>選択したルールを移動する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="649"/>
        <source>&amp;Discard</source>
        <translation>破棄する(&amp;D)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="652"/>
        <source>Discard</source>
        <translation>破棄する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="655"/>
        <source>Discard Changes and Overwrite With Clean Copy Of The Head Revision From RCS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="660"/>
        <source>Co&amp;mmit</source>
        <translation>コミットする(&amp;M)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="663"/>
        <source>Commit</source>
        <translation>コミットする</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="666"/>
        <source>Commit Opened File to RCS and Continue Editing</source>
        <translation>開いているファイルを RCS にコミットして編集を続ける</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="674"/>
        <source>Lock</source>
        <translation>ロックする</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="682"/>
        <source>Unlock</source>
        <translation>ロック解除する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="690"/>
        <source>Discovery Druid</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="698"/>
        <source>new item</source>
        <translation>新規項目</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="706"/>
        <source>Find Conflicting Objects in Two Files</source>
        <translation>ふたつのファイルから衝突したオブジェクトを検索する</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="711"/>
        <source>Import Po&amp;licy</source>
        <translation>ポリシーのインポート(&amp;L)</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="714"/>
        <source>Import Policy</source>
        <translation>ポリシーをインポートする</translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="134"/>
        <source>Open Recent</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="719"/>
        <source>Firewall Builder Help</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWBMainWindow_q.ui" line="727"/>
        <source>Clear Menu</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FWObjectDropArea</name>
    <message>
        <location filename="FWObjectDropArea.cpp" line="71"/>
        <source>Drop object here.</source>
        <translation>ここにオブジェクトをドロップします。</translation>
    </message>
    <message>
        <location filename="FWObjectDropArea.cpp" line="158"/>
        <source>Show in the tree</source>
        <translation>ツリーの中を表示する</translation>
    </message>
    <message>
        <location filename="FWObjectDropArea.cpp" line="159"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="FWObjectDropArea.cpp" line="162"/>
        <source>Paste</source>
        <translation>貼り付け</translation>
    </message>
    <message>
        <location filename="FWObjectDropArea.cpp" line="164"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
</context>
<context>
    <name>FWObjectDropArea_q</name>
    <message>
        <location filename="fwobjectdroparea_q.ui" line="30"/>
        <source>Form1</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FWWindow</name>
    <message>
        <location filename="FWWindow.cpp" line="1746"/>
        <source>A new version of Firewall Builder is available at http://www.fwbuilder.org</source>
        <translation>ファイアウォールビルダーの新しいバージョンを利用できます http://www.fwbuilder.org</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Legend</source>
        <translation type="obsolete">凡例</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Objects</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Groups</source>
        <translation type="obsolete">グループ</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>EMPTY</source>
        <translation type="obsolete">空</translation>
    </message>
    <message>
        <location filename="FWWindowPrint.cpp" line="201"/>
        <source>Printing aborted</source>
        <translation>印刷を中断しました</translation>
    </message>
    <message>
        <location filename="FWWindowPrint.cpp" line="202"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="FWWindowPrint.cpp" line="205"/>
        <source>Printing completed</source>
        <translation>印刷を完了しました</translation>
    </message>
    <message>
        <location filename="FWWindowPrint.cpp" line="209"/>
        <source>Printing &lt;ed</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FilterDialog</name>
    <message>
        <location filename="FilterDialog.cpp" line="112"/>
        <source>Filter error</source>
        <translation>フィルターエラー</translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="112"/>
        <source>Invalid RegExp.</source>
        <translation>不正な正規表現です。</translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="413"/>
        <source>Name</source>
        <translation>名前</translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="414"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="417"/>
        <source>Contains</source>
        <translation>内容</translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="418"/>
        <source>Is equal to</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="419"/>
        <source>Starts with</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="420"/>
        <source>Ends with</source>
        <translation type="unfinished">編集</translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="421"/>
        <source>Matches Wildcard</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FilterDialog.cpp" line="422"/>
        <source>Matches RegExp</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FilterDialog_q</name>
    <message>
        <location filename="filterdialog_q.ui" line="13"/>
        <source>Filter</source>
        <translation>フィルター</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="33"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="40"/>
        <source>Load</source>
        <translation>読み込み</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="63"/>
        <source>Ok</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="73"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="82"/>
        <source>Match</source>
        <translation>一致</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="93"/>
        <source>all</source>
        <translation>すべて</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="98"/>
        <source>any</source>
        <translation>何でも</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="106"/>
        <source>of the following:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="150"/>
        <source>Add a new pattern</source>
        <translation>新規パターンを追加する</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="153"/>
        <source>+</source>
        <translation>+</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="172"/>
        <source>Target</source>
        <translation>ターゲット</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="177"/>
        <source>Type</source>
        <translation>種類</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="182"/>
        <source>Pattern</source>
        <translation>パターン</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="190"/>
        <source>Case sensitive</source>
        <translation>英大小文字の区別</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="221"/>
        <source>Remove a pattern</source>
        <translation>パターンを削除する</translation>
    </message>
    <message>
        <location filename="filterdialog_q.ui" line="224"/>
        <source>-</source>
        <translation>-</translation>
    </message>
</context>
<context>
    <name>FindObjectWidget</name>
    <message>
        <location filename="FindObjectWidget.cpp" line="327"/>
        <source>Search hit the end of the policy rules.</source>
        <translation>検索のヒットがオブジェクトツリーの最後です。</translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="336"/>
        <source>&amp;Continue at top</source>
        <translation type="unfinished">続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="336"/>
        <source>&amp;Stop</source>
        <translation>停止(&amp;S)</translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="335"/>
        <source>Search hit the end of the object tree.</source>
        <translation>検索のヒットがオブジェクトツリーの最後です。</translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="366"/>
        <source>Search or Replace object ind&apos;t specified.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="376"/>
        <source>Cannot replace object by itself.</source>
        <translation>自分自身でオブジェクトを置換できません。</translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="384"/>
        <source>Search and Replace objects are incompatible.</source>
        <translation>オブジェクトの検索と置換は互換性がありません。
</translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="476"/>
        <source>Replaced %1 objects.</source>
        <translation>%1 個のオブジェクトを置換しました。</translation>
    </message>
    <message>
        <location filename="FindObjectWidget.cpp" line="593"/>
        <source>Policy of firewall &apos;</source>
        <translation>ファイアウォールのポリシー</translation>
    </message>
</context>
<context>
    <name>FindWhereUsedWidget</name>
    <message>
        <location filename="FindWhereUsedWidget.cpp" line="322"/>
        <source>Type: </source>
        <translation>種類: </translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>NAT</source>
        <translation type="obsolete">NAT</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Policy</source>
        <translation type="obsolete">ポリシー</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Routing</source>
        <translation type="obsolete">ルーティング</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Unknown rule set</source>
        <translation type="obsolete">不明なルールセット</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>/Rule%1</source>
        <translation type="obsolete">/ルール %1</translation>
    </message>
    <message>
        <location filename="FindWhereUsedWidget.cpp" line="293"/>
        <source>NAT rule set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FindWhereUsedWidget.cpp" line="296"/>
        <source>Policy rule set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FindWhereUsedWidget.cpp" line="299"/>
        <source>Routing rule set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FindWhereUsedWidget.cpp" line="302"/>
        <source>Rule set of unknown type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FindWhereUsedWidget.cpp" line="313"/>
        <source>%1 &quot;%2&quot; / Rule %3 / %4</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FirewallDialog</name>
    <message>
        <location filename="FirewallDialog.cpp" line="337"/>
        <source>FWBuilder API error: %1</source>
        <translation>FWBuilder API エラー: %1</translation>
    </message>
    <message>
        <location filename="FirewallDialog.cpp" line="338"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>FirewallDialog_q</name>
    <message>
        <location filename="firewalldialog_q.ui" line="78"/>
        <source>Firewall</source>
        <translation>ファイアウォール</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="157"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="195"/>
        <source>Platform:</source>
        <translation>プラットフォーム:</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="227"/>
        <source>Version:</source>
        <translation>バージョン:</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="259"/>
        <source>Host OS:</source>
        <translation>ホスト OS:</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="318"/>
        <source>Host OS Settings ...</source>
        <translation>ホスト OS 設定 ...</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="336"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="385"/>
        <source>Firewall Settings ...</source>
        <translation>ファイアウォール設定 ...</translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="398"/>
        <source>Skip this firewall for batch compile and install operations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="firewalldialog_q.ui" line="401"/>
        <source>Inactive firewall</source>
        <translation type="unfinished">新規ファイアウォール</translation>
    </message>
</context>
<context>
    <name>FirewallInstallerUnx</name>
    <message>
        <location filename="FirewallInstallerUnx.cpp" line="142"/>
        <source>Generated script file %1 not found.</source>
        <translation>生成したスクリプトファイル %1 が見つかりません。</translation>
    </message>
    <message>
        <location filename="FirewallInstallerUnx.cpp" line="143"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="FirewallInstallerUnx.cpp" line="229"/>
        <source>Copying %1 -&gt; %2:%3
</source>
        <translation>コピーしています %1 → %2:%3

</translation>
    </message>
    <message>
        <location filename="FirewallInstallerUnx.cpp" line="290"/>
        <source>Error: Failed to start program:</source>
        <translation>エラー: プログラムの開始でエラー:</translation>
    </message>
</context>
<context>
    <name>GroupObjectDialog</name>
    <message>
        <location filename="GroupObjectDialog.cpp" line="586"/>
        <source>Open</source>
        <translation>開く</translation>
    </message>
    <message>
        <location filename="GroupObjectDialog.cpp" line="588"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="GroupObjectDialog.cpp" line="591"/>
        <source>Copy</source>
        <translation>コピー</translation>
    </message>
    <message>
        <location filename="GroupObjectDialog.cpp" line="592"/>
        <source>Cut</source>
        <translation>切り取り</translation>
    </message>
    <message>
        <location filename="GroupObjectDialog.cpp" line="593"/>
        <source>Paste</source>
        <translation>貼り付け</translation>
    </message>
    <message>
        <location filename="GroupObjectDialog.cpp" line="594"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
</context>
<context>
    <name>GroupObjectDialog_q</name>
    <message>
        <location filename="groupobjectdialog_q.ui" line="75"/>
        <source>Group</source>
        <translation>グループ</translation>
    </message>
    <message>
        <location filename="groupobjectdialog_q.ui" line="159"/>
        <source>I</source>
        <translation>I</translation>
    </message>
    <message>
        <location filename="groupobjectdialog_q.ui" line="190"/>
        <source>L</source>
        <translation>L</translation>
    </message>
    <message>
        <location filename="groupobjectdialog_q.ui" line="232"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="groupobjectdialog_q.ui" line="292"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
</context>
<context>
    <name>HostDialog_q</name>
    <message>
        <location filename="hostdialog_q.ui" line="75"/>
        <source>Host</source>
        <translation>ホスト</translation>
    </message>
    <message>
        <location filename="hostdialog_q.ui" line="128"/>
        <source>MAC matching</source>
        <translation>MAC 一致</translation>
    </message>
    <message>
        <location filename="hostdialog_q.ui" line="151"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="hostdialog_q.ui" line="186"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>ICMPServiceDialog_q</name>
    <message>
        <location filename="icmpservicedialog_q.ui" line="22"/>
        <source>ICMP</source>
        <translation>ICMP</translation>
    </message>
    <message>
        <location filename="icmpservicedialog_q.ui" line="78"/>
        <source>ICMP Service</source>
        <translation>ICMP サービス</translation>
    </message>
    <message>
        <location filename="icmpservicedialog_q.ui" line="137"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="icmpservicedialog_q.ui" line="163"/>
        <source>ICMP Type:</source>
        <translation>ICMP の種類:</translation>
    </message>
    <message>
        <location filename="icmpservicedialog_q.ui" line="196"/>
        <source>any</source>
        <translation>どれか</translation>
    </message>
    <message>
        <location filename="icmpservicedialog_q.ui" line="186"/>
        <source>ICMP Code:</source>
        <translation>ICMP コード:</translation>
    </message>
    <message>
        <location filename="icmpservicedialog_q.ui" line="247"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>IPServiceDialog</name>
    <message>
        <location filename="IPServiceDialog.cpp" line="70"/>
        <source>DSCP code or class:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="IPServiceDialog.cpp" line="75"/>
        <source>TOS code (numeric):</source>
        <translation>TOS コード (数値):</translation>
    </message>
</context>
<context>
    <name>IPServiceDialog_q</name>
    <message>
        <location filename="ipservicedialog_q.ui" line="22"/>
        <source>IP</source>
        <translation>IP</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="78"/>
        <source>IP Service</source>
        <translation>IP サービス</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="125"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="151"/>
        <source>Protocol number:
 (0- any protocol)</source>
        <translation>プロトコル番号: (0: いくつかのプロトコル)</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="169"/>
        <source>DiffServ</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="175"/>
        <source>Use DSCP</source>
        <translation>DSCP を使う</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="185"/>
        <source>Use TOS</source>
        <translation>TOS を使う</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="192"/>
        <source>DSCP or TOS code
(numerical, dec or hex):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="227"/>
        <source>lsrr (loose source route)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="234"/>
        <source>ssrr (strict source route)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="241"/>
        <source>rr (record route)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="248"/>
        <source>timestamp</source>
        <translation>タイムスタンプ</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="255"/>
        <source>all fragments</source>
        <translation>すべてのフラグメント</translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="262"/>
        <source>&apos;short&apos; fragments</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipservicedialog_q.ui" line="281"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>IPv4Dialog</name>
    <message>
        <location filename="IPv4Dialog.cpp" line="159"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation>不当な IP アドレス 「%1」</translation>
    </message>
    <message>
        <location filename="IPv4Dialog.cpp" line="175"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="IPv4Dialog.cpp" line="174"/>
        <source>Illegal netmask &apos;%1&apos;</source>
        <translation>不当なネットマスク 「%1」</translation>
    </message>
    <message>
        <location filename="IPv4Dialog.cpp" line="272"/>
        <source>DNS lookup failed for both names of the address object &apos;%1&apos; and the name of the host &apos;%2&apos;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="IPv4Dialog.cpp" line="279"/>
        <source>DNS lookup failed for name of the address object &apos;%1&apos;.</source>
        <translation>アドレスオブジェクト「%1」の名前のDNS 問い合わせに失敗しました。</translation>
    </message>
</context>
<context>
    <name>IPv4Dialog_q</name>
    <message>
        <location filename="ipv4dialog_q.ui" line="16"/>
        <source>IPv4</source>
        <translation>IPv4</translation>
    </message>
    <message>
        <location filename="ipv4dialog_q.ui" line="87"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="ipv4dialog_q.ui" line="143"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="ipv4dialog_q.ui" line="169"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="ipv4dialog_q.ui" line="189"/>
        <source>Netmask:</source>
        <translation>ネットマスク:</translation>
    </message>
    <message>
        <location filename="ipv4dialog_q.ui" line="233"/>
        <source>DNS Lookup...</source>
        <translation>DNS 問い合わせ...</translation>
    </message>
    <message>
        <location filename="ipv4dialog_q.ui" line="251"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>IPv6Dialog</name>
    <message>
        <location filename="IPv6Dialog.cpp" line="159"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation>不当な IP アドレス「%1」</translation>
    </message>
    <message>
        <location filename="IPv6Dialog.cpp" line="176"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="IPv6Dialog.cpp" line="175"/>
        <source>Illegal netmask &apos;%1&apos;</source>
        <translation>不当なネットマスク 「%1」</translation>
    </message>
    <message>
        <location filename="IPv6Dialog.cpp" line="283"/>
        <source>DNS lookup failed for both names of the address object &apos;%1&apos; and the name of the host &apos;%2&apos;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="IPv6Dialog.cpp" line="290"/>
        <source>DNS lookup failed for name of the address object &apos;%1&apos;.</source>
        <translation>アドレスオブジェクト「%1」の名前のDNS 問い合わせに失敗しました。</translation>
    </message>
</context>
<context>
    <name>IPv6Dialog_q</name>
    <message>
        <location filename="ipv6dialog_q.ui" line="16"/>
        <source>IPv6</source>
        <translation>IPv6</translation>
    </message>
    <message>
        <location filename="ipv6dialog_q.ui" line="87"/>
        <source>Address IPv6</source>
        <translation>IPv6 アドレス</translation>
    </message>
    <message>
        <location filename="ipv6dialog_q.ui" line="161"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="ipv6dialog_q.ui" line="199"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="ipv6dialog_q.ui" line="243"/>
        <source>Network (bit length)</source>
        <translation>ネットワーク (ビット長)</translation>
    </message>
    <message>
        <location filename="ipv6dialog_q.ui" line="287"/>
        <source>DNS Lookup...</source>
        <translation>DNS 問い合わせ...</translation>
    </message>
    <message>
        <location filename="ipv6dialog_q.ui" line="305"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>InterfaceDialog</name>
    <message>
        <location filename="InterfaceDialog.cpp" line="234"/>
        <source>Group: </source>
        <translation>グループ: </translation>
    </message>
    <message>
        <location filename="InterfaceDialog.cpp" line="253"/>
        <source>Network: </source>
        <translation>ネットワーク: </translation>
    </message>
</context>
<context>
    <name>InterfaceDialog_q</name>
    <message>
        <location filename="interfacedialog_q.ui" line="70"/>
        <source>Interface</source>
        <translation>インターフェース</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="150"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="182"/>
        <source>Label:</source>
        <translation>ラベル:</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="220"/>
        <source>Security level:</source>
        <translation>セキュリティレベル:</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="242"/>
        <source>&lt;p&gt;Each interface of the firewall must have security level associated with it.&lt;br&gt;Security level can be any number between 0 and 100, 0 being least secure and 100 being most secure levels. Interface with security level 0 ususally serves Internet connection.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="245"/>
        <source>&lt;p&gt;Each interface of the firewall must have security level associated with it.&lt;br&gt;
Security level can be any number between 0 and 100, 0 being least secure and 100 being most secure levels. Interface with security level 0 ususally serves Internet connection.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="292"/>
        <source>&lt;p&gt;One interface of the firewall must be marked as &apos;external&apos;. This interface should be connected to the least secure network, usually the Internet.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="295"/>
        <source>One interface of the firewall must be marked as &apos;external&apos;. This interface should be connected to the least secure network, usually the Internet.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="298"/>
        <source>This interface is external (insecure)</source>
        <translation>このインターフェースは外部です (セキュアではありません)</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="305"/>
        <source>&lt;p&gt;Check if this interface is used for management (SNMP queries, remote policy install etc.)&lt;p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="308"/>
        <source>Management interface</source>
        <translation>インターフェンス管理</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="315"/>
        <source>Unnumbered interface</source>
        <translation>アンナンバードインターフェース</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="322"/>
        <source>Address is assigned dynamically</source>
        <translation>アドレスは動的に割り当てました</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="329"/>
        <source>Regular interface</source>
        <translation>レギュラーインターフェース</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="336"/>
        <source>Skip this interface while assigning policy rules</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="339"/>
        <source>Unprotected interface</source>
        <translation>保護なしインターフェース</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="346"/>
        <source>Bridge port</source>
        <translation>ブリッジポート</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="362"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="410"/>
        <source>Network zone:</source>
        <translation>ネットワークゾーン:</translation>
    </message>
    <message>
        <location filename="interfacedialog_q.ui" line="437"/>
        <source>&lt;p&gt;Network zone consists of hosts and networks that can be reached through this interface of the firewall. Subnet to which this interface is directly attached must be part of its network zone. Other subnets reachable by means of routing should alse be added to the network zone.
&lt;br&gt;
If network zone for this interface consists of only one subnet, you can simply choose that network&apos;s object in the pull-down below. If your network zone should include multiple subnets, you need to create an Object Group, then put all hosts and networks which are going to be part of the network zone into that group and finally choose this group in the pull-down below.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LibExport_q</name>
    <message>
        <location filename="libexport_q.ui" line="21"/>
        <source>Export</source>
        <translation>エクスポート</translation>
    </message>
    <message>
        <location filename="libexport_q.ui" line="67"/>
        <source>This will export a library to a file which can later be imported back into Firewall Builder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="libexport_q.ui" line="84"/>
        <source>New Item</source>
        <translation>新規項目</translation>
    </message>
    <message>
        <location filename="libexport_q.ui" line="92"/>
        <source>Make exported libraries read-only</source>
        <translation>エクスポートしたライブラリーは読み込み専用に設定します</translation>
    </message>
    <message>
        <location filename="libexport_q.ui" line="99"/>
        <source>Choose libraries to be exported:</source>
        <translation>エクスポートするライブラリーを選択:</translation>
    </message>
    <message>
        <location filename="libexport_q.ui" line="170"/>
        <source>Ok</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="libexport_q.ui" line="177"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
</context>
<context>
    <name>LibraryDialog_q</name>
    <message>
        <location filename="librarydialog_q.ui" line="78"/>
        <source>Library</source>
        <translation>ライブラリー</translation>
    </message>
    <message>
        <location filename="librarydialog_q.ui" line="128"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="librarydialog_q.ui" line="154"/>
        <source>Color:</source>
        <translation>色:</translation>
    </message>
    <message>
        <location filename="librarydialog_q.ui" line="202"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>MetricEditorPanel_q</name>
    <message>
        <location filename="metriceditorpanel_q.ui" line="16"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
    <message>
        <location filename="metriceditorpanel_q.ui" line="107"/>
        <source>textLabel2</source>
        <translation>テキストラベル 2</translation>
    </message>
</context>
<context>
    <name>NATRuleOptionsDialog_q</name>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="13"/>
        <source>NAT Rule Options</source>
        <translation>NAT ルールオプション</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="99"/>
        <source>fw/rule num</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="164"/>
        <source>No options are available for this firewall platform</source>
        <translation>このファイアウォールプラットフォームで利用できるオプションはありません</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="237"/>
        <source>Pool type</source>
        <translation>プールの種類</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="261"/>
        <source>default</source>
        <translation>標準</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="271"/>
        <source>bitmask</source>
        <translation>ビットマスク</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="278"/>
        <source>random</source>
        <translation>ランダム</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="285"/>
        <source>source-hash</source>
        <translation>ソースハッシュ</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="292"/>
        <source>round-robin</source>
        <translation>ラウンドロビン</translation>
    </message>
    <message>
        <location filename="natruleoptionsdialog_q.ui" line="334"/>
        <source>static-port</source>
        <translation>静的ポート</translation>
    </message>
</context>
<context>
    <name>NetworkDialog</name>
    <message>
        <location filename="NetworkDialog.cpp" line="120"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation>不当な IP アドレス「%1」</translation>
    </message>
    <message>
        <location filename="NetworkDialog.cpp" line="154"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="NetworkDialog.cpp" line="153"/>
        <source>Illegal netmask &apos;%1&apos;</source>
        <translation>不当なネットマスク 「%1」</translation>
    </message>
</context>
<context>
    <name>NetworkDialogIPv6</name>
    <message>
        <location filename="NetworkDialogIPv6.cpp" line="121"/>
        <source>Illegal IPv6 address &apos;%1&apos;</source>
        <translation>不当な IPv6 アドレス「%1」</translation>
    </message>
    <message>
        <location filename="NetworkDialogIPv6.cpp" line="138"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="NetworkDialogIPv6.cpp" line="136"/>
        <source>Illegal netmask &apos;%1&apos;</source>
        <translation>不当なネットマスク 「%1」</translation>
    </message>
</context>
<context>
    <name>NetworkDialogIPv6_q</name>
    <message>
        <location filename="networkdialogipv6_q.ui" line="87"/>
        <source>Network</source>
        <translation>ネットワーク</translation>
    </message>
    <message>
        <location filename="networkdialogipv6_q.ui" line="159"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="networkdialogipv6_q.ui" line="187"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="networkdialogipv6_q.ui" line="213"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="networkdialogipv6_q.ui" line="257"/>
        <source>Netmask (bit length)</source>
        <translation>ネットマスク (ビット長)</translation>
    </message>
</context>
<context>
    <name>NetworkDialog_q</name>
    <message>
        <location filename="networkdialog_q.ui" line="90"/>
        <source>Network</source>
        <translation>ネットワーク</translation>
    </message>
    <message>
        <location filename="networkdialog_q.ui" line="152"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="networkdialog_q.ui" line="178"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="networkdialog_q.ui" line="198"/>
        <source>Netmask:</source>
        <translation>ネットマスク:</translation>
    </message>
    <message>
        <location filename="networkdialog_q.ui" line="237"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>ObjConflictResolutionDialog</name>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="67"/>
        <source>Keep current object</source>
        <translation>現在のオブジェクトを保持する</translation>
    </message>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="68"/>
        <source>Replace with this object</source>
        <translation>このオブジェクトと置換する</translation>
    </message>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="147"/>
        <source>Object &apos;%1&apos; has been deleted</source>
        <translation>オブジェクト「%1」を削除しました</translation>
    </message>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="148"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="182"/>
        <source>Object &apos;%1&apos; in the objects tree</source>
        <translation>オブジェクトツリー中のオブジェクト「%1」</translation>
    </message>
    <message>
        <location filename="ObjConflictResolutionDialog.cpp" line="186"/>
        <source>Object &apos;%1&apos; in file %2</source>
        <translation>ファイル %2 中の オブジェクト「%1」</translation>
    </message>
</context>
<context>
    <name>ObjConflictResolutionDialog_q</name>
    <message>
        <location filename="objconflictresolutiondialog_q.ui" line="13"/>
        <source>Conflict Resolution</source>
        <translation>衝突の解決</translation>
    </message>
    <message>
        <location filename="objconflictresolutiondialog_q.ui" line="54"/>
        <source>There is a conflict between an object in your tree and object in the file you are trying to open. Choose which version of this object you want to use:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="objconflictresolutiondialog_q.ui" line="80"/>
        <source>Current Object </source>
        <translation>現在のオブジェクト </translation>
    </message>
    <message>
        <location filename="objconflictresolutiondialog_q.ui" line="116"/>
        <source>Keep current object</source>
        <translation>現在のオブジェクトを保持する</translation>
    </message>
    <message>
        <location filename="objconflictresolutiondialog_q.ui" line="219"/>
        <source>Always choose this
object if there is a conflict</source>
        <translation>衝突する場合、常にこのオブジェクトを選んでください</translation>
    </message>
    <message>
        <location filename="objconflictresolutiondialog_q.ui" line="160"/>
        <source>New Object</source>
        <translation>新規オブジェクト</translation>
    </message>
    <message>
        <location filename="objconflictresolutiondialog_q.ui" line="196"/>
        <source>Replace with this object</source>
        <translation>このオブジェクトと置換する</translation>
    </message>
</context>
<context>
    <name>ObjectEditor</name>
    <message>
        <location filename="ObjectEditor.cpp" line="465"/>
        <source>Modifications done to this object can not be saved.
Do you want to continue editing it ?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ObjectEditor.cpp" line="499"/>
        <source>&amp;Continue editing</source>
        <translation>編集を続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="ObjectEditor.cpp" line="467"/>
        <source>&amp;Discard changes</source>
        <translation>変更を破棄する(&amp;D)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>This object has been modified but not saved.
Do you want to save it ?</source>
        <translation type="obsolete">このオブジェクトの修正が保存されていません。
保存しますか?</translation>
    </message>
    <message>
        <location filename="ObjectEditor.cpp" line="499"/>
        <source>&amp;Save</source>
        <translation>保存(&amp;S)</translation>
    </message>
    <message>
        <location filename="ObjectEditor.cpp" line="499"/>
        <source>&amp;Discard</source>
        <translation>破棄する(&amp;D)</translation>
    </message>
    <message>
        <location filename="ObjectEditor.cpp" line="498"/>
        <source>Object %1 has been modified but not saved.
Do you want to save it ?</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ObjectManipulator</name>
    <message>
        <location filename="ObjectManipulator.cpp" line="141"/>
        <source>Object Manipulator</source>
        <translation>オブジェクトマニピュレーター</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="166"/>
        <source>New &amp;Library</source>
        <translation>新規ライブラリー(&amp;L)</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1052"/>
        <source>New Firewall</source>
        <translation>新規ファイアウォール</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1079"/>
        <source>New Host</source>
        <translation>新規ホスト</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2817"/>
        <source>New Interface</source>
        <translation>新規インターフェース</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2834"/>
        <source>New Network</source>
        <translation>新規ネットワーク</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2843"/>
        <source>New Network IPv6</source>
        <translation>新規 IPv6 ネットワーク</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2883"/>
        <source>New Address</source>
        <translation>新規アドレス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2900"/>
        <source>New Address IPv6</source>
        <translation>新規 IPv6 アドレス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2913"/>
        <source>New DNS Name</source>
        <translation>新規 DNS 名</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2924"/>
        <source>New Address Table</source>
        <translation>新規アドレステーブル</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3026"/>
        <source>New Address Range</source>
        <translation>新規アドレスの範囲</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3037"/>
        <source>New Object Group</source>
        <translation>新規オブジェクトグループ</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3049"/>
        <source>New Custom Service</source>
        <translation>新規カスタムサービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3060"/>
        <source>New IP Service</source>
        <translation>新規 IP サービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3071"/>
        <source>New ICMP Service</source>
        <translation>新規 ICMP サービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3082"/>
        <source>New ICMP6 Service</source>
        <translation>新規 ICMP6 サービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="211"/>
        <source>New TCP Serivce</source>
        <translation>新規 TCP サービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3104"/>
        <source>New UDP Service</source>
        <translation>新規 UDP サービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2982"/>
        <source>New TagService</source>
        <translation>新規タグサービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2992"/>
        <source>New User Service</source>
        <translation>新規ユーザーサービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3115"/>
        <source>New Service Group</source>
        <translation>新規サービスグループ</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3127"/>
        <source>New Time Interval</source>
        <translation>新規時間間隔</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="589"/>
        <source>The name of the object &apos;%1&apos; has changed. The program can also
rename IP address objects that belong to this object,
using standard naming scheme &apos;host_name:interface_name:ip&apos;.
This makes it easier to distinguish what host or a firewall
given IP address object belongs to when it is used in 
the policy or NAT rule. The program also renames MAC address
objects using scheme &apos;host_name:interface_name:mac&apos;.
Do you want to rename child IP and MAC address objects now?
(If you click &apos;No&apos;, names of all address objects that belong to
%2 will stay the same.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2005"/>
        <source>&amp;Yes</source>
        <translation>はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2005"/>
        <source>&amp;No</source>
        <translation>いいえ(&amp;N)</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="621"/>
        <source>The name of the interface &apos;%1&apos; has changed. The program can also
rename IP address objects that belong to this interface,
using standard naming scheme &apos;host_name:interface_name:ip&apos;.
This makes it easier to distinguish what host or a firewall
given IP address object belongs to when it is used in 
the policy or NAT rule. The program also renames MAC address
objects using scheme &apos;host_name:interface_name:mac&apos;.
Do you want to rename child IP and MAC address objects now?
(If you click &apos;No&apos;, names of all address objects that belong to
interface &apos;%2&apos; will stay the same.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="947"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="955"/>
        <source>Duplicate ...</source>
        <translation>複製 ...</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="956"/>
        <source>Move ...</source>
        <translation>移動 ...</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="988"/>
        <source>place in library %1</source>
        <translation>ライブラリー %1 の中の場所</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="999"/>
        <source>to library %1</source>
        <translation>ライブラリー %1 へ</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1008"/>
        <source>Copy</source>
        <translation>コピー</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1010"/>
        <source>Cut</source>
        <translation>切り取り</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1012"/>
        <source>Paste</source>
        <translation>貼り付け</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1017"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1030"/>
        <source>Add Interface</source>
        <translation>インターフェース追加</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1036"/>
        <source>Add Policy Rule Set</source>
        <translation>ポリシールールセット追加</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1038"/>
        <source>Add NAT Rule Set</source>
        <translation>NAT ルールセット追加</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1043"/>
        <source>Add IP Address</source>
        <translation>IP アドレス追加</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1045"/>
        <source>Add IPv6 Address</source>
        <translation>IPv6 アドレス追加</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1047"/>
        <source>Add MAC Address</source>
        <translation>MAC アドレス追加</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1122"/>
        <source>New Group</source>
        <translation>新規グループ</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3093"/>
        <source>New TCP Service</source>
        <translation>新規 TCP サービス</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1130"/>
        <source>Find</source>
        <translation>検索</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1132"/>
        <source>Where used</source>
        <translation>使うとき</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1135"/>
        <source>Group</source>
        <translation>グループ</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1143"/>
        <source>Compile</source>
        <translation>コンパイル</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1144"/>
        <source>Install</source>
        <translation>インストール</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1151"/>
        <source>Lock</source>
        <translation>ロックする</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1153"/>
        <source>Unlock</source>
        <translation>ロック解除する</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1162"/>
        <source>dump</source>
        <translation>ダンプ</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1927"/>
        <source>Emptying the &apos;Deleted Objects&apos; in a library file is not recommended.
When you remove deleted objects from a library file, Firewall Builder
loses ability to track them. If a group or a policy rule in some
data file still uses removed object from this library, you may encounter
unusual and unexpected behavior of the program.
Do you want to delete selected objects anyway ?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2004"/>
        <source>When you delete a library, all objects that belong to it
disappear from the tree and all groups and rules that reference them.
You won&apos;t be able to reverse this operation later.
Do you still want to delete library %1?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="3242"/>
        <source>Searching for firewalls affected by the change...</source>
        <translation>変更に影響したファイアウォールを検索しています...</translation>
    </message>
</context>
<context>
    <name>ObjectManipulator_q</name>
    <message>
        <location filename="objectmanipulator_q.ui" line="22"/>
        <source>Tree of Objects</source>
        <translation>オブジェクトのツリー</translation>
    </message>
    <message>
        <location filename="objectmanipulator_q.ui" line="105"/>
        <source>Back</source>
        <translation>戻る</translation>
    </message>
    <message>
        <location filename="objectmanipulator_q.ui" line="108"/>
        <source>Go back to the previous object</source>
        <translation>前のオブジェクトに戻る</translation>
    </message>
    <message>
        <location filename="objectmanipulator_q.ui" line="167"/>
        <source>New Object</source>
        <translation>新規オブジェクト</translation>
    </message>
    <message>
        <location filename="objectmanipulator_q.ui" line="170"/>
        <source>Create New Object</source>
        <translation>新規オブジェクト作成</translation>
    </message>
</context>
<context>
    <name>ObjectTreeView</name>
    <message>
        <location filename="ObjectTreeView.cpp" line="129"/>
        <source>Object</source>
        <translation>オブジェクト</translation>
    </message>
</context>
<context>
    <name>PhysAddressDialog_q</name>
    <message>
        <location filename="physaddressdialog_q.ui" line="16"/>
        <source>physAddress</source>
        <translation>物理アドレス</translation>
    </message>
    <message>
        <location filename="physaddressdialog_q.ui" line="78"/>
        <source>MAC Address</source>
        <translation>MAC アドレス</translation>
    </message>
    <message>
        <location filename="physaddressdialog_q.ui" line="125"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="physaddressdialog_q.ui" line="151"/>
        <source>Physical address (MAC):</source>
        <translation>物理アドレス (MAC):</translation>
    </message>
    <message>
        <location filename="physaddressdialog_q.ui" line="196"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>PrefsDialog</name>
    <message>
        <location filename="PrefsDialog.cpp" line="293"/>
        <source>Find working directory</source>
        <translation>作業ディレクトリの検索</translation>
    </message>
    <message>
        <location filename="PrefsDialog.cpp" line="302"/>
        <source>Find Secure Shell utility</source>
        <translation>セキュアシェルユーティリティの検索</translation>
    </message>
    <message>
        <location filename="PrefsDialog.cpp" line="311"/>
        <source>Find SCP utility</source>
        <translation>SCP ユーティリティの検索</translation>
    </message>
    <message>
        <location filename="PrefsDialog.cpp" line="449"/>
        <source>Your version of Firewall Builder is up to date.</source>
        <translation>ファイアウォールビルダーのバージョンは最新です。</translation>
    </message>
    <message>
        <location filename="PrefsDialog.cpp" line="455"/>
        <source>A new version of Firewall Builder is available at http://www.fwbuilder.org</source>
        <translation>ファイアウォールビルダーの新しいバージョンを利用できます http://www.fwbuilder.org</translation>
    </message>
    <message>
        <location filename="PrefsDialog.cpp" line="462"/>
        <source>Error checking for software updates:
%1</source>
        <translation>ソフトウェアの更新の確認中にエラー:
%1</translation>
    </message>
</context>
<context>
    <name>PrintingProgressDialog</name>
    <message>
        <location filename="PrintingProgressDialog.cpp" line="60"/>
        <source>Printing (page %1/%2)</source>
        <translation>印刷中 (ページ %1/%2)</translation>
    </message>
    <message>
        <location filename="PrintingProgressDialog.cpp" line="62"/>
        <source>Printing page %1</source>
        <translation>印刷中 (ページ %1)</translation>
    </message>
    <message>
        <location filename="PrintingProgressDialog.cpp" line="80"/>
        <source>Aborting print operation</source>
        <translation>印刷操作を中断しています</translation>
    </message>
    <message>
        <location filename="PrintingProgressDialog.cpp" line="82"/>
        <source>Cannot abort printing</source>
        <translation>印刷を中断できません</translation>
    </message>
</context>
<context>
    <name>ProjectPanel</name>
    <message>
        <location filename="ProjectPanel.cpp" line="145"/>
        <source>No firewalls defined</source>
        <translation>ファイアウォールは未定義です</translation>
    </message>
    <message>
        <location filename="ProjectPanel.cpp" line="195"/>
        <source>Untitled</source>
        <translation>名称未設定</translation>
    </message>
    <message>
        <location filename="ProjectPanel.cpp" line="200"/>
        <source>(read-only)</source>
        <translation>(読み込み専用)</translation>
    </message>
    <message>
        <location filename="ProjectPanel.cpp" line="406"/>
        <source>Building policy view...</source>
        <translation>ポリシービューを構築しています...</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="88"/>
        <source>&amp;Save</source>
        <translation>保存(&amp;S)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="88"/>
        <source>&amp;Discard</source>
        <translation>破棄する(&amp;D)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="361"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="114"/>
        <source>FWB Files (*.fwb);;All Files (*)</source>
        <translation>FWB ファイル (*.fwb);;すべてのファイル (*)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1138"/>
        <source>Choose name and location for the new file</source>
        <translation>新規ファイルの名前とフォルダーを選択する</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="184"/>
        <source>Open File</source>
        <translation>ファイルを開く</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="290"/>
        <source>Saving data to file...</source>
        <translation>ファイルにデータを保存しています...</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="316"/>
        <source>Choose name and location for the file</source>
        <translation>ファイルの名前とフォルダーを選択する</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="359"/>
        <source>This operation discards all changes that have been saved
into the file so far, closes it and replaces it with a clean
copy of its head revision from RCS.

All changes will be lost if you do this.

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="360"/>
        <source>&amp;Discard changes</source>
        <translation>変更を破棄する(&amp;D)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="410"/>
        <source>File %1 has been added to RCS.</source>
        <translation>ファイル %1 を RCS に追加しました。</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1415"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="419"/>
        <source>Error adding file to RCS:
%1</source>
        <translation>RCS へのファイルの追加エラー:
%1</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="433"/>
        <source>Choose a file to import</source>
        <translation>インポートするファイルを選んでください</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="450"/>
        <source>This operation inspects two data files (either .fwb or .fwl) and finds conflicting objects. Conflicting objects have the same internal ID but different attributes. Two data files can not be merged, or one imported into another, if they contain such objects. This operation also helps identify changes made to objects in two copies of the same data file.&lt;br&gt;&lt;br&gt;This operation does not find objects present in one file but not in the other, such objects present no problem for merge or import operations.&lt;br&gt;&lt;br&gt;This operation works with two external files, neither of which needs to be opened in the program. Currently opened data file is not affected by this operation and objects in the tree do not change.&lt;br&gt;&lt;br&gt;Do you want to proceed ?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="459"/>
        <source>Choose the first file</source>
        <translation>1 つ目のファイルを選んでください</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="467"/>
        <source>Choose the second file</source>
        <translation>2 つ目のファイルを選んでください</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="511"/>
        <source>Error loading file %1:
%2</source>
        <translation>ファイル %1 の読み込み中のエラー:
%2</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="534"/>
        <source>Total number of conflicting objects: %1.
Do you want to generate report?</source>
        <translation>衝突オブジェクトの総数: %1
報告を生成しますか?</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="544"/>
        <source>Choose name and location for the report file</source>
        <translation>報告ファイルの場所と名前を選んでください</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="546"/>
        <source>TXT Files (*.txt);;All Files (*)</source>
        <translation>テキストファイル (*.txt);;すべてのファイル (*)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="571"/>
        <source>Can not open report file for writing. File &apos;%1&apos;</source>
        <translation>報告ファイルを書き込むために開けません。ファイル「%1」</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="583"/>
        <source>Unexpected error comparing files %1 and %2:
%3</source>
        <translation>%1 と %2 のファイル比較で予期せぬエラー:
%3</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="613"/>
        <source>Please select a library you want to export.</source>
        <translation>エクスポートしたいライブラリーを選択してください。</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="637"/>
        <source>The file %1 already exists.
Do you want to overwrite it ?</source>
        <translation>ファイル %1 は既に存在します。
上書きしますか?</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="638"/>
        <source>&amp;Yes</source>
        <translation>はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="638"/>
        <source>&amp;No</source>
        <translation>いいえ(&amp;N)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="770"/>
        <source>A library that you are trying to export contains references
to objects in the other libraries and can not be exported.
The following objects need to be moved outside of it or
objects that they refer to moved in it:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1217"/>
        <source>(Long error message was truncated)</source>
        <translation>(長いエラーメッセージを切り詰めました)</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="880"/>
        <source>The program encountered error trying to load file %1.
The file has not been loaded. Error:
%2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="978"/>
        <source>Loading system objects...</source>
        <translation>システムオブジェクトを読み込んでいます...</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="943"/>
        <source>Error loading file:
%1</source>
        <translation>ファイルの読み込み中のエラー:
%1</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="986"/>
        <source>Reading and parsing data file...</source>
        <translation>データファイルの読み込みと解析をしています...</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1025"/>
        <source>Merging with system objects...</source>
        <translation>システムオブジェクトと結合しています...</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1133"/>
        <source>Firewall Builder 2 uses file extension &apos;.fwb&apos; and
needs to rename old data file &apos;%1&apos; to &apos;%2&apos;,
but file &apos;%3&apos; already exists.
Choose a different name for the new file.</source>
        <translation>ファイアウォールビルダー 2 はファイル拡張子に「.fwb」を使っていましたが、
古いデータファイル「%1」の名前を「%2」に変える必要があります。
しかしファイル「%3」は既に存在します。
新しいファイルのための違う名前を選んでください。</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1149"/>
        <source>Load operation cancelled and data file revertedto original version.</source>
        <translation>読み込み操作を取り消し、データファイルをオリジナルバージョンに戻しました。</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1166"/>
        <source>Firewall Builder 2 uses file extension &apos;.fwb&apos;. Your datafile &apos;%1&apos; 
has been renamed &apos;%2&apos;</source>
        <translation>ファイアウォールビルダー 2 はファイル拡張子に「.fwb」を使っていました。
データファイル「%1」の名前を「%2」に変える必要があります</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1194"/>
        <source>Exception: %1</source>
        <translation>例外: %1</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1198"/>
        <source>Failed transformation : %1</source>
        <translation>変換に失敗しました : %1</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1203"/>
        <source>XML element : %1</source>
        <translation>XML 要素 : %1</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1223"/>
        <source>The program encountered error trying to load data file.
The file has not been loaded. Error:
%1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1238"/>
        <source>Building object tree...</source>
        <translation>オブジェクトツリーを構築しています...</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1244"/>
        <source>Indexing...</source>
        <translation>インデックスを作成しています...</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1278"/>
        <source>Checking file %1 in RCS</source>
        <translation>RCS にファイル %1 をチェックインしています</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1312"/>
        <source>Error checking in file %1:
%2</source>
        <translation>ファイル %1 の検査中のエラー:%2</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1408"/>
        <source>File is read-only</source>
        <translation>ファイルは読み込み専用です</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="1414"/>
        <source>Error saving file %1: %2</source>
        <translation>ファイル %1 の保存中のエラー: %2</translation>
    </message>
</context>
<context>
    <name>ProjectPanel_q</name>
    <message>
        <location filename="projectpanel_q.ui" line="13"/>
        <source>Form</source>
        <translation>フォーム</translation>
    </message>
    <message>
        <location filename="projectpanel_q.ui" line="173"/>
        <source>Click here to change amount of information shown about object selected in the tree</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="projectpanel_q.ui" line="482"/>
        <source>Apply</source>
        <translation>適用する</translation>
    </message>
    <message>
        <location filename="projectpanel_q.ui" line="495"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
    <message>
        <location filename="projectpanel_q.ui" line="572"/>
        <source>Firewall / ruleset</source>
        <translation>ファイアウォール / ルールセット</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="DialogFactory.cpp" line="206"/>
        <source>Support module for %1 is not available</source>
        <translation>%1 のサポートモジュールは利用できません</translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="2237"/>
        <source>Firewall Builder can import Cisco IOS access lists from the router configuration saved using &apos;show run&apos; or any other command that saves running config. The name of the created firewall object, all of its interfaces and their addresses will be configured automatically if this information can be found in the configuration file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="DiscoveryDruid.cpp" line="2247"/>
        <source>Firewall Builder can import iptables rules from a file in iptables-save format. Firewall name and addresses of its interfaces need to be configured manually because iptables-save file does not have this information. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWBSettings.cpp" line="139"/>
        <source>Working directory %1 does not exist and could not be created.
Ignoring this setting.</source>
        <translation>作業ディレクトリ %1 が存在しない上に作成できませんでした。
この設定は無効になります。</translation>
    </message>
    <message>
        <location filename="FWBTree.cpp" line="419"/>
        <source>New Library</source>
        <translation>新規ライブラリー</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="113"/>
        <source>DNS record: </source>
        <translation>DNS レコード: </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="117"/>
        <source>Address Table: </source>
        <translation>アドレステーブル: </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="178"/>
        <source> objects</source>
        <translation> オブジェクト</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="199"/>
        <source>protocol: %1</source>
        <translation>プロトコル: %1</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="203"/>
        <source>type: %1</source>
        <translation>種類: %1</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="205"/>
        <source>code: %1</source>
        <translation>コード: %1</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="269"/>
        <source>&lt;b&gt;Library:&lt;/b&gt; </source>
        <translation>&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="274"/>
        <source>&lt;b&gt;Object Id:&lt;/b&gt; </source>
        <translation>&lt;b&gt;オブジェクト ID:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="279"/>
        <source>&lt;b&gt;Object Type:&lt;/b&gt; </source>
        <translation>&lt;b&gt;オブジェクトの種類:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="283"/>
        <source>&lt;b&gt;Object Name:&lt;/b&gt; </source>
        <translation>&lt;b&lt;&gt;オブジェクト名:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="316"/>
        <source>&lt;b&gt;DNS record:&lt;/b&gt;</source>
        <translation>&lt;b&gt;DNS レコード:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="327"/>
        <source>Run-time</source>
        <translation>ランタイム</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="327"/>
        <source>Compile-time</source>
        <translation>コンパイル時間</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="324"/>
        <source>&lt;b&gt;Table file:&lt;/b&gt;</source>
        <translation>&lt;b&lt;&gt;テーブルファイル:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="368"/>
        <source>%1 objects&lt;br&gt;
</source>
        <translation>%1 オブジェクト&lt;br&gt;
</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="433"/>
        <source>&lt;b&gt;Path:&lt;/b&gt; </source>
        <translation>&lt;b&gt;パス:&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="499"/>
        <source>protocol </source>
        <translation>プロトコル </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="504"/>
        <source>type: </source>
        <translation>種類: </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="506"/>
        <source>code: </source>
        <translation>コード: </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="526"/>
        <source>Pattern: &quot;%1&quot;</source>
        <translation>パターン: 「%1」</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="530"/>
        <source>User id: &quot;%1&quot;</source>
        <translation>ユーザー ID: 「%1」</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="676"/>
        <source>&lt;b&gt;Action   :&lt;/b&gt; </source>
        <translation>&lt;b&gt;操作     :&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="679"/>
        <source>&lt;b&gt;Parameter:&lt;/b&gt; </source>
        <translation>&lt;b&gt;パラメーター:&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="702"/>
        <source>&lt;b&gt;Log prefix    :&lt;/b&gt; </source>
        <translation>&lt;b&gt;ログ接頭語    :&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="708"/>
        <source>&lt;b&gt;Log Level     :&lt;/b&gt; </source>
        <translation>&lt;b&gt;ログレベル    :&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="715"/>
        <source>&lt;b&gt;Netlink group :&lt;/b&gt; </source>
        <translation>&lt;b&gt;ネットリンクグループ :&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="721"/>
        <source>&lt;b&gt;Limit Value   :&lt;/b&gt; </source>
        <translation>&lt;b&gt;制限値        :&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="727"/>
        <source>&lt;b&gt;Limit suffix  :&lt;/b&gt; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="734"/>
        <source>&lt;b&gt;Limit burst   :&lt;/b&gt; </source>
        <translation>&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="741"/>
        <source>&lt;li&gt;&lt;b&gt;Part of Any&lt;/b&gt;&lt;/li&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="829"/>
        <source>&lt;li&gt;&lt;b&gt;Stateless&lt;/b&gt;&lt;/li&gt; </source>
        <translation>&lt;li&gt;&lt;b&gt;ステートレス&lt;/b&gt;&lt;/li&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="756"/>
        <source>&lt;b&gt;Log facility:&lt;/b&gt; </source>
        <translation>&lt;b&gt;ログファシリティ:&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="846"/>
        <source>&lt;b&gt;Log level   :&lt;/b&gt; </source>
        <translation>&lt;b&gt;ログレベル  :&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="771"/>
        <source>&lt;li&gt;&lt;b&gt;Send &apos;unreachable&apos;&lt;/b&gt;&lt;/li&gt;</source>
        <translation>&lt;li&gt;&lt;b&gt;「未到達(unreachable)」を送る&lt;/b&gt;&lt;/li&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="783"/>
        <source>&lt;li&gt;&lt;b&gt;Keep information on fragmented packets&lt;/b&gt;&lt;/li&gt; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="793"/>
        <source>&lt;b&gt;Log prefix :&lt;/b&gt; </source>
        <translation>&lt;b&gt;ログ接頭語 :&lt;/b&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="799"/>
        <source>&lt;b&gt;Max state  :&lt;/b&gt; </source>
        <translation type="unfinished">&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="812"/>
        <source>&lt;li&gt;&lt;b&gt;Source tracking&lt;/b&gt;&lt;/li&gt; </source>
        <translation>&lt;li&gt;&lt;b&gt;送信元の追跡&lt;/b&gt;&lt;/li&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="815"/>
        <source>&lt;b&gt;Max src nodes :&lt;/b&gt; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="818"/>
        <source>&lt;b&gt;Max src states:&lt;/b&gt; </source>
        <translation type="unfinished">&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="838"/>
        <source>&lt;u&gt;&lt;b&gt;Ver:%1&lt;/b&gt;&lt;/u&gt;&lt;br&gt;
</source>
        <translation>&lt;u&gt;&lt;b&gt;バージョン:%1&lt;/b&gt;&lt;/u&gt;&lt;br&gt;
</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="852"/>
        <source>&lt;b&gt;Log interval  :&lt;/b&gt; </source>
        <translation></translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="859"/>
        <source>&lt;li&gt;&lt;b&gt;Disable logging for this rule&lt;/b&gt;&lt;/li&gt; </source>
        <translation>&lt;li&gt;&lt;b&gt;このルールのログ記録をの無効化&lt;/b&gt;&lt;/li&gt; </translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="891"/>
        <source>bitmask</source>
        <translation>ビットマスク</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="892"/>
        <source>random</source>
        <translation>ランダム</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="893"/>
        <source>source-hash</source>
        <translation>ソースハッシュ</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="894"/>
        <source>round-robin</source>
        <translation>ラウンドロビン</translation>
    </message>
    <message>
        <location filename="FWObjectPropertiesFactory.cpp" line="896"/>
        <source>static-port</source>
        <translation>静的ポート</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="78"/>
        <source>Firewall</source>
        <translation>ファイアウォール</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="79"/>
        <source>Host</source>
        <translation>ホスト</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="81"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="82"/>
        <source>Addres Range</source>
        <translation>アドレス範囲</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="632"/>
        <source>Interface</source>
        <translation>インターフェース</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="84"/>
        <source>Network</source>
        <translation>ネットワーク</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="85"/>
        <source>Group of objects</source>
        <translation>オブジェクトのグループ</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="86"/>
        <source>Custom Service</source>
        <translation>カスタムサービス</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="87"/>
        <source>IP Service</source>
        <translation>IP サービス</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="89"/>
        <source>ICMP Service</source>
        <translation>ICMP サービス</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="90"/>
        <source>TCP Service</source>
        <translation>TCP サービス</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="91"/>
        <source>UDP Service</source>
        <translation>UDP サービス</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="92"/>
        <source>Group of services</source>
        <translation>サービスのグループ</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="93"/>
        <source>Time Interval</source>
        <translation>時間間隔</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="329"/>
        <source>Firewall name: %1</source>
        <translation>ファイアウォール名: %1</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="331"/>
        <source>Platform: </source>
        <translation>プラットフォーム: </translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="332"/>
        <source>Version: </source>
        <translation>バージョン: </translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="333"/>
        <source>Host OS: </source>
        <translation>ホスト OS: </translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="283"/>
        <source>Policy: </source>
        <translation>ポリシー:</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>NAT: </source>
        <translation type="obsolete">NAT:</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="580"/>
        <source>Routing</source>
        <translation>ルーティング</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="278"/>
        <source> ( read only )</source>
        <translation>( 読み込み専用 )</translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="1724"/>
        <source>Impossible to insert object %1 (type %2) into %3
because of incompatible type.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ObjectManipulator.cpp" line="2638"/>
        <source>Type &apos;%1&apos;: new object can not be created because
corresponding branch is missing in the object tree.
Please repair the tree using command &apos;fwbedit -s -f file.fwb&apos;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="733"/>
        <source>Library %1: Firewall &apos;%2&apos; (global policy rule #%3) uses object &apos;%4&apos; from library &apos;%5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="743"/>
        <source>Library %1: Firewall &apos;%2&apos; (NAT rule #%3) uses object &apos;%4&apos; from library &apos;%5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="753"/>
        <source>Library %1: Group &apos;%2&apos; uses object &apos;%3&apos; from library &apos;%4&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="809"/>
        <source>File is read-only</source>
        <translation>ファイルは読み込み専用です</translation>
    </message>
    <message>
        <location filename="ProjectPanel_file_ops.cpp" line="814"/>
        <source>Error saving file %1: %2</source>
        <translation>ファイル %1 の保存中にエラー: %2</translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="535"/>
        <source>Fatal error during initial RCS checkin of file %1 :
 %2
Exit status %3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="931"/>
        <source>Fatal error running rlog for %1</source>
        <translation>%1 の rlog を実行中に致命的なエラー</translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="977"/>
        <source>Fatal error running rcsdiff for file %1</source>
        <translation>ファイル %1 のrcsdiff を実行中に致命的なエラー</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="358"/>
        <source>Can not open file %1</source>
        <translation>ファイル %1 を開くことができません</translation>
    </message>
    <message>
        <location filename="SSHSession.cpp" line="506"/>
        <source>ERROR</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="SSHSession.cpp" line="506"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="UDPServiceDialog.cpp" line="128"/>
        <source>Invalid range defined for the source port.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="utils.cpp" line="237"/>
        <source>&amp;Continue editing</source>
        <translation>編集を続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="UDPServiceDialog.cpp" line="136"/>
        <source>Invalid range defined for the destination port.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="solarisAdvancedDialog.cpp" line="67"/>
        <source>No change</source>
        <translation>変更なし</translation>
    </message>
    <message>
        <location filename="solarisAdvancedDialog.cpp" line="70"/>
        <source>On</source>
        <translation>オン</translation>
    </message>
    <message>
        <location filename="solarisAdvancedDialog.cpp" line="73"/>
        <source>Off</source>
        <translation>オフ</translation>
    </message>
    <message>
        <location filename="instDialog_compile.cpp" line="92"/>
        <source>Compiling rule sets for firewall: %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_installer.cpp" line="90"/>
        <source>Installing policy rules on firewall &apos;%1&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="226"/>
        <source>&lt;b&gt;Summary:&lt;/b&gt;
</source>
        <translation>&lt;b&gt;要約:&lt;/b&gt;
</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="228"/>
        <source>* firewall name : %1
</source>
        <translation>* ファイアウォール名 : %1
</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="230"/>
        <source>* user name : %1
</source>
        <translation>* ユーザー名 : %1
</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="231"/>
        <source>* management address : %1
</source>
        <translation>* アドレス管理 : %1
</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="233"/>
        <source>* platform : %1
</source>
        <translation>* プラットフォーム : %1
</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="235"/>
        <source>* host OS : %1
</source>
        <translation>* ホスト OS : %1
</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="237"/>
        <source>* Loading configuration from file %1
</source>
        <translation>* ファイル %1 からせって意を読み込んでいます</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="241"/>
        <source>* Incremental install
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="247"/>
        <source>* Configuration diff will be saved in file %1
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="253"/>
        <source>* Commands will not be executed on the firewall
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="1055"/>
        <source>Only one interface of the firewall &apos;%1&apos; must be marked as management interface.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="1063"/>
        <source>One of the interfaces of the firewall &apos;%1&apos; must be marked as management interface.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="1071"/>
        <source>Management interface does not have IP address, can not communicate with the firewall.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="128"/>
        <source>0 - System Unusable</source>
        <translation>0 - システム未使用</translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="133"/>
        <source>1 - Take Immediate Action</source>
        <translation>1 - すぐさま処理をする</translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="138"/>
        <source>2 - Critical Condition</source>
        <translation>2 - 危険な状態</translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="143"/>
        <source>3 - Error Message</source>
        <translation>3 - エラーメッセージ</translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="148"/>
        <source>4 - Warning Message</source>
        <translation>4 - 警告メッセージ</translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="153"/>
        <source>5 - Normal but significant condition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="158"/>
        <source>6 - Informational</source>
        <translation>6 - 情報</translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="163"/>
        <source>7 - Debug Message</source>
        <translation>7 - デバッグメッセージ</translation>
    </message>
    <message>
        <location filename="listOfLibraries.cpp" line="152"/>
        <source>The library file you are trying to open
has been saved in an older version of
Firewall Builder and needs to be upgraded.
To upgrade it, just load it in the Firewall
Builder GUI and save back to file again.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="utils.cpp" line="214"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="listOfLibraries.cpp" line="192"/>
        <source>Error loading file %1:
%2</source>
        <translation>ファイル %1 の読み込み中のエラー:
%2</translation>
    </message>
    <message>
        <location filename="listOfLibraries.cpp" line="219"/>
        <source>Duplicate library &apos;%1&apos;</source>
        <translation>ライブラリー「%1」の複製</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>dynamic</source>
        <translation type="obsolete">ダイナミック</translation>
    </message>
    <message>
        <location filename="pfAdvancedDialog.cpp" line="101"/>
        <source>Aggressive</source>
        <translation>アグレッシブ</translation>
    </message>
    <message>
        <location filename="pfAdvancedDialog.cpp" line="103"/>
        <source>Conservative</source>
        <translation>コンサバティブ</translation>
    </message>
    <message>
        <location filename="pfAdvancedDialog.cpp" line="105"/>
        <source>For high latency</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfAdvancedDialog.cpp" line="107"/>
        <source>Normal</source>
        <translation>通常</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="61"/>
        <source>alert</source>
        <translation>alert</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="63"/>
        <source>crit</source>
        <translation>crit</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="65"/>
        <source>error</source>
        <translation>error</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="67"/>
        <source>warning</source>
        <translation>warning</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="69"/>
        <source>notice</source>
        <translation>notice</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="71"/>
        <source>info</source>
        <translation>info</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="73"/>
        <source>debug</source>
        <translation>debug</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="79"/>
        <source>kern</source>
        <translation>kern</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="81"/>
        <source>user</source>
        <translation>user</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="83"/>
        <source>mail</source>
        <translation>mail</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="85"/>
        <source>daemon</source>
        <translation>daemon</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="87"/>
        <source>auth</source>
        <translation>auth</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="89"/>
        <source>syslog</source>
        <translation>syslog</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="91"/>
        <source>lpr</source>
        <translation>lpr</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="93"/>
        <source>news</source>
        <translation>news</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="95"/>
        <source>uucp</source>
        <translation>uucp</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="97"/>
        <source>cron</source>
        <translation>cron</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="99"/>
        <source>authpriv</source>
        <translation>authpriv</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="101"/>
        <source>ftp</source>
        <translation>ftp</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="103"/>
        <source>local0</source>
        <translation>local0</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="105"/>
        <source>local1</source>
        <translation>local1</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="107"/>
        <source>local2</source>
        <translation>local2</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="109"/>
        <source>local3</source>
        <translation>local3</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="111"/>
        <source>local4</source>
        <translation>local4</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="113"/>
        <source>local5</source>
        <translation>local5</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="115"/>
        <source>local6</source>
        <translation>local6</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="117"/>
        <source>local7</source>
        <translation>local7</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="122"/>
        <source>ICMP admin prohibited</source>
        <translation>ICMP 管理は禁止されました</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="124"/>
        <source>ICMP host prohibited</source>
        <translation>ICMP ホストは禁止されました</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="126"/>
        <source>ICMP host unreachable</source>
        <translation>ICMP ホストに到達しません</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="128"/>
        <source>ICMP net prohibited</source>
        <translation>ICMP ネットは禁止されました</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="130"/>
        <source>ICMP net unreachable</source>
        <translation>ICMP ネットに到達しません</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="132"/>
        <source>ICMP port unreachable</source>
        <translation>ICMP ポートに到達しません</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="134"/>
        <source>ICMP protocol unreachable</source>
        <translation>ICMP プロトコルに到達しません</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="136"/>
        <source>TCP RST</source>
        <translation>TCP RST</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="148"/>
        <source>None</source>
        <translation>なし</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="141"/>
        <source>Route through</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="143"/>
        <source>Route reply through</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="145"/>
        <source>Route a copy through</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="150"/>
        <source>Random</source>
        <translation>ランダム</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="152"/>
        <source>Source Hash</source>
        <translation>ソースハッシュ</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="154"/>
        <source>Round Robin</source>
        <translation>ラウンドロビン</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="157"/>
        <source>on top of the script</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="159"/>
        <source>after interface configuration</source>
        <translation>インターフェース設定の後</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="161"/>
        <source>after policy reset</source>
        <translation>ポリシーリセットの後</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="164"/>
        <source>in the activation shell script</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="167"/>
        <source>in the pf rule file, at the very top</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="170"/>
        <source>in the pf rule file, after set comamnds</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="173"/>
        <source>in the pf rule file, after scrub comamnds</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="176"/>
        <source>in the pf rule file, after table definitions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="181"/>
        <source>/day</source>
        <translation>/日</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="183"/>
        <source>/hour</source>
        <translation>/時</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="185"/>
        <source>/minute</source>
        <translation>/分</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="187"/>
        <source>/second</source>
        <translation>/秒</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="392"/>
        <source>- any -</source>
        <translation>- 任意 -</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="393"/>
        <source>1.2.5 or earlier</source>
        <translation>1.2.5 以上</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="394"/>
        <source>1.2.6 to 1.2.8</source>
        <translation>1.2.6 から 1.2.8</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="395"/>
        <source>1.2.9 to 1.2.11</source>
        <translation>1.2.9 から 1.2.11</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="396"/>
        <source>1.3.x</source>
        <translation>4.x {1.3.?}</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="397"/>
        <source>1.4.0 or later</source>
        <translation>1.3.0 以上 {1.4.0 ?}</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="415"/>
        <source>3.x</source>
        <translation>3.x</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="416"/>
        <source>3.7 to 3.9</source>
        <translation>3.7 から 3.9</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="417"/>
        <source>4.x</source>
        <translation>4.x</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="539"/>
        <source>Accept</source>
        <translation>受理</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="541"/>
        <source>Deny</source>
        <translation>破棄</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="543"/>
        <source>Reject</source>
        <translation>拒否</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="545"/>
        <source>Scrub</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="547"/>
        <source>Return</source>
        <translation>返す</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="549"/>
        <source>Skip</source>
        <translation>飛ばす</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="551"/>
        <source>Continue</source>
        <translation>続ける</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="553"/>
        <source>Modify</source>
        <translation>修正</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="555"/>
        <source>Classify</source>
        <translation>分類</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="557"/>
        <source>Custom</source>
        <translation>カスタム</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="560"/>
        <source>Branch</source>
        <translation>分岐</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="561"/>
        <source>Chain</source>
        <translation>連結</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="562"/>
        <source>Anchor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="566"/>
        <source>Accounting</source>
        <translation>アカウント</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="567"/>
        <source>Count</source>
        <translation>回数</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="571"/>
        <source>Tag</source>
        <translation>タグ</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="572"/>
        <source>Mark</source>
        <translation>マスク</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="576"/>
        <source>Pipe</source>
        <translation>パイプ</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="577"/>
        <source>Queue</source>
        <translation>キュー</translation>
    </message>
    <message>
        <location filename="printerStream.cpp" line="156"/>
        <source>Page %1</source>
        <translation>ページ %1</translation>
    </message>
    <message>
        <location filename="utils.cpp" line="213"/>
        <source>Impossible to apply changes because object is located in read-only
part of the tee or data file was opened read-only</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="utils.cpp" line="236"/>
        <source>Object with name &apos;%1&apos; already exists, please choose different name.</source>
        <translation>オブジェクト名「%1」は既に存在します。別の名前を選択してください。</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="356"/>
        <source>Legend</source>
        <translation type="unfinished">凡例</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="434"/>
        <source>Objects</source>
        <translation type="unfinished">オブジェクト</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="522"/>
        <source>Groups</source>
        <translation type="unfinished">グループ</translation>
    </message>
    <message>
        <location filename="PrintingController.cpp" line="564"/>
        <source>EMPTY</source>
        <translation type="unfinished">空</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="616"/>
        <source>Source</source>
        <translation type="unfinished">ソース</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="630"/>
        <source>Destination</source>
        <translation type="unfinished">ディスティネーション</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="618"/>
        <source>Service</source>
        <translation type="unfinished">サービス</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="620"/>
        <source>Time</source>
        <translation type="unfinished">時間</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="622"/>
        <source>Original Src</source>
        <translation type="unfinished">オリジナルソース</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="623"/>
        <source>Original Dst</source>
        <translation type="unfinished">オリジナルディスティネーション</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="624"/>
        <source>Original Srv</source>
        <translation type="unfinished">オリジナルサービス</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="626"/>
        <source>Translated Src</source>
        <translation type="unfinished">転送元ソース</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="627"/>
        <source>Translated Dst</source>
        <translation type="unfinished">転送先</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="628"/>
        <source>Translated Srv</source>
        <translation type="unfinished">転送サービス</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="631"/>
        <source>Gateway</source>
        <translation type="unfinished">ゲートウェイ</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="636"/>
        <source>Direction</source>
        <translation type="unfinished">方向</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="637"/>
        <source>Action</source>
        <translation type="unfinished">操作</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="638"/>
        <source>Options</source>
        <translation type="unfinished">オプション</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="639"/>
        <source>Metric</source>
        <translation type="unfinished">サービス</translation>
    </message>
    <message>
        <location filename="platforms.cpp" line="640"/>
        <source>Comment</source>
        <translation type="unfinished">コメント</translation>
    </message>
</context>
<context>
    <name>RCS</name>
    <message>
        <location filename="RCS.cpp" line="786"/>
        <source>Error checking file out: %1</source>
        <translation>ファイル取り出しの確認中のエラー: %1</translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="787"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="665"/>
        <source>Error creating temporary file </source>
        <translation>一時ファイルの作成に失敗しました </translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="678"/>
        <source>Error writing to temporary file </source>
        <translation>一時ファイルの書き込みに失敗しました </translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="711"/>
        <source>File is opened and locked by %1.
You can only open it read-only.</source>
        <translation>ファイルは %1 により開かれ、ロックされています
読み込み専用でのみ開くことができます。</translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="725"/>
        <source>Revision %1 of this file has been checked out and locked by you earlier.
The file may be opened in another copy of Firewall Builder or was left opened
after the program crashed.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="726"/>
        <source>Open &amp;read-only</source>
        <translation>読み込み専用で開く(&amp;O)</translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="726"/>
        <source>&amp;Open and continue editing</source>
        <translation>読み込んで編集を続ける(&amp;O)</translation>
    </message>
    <message>
        <location filename="RCS.cpp" line="726"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>RCSFilePreview</name>
    <message>
        <location filename="RCSFilePreview.cpp" line="135"/>
        <source>File is not in RCS</source>
        <translation>ファイルは RCS 内にありません</translation>
    </message>
</context>
<context>
    <name>RCSFilePreview_q</name>
    <message>
        <location filename="rcsfilepreview_q.ui" line="25"/>
        <source>RCSFilePreview</source>
        <translation>RCS ファイルプレビュー</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="59"/>
        <source>Revision</source>
        <translation>リビジョン</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="64"/>
        <source>Date</source>
        <translation>日付</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="69"/>
        <source>Author</source>
        <translation>著者</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="74"/>
        <source>Locked by</source>
        <translation>ロックした人</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="106"/>
        <source>RCS log:</source>
        <translation>RCS ログ:</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="173"/>
        <source>Open</source>
        <translation>開く</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="186"/>
        <source>Open read-only</source>
        <translation>読み込み専用で開く</translation>
    </message>
    <message>
        <location filename="rcsfilepreview_q.ui" line="199"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
</context>
<context>
    <name>RCSFileSaveDialog_q</name>
    <message>
        <location filename="rcsfilesavedialog_q.ui" line="19"/>
        <source>Log record for the new revision</source>
        <translation>新規リビジョンのログ記録</translation>
    </message>
    <message>
        <location filename="rcsfilesavedialog_q.ui" line="28"/>
        <source>Do not ask me anymore, always check files in with empty log</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="rcsfilesavedialog_q.ui" line="59"/>
        <source>Check file &amp;in</source>
        <translation>ファイルのチェックイン(&amp;I)</translation>
    </message>
    <message>
        <location filename="rcsfilesavedialog_q.ui" line="62"/>
        <source>Alt+I</source>
        <translation>Alt+I</translation>
    </message>
    <message>
        <location filename="rcsfilesavedialog_q.ui" line="75"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="rcsfilesavedialog_q.ui" line="108"/>
        <source>Checking file %1 into RCS</source>
        <translation>RCS にファイル %1 をチェックインしています</translation>
    </message>
    <message>
        <location filename="rcsfilesavedialog_q.ui" line="126"/>
        <source>Log record for this revision: </source>
        <translation>このリビジョンのログ記録:</translation>
    </message>
</context>
<context>
    <name>RoutingRuleOptionsDialog_q</name>
    <message>
        <location filename="routingruleoptionsdialog_q.ui" line="13"/>
        <source>Routing Rule Options</source>
        <translation>ルーティングのルールオプション</translation>
    </message>
    <message>
        <location filename="routingruleoptionsdialog_q.ui" line="99"/>
        <source>fw/rule num</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="routingruleoptionsdialog_q.ui" line="173"/>
        <source>If installation of this routing rule fails, just carry on</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="routingruleoptionsdialog_q.ui" line="248"/>
        <source>No options available for routing rules of this firewall platform</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>RuleOptionsDialog_q</name>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="13"/>
        <source>Rule Options for ipt</source>
        <translation>ipt 用ルールオプション</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="61"/>
        <source>fw/rule num</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1237"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="162"/>
        <source>Assume firewall is part of &apos;any&apos; (this setting only affects code generated for this rule)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1816"/>
        <source>Stateless rule</source>
        <translation>ステートレスルール</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1829"/>
        <source>Normally policy compiler uses stateful inspection in each rule. Activating next option makes this rule stateless.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1296"/>
        <source>Logging</source>
        <translation>ログ記録</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="252"/>
        <source>alert</source>
        <translation>alert</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="257"/>
        <source>crit</source>
        <translation>crit</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="262"/>
        <source>error</source>
        <translation>error</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="267"/>
        <source>warning</source>
        <translation>warning</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="272"/>
        <source>notice</source>
        <translation>notice</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="277"/>
        <source>info</source>
        <translation>info</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="282"/>
        <source>debug</source>
        <translation>debug</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1311"/>
        <source>Log prefix:</source>
        <translation>ログ接頭語:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1930"/>
        <source>Log level:</source>
        <translation>ログレベル:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="345"/>
        <source>Netlink group (if using ULOG): </source>
        <translation>ネットリンクグループ (ULOG を使う場合):</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="382"/>
        <source>limit</source>
        <translation>制限</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="394"/>
        <source>Rate (rule matches if it hits this often or less):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="404"/>
        <source>Module limit</source>
        <translation>モジュール制限</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="769"/>
        <source>Burst:</source>
        <translation>バースト:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="746"/>
        <source>/day</source>
        <translation>/日</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="751"/>
        <source>/hour</source>
        <translation>/時</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="756"/>
        <source>/minute</source>
        <translation>/分</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="761"/>
        <source>/second</source>
        <translation>/秒</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="517"/>
        <source>connlimit</source>
        <translation>接続制限</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="535"/>
        <source>bit</source>
        <translation>ビット</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="580"/>
        <source>per network with netmask of </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="619"/>
        <source>Number of allowed connections per client host</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="629"/>
        <source>Module connlimit</source>
        <translation>モジュールの接続制限</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="656"/>
        <source>hashlimit</source>
        <translation>ハッシュ制限</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="668"/>
        <source>Module hashlimit</source>
        <translation>モジュールのハッシュ制限</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="678"/>
        <source>On some older systems this module has name &apos;dstlimit&apos;. Check here if you need to use this name.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="685"/>
        <source>Rate:</source>
        <translation>日付:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="695"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="779"/>
        <source>Mode:</source>
        <translation>モード:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="802"/>
        <source>htable-size:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="812"/>
        <source>The number of buckets of the hash table (omit this option in generated script if set to 0)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="822"/>
        <source>htable-max:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="832"/>
        <source>Maximum number of entries in the hash (omit this option in generated script if set to 0)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="842"/>
        <source>htable-expire:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="852"/>
        <source>After how many milliseconds do hash entries expire (omit this option in the generated script if set to 0)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="862"/>
        <source>htable-gcinterval:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="872"/>
        <source>How many milliseconds between garbage collection intervals (omit this option in generated script if set to 0)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="882"/>
        <source>Options below control size of the hash table and expiration time. They will be omitted from the generated script if set to zero.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="938"/>
        <source>srcip</source>
        <translation>接続元 IP</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="945"/>
        <source>dstip</source>
        <translation>接続先 IP</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="952"/>
        <source>srcport</source>
        <translation>接続元ポート</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="959"/>
        <source>dstport</source>
        <translation>接続先ポート</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1073"/>
        <source>Send ICMP &apos;unreachable&apos; packet masquerading as being from the original destination</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1080"/>
        <source>Keep information on fragmented packets, to be applied to later fragments</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1100"/>
        <source>Log facility:</source>
        <translation>ログファシリティ:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1275"/>
        <source>In PF 4.x &quot;flags S/SA keep state&quot; is the default. Compiler will omit these flags while generating code for stateful rules matching tcp services. However, according to the PF FAQ, care should be taken while dealing with states and interface enc0. To avoid leaking unencrypted traffic out, the FAQ recommends setting &apos;keep state&apos; explicitly in all rules on the enc0 interface. This option applies only if version is set to 4.x.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1288"/>
        <source>Add &apos;keep state&apos; </source>
        <translation>「状態維持」を追加する</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1354"/>
        <source>Tracking</source>
        <translation>追跡</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1366"/>
        <source>When this option is checked, the number of states per source IP is tracked </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1369"/>
        <source>Activate source tracking</source>
        <translation>ソース追跡を有効にする</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1430"/>
        <source>Maximum number of source addresses which can simultaneously have state table entries (max-src-nodes):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1455"/>
        <source>Maximum number of simultaneous state entries that a single source address can create with this rule (max-src-states):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1501"/>
        <source>Limits</source>
        <translation>制限</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1513"/>
        <source>Maximum number of concurrent states this rule may create. Unlimited if set to zero  (option &apos;max&apos;).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1526"/>
        <source>When this limit is reached, further packets matching the rule that would create state are dropped, until existing states time out.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1552"/>
        <source>Maximum number of simultaneous TCP connections that a single host can make (max-src-conn):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1594"/>
        <source>The limit of new connections over a time interval (max-src-conn-rate):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1626"/>
        <source>/</source>
        <translation>/</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1655"/>
        <source>sec</source>
        <translation>秒</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1671"/>
        <source>overload table:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1691"/>
        <source>flush</source>
        <translation>flush</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1698"/>
        <source>global</source>
        <translation>global</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1738"/>
        <source>TCP</source>
        <translation>TCP</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1744"/>
        <source>Modulate state</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1751"/>
        <source>synproxy</source>
        <translation>synproxy</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1886"/>
        <source>These options are only valid for PIX running software v6.3 or later</source>
        <translation>これらのオプションは実行中の PIX ソフトウェアバージョン 6.3 以上で有効です</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1915"/>
        <source>completely disable logging for this rule</source>
        <translation>このルールのログ記の無効化を完了しました</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="1969"/>
        <source>Logging interval:</source>
        <translation>ログ記録間隔:</translation>
    </message>
    <message>
        <location filename="ruleoptionsdialog_q.ui" line="2049"/>
        <source>There are no options for this firewall platform</source>
        <translation>このファイアウォールプラットフォームでそのオプションはありません</translation>
    </message>
</context>
<context>
    <name>RuleSetDialog</name>
    <message>
        <location filename="RuleSetDialog.cpp" line="176"/>
        <source>Rule set name &apos;%1&apos; is invalid. Only &apos;[a-z][A-Z][0-9]_-+=@%^&apos; characters are allowed.</source>
        <translation>ルールセット名「%1」は正しくありません。英数字と一部の記号(「_」「-」「+」「=」「@」「%」のみ許可しています。</translation>
    </message>
    <message>
        <location filename="RuleSetDialog.cpp" line="177"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>RuleSetDialog_q</name>
    <message>
        <location filename="RuleSetDialog.cpp" line="110"/>
        <source>On iptables &quot;top&quot; rule set goes into 
the built-in chains INPUT, OUTPUT,
FORWARD; if this flag is unchecked,
rules go into user-defined chain 
with the name the same as the name of 
the rule set.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="RuleSetDialog.cpp" line="139"/>
        <source>If this flag is unchecked, rules go 
into anchor with the name the same as
the name of the rule set.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="RuleSetDialog.cpp" line="150"/>
        <source>If this flag is unchecked, generated
access list will not be assigned to
interfaces with &quot;ip access-group&quot;
command. The name of the rule set will
be used as a prefix for names of
access access lists generated for it.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="16"/>
        <source>Ruleset</source>
        <translation>ルールセット</translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="94"/>
        <source>Rule set</source>
        <translation>ルールセット</translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="128"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="164"/>
        <source>This is IPv4 rule set</source>
        <translation>これは IPv4 ルールセットです</translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="169"/>
        <source>This is IPv6 rule set</source>
        <translation>これは IPv6 ルールセットです</translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="206"/>
        <source>filter+mangle table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="213"/>
        <source>mangle table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="223"/>
        <source>Top ruleset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="252"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="rulesetdialog_q.ui" line="174"/>
        <source>This is combined IPv4 and IPv6 rule set</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>RuleSetView</name>
    <message>
        <location filename="RuleSetView.cpp" line="1412"/>
        <source>Outbound </source>
        <translation>外向き</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="1499"/>
        <source>Original</source>
        <translation>オリジナル</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="1500"/>
        <source>Default</source>
        <translation>標準</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="1503"/>
        <source>All</source>
        <translation>すべて</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="1512"/>
        <source>Any</source>
        <translation>どれか</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="2847"/>
        <source>Enter group name:</source>
        <translation>グループ名の入力:</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3450"/>
        <source>Insert Rule</source>
        <translation>ルール挿入</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3073"/>
        <source>Paste Rule</source>
        <translation>ルール貼り付け</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3098"/>
        <source>New group</source>
        <translation>新規グループ</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3111"/>
        <source>Add to the group </source>
        <translation>グループに追加する</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3119"/>
        <source>Remove from the group</source>
        <translation>グループから削除する</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3127"/>
        <source>Insert Rule Above</source>
        <translation>上にルールの挿入</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3130"/>
        <source>Rename group</source>
        <translation>グループ名を変更する</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3404"/>
        <source>Change color</source>
        <translation>色変更</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3408"/>
        <source>No color</source>
        <translation>色なし</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Red</source>
        <translation type="obsolete">赤色</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Orange</source>
        <translation type="obsolete">オレンジ色</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Yellow</source>
        <translation type="obsolete">黄色</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Green</source>
        <translation type="obsolete">緑色</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Blue</source>
        <translation type="obsolete">青色</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Purple</source>
        <translation type="obsolete">紫色</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Gray</source>
        <translation type="obsolete">灰色</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3283"/>
        <source>Parameters</source>
        <translation>パラメーター</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3299"/>
        <source>Inbound</source>
        <translation>内向き</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3302"/>
        <source>Outbound</source>
        <translation>外向き</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3305"/>
        <source>Both</source>
        <translation>両方</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3313"/>
        <source>Rule Options</source>
        <translation>ルールオプション</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3320"/>
        <source>Logging On</source>
        <translation>記録する</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3323"/>
        <source>Logging Off</source>
        <translation>記録しない</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3502"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3338"/>
        <source>Copy</source>
        <translation>コピー</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3340"/>
        <source>Cut</source>
        <translation>切り取り</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3341"/>
        <source>Paste</source>
        <translation>貼り付け</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3344"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3347"/>
        <source>Where used</source>
        <translation>使うとき</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3349"/>
        <source>Reveal in tree</source>
        <translation type="unfinished">ツリーから検索</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3352"/>
        <source>Negate</source>
        <translation>否定</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3399"/>
        <source>Rules %1-%2</source>
        <translation>ルール %1-%2</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3402"/>
        <source>Rule %1</source>
        <translation>ルール %1</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3452"/>
        <source>Add Rule Below</source>
        <translation>下にルール追加</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3455"/>
        <source>Remove Rule</source>
        <translation>ルールの削除</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3456"/>
        <source>Remove Rules</source>
        <translation>複数ルールの削除</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3459"/>
        <source>Move Rule</source>
        <translation>ルール移動</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3460"/>
        <source>Move Rules</source>
        <translation>複数ルール移動</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3466"/>
        <source>Copy Rule</source>
        <translation>ルールのコピー</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3468"/>
        <source>Cut Rule</source>
        <translation>ルールの切り取り</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3470"/>
        <source>Paste Rule Above</source>
        <translation>上にルールを貼り付け</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3472"/>
        <source>Paste Rule Below</source>
        <translation>下にルールを貼り付け</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3479"/>
        <source>Enable Rule</source>
        <translation>ルール有効</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3480"/>
        <source>Enable Rules</source>
        <translation>ルール有効</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3484"/>
        <source>Disable Rule</source>
        <translation>ルール無効</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="3485"/>
        <source>Disable Rules</source>
        <translation>ルール無効</translation>
    </message>
    <message>
        <location filename="RuleSetView.cpp" line="2797"/>
        <source></source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>SSHPIX</name>
    <message>
        <location filename="SSHPIX.cpp" line="140"/>
        <source>
*** Fatal error :</source>
        <translation>
*** 致命的なエラー :</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="174"/>
        <source>Logged in</source>
        <translation>ログイン</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="175"/>
        <source>Switching to enable mode...</source>
        <translation>有効モードに切り替え中...
</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="209"/>
        <source>New RSA key</source>
        <translation>新規 RSA 鍵</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="210"/>
        <source>Yes</source>
        <translation>はい</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="210"/>
        <source>No</source>
        <translation>いいえ</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="256"/>
        <source>In enable mode.</source>
        <translation>有効モード中です。</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="814"/>
        <source>Pushing firewall configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="435"/>
        <source>Rule %1</source>
        <translation>ルール %1</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="461"/>
        <source>End</source>
        <translation>終了</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="544"/>
        <source>Making backup copy of the firewall configuration</source>
        <translation>ファイアウォール設定のバックアップコピーを作成しています</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="617"/>
        <source>*** Clearing unused access lists</source>
        <translation>*** 未使用アクセス一覧の整理中</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="684"/>
        <source>*** Clearing unused object groups</source>
        <translation>*** 未使用オブジェクトグループの整理中</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="706"/>
        <source>*** End </source>
        <translation>*** 終了</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="717"/>
        <source>Reading current firewall configuration</source>
        <translation>現在のファイアウォール設定を読み込んでいます</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="746"/>
        <source>Generating configuration diff</source>
        <translation>設定の差分を生成しています</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="763"/>
        <source>Fork failed for %1</source>
        <translation>%1 のフォークに失敗しました</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="769"/>
        <source>Not enough memory.</source>
        <translation>メモリーがありません。</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="774"/>
        <source>Too many opened file descriptors in the system.</source>
        <translation>システムでファイルディスクリプタを開きすぎています。</translation>
    </message>
    <message>
        <location filename="SSHPIX.cpp" line="800"/>
        <source>Empty configuration diff</source>
        <translation>設定の差分がありません</translation>
    </message>
</context>
<context>
    <name>SSHSession</name>
    <message>
        <location filename="SSHSession.cpp" line="91"/>
        <source>You are connecting to the firewall &lt;b&gt;&apos;%1&apos;&lt;/b&gt; for the first time. It has provided you its identification in a form of its host public key. The fingerprint of the host public key is: &quot;%2&quot; You can save the host key to the local database by pressing YES, or you can cancel connection by pressing NO. You should press YES only if you are sure you are really connected to the firewall &lt;b&gt;&apos;%3&apos;&lt;/b&gt;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="SSHSession.cpp" line="204"/>
        <source>Failed to start ssh</source>
        <translation>ssh の開始に失敗しました</translation>
    </message>
    <message>
        <location filename="SSHSession.cpp" line="508"/>
        <source>SSH session terminated, exit status: %1</source>
        <translation>SSH 接続が切断しました。終了ステータス: %1</translation>
    </message>
</context>
<context>
    <name>SSHUnx</name>
    <message>
        <location filename="SSHUnx.cpp" line="101"/>
        <source>
*** Fatal error :</source>
        <translation>
*** 致命的なエラー :</translation>
    </message>
    <message>
        <location filename="SSHUnx.cpp" line="166"/>
        <source>Logged in</source>
        <translation>ログイン</translation>
    </message>
    <message>
        <location filename="SSHUnx.cpp" line="190"/>
        <source>New RSA key</source>
        <translation>新規 RSA 鍵</translation>
    </message>
    <message>
        <location filename="SSHUnx.cpp" line="191"/>
        <source>Yes</source>
        <translation>はい</translation>
    </message>
    <message>
        <location filename="SSHUnx.cpp" line="191"/>
        <source>No</source>
        <translation>いいえ</translation>
    </message>
    <message>
        <location filename="SSHUnx.cpp" line="251"/>
        <source>Done</source>
        <translation>終了しました</translation>
    </message>
    <message>
        <location filename="SSHUnx.cpp" line="263"/>
        <source>Error in SSH</source>
        <translation>SSH のエラー</translation>
    </message>
</context>
<context>
    <name>SimpleIntEditor_q</name>
    <message>
        <location filename="simpleinteditor_q.ui" line="16"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
    <message>
        <location filename="simpleinteditor_q.ui" line="69"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="simpleinteditor_q.ui" line="95"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
</context>
<context>
    <name>SimpleTextEditor</name>
    <message>
        <location filename="SimpleTextEditor.cpp" line="74"/>
        <source>Warning: loading from file discards current contents of the script.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="SimpleTextEditor.cpp" line="77"/>
        <source>Choose file</source>
        <translation>ファイルを選ぶ</translation>
    </message>
    <message>
        <location filename="SimpleTextEditor.cpp" line="87"/>
        <source>Could not open file %1</source>
        <translation>ファイル %1 を開くことが出来ません</translation>
    </message>
</context>
<context>
    <name>SimpleTextEditor_q</name>
    <message>
        <location filename="simpletexteditor_q.ui" line="19"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
    <message>
        <location filename="simpletexteditor_q.ui" line="63"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="simpletexteditor_q.ui" line="73"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="simpletexteditor_q.ui" line="80"/>
        <source>Import from file ...</source>
        <translation>ファイルからインポート ...</translation>
    </message>
</context>
<context>
    <name>SimpleTextView_q</name>
    <message>
        <location filename="simpletextview_q.ui" line="16"/>
        <source>Text viewer</source>
        <translation>テキストビュアー</translation>
    </message>
    <message>
        <location filename="simpletextview_q.ui" line="40"/>
        <source>Object Name</source>
        <translation>オブジェクト名</translation>
    </message>
    <message>
        <location filename="simpletextview_q.ui" line="91"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
</context>
<context>
    <name>StartTipDialog_q</name>
    <message>
        <location filename="starttipdialog_q.ui" line="13"/>
        <source>Dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="starttipdialog_q.ui" line="60"/>
        <source>Do not show this again</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="starttipdialog_q.ui" line="80"/>
        <source>Close</source>
        <translation type="unfinished">閉じる</translation>
    </message>
</context>
<context>
    <name>TCPServiceDialog_q</name>
    <message>
        <location filename="tcpservicedialog_q.ui" line="22"/>
        <source>TCP</source>
        <translation>TCP</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="96"/>
        <source>TCP Service</source>
        <translation>TCP サービス</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="173"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="259"/>
        <source>U</source>
        <translation>U</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="269"/>
        <source>TCP flags that must be set (see man iptables, option --tcp-flags)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="272"/>
        <source>Settings:</source>
        <translation>設定:</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="340"/>
        <source>R</source>
        <translation>R</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="356"/>
        <source>P</source>
        <translation>P</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="392"/>
        <source>TCP flags that should be examined (see man iptables, option --tcp-flags)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="395"/>
        <source>Mask:</source>
        <translation>マスク:</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="437"/>
        <source>Flags:</source>
        <translation>フラグ:</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="466"/>
        <source>A</source>
        <translation>A</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="482"/>
        <source>S</source>
        <translation>S</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="511"/>
        <source>F</source>
        <translation>F</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="529"/>
        <source>Option &quot;established&quot; can be used if supported
by the target firewall platform</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="533"/>
        <source>Established</source>
        <translation>確立済み</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="615"/>
        <source>Source Port Range</source>
        <translation>送信元ポート範囲</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="752"/>
        <source>Start:</source>
        <translation>開始:</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="765"/>
        <source>End:</source>
        <translation>終了:</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="708"/>
        <source>Destination Port Range</source>
        <translation>送信先ポート範囲</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="790"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="205"/>
        <source>QGroupBox {
    margin-top: 0px;
<byte value="x9"/>padding-top: 1ex;
    padding-bottom: 0px;
}
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="tcpservicedialog_q.ui" line="688"/>
        <source>QGroupBox {
<byte value="x9"/>margin-top: 1ex;
<byte value="x9"/>padding-top: 1ex;
<byte value="x9"/>padding-bottom: 0px;
<byte value="x9"/>padding-left: 2px;
}

QGroupBox::title {  
<byte value="x9"/>subcontrol-origin: margin;
<byte value="x9"/>subcontrol-position: top left;
<byte value="x9"/>padding: 0 3px;
}

QSpinBox {
<byte value="x9"/>margin: 0px;
<byte value="x9"/>padding: 0px;
}
</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>TagServiceDialog_q</name>
    <message>
        <location filename="tagservicedialog_q.ui" line="13"/>
        <source>Form1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="tagservicedialog_q.ui" line="69"/>
        <source>Tag Service</source>
        <translation>タグサービス</translation>
    </message>
    <message>
        <location filename="tagservicedialog_q.ui" line="116"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="tagservicedialog_q.ui" line="142"/>
        <source>Code:</source>
        <translation>コード:</translation>
    </message>
    <message>
        <location filename="tagservicedialog_q.ui" line="187"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>TimeDialog_q</name>
    <message>
        <location filename="timedialog_q.ui" line="16"/>
        <source>Time</source>
        <translation>時間</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="85"/>
        <source>Time Interval</source>
        <translation>時間間隔</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="116"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="189"/>
        <source>Start date:</source>
        <translation>開始日:</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="282"/>
        <source>M/d/yyyy</source>
        <translation></translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="226"/>
        <source>Start time:</source>
        <translation>開始時間:</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="275"/>
        <source>End date:  </source>
        <translation>終了日:</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="312"/>
        <source>End time:</source>
        <translation>終了時間:</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="374"/>
        <source>Mon</source>
        <translation>月</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="381"/>
        <source>Tue</source>
        <translation>火</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="388"/>
        <source>Wed</source>
        <translation>水</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="395"/>
        <source>Thu</source>
        <translation>木</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="402"/>
        <source>Fri</source>
        <translation>金</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="409"/>
        <source>Sat</source>
        <translation>土</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="416"/>
        <source>Sun</source>
        <translation>日</translation>
    </message>
    <message>
        <location filename="timedialog_q.ui" line="438"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>UDPServiceDialog_q</name>
    <message>
        <location filename="udpservicedialog_q.ui" line="22"/>
        <source>UDP</source>
        <translation>UDP</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="99"/>
        <source>UDP Service</source>
        <translation>UDP サービス</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="186"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="268"/>
        <source>Source Port Range</source>
        <translation>送信元ポート幅</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="385"/>
        <source>Start:</source>
        <translation>開始:</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="408"/>
        <source>End:</source>
        <translation>終了:</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="361"/>
        <source>Destination Port Range</source>
        <translation>送信先ポート幅</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="443"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
    <message>
        <location filename="udpservicedialog_q.ui" line="341"/>
        <source>QGroupBox {
<byte value="x9"/>margin-top: 1ex;
<byte value="x9"/>padding-top: 1ex;
<byte value="x9"/>padding-bottom: 0px;
<byte value="x9"/>padding-left: 2px;
}

QGroupBox::title {  
<byte value="x9"/>subcontrol-origin: margin;
<byte value="x9"/>subcontrol-position: top left;
<byte value="x9"/>padding: 0 3px;
}

QSpinBox {
<byte value="x9"/>margin: 0px;
<byte value="x9"/>padding: 0px;
}
</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>UserDialog_q</name>
    <message>
        <location filename="userdialog_q.ui" line="90"/>
        <source>User</source>
        <translation>ユーザー</translation>
    </message>
    <message>
        <location filename="userdialog_q.ui" line="143"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="userdialog_q.ui" line="169"/>
        <source>User id:</source>
        <translation>ユーザー ID:</translation>
    </message>
    <message>
        <location filename="userdialog_q.ui" line="214"/>
        <source>Comment:</source>
        <translation>コメント:</translation>
    </message>
</context>
<context>
    <name>askRuleNumberDialog_q</name>
    <message>
        <location filename="askrulenumberdialog_q.ui" line="13"/>
        <source>Enter New Position For The Rule</source>
        <translation>ルールの&lt;新しい位置を入力する</translation>
    </message>
    <message>
        <location filename="askrulenumberdialog_q.ui" line="19"/>
        <source>Enter new position for selected rules:</source>
        <translation>選択されたルール群の新しい位置を入力する:</translation>
    </message>
    <message>
        <location filename="askrulenumberdialog_q.ui" line="86"/>
        <source>&amp;Move</source>
        <translation>移動(&amp;M)</translation>
    </message>
    <message>
        <location filename="askrulenumberdialog_q.ui" line="89"/>
        <source>Alt+M</source>
        <translation>Alt+M</translation>
    </message>
    <message>
        <location filename="askrulenumberdialog_q.ui" line="102"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="askrulenumberdialog_q.ui" line="105"/>
        <source>Alt+C</source>
        <translation>Alt+C</translation>
    </message>
</context>
<context>
    <name>asklibforcopydialog_q</name>
    <message>
        <location filename="asklibforcopydialog_q.ui" line="13"/>
        <source>Copying</source>
        <translation>コピーしています</translation>
    </message>
    <message>
        <location filename="asklibforcopydialog_q.ui" line="19"/>
        <source>Object will be copied to library:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>colorLabelMenuItem_q</name>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="40"/>
        <source>Orange</source>
        <translation>オレンジ色</translation>
    </message>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="59"/>
        <source>Green</source>
        <translation>緑色</translation>
    </message>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="78"/>
        <source>Purple</source>
        <translation>紫色</translation>
    </message>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="97"/>
        <source>Blue</source>
        <translation>青色</translation>
    </message>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="116"/>
        <source>Yellow</source>
        <translation>黄色</translation>
    </message>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="135"/>
        <source>Gray</source>
        <translation>灰色</translation>
    </message>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="154"/>
        <source>Red</source>
        <translation>赤色</translation>
    </message>
    <message>
        <location filename="colorlabelmenuitem_q.ui" line="173"/>
        <source>No color</source>
        <translation>色なし</translation>
    </message>
</context>
<context>
    <name>debugDialog_q</name>
    <message>
        <location filename="debugdialog_q.ui" line="16"/>
        <source>Debugging Info</source>
        <translation>デバッグ情報</translation>
    </message>
    <message>
        <location filename="debugdialog_q.ui" line="46"/>
        <source>&amp;Close</source>
        <translation>閉じる(&amp;C)</translation>
    </message>
    <message>
        <location filename="debugdialog_q.ui" line="49"/>
        <source>Alt+C</source>
        <translation>Alt+C</translation>
    </message>
</context>
<context>
    <name>filePropDialog</name>
    <message>
        <location filename="filePropDialog.cpp" line="70"/>
        <source>Opened read-only</source>
        <translation>読み込み専用で開いています</translation>
    </message>
    <message>
        <location filename="filePropDialog.cpp" line="88"/>
        <source>Revision %1</source>
        <translation>リビジョン %1</translation>
    </message>
</context>
<context>
    <name>filePropDialog_q</name>
    <message>
        <location filename="filepropdialog_q.ui" line="16"/>
        <source>File Properties</source>
        <translation>ファイルのプロパティ</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="28"/>
        <source>Location:</source>
        <translation>場所:</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="44"/>
        <source>location</source>
        <translation>場所</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="54"/>
        <source>RO</source>
        <translation>読み込み専用</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="64"/>
        <source>Time of last modification:</source>
        <translation>最終修正の時間:</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="74"/>
        <source>lastModified</source>
        <translation>最終更新日時</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="97"/>
        <source>Revision Control:</source>
        <translation>リビジョン制御:</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="107"/>
        <source>Revision:</source>
        <translation>リビジョン:</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="123"/>
        <source>rev</source>
        <translation>リビジョン</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="133"/>
        <source>Locked by user:</source>
        <translation>ユーザーがロック中:</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="149"/>
        <source>lockedBy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="159"/>
        <source>Revision history:</source>
        <translation>リビジョン履歴:</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="184"/>
        <source>Print</source>
        <translation>印刷</translation>
    </message>
    <message>
        <location filename="filepropdialog_q.ui" line="210"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
</context>
<context>
    <name>findDialog</name>
    <message>
        <location filename="findDialog.cpp" line="271"/>
        <source>Search hit the end of the object tree.</source>
        <translation type="unfinished">検索のヒットがオブジェクトツリーの最後です。</translation>
    </message>
    <message>
        <location filename="findDialog.cpp" line="272"/>
        <source>&amp;Continue at top</source>
        <translation type="unfinished">続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="findDialog.cpp" line="272"/>
        <source>&amp;Stop</source>
        <translation>停止(&amp;S)</translation>
    </message>
</context>
<context>
    <name>findDialog_q</name>
    <message>
        <location filename="finddialog_q.ui" line="22"/>
        <source>Find Object</source>
        <translation>オブジェクト検索</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="41"/>
        <source>Text to be found in object names:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="51"/>
        <source>Search in policy rules</source>
        <translation>ポリシールールから検索</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="58"/>
        <source>Search in the tree</source>
        <translation>ツリーから検索</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="84"/>
        <source>Find</source>
        <translation>検索</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="111"/>
        <source>Matching attribute:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="122"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="127"/>
        <source>TCP/UDP port</source>
        <translation>TCP/UDP ポート</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="132"/>
        <source>Protocol number</source>
        <translation>プロトコル番号</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="137"/>
        <source>ICMP type</source>
        <translation>ICMP の種類</translation>
    </message>
    <message>
        <location filename="finddialog_q.ui" line="145"/>
        <source>Search for substring using regular expressions</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>findObjectWidget_q</name>
    <message>
        <location filename="findobjectwidget_q.ui" line="16"/>
        <source>Form1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="44"/>
        <source>Replace &amp;&amp; Find</source>
        <translation>置換と検索</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="67"/>
        <source> Replace object </source>
        <translation>オブジェクトの置換</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="113"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="165"/>
        <source>Scope for search and replace :</source>
        <translation>検索と置換のスコープ :</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="178"/>
        <source>Tree only</source>
        <translation>ツリーのみ</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="183"/>
        <source>Tree and policy of all firewalls</source>
        <translation>すべてのファイアウォールのポリシーとツリー</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="188"/>
        <source>Policy of all firewalls</source>
        <translation>すべてのファイアウォールのポリシー</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="193"/>
        <source>policy of the opened firewall</source>
        <translation>開いているファイアウォールのポリシー</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="220"/>
        <source>Replace</source>
        <translation>置換する</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="227"/>
        <source>Replace all</source>
        <translation>すべて置換する</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="234"/>
        <source>Next</source>
        <translation>次へ</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="257"/>
        <source> Find object</source>
        <translation>オブジェクト検索</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="298"/>
        <source>Name</source>
        <translation>名前</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="303"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="308"/>
        <source>TCP/UDP port</source>
        <translation>TCP/UDP ポート</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="313"/>
        <source>Protocol number</source>
        <translation>プロトコル番号</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="318"/>
        <source>ICMP type</source>
        <translation>ICMP の種類</translation>
    </message>
    <message>
        <location filename="findobjectwidget_q.ui" line="338"/>
        <source>Use regular expressions</source>
        <translation>正規表現を使う</translation>
    </message>
</context>
<context>
    <name>findWhereUsedWidget_q</name>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="19"/>
        <source>Form1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="28"/>
        <source>Object:</source>
        <translation>オブジェクト:</translation>
    </message>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="81"/>
        <source>Object is found in :</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Parent Object</source>
        <translation type="obsolete">親オブジェクト</translation>
    </message>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="104"/>
        <source>Details</source>
        <translation>詳細</translation>
    </message>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="144"/>
        <source>Find</source>
        <translation>検索</translation>
    </message>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="151"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="94"/>
        <source>Object</source>
        <translation type="unfinished">オブジェクト</translation>
    </message>
    <message>
        <location filename="findwhereusedwidget_q.ui" line="99"/>
        <source>Parent</source>
        <translation type="unfinished">親</translation>
    </message>
</context>
<context>
    <name>freebsdAdvancedDialog_q</name>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="13"/>
        <source>FreeBSD: advanced settings</source>
        <translation>FreeBSD: 高度な設定</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="70"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="86"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="108"/>
        <source>Options</source>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="178"/>
        <source>Packet forwarding</source>
        <translation>パケット転送</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="256"/>
        <source>No change</source>
        <translation>変更なし</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="261"/>
        <source>On</source>
        <translation>オン</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="266"/>
        <source>Off</source>
        <translation>オフ</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="210"/>
        <source>Forward source routed packets</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="242"/>
        <source>Generate ICMP redirects</source>
        <translation>ICMP リダイレクト生成</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="307"/>
        <source>Path</source>
        <translation>パス</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="337"/>
        <source>Specify directory path and a file name for the following utilities on the OS your firewall machine is running. Leave these empty if you want to use default values.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="382"/>
        <source>ipnat:</source>
        <translation>ipnat:</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="395"/>
        <source>sysctl:</source>
        <translation>sysctl:</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="418"/>
        <source>ipf:</source>
        <translation>ipf:</translation>
    </message>
    <message>
        <location filename="freebsdadvanceddialog_q.ui" line="441"/>
        <source>ipfw:</source>
        <translation>ipfw:</translation>
    </message>
</context>
<context>
    <name>instBatchOptionsDialog</name>
    <message>
        <location filename="instBatchOptionsDialog.cpp" line="48"/>
        <source>Batch install options</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>instDialog</name>
    <message>
        <location filename="instDialog.cpp" line="135"/>
        <source>There are no firewalls to process.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog.cpp" line="156"/>
        <source>&lt;p align=&quot;center&quot;&gt;&lt;b&gt;&lt;font size=&quot;+2&quot;&gt;Select firewalls for compilation.&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog.cpp" line="170"/>
        <source>Unknown operation.</source>
        <translation>不明な操作です。</translation>
    </message>
    <message>
        <location filename="instDialog.cpp" line="414"/>
        <source>Policy installer uses Secure Shell to communicate with the firewall.
Please configure directory path to the secure shell utility 
installed on your machine using Preferences dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_compile.cpp" line="180"/>
        <source>&amp;Continue</source>
        <translation>続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="instDialog_compile.cpp" line="72"/>
        <source>Compiling rule sets for firewall: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instDialog_compile.cpp" line="85"/>
        <source>Compiling ...</source>
        <translation>コンパイル中...</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="81"/>
        <source>Stop</source>
        <translation>停止</translation>
    </message>
    <message>
        <location filename="instDialog_compile.cpp" line="124"/>
        <source>Error: Failed to start program</source>
        <translation>エラー: プログラムの開始に失敗しました</translation>
    </message>
    <message>
        <location filename="instDialog_compile.cpp" line="179"/>
        <source>Firewall platform is not specified in this object.
Can&apos;t compile firewall policy.</source>
        <translation>ファイアウォールプラットフォームがこのオブジェクトで指定されていません。
ファイアウォールポリシーをコンパイルできません。</translation>
    </message>
    <message>
        <location filename="instDialog_installer.cpp" line="92"/>
        <source>Install firewall: </source>
        <translation>ファイアウォールインストール:</translation>
    </message>
    <message>
        <location filename="instDialog_installer.cpp" line="105"/>
        <source>Installing ...</source>
        <translation>インストールしています...</translation>
    </message>
    <message>
        <location filename="instDialog_installer.cpp" line="192"/>
        <source>Failure</source>
        <translation>失敗</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="151"/>
        <source>Success</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="158"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="165"/>
        <source>Cancelled</source>
        <translation>取り消しました</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="186"/>
        <source>Show details</source>
        <translation>詳細を表示する</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="192"/>
        <source>Hide details</source>
        <translation>詳細を隠す</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="373"/>
        <source>Running command &apos;%1&apos;
</source>
        <translation>コマンド &apos;%1&apos; を実行しています
</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="774"/>
        <source>Show selected</source>
        <translation>選択の表示</translation>
    </message>
    <message>
        <location filename="instDialog_ui_ops.cpp" line="780"/>
        <source>Show all</source>
        <translation>すべて表示する</translation>
    </message>
</context>
<context>
    <name>instDialog_q</name>
    <message>
        <location filename="instdialog_q.ui" line="46"/>
        <source>TextLabel</source>
        <translation>テキストラベル</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="104"/>
        <source>&lt;p align=&quot;center&quot;&gt;&lt;b&gt;&lt;font size=&quot;+2&quot;&gt;Select firewalls to compile and install.&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="141"/>
        <source>Perform batch install</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="148"/>
        <source>Check this option if you want to install all selected firewalls automatically. This only works if you use the same user name and password to authenticate to all these firewalls. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="573"/>
        <source>Compile</source>
        <translation>コンパイル</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="578"/>
        <source>Install</source>
        <translation>インストール</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="568"/>
        <source>Firewall</source>
        <translation>ファイアウォール</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="210"/>
        <source>Library</source>
        <translation>ライブラリー</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="215"/>
        <source>Last Modified</source>
        <translation>最終更新日時</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="220"/>
        <source>Last Compiled</source>
        <translation>最終コンパイル日時</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="225"/>
        <source>Last Installed</source>
        <translation>最終インストール日時</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="245"/>
        <source>All</source>
        <translation>すべて</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="252"/>
        <source>None</source>
        <translation>なし</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="275"/>
        <source>Show selected</source>
        <translation>選択の表示</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="306"/>
        <source>Stop</source>
        <translation>停止</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="338"/>
        <source>Progress</source>
        <translation>処理中</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="369"/>
        <source>Firewalls:</source>
        <translation>ファイアウォール:</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="391"/>
        <source>firewall</source>
        <translation>ファイアウォール</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="424"/>
        <source>current operation here</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="461"/>
        <source>Show Details</source>
        <translation>詳細を表示する</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="491"/>
        <source>Process log</source>
        <translation>処理ログ</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="516"/>
        <source>Save log to file</source>
        <translation>ファイルにログを保存する</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="624"/>
        <source>&lt; &amp;Back</source>
        <translation>&lt; 戻る(&amp;B)</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="631"/>
        <source>&amp;Next &gt;</source>
        <translation>次へ(&amp;N) &gt;</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="641"/>
        <source>&amp;Finish</source>
        <translation>終了(&amp;F)</translation>
    </message>
    <message>
        <location filename="instdialog_q.ui" line="651"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>instOptionsDialog</name>
    <message>
        <location filename="instOptionsDialog.cpp" line="99"/>
        <source>Install options for firewall &apos;%1&apos;</source>
        <translation>ファイアウォール「%」のオプションをインストールする</translation>
    </message>
</context>
<context>
    <name>instOptionsDialog_q</name>
    <message>
        <location filename="instoptionsdialog_q.ui" line="28"/>
        <source>Install options</source>
        <translation>インストールのオプション</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="61"/>
        <source>&lt;p align=&quot;center&quot;&gt;&lt;b&gt;&lt;font size=&quot;+2&quot;&gt;Install options for firewall &apos;%1&apos;&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="95"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="102"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="158"/>
        <source>min</source>
        <translation>最小</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="191"/>
        <source>Test run: run the script on the firewall but do not store it permanently.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="214"/>
        <source>Rebooting the firewall will restore its original policy. To cancel reboot, install the policy with &quot;test run&quot; option turned off</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="217"/>
        <source>Schedule reboot in </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="224"/>
        <source>If you install the policy in test mode, it will not be saved permanently, so you can revert to the last working configuration by rebooting the firewall</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="237"/>
        <source>Cancel reboot if policy activation was successfull</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="262"/>
        <source>Quiet install: do not print anything as commands are executed on the firewall</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="269"/>
        <source>Verbose: print all commands as they are executed on the firewall</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="276"/>
        <source>Remove comments from configuration</source>
        <translation>設定からコメントを削除する</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="283"/>
        <source>Compress script</source>
        <translation>スクリプト圧縮</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="290"/>
        <source>Store a copy of fwb file on the firewall</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="306"/>
        <source>Address that will be used to communicate with the firewall:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="353"/>
        <source>Write configuration to standby PIX</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="360"/>
        <source>Dry run (commands won&apos;t be executed on the firewall)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="367"/>
        <source>Store configuration diff in a file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="374"/>
        <source>Calculate difference between current firewall state and generated configuration and install only those commands that update state of the firewall</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="377"/>
        <source>install only ACL, &apos;icmp&apos;, &apos;telnet&apos;, &apos;ssh&apos;, &apos;nat&apos;, &apos;global&apos; and &apos;static&apos; commands</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="390"/>
        <source>Make a backup copy of the firewall configuration in this file:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="434"/>
        <source>Password or passphrase:</source>
        <translation>パスワードかパスフレーズ:</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="444"/>
        <source>User name:</source>
        <translation>ユーザー名:</translation>
    </message>
    <message>
        <location filename="instoptionsdialog_q.ui" line="492"/>
        <source>Enable password:</source>
        <translation>有効なパスワード:</translation>
    </message>
</context>
<context>
    <name>iosAdvancedDialog_q</name>
    <message>
        <location filename="iosadvanceddialog_q.ui" line="16"/>
        <source>IOS Advanced Configuration Options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosadvanceddialog_q.ui" line="23"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="iosadvanceddialog_q.ui" line="41"/>
        <source>Set router name using object&apos;s name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosadvanceddialog_q.ui" line="48"/>
        <source>Generate commands to configure addresses for interfaces</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosadvanceddialog_q.ui" line="90"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="iosadvanceddialog_q.ui" line="97"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
</context>
<context>
    <name>iosaclAdvancedDialog</name>
    <message>
        <location filename="iosaclAdvancedDialog.cpp" line="381"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>iosaclAdvancedDialog_q</name>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="16"/>
        <source>IOS ACL Firewall Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="1067"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="1084"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="29"/>
        <source>Compiler Options</source>
        <translation>コンパイラーオプション</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="41"/>
        <source>Output file name (if left blank, the file name is constructed of the firewall object name and extension &quot;.fw&quot;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="120"/>
        <source>Policy Compiler Options</source>
        <translation>コンパイラーオプションのポリシー</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="138"/>
        <source>If the option is deactivated, compiler treats empty groups as an error and aborts processing the policy. If this option is activated, compiler removes all empty groups from all rule elements. If rule element becomes &apos;any&apos; after the last empty group has been removed, the whole rule will be ignored. Use this option only if you fully understand how it works!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="141"/>
        <source>Ignore empty groups in rules</source>
        <translation type="unfinished">ルールのからグループを無視する</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="148"/>
        <source>Shadowing happens because a rule is a superset of a subsequent rule and any packets potentially matched by the subsequent rule have already been matched by the prior rule.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="151"/>
        <source>Detect rule shadowing in the policy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="161"/>
        <source>Always permit ssh access from the management
workstation with this address:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="214"/>
        <source>Script Options</source>
        <translation>スクリプトオプション</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="238"/>
        <source>Insert comments into generated IOSACL configuration file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="241"/>
        <source>Comment the code</source>
        <translation>コメントコード</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="248"/>
        <source>Group IOSACL commands in the script so that similar commands appear next to each other, just like IOSACL does it when you use &apos;show config&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="251"/>
        <source>Group similar commands together</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="279"/>
        <source>Clear all access lists then install new ones. This method may interrupt access to the firewall if you manage it remotely via IPSEC tunnel. This is the way access lists were generated in older versions of Firewall Builder for IOSACL.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="298"/>
        <source>Do not clear access lists and object group, just generate IOSACL commands for the new ones. Use this optin if you have your own policy installation scripts.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="330"/>
        <source>&quot;Safety net&quot; method:

First, create temporary access list to permit connections from the management subnet specified below to the firewall and assign it to outside interface. This temporary ACL helps maintain session between management station and the firewall while access lists are reloaded in case connection comes over IPSEC tunnel. Then clear permanent lists, recreate them and assign to interfaces. This method ensures that remote access to the firewall is maintained without interruption  at a cost of slightly larger configuration.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="360"/>
        <source>Temporary access list should permit access from this address or subnet (use prefix notation to specify subnet, e.g. 192.0.2.0/24):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="473"/>
        <source>Installer</source>
        <translation>インストーラー</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="479"/>
        <source>Built-in installer</source>
        <translation>組み込みインストーラー</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="488"/>
        <source>User name used to authenticate to the firewall (leave this empty if you use putty session):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="511"/>
        <source>Alternative name or address used to communicate with the firewall (also putty session name on Windows)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="536"/>
        <source>Additional command line parameters for ssh</source>
        <translation type="unfinished">ssh の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="566"/>
        <source>Additional command line parameters for scp</source>
        <translation type="unfinished">scp の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="597"/>
        <source>External install script</source>
        <translation type="unfinished">外部インストールスクリプト</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="614"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation>インストールスクリプトのポリシー (この項目が空の場合に使用する組み込みインストーラー):</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="653"/>
        <source>Command line options for the script:</source>
        <translation>スクリプトのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="704"/>
        <source>Prolog/Epilog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="773"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="745"/>
        <source>The following commands will be added verbatim on top of generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="799"/>
        <source>The following commands will be added verbatim after generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="822"/>
        <source>Logging</source>
        <translation>ログ記録</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="828"/>
        <source>Generate logging commands</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="835"/>
        <source>Syslog</source>
        <translation>Syslog</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="847"/>
        <source>Syslog host (name or IP address):</source>
        <translation>Syslog ホスト (名前か IP アドレス):</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="860"/>
        <source>syslog facility:</source>
        <translation>syslog ファシリティ:</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="873"/>
        <source>syslog level (&apos;logging trap&apos;):</source>
        <translation>syslog レベル (&apos;logging trap&apos;):</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="918"/>
        <source>The logging timestamp command requires that the clock command be set.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="931"/>
        <source>Enable logging timestamps on syslog file</source>
        <translation type="unfinished">syslog ファイルで日付の記録を有効にする</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="941"/>
        <source>Other logging destinations and levels:</source>
        <translation type="unfinished">その他のログ記録先とレベル:</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="950"/>
        <source>Internal buffer</source>
        <translation>内部バッファ</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="957"/>
        <source>Console</source>
        <translation>コンソール</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="990"/>
        <source>IPv6</source>
        <translation>IPv6</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="996"/>
        <source>The order in which ipv4 and ipv6 rules should be generated:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="1017"/>
        <source>IPv4 before IPv6</source>
        <translation>IPv4 の前に IPv6</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="1022"/>
        <source>IPv6 before IPv4</source>
        <translation>IPv6 の前に IPv4</translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="82"/>
        <source>Compiler creates multiple access lists from the same policy,
two for each interface: one for inbound and another for
outbound. If the policy is written in a such way that no rule
can possibly be associated with an interface, this interface
gets no access list at all. Also, interfaces marked as 
&quot;unprotected&quot; never get access list regardless of how the policy
rules are designed.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="92"/>
        <source>Generate separate access list for each interface</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="99"/>
        <source>Compiler creates one access list and assigns it to all
interfaces.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iosacladvanceddialog_q.ui" line="104"/>
        <source>Create one access list and attach it to all interfaces</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ipfAdvancedDialog</name>
    <message>
        <location filename="ipfAdvancedDialog.cpp" line="189"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>ipfAdvancedDialog_q</name>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="13"/>
        <source>ipf: advanced settings</source>
        <translation>ipf: 高度な設定</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="43"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="59"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="78"/>
        <source>Protocol Helpers</source>
        <translation>プロトコルヘッダー</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="103"/>
        <source>Use raudio proxy in NAT rules</source>
        <translation>NAT ルールで raudio を使用する</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="110"/>
        <source>Use h323 proxy in NAT rules</source>
        <translation>NAT ルールで h323 プロキシーを使う</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="117"/>
        <source>Use ipsec proxy in NAT rules</source>
        <translation>NAT ルールで ipsec プロキシーを使う</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="140"/>
        <source>Use ftp proxy in NAT rules</source>
        <translation>NAT ルール中で ftp プロキシーを使用する</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="147"/>
        <source>Use rcmd proxy in NAT rules</source>
        <translation>NAT ルールで rcmd プロキシーを使う</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="184"/>
        <source>Use PPTP proxy in NAT rules</source>
        <translation>NAT ルールで PPTP プロキシーを使う</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="191"/>
        <source>Use IRC proxy in NAT rules for DCC</source>
        <translation>DCC 用の NAT ルールで IRC プロキシーを使う</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="204"/>
        <source>Some protocols involve multiple associated network connections. Firewall can keep track of such connections automatically if you activate one or all of the following options:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="218"/>
        <source>Compiler</source>
        <translation>コンパイラー</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="227"/>
        <source>Compiler:</source>
        <translation>コンパイラー:</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="240"/>
        <source>There are two ways compiler can generate code for rules in the Global Policy: it can either create two ipf rules to control both incoming and outgoing packets for each rule, or it can create only one ipf rule for incoming packets and permit all outgoing ones.You get more control over the packets crossing the firewall in the first mode, but generated script is going to be smaller if you choose the second.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="253"/>
        <source>Masquerade returned icmp as being from original
packet&apos;s destination</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="270"/>
        <source>Generate both &apos;in&apos; and &apos;out&apos; rules</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="277"/>
        <source>Pass all outgoing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="287"/>
        <source>Accept TCP sessions opened prior to firewall restart</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="294"/>
        <source>Find and eliminate duplicate rules</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="301"/>
        <source>Shadowing happens because a rule is a superset of a subsequent rule and any packets potentially matched by the subsequent rule have already been matched by the prior rule.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="304"/>
        <source>Detect rule shadowing in policy</source>
        <translation>ポリシー中の隠れたルールを検出する</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="311"/>
        <source>If the option is deactivated, compiler treats empty groups as an error and aborts processing the policy. If this option is activated, compiler removes all empty groups from all rule elements. If rule element becomes &apos;any&apos; after the last empty group has been removed, the whole rule will be ignored. Use this option only if you fully understand how it works!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="314"/>
        <source>Ignore empty groups in rules</source>
        <translation type="unfinished">ルールのからグループを無視する</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="369"/>
        <source>Always permit ssh access from
the management workstation
with this address:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="384"/>
        <source>Default action on &apos;Reject&apos;:</source>
        <translation>「Reject(拒否)」標準動作:</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="490"/>
        <source>Command line options for the compiler:</source>
        <translation>コンパイラーのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="509"/>
        <source>Output file name (if left blank, the file name is constructed of the firewall object name and extension &quot;.fw&quot;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="553"/>
        <source>Installer</source>
        <translation>インストーラー</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="559"/>
        <source>Built-in installer</source>
        <translation>組み込みインストーラー</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="565"/>
        <source>Directory on the firewall where configuration files should be installed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="588"/>
        <source>User name used to authenticate to the firewall (leave this empty if you use putty session):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="611"/>
        <source>Alternative name or address used to communicate with the firewall (also putty session name on Windows)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="634"/>
        <source>A command that installer should execute on the firewall in order to activate the policy (if this field is blank, installer runs firewall script in the directory specified above; it uses sudo if user name is not &apos;root&apos;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="659"/>
        <source>Additional command line parameters for ssh</source>
        <translation type="unfinished">ssh の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="689"/>
        <source>Additional command line parameters for scp</source>
        <translation type="unfinished">scp の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="736"/>
        <source>External install script</source>
        <translation type="unfinished">外部インストールスクリプト</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="750"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation type="unfinished">ポリシーインストールスクリプト (もしこのフィールドがブランクの時に使用する組み込みインストーラー)</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="789"/>
        <source>Command line options for the script:</source>
        <translation>スクリプトのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="840"/>
        <source>Prolog/Epilog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="916"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="881"/>
        <source>The following commands will be added verbatim on top of generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="949"/>
        <source>The following commands will be added verbatim after generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="966"/>
        <source>Logging</source>
        <translation>ログ記録</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1039"/>
        <source>Log facility:</source>
        <translation>ログファシリティ:</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1052"/>
        <source>Log level:</source>
        <translation>ログレベル:</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1065"/>
        <source>Log packet body</source>
        <translation>ログパケット本体</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1072"/>
        <source>Block if can not log</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1096"/>
        <source>Script Options</source>
        <translation>スクリプトオプション</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1121"/>
        <source>Add virtual addresses for NAT</source>
        <translation>NAT 用バーチャルアドレスの追加</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1128"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation>ファイアウォールマシンのインターフェース構成</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1135"/>
        <source>Turn debugging on in generated script</source>
        <translation>生成スクリプトでデバッグに切り替え</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1142"/>
        <source>If this option is on, policy compiler adds virtual addresses to the interfaces to make the firewall answer to ARP queries for addresses used in NAT rules.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1145"/>
        <source>Optimization</source>
        <translation>最適化</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1152"/>
        <source>These options enable auxiliary sections in the generated shell script.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1197"/>
        <source>Determine addresses of dynamic interfaces at run time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1205"/>
        <source>IPv6</source>
        <translation>IPv6</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1211"/>
        <source>Enable IPv6 support</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1218"/>
        <source>The order in which ipv4 and ipv6 rules should be generated:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1239"/>
        <source>IPv4 before IPv6</source>
        <translation type="unfinished">IPv4 の前に IPv6</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="1244"/>
        <source>IPv6 before IPv4</source>
        <translation type="unfinished">IPv6 の前に IPv4</translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="154"/>
        <source>Use Kerberos rcmd proxy in NAT rules</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfadvanceddialog_q.ui" line="161"/>
        <source>Use Kerberos ekshell proxy in NAT rules</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ipfwAdvancedDialog</name>
    <message>
        <location filename="ipfwAdvancedDialog.cpp" line="174"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>ipfwAdvancedDialog_q</name>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="13"/>
        <source>ipfw: advanced settings</source>
        <translation>ipfw: 高度な設定</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="52"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="68"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="87"/>
        <source>Compiler</source>
        <translation>コンパイラー</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="93"/>
        <source>Compiler:</source>
        <translation>コンパイラー:</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="122"/>
        <source>Command line options for the compiler:</source>
        <translation>コンパイラーのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="151"/>
        <source>Output file name (if left blank, the file name is constructed of the firewall object name and extension &quot;.fw&quot;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="212"/>
        <source>Add rule to accept packets matching dynamic rules created for
known sessions on top of the policy (action &apos;check-state&apos;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="226"/>
        <source>Shadowing happens because a rule is a superset of a subsequent rule and any packets potentially matched by the subsequent rule have already been matched by the prior rule.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="229"/>
        <source>Detect rule shadowing in policy</source>
        <translation type="unfinished">ポリシー中の隠れたルールを検出する</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="242"/>
        <source>If the option is deactivated, compiler treats empty groups as an error and aborts processing the policy. If this option is activated, compiler removes all empty groups from all rule elements. If rule element becomes &apos;any&apos; after the last empty group has been removed, the whole rule will be ignored. Use this option only if you fully understand how it works!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="245"/>
        <source>Ignore empty groups in rules</source>
        <translation type="unfinished">ルールのからグループを無視する</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="252"/>
        <source>Always permit ssh access from
the management workstation
with this address:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="294"/>
        <source>Installer</source>
        <translation>インストーラー</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="300"/>
        <source>Built-in installer</source>
        <translation>組み込みインストーラー</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="306"/>
        <source>Directory on the firewall where script should be installed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="329"/>
        <source>User name used to authenticate to the firewall (leave this empty if you use putty session):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="352"/>
        <source>Alternative name or address used to communicate with the firewall (also putty session name on Windows)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="375"/>
        <source>A command that installer should execute on the firewall in order to activate the policy (if this field is blank, installer runs firewall script in the directory specified above; it uses sudo if user name is not &apos;root&apos;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="406"/>
        <source>Additional command line parameters for ssh</source>
        <translation type="unfinished">ssh の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="436"/>
        <source>Additional command line parameters for scp</source>
        <translation type="unfinished">scp の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="467"/>
        <source>External install script</source>
        <translation>外部インストールスクリプト</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="481"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation>インストールスクリプトのポリシー (この項目が空の場合に使用する組み込みインストーラー):</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="520"/>
        <source>Command line options for the script:</source>
        <translation>スクリプトのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="571"/>
        <source>Prolog/Epilog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="665"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="621"/>
        <source>The following commands will be added verbatim after generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="672"/>
        <source>The following commands will be added verbatim on top of generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="692"/>
        <source>Script Options</source>
        <translation>スクリプトオプション</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="720"/>
        <source>Add virtual addresses for NAT</source>
        <translation>NAT 用バーチャルアドレス群の追加</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="727"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation>ファイアウォールマシンのインターフェース構成</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="734"/>
        <source>Turn debugging on in generated script</source>
        <translation>生成スクリプトでデバッグに切り替え</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="741"/>
        <source>These options enable auxiliary sections in the generated shell script.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="787"/>
        <source>IPv6</source>
        <translation>IPv6</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="793"/>
        <source>The order in which ipv4 and ipv6 rules should be generated:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="814"/>
        <source>IPv4 before IPv6</source>
        <translation type="unfinished">IPv4 の前に IPv6</translation>
    </message>
    <message>
        <location filename="ipfwadvanceddialog_q.ui" line="819"/>
        <source>IPv6 before IPv4</source>
        <translation type="unfinished">IPv6 の前に IPv4</translation>
    </message>
</context>
<context>
    <name>iptAdvancedDialog</name>
    <message>
        <location filename="iptAdvancedDialog.cpp" line="231"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>iptAdvancedDialog_q</name>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="22"/>
        <source>iptables: advanced settings</source>
        <translation>iptables: 高度な設定</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="41"/>
        <source>Compiler</source>
        <translation>コンパイラー</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="56"/>
        <source>Command line options for the compiler:</source>
        <translation>コンパイラーのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="75"/>
        <source>Accept ESTABLISHED and RELATED packets before the first rule</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="88"/>
        <source>Bridging firewall</source>
        <translation>ブリッジ中のファイアウォール</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="117"/>
        <source>Detect shadowing in policy rules</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="130"/>
        <source>Assume firewall is part of &apos;any&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="143"/>
        <source>Accept TCP sessions opened prior to firewall restart</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="156"/>
        <source>Enable support for NAT of locally originated connections</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="163"/>
        <source>Compiler:</source>
        <translation>コンパイラー:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="182"/>
        <source>Output file name (if left blank, the file name is constructed of the firewall object name and extension &quot;.fw&quot;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="208"/>
        <source>Drop packets that are associated with
no known connection</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="216"/>
        <source>and log them</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="282"/>
        <source>Clamp MSS to MTU</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="295"/>
        <source>Ignore empty groups in rules</source>
        <translation>ルールのからグループを無視する</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="350"/>
        <source>Always permit ssh access from
the management workstation
with this address:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="375"/>
        <source>Default action on &apos;Reject&apos;:</source>
        <translation>「Reject(拒否)」の標準動作:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="401"/>
        <source>Make Tag and Classify actions terminating</source>
        <translation>タグと分類の設定操作を中断しました</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="409"/>
        <source>Installer</source>
        <translation>インストーラー</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="415"/>
        <source>Built-in installer</source>
        <translation>組み込みインストーラー</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="421"/>
        <source>Directory on the firewall where script should be installed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="444"/>
        <source>User name used to authenticate to the firewall (leave this empty if you use putty session):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="467"/>
        <source>Alternative name or address used to communicate with the firewall (also putty session name on Windows)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="490"/>
        <source>A command that installer should execute on the firewall in order to activate the policy (if this field is blank, installer runs firewall script in the directory specified above; it uses sudo if user name is not &apos;root&apos;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="515"/>
        <source>Additional command line parameters for ssh</source>
        <translation>ssh の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="545"/>
        <source>Additional command line parameters for scp</source>
        <translation>scp の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="576"/>
        <source>External install script</source>
        <translation>外部インストールスクリプト</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="590"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation type="unfinished">ポリシーインストールスクリプト (もしこのフィールドがブランクの時に使用する組み込みインストーラー)</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="629"/>
        <source>Command line options for the script:</source>
        <translation>スクリプトのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="680"/>
        <source>Prolog/Epilog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="765"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="724"/>
        <source>The following commands will be added verbatim after generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="752"/>
        <source>The following commands will be added verbatim on top of generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="772"/>
        <source>Insert prolog script </source>
        <translation>prolog スクリプトの挿入</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="789"/>
        <source>on top of the script</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="794"/>
        <source>after interface configuration</source>
        <translation>インターフェース設定の後</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="799"/>
        <source>after policy reset</source>
        <translation>ポリシーリセットの後</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="811"/>
        <source>Logging</source>
        <translation type="unfinished">ログ記録</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="829"/>
        <source>use ULOG</source>
        <translation>ULOG を使う</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="836"/>
        <source>use LOG</source>
        <translation>LOG を使う</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="856"/>
        <source>log TCP seq. numbers</source>
        <translation>TCP シーケンス番号を記録する</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="863"/>
        <source>log IP options</source>
        <translation>IP オプションをログ記録する</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="870"/>
        <source>use numeric syslog levels</source>
        <translation>使用する syslog レベルの数値</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="877"/>
        <source>Log level:</source>
        <translation>ログレベル:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="890"/>
        <source>log TCP options</source>
        <translation>TCP オプションを記録する</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="914"/>
        <source>cprange</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="931"/>
        <source>queue threshold:</source>
        <translation>キュー閾値:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="941"/>
        <source>netlink group:</source>
        <translation>ネットリンクグループ:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1009"/>
        <source>Log prefix:</source>
        <translation>ログ接頭語:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1026"/>
        <source>Logging limit:</source>
        <translation>ログ記録制限:</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1053"/>
        <source>Activate logging in all rules
(overrides rule options, use for debugging)</source>
        <translation>すべてのルールのログを記録する
(ルールオプションを上書きします。デバッグで使います)</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1094"/>
        <source>Script</source>
        <translation>スクリプト</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1103"/>
        <source>These options enable auxiliary sections in the generated shell script.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1132"/>
        <source>iptables-restore replaces firewall policy in one atomic transaction</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1135"/>
        <source>Use iptables-restore to activate policy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1142"/>
        <source>Add virtual addresses for NAT</source>
        <translation>NAT 用バーチャルアドレス群の追加</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1165"/>
        <source>Load modules</source>
        <translation>モジュール読み込み</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1172"/>
        <source>Verify interfaces before loading firewall policy</source>
        <translation>ファイアウォールポリシーを読み込む前にインターフェースの確認</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1179"/>
        <source>Turn debugging on in generated script</source>
        <translation>生成スクリプトでデバッグに切り替え</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1186"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation>ファイアウォールマシンのインターフェース構成</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1210"/>
        <source>IPv6</source>
        <translation>IPv6</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1216"/>
        <source>The order in which ipv4 and ipv6 rules should be generated:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1224"/>
        <source>IPv4 before IPv6</source>
        <translation>IPv4 の前に IPv6</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1229"/>
        <source>IPv6 before IPv4</source>
        <translation>IPv6 の前に IPv4</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1269"/>
        <source>Help</source>
        <translation>ヘルプ</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1292"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="1308"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="iptadvanceddialog_q.ui" line="275"/>
        <source>This adds a rule on top of the policy with iptables target TCPMSS
and option --clamp-mss-to-pmtu. Generation of this command is
version-dependent and also depends on the setting of ip or ipv6 forwarding
in host settings dialog.
</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>linksysAdvancedDialog_q</name>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="13"/>
        <source>Linksys/Sveasoft: advanced settings</source>
        <translation>Linksys/Sveasoft: 高度な設定</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="70"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="86"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="108"/>
        <source>Path</source>
        <translation>パス</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="132"/>
        <source>modprobe:</source>
        <translation>modprobe:</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="145"/>
        <source>logger:</source>
        <translation>logger:</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="158"/>
        <source>ip:</source>
        <translation>ip:</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="171"/>
        <source>lsmod</source>
        <translation>lsmod</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="184"/>
        <source>iptables:</source>
        <translation>iptables:</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="244"/>
        <source>Specify directory path and a file name for each utility on your firewall machine. Leave these empty if you want to use default values.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="290"/>
        <source>Prompts</source>
        <translation>プロンプト</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="320"/>
        <source>Policy installer relies on the shell prompt on the firewall to execute commands. Installer tries both prompt string patterns configured here; it assumes that the firewall is ready to accept a command if either prompt matches. You should only need to change these string patterns if Sveasoft changes the shell prompt in the future releases of the software.
&lt;br&gt;
&lt;br&gt;
The default strings work for Sveasoft Alchemy pre-5.1 and pre-5.2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="358"/>
        <source>Use default prompts</source>
        <translation>標準プロンプトを使う</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="368"/>
        <source>prompt 2</source>
        <translation>プロンプト2</translation>
    </message>
    <message>
        <location filename="linksysadvanceddialog_q.ui" line="400"/>
        <source>prompt 1</source>
        <translation>プロンプト1</translation>
    </message>
</context>
<context>
    <name>linux24AdvancedDialog_q</name>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="13"/>
        <source>Linux 2.4: advanced settings</source>
        <translation>Linux 2.4: 高度な設定</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="26"/>
        <source>Options</source>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="48"/>
        <source>IPv4 Packet forwarding</source>
        <translation>IPv4 パケット転送</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="643"/>
        <source>No change</source>
        <translation>変更なし</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="648"/>
        <source>On</source>
        <translation>オン</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="653"/>
        <source>Off</source>
        <translation>オフ</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="96"/>
        <source>IPv6 Packet forwarding</source>
        <translation>IPv6 パケット転送</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="128"/>
        <source>Kernel anti-spoofing protection</source>
        <translation>カーネルの spoofing 対策防御</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="160"/>
        <source>Ignore broadcast pings</source>
        <translation>ブロードキャスト ping の無視</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="192"/>
        <source>Ignore all pings</source>
        <translation>すべての ping の無視</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="224"/>
        <source>Accept source route</source>
        <translation>ソースルートの受理</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="256"/>
        <source>Accept ICMP redirects</source>
        <translation>ICMP リダイレクトの受理</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="288"/>
        <source>Ignore bogus ICMP errors</source>
        <translation>bogus ICMP エラーの無視</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="320"/>
        <source>Allow dynamic addresses</source>
        <translation>ダイナミックアドレス群の許可</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="368"/>
        <source>Log martians</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="381"/>
        <source>whats this text</source>
        <comment>whats this comment</comment>
        <translation>このテキストは何</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="420"/>
        <source>TCP</source>
        <translation>TCP</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="444"/>
        <source>These parameters make sense for connections to or from the firewall host</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="661"/>
        <source>TCP sack</source>
        <translation>TCP sack</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="674"/>
        <source>TCP window scaling</source>
        <translation>TCP ウィンドウスケーリング</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="687"/>
        <source>TCP ECN</source>
        <translation>TCP ECN</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="700"/>
        <source>TCP SYN cookies</source>
        <translation>TCP SYN クッキー</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="713"/>
        <source>TCP keepalive time (sec)</source>
        <translation>TCP キープアライブ (秒)</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="726"/>
        <source>TCP fack</source>
        <translation>TCP fack</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="739"/>
        <source>TCP timestamps</source>
        <translation>TCP タイムスタンプ</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="752"/>
        <source>TCP FIN timeout (sec)</source>
        <translation>TCP FIN タイムアウト(秒)</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="766"/>
        <source>Path</source>
        <translation>パス</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="788"/>
        <source>Specify directory path and a file name for each utility on your firewall machine. Leave these empty if you want to use default values.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="801"/>
        <source>iptables:</source>
        <translation>iptables:</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="820"/>
        <source>:ip6tables</source>
        <translation>:ip6tables</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="830"/>
        <source>ip:</source>
        <translation>ip:</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="846"/>
        <source>logger:</source>
        <translation>logger:</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="862"/>
        <source>modprobe:</source>
        <translation>modprobe:</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="878"/>
        <source>lsmod</source>
        <translation>lsmod</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="894"/>
        <source>iptables-restore:</source>
        <translation>iptables-restore:</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="913"/>
        <source>:ip6tables-restore</source>
        <translation>:ip6tables-restore</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="961"/>
        <source>Help</source>
        <translation>ヘルプ</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="984"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="linux24advanceddialog_q.ui" line="1000"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>longTextDialog_q</name>
    <message>
        <location filename="longtextdialog_q.ui" line="13"/>
        <source>longTextDialog_q</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="longtextdialog_q.ui" line="25"/>
        <source>Continue</source>
        <translation>続ける</translation>
    </message>
    <message>
        <location filename="longtextdialog_q.ui" line="114"/>
        <source>this is the error text</source>
        <translation>これはエラーテキストです</translation>
    </message>
</context>
<context>
    <name>macosxAdvancedDialog_q</name>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="13"/>
        <source>MacOS X: advanced settings</source>
        <translation>Mac OS X: 高度な設定</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="52"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="68"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="90"/>
        <source>Options</source>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="108"/>
        <source>Generate ICMP redirects</source>
        <translation>ICMP リダイレクト生成</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="121"/>
        <source>Packet forwarding</source>
        <translation>パケット転送</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="173"/>
        <source>No change</source>
        <translation>変更なし</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="178"/>
        <source>On</source>
        <translation>オン</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="183"/>
        <source>Off</source>
        <translation>オフ</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="191"/>
        <source>Forward source routed packets</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="269"/>
        <source>Path</source>
        <translation>パス</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="287"/>
        <source>ipfw:</source>
        <translation>ipfw:</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="300"/>
        <source>sysctl:</source>
        <translation>sysctl:</translation>
    </message>
    <message>
        <location filename="macosxadvanceddialog_q.ui" line="313"/>
        <source>Specify directory path and a file name for the following utilities on the OS your firewall machine is running. Leave these empty if you want to use default values.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>newFirewallDialog</name>
    <message>
        <location filename="newFirewallDialog.cpp" line="122"/>
        <source>Check option &apos;dynamic address&apos; for the interface that gets its IP address dynamically via DHCP or PPP protocol.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="123"/>
        <source>Check option &apos;Unnumbered interface&apos; for the interface that does not have an IP address. Examples of interfaces of this kind are those used to terminate PPPoE or VPN tunnels.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="131"/>
        <source>FWBuilder template files</source>
        <translation>ファイアウォールビルダー用テンプレートファイル</translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="131"/>
        <source>FWBuilder template files (*.xml *.fwb)</source>
        <translation>ファイアウォールビルダー用テンプレートファイル (*.xml *.fwb)</translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="290"/>
        <source>Missing SNMP community string.</source>
        <translation>SNMP コミュニティ文字列が足りません。</translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="310"/>
        <source>Address of %1 could not be obtained via DNS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="584"/>
        <source>Interface: %1 (%2)</source>
        <translation>インターフェース: %1 (%2)</translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="591"/>
        <source>Dynamic address</source>
        <translation>ダイナミックアドレス</translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="593"/>
        <source>Unnumbered interface</source>
        <translation>アンナンバードインターフェース</translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="595"/>
        <source>Bridge port</source>
        <translation>ブリッジポート</translation>
    </message>
    <message>
        <location filename="newFirewallDialog.cpp" line="650"/>
        <source>Illegal address &apos;%1/%2&apos;</source>
        <translation>不当なアドレス 「%1/%2」</translation>
    </message>
</context>
<context>
    <name>newFirewallDialog_q</name>
    <message>
        <location filename="newfirewalldialog_q.ui" line="37"/>
        <source>TextLabel</source>
        <translation>テキストラベル</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="60"/>
        <source>Enter the name of the new object below:</source>
        <translation>新しいオブジェクトの名前を入力してください:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="76"/>
        <source>Choose firewall software it is running:</source>
        <translation>実行中のファイアウォールソフトウェアの選択してください:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="92"/>
        <source>Choose OS the new firewall runs on:</source>
        <translation>新規ファイアウォールを実行する OS を選択してください:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="133"/>
        <source>Use preconfigured template firewall objects</source>
        <translation>あらかじめ設定されたテンプレートオブジェクトを使用する</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="149"/>
        <source>Template file:</source>
        <translation>テンプレートファイル:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="166"/>
        <source>Browse</source>
        <translation>参照</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="173"/>
        <source>Use standard template library</source>
        <translation>標準テンプレートライブラリーを使う</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="219"/>
        <source>Next step is to add interfaces to the new firewall. There are two ways to do it: using SNMP query or manually. Adding them using SNMP query is fast and automatic, but is only possible if firewall runs SNMP agent and you know SNMP community string &apos;read&apos;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="260"/>
        <source>Configure interfaces manually</source>
        <translation>インターフェースを手動設定する</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="270"/>
        <source>Use SNMP to discover interfaces of the firewall</source>
        <translation>SNMP を使用し、ファイアウォールのインターフェースを検出する</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="301"/>
        <source>Discover Interfaces using SNMP</source>
        <translation>SNMP を用いてインターフェースを検出する</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="308"/>
        <source>SNMP &apos;read&apos; community string:</source>
        <translation>SNMP「read」コミュニティ文字列:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="368"/>
        <source>Here you can add or edit interfaces manually. &apos;Name&apos; corresponds to the name of the physical interface, such as &apos;eth0&apos;, &apos;fxp0&apos;, &apos;ethernet0&apos; etc. &apos;Label&apos; is used to mark interface to reflect network topology, e.g. &apos;outside&apos; or &apos;inside&apos;. Label is mandatory for PIX firewall.</source>
        <translation type="unfinished">;;</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="381"/>
        <source>Click &apos;Next&apos; when done.</source>
        <translation>「次へ」をクリックすると処理します。</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="779"/>
        <source>Name</source>
        <translation>名前</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="784"/>
        <source>Label</source>
        <translation>ラベル</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="789"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="431"/>
        <source>Netmask</source>
        <translation>ネットマスク</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="436"/>
        <source>Dyn</source>
        <translation>Dyn</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="441"/>
        <source>MAC</source>
        <translation>MAC</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="449"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="568"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="603"/>
        <source>Netmask:</source>
        <translation>ネットマスク:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="638"/>
        <source>MAC:</source>
        <translation>MAC:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="533"/>
        <source>Label:</source>
        <translation>ラベル:</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="499"/>
        <source>Bridge port</source>
        <translation>ブリッジポート</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="506"/>
        <source>Unnumbered interface</source>
        <translation>アンナンバードインターフェース</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="513"/>
        <source>Dynamic address</source>
        <translation>ダイナミックアドレス</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="520"/>
        <source>Regular interface</source>
        <translation>レギュラーインターフェース</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="707"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="694"/>
        <source>Update</source>
        <translation>更新</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="681"/>
        <source>Add</source>
        <translation>追加</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="764"/>
        <source>up</source>
        <translation>上</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="771"/>
        <source>down</source>
        <translation>下</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="794"/>
        <source>Security Level</source>
        <translation>セキュリティレベル</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="805"/>
        <source>Click &apos;Finish&apos; when done.</source>
        <translation>「終了を」をクリックすると処理します。</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="818"/>
        <source>In order to be able to build firewall policy properly, Firewall Builder needs information about &apos;security level&apos; of the firewall&apos;s interfaces. Interface that connects it to the Internet is considered &apos;insecure&apos; and has security level &apos;0&apos;, while interface connected to the internal network is supposed to be &apos;secure&apos; (security level &apos;100&apos;). You can arrange interfaces in the order of their security level below.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="1084"/>
        <source>Choose template object in the list and click &apos;Finish&apos; when ready. Template objects use generic interface names that will be iherited by the firewall object you create. You may need to rename them later to reflect real names of interfaces on your firewall machine.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="1148"/>
        <source>&lt; &amp;Back</source>
        <translation>&lt; 戻る(&amp;B)</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="1155"/>
        <source>&amp;Next &gt;</source>
        <translation>次へ(&amp;N) &gt;</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="1165"/>
        <source>&amp;Finish</source>
        <translation>終了(&amp;F)</translation>
    </message>
    <message>
        <location filename="newfirewalldialog_q.ui" line="1175"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>newGroupDialog_q</name>
    <message>
        <location filename="newgroupdialog_q.ui" line="24"/>
        <source>New Group</source>
        <translation>新規グループ</translation>
    </message>
    <message>
        <location filename="newgroupdialog_q.ui" line="30"/>
        <source>Library:</source>
        <translation>ライブラリー:</translation>
    </message>
    <message>
        <location filename="newgroupdialog_q.ui" line="55"/>
        <source>Group Name:</source>
        <translation>グループ名:</translation>
    </message>
    <message>
        <location filename="newgroupdialog_q.ui" line="65"/>
        <source>This operation will create a new group and put selected objects in it</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newgroupdialog_q.ui" line="94"/>
        <source>Create a group</source>
        <translation>グループ作成</translation>
    </message>
    <message>
        <location filename="newgroupdialog_q.ui" line="101"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
</context>
<context>
    <name>newHostDialog</name>
    <message>
        <location filename="newHostDialog.cpp" line="114"/>
        <source>FWBuilder template files</source>
        <translation>ファイアウォールビルダー用テンプレートファイル</translation>
    </message>
    <message>
        <location filename="newHostDialog.cpp" line="114"/>
        <source>FWBuilder template files (*.xml *.fwb)</source>
        <translation>ファイアウォールビルダー用テンプレートファイル (*.xml *.fwb)</translation>
    </message>
    <message>
        <location filename="newHostDialog.cpp" line="281"/>
        <source>Missing SNMP community string.</source>
        <translation>SNMP コミュニティ文字列が足りません。</translation>
    </message>
    <message>
        <location filename="newHostDialog.cpp" line="301"/>
        <source>Address of %1 could not be obtained via DNS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newHostDialog.cpp" line="489"/>
        <source>Interface: %1 (%2)</source>
        <translation>インターフェース: %1 (%2)</translation>
    </message>
    <message>
        <location filename="newHostDialog.cpp" line="496"/>
        <source>Dynamic address</source>
        <translation>ダイナミックアドレス</translation>
    </message>
    <message>
        <location filename="newHostDialog.cpp" line="498"/>
        <source>Unnumbered interface</source>
        <translation>アンナンバードインターフェース</translation>
    </message>
    <message>
        <location filename="newHostDialog.cpp" line="553"/>
        <source>Illegal address &apos;%1/%2&apos;</source>
        <translation>不当なアドレス 「%1/%2」</translation>
    </message>
</context>
<context>
    <name>newHostDialog_q</name>
    <message>
        <location filename="newhostdialog_q.ui" line="37"/>
        <source>TextLabel</source>
        <translation>テキストラベル</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="66"/>
        <source>Enter the name of the new object below:</source>
        <translation>新しいオブジェクトの名前を入力してください:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="107"/>
        <source>Use preconfigured template host objects</source>
        <translation>事前定義されたホストオブジェクトテンプレートを使う</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="123"/>
        <source>Template file:</source>
        <translation>テンプレートファイル:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="153"/>
        <source>Browse</source>
        <translation>参照</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="160"/>
        <source>Use standard template library</source>
        <translation>標準テンプレートライブラリーを使う</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="190"/>
        <source>Next step is to add interfaces to the new host. There are two ways to do it: using SNMP query or manually. Adding them using SNMP query is fast and automatic, but is only possible if the host runs SNMP agent and you know SNMP community string &apos;read&apos;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="228"/>
        <source>Configure interfaces manually</source>
        <translation>インターフェースを手動設定する</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="238"/>
        <source>Use SNMP to discover interfaces of the host</source>
        <translation>SNMP を使用し、ホストのインターフェースを検出する</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="266"/>
        <source>Discover Interfaces using SNMP</source>
        <translation>SNMP を用いてインターフェースを検出する</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="273"/>
        <source>SNMP &apos;read&apos; community string:</source>
        <translation>SNMP「read」コミュニティ文字列:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="339"/>
        <source>Here you can add or edit interfaces manually. &apos;Name&apos; corresponds to the name of the physical interface, such as &apos;eth0&apos;, &apos;fxp0&apos;, &apos;ethernet0&apos; etc. &apos;Label&apos; is used to mark interface to reflect network topology, e.g. &apos;outside&apos; or &apos;inside&apos;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="358"/>
        <source>Check option &apos;Unnumbered interface&apos; for the interface that does not have an IP address. Examples of interfaces of this kind are those used to terminate PPPoE or VPN tunnels.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="377"/>
        <source>Check option &apos;dynamic address&apos; for the interface that gets its IP address dynamically via DHCP or PPP protocol.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="390"/>
        <source>Click &apos;Next&apos; when done.</source>
        <translation>「次へ」をクリックすると処理します。</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="421"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="434"/>
        <source>Label:</source>
        <translation>ラベル:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="447"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="460"/>
        <source>This is unnumbered interface, that is, it does not have an IP address. You can use this for interfaces that terminate PPPoE or other VPN tunnels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="463"/>
        <source>Unnumbered interface</source>
        <translation>アンナンバードインターフェース</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="477"/>
        <source>Name</source>
        <translation>名前</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="482"/>
        <source>Label</source>
        <translation>ラベル</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="487"/>
        <source>Address</source>
        <translation>アドレス</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="492"/>
        <source>Netmask</source>
        <translation>ネットマスク</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="497"/>
        <source>Dyn</source>
        <translation>Dyn</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="502"/>
        <source>MAC</source>
        <translation>MAC</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="525"/>
        <source>MAC:</source>
        <translation>MAC:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="538"/>
        <source>Address of this interface is assigned dynamically using DHCP or PPP protocol</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="541"/>
        <source>Dynamic address</source>
        <translation>ダイナミックアドレス</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="548"/>
        <source>Netmask:</source>
        <translation>ネットマスク:</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="563"/>
        <source>Add</source>
        <translation>追加</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="570"/>
        <source>Update</source>
        <translation>更新</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="577"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="781"/>
        <source>Choose template object in the list and click &apos;Finish&apos; when ready. Template objects use generic interface names that will be iherited by the firewall object you create. You may need to rename them later to reflect real names of interfaces on your firewall machine.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="835"/>
        <source>&lt; &amp;Back</source>
        <translation>&lt; 戻る(&amp;B)</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="842"/>
        <source>&amp;Next &gt;</source>
        <translation>次へ(&amp;N) &gt;</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="852"/>
        <source>&amp;Finish</source>
        <translation>終了(&amp;F)</translation>
    </message>
    <message>
        <location filename="newhostdialog_q.ui" line="862"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>openbsdAdvancedDialog_q</name>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="13"/>
        <source>OpenBSD: advanced settings</source>
        <translation>OpenBSD: 高度な設定</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="70"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="86"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="105"/>
        <source>Options</source>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="127"/>
        <source>IPv4 Packet forwarding</source>
        <translation>IPv4 パケット転送</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="291"/>
        <source>No change</source>
        <translation>変更なし</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="296"/>
        <source>On</source>
        <translation>オン</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="301"/>
        <source>Off</source>
        <translation>オフ</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="178"/>
        <source>IPv6 Packet forwarding</source>
        <translation>IPv6 パケット転送</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="213"/>
        <source>Enable directed broadcast</source>
        <translation>ダイレクトブロードキャストを入力する</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="245"/>
        <source>Forward source routed packets</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="277"/>
        <source>Generate ICMP redirects</source>
        <translation>ICMP リダイレクト生成</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="342"/>
        <source>Path</source>
        <translation>パス</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="366"/>
        <source>pfctl:</source>
        <translation>pfctl:</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="399"/>
        <source>sysctl:</source>
        <translation>sysctl:</translation>
    </message>
    <message>
        <location filename="openbsdadvanceddialog_q.ui" line="412"/>
        <source>Specify directory path and a file name for the following utilities on the OS your firewall machine is running. Leave these empty if you want to use default values.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>pageSetupDialog_q</name>
    <message>
        <location filename="pagesetupdialog_q.ui" line="16"/>
        <source>Page Setup</source>
        <translation>ページ設定</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="22"/>
        <source>start each section on a new page</source>
        <translation>新規ページで各セクションを開始する</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="29"/>
        <source>print header on every page</source>
        <translation>各ページにヘッダーを印刷する</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="36"/>
        <source>print legend</source>
        <translation>凡例を印刷する</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="43"/>
        <source>print objects used in rules</source>
        <translation>ルールで使用したオブジェクトを印刷する</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="71"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="74"/>
        <source>Alt+O</source>
        <translation>Alt+O</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="87"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="90"/>
        <source>Alt+C</source>
        <translation>Alt+C</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="110"/>
        <source>Scale tables: </source>
        <translation>テーブルスケール: </translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="121"/>
        <source>50%</source>
        <translation>50%</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="126"/>
        <source>75%</source>
        <translation>75%</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="131"/>
        <source>100%</source>
        <translation>100%</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="136"/>
        <source>150%</source>
        <translation>150%</translation>
    </message>
    <message>
        <location filename="pagesetupdialog_q.ui" line="141"/>
        <source>200%</source>
        <translation>200%</translation>
    </message>
</context>
<context>
    <name>pfAdvancedDialog</name>
    <message>
        <location filename="pfAdvancedDialog.cpp" line="296"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>pfAdvancedDialog_q</name>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="19"/>
        <source>pf: advanced settings</source>
        <translation>pf: 高度な設定</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="41"/>
        <source>Compiler</source>
        <translation>コンパイラー</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="50"/>
        <source>Compiler:</source>
        <translation>コンパイラー:</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="79"/>
        <source>Command line options for the compiler:</source>
        <translation>コンパイラーのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="108"/>
        <source>Output file name (if left blank, the file name is constructed of the firewall object name and extension &quot;.fw&quot;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="220"/>
        <source>Accept TCP sessions opened prior to firewall restart</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="168"/>
        <source>Modulate state for all stateful rules (applies only to TCP services)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="197"/>
        <source>Shadowing happens because a rule is a superset of a subsequent rule and any packets potentially matched by the subsequent rule have already been matched by the prior rule.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="200"/>
        <source>Detect rule shadowing in policy</source>
        <translation type="unfinished">ポリシー中の隠れたルールを検出する</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="181"/>
        <source>If the option is deactivated, compiler treats empty groups as an error and aborts processing the policy. If this option is activated, compiler removes all empty groups from all rule elements. If rule element becomes &apos;any&apos; after the last empty group has been removed, the whole rule will be ignored. Use this option only if you fully understand how it works!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="184"/>
        <source>Ignore empty groups in rules</source>
        <translation>ルールのからグループを無視する</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="207"/>
        <source>Optimization:</source>
        <translation>最適化:</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="252"/>
        <source>Always permit ssh access from
the management workstation
with this address:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="294"/>
        <source>Scrub</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="319"/>
        <source>Clears the don&apos;t fragment bit from the IP packet header.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="322"/>
        <source>Clear DF bit</source>
        <translation>DF ビット解除</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="329"/>
        <source>Replaces the IP identification field of outgoing packets with random values to compensate for operating systems that use predictable values.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="332"/>
        <source>Use random ID</source>
        <translation>ランダム ID を使う</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="357"/>
        <source>Enforce Minimum TTL:</source>
        <translation>TTL 最小値の強化:</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="364"/>
        <source>Enforces a minimum Time To Live (TTL) in IP packet headers.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="397"/>
        <source>Enforce Maximum MSS:</source>
        <translation>MSS 最大値の強化:</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="404"/>
        <source>Enforces a maximum Maximum Segment Size (MSS) in TCP packet headers.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="451"/>
        <source>Reassemble fragments</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="479"/>
        <source>Buffers incoming packet fragments and reassembles them into a complete packet before passing them to the filter engine.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="482"/>
        <source>Buffer and reassemble fragments (default)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="495"/>
        <source>Causes duplicate fragments to be dropped and any overlaps to be cropped.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="498"/>
        <source>Drop duplicate fragments, do not buffer and reassemble</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="511"/>
        <source>Similar to &apos;Drop duplicate fragments&apos; except that all duplicate or overlapping fragments will be dropped as well as any further corresponding fragments.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="514"/>
        <source>Drop duplicate and subsequent fragments</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="541"/>
        <source>Limits</source>
        <translation>制限</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="559"/>
        <source>maximum number of entries in the memory pool used for packet reassembly</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="578"/>
        <source>table-entries</source>
        <translation>テーブルエントリー</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="591"/>
        <source>maximum number of addresses that canbe stored in tables</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="616"/>
        <source>maximum number of entries in the memory pool used for state table entries</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="635"/>
        <source>state table size: </source>
        <translation>ステートテーブルの大きさ: </translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="642"/>
        <source>reassembly pool: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="671"/>
        <source>maximum number of entries in the memory pool used for tracking source IP addresses</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="696"/>
        <source>maximum number of tables that can exist in the memory simultaneously</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="715"/>
        <source>tables</source>
        <translation>テーブル</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="722"/>
        <source>src-nodes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="778"/>
        <source>Timeouts</source>
        <translation>タイムアウト</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="790"/>
        <source>When a packet matches a stateful connection, the seconds to live for the connection will be updated to the value which corresponds to the connection state.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="803"/>
        <source>TCP</source>
        <translation>TCP</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1182"/>
        <source>first</source>
        <translation>first</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1253"/>
        <source>The state after the first packet.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="847"/>
        <source>opening</source>
        <translation>opening</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="860"/>
        <source> The state before the destination host ever sends a packet.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="876"/>
        <source>established</source>
        <translation>確立済み</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="889"/>
        <source>The fully established state.</source>
        <translation>完全な確立状態です。</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="914"/>
        <source>The state after the first FIN has been sent.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="930"/>
        <source>closing</source>
        <translation>closing</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="943"/>
        <source>The state after both FINs have been exchanged and the connection is closed.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="959"/>
        <source>finwait</source>
        <translation>finwait</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="972"/>
        <source>The state after one endpoint sends an RST.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="988"/>
        <source>closed</source>
        <translation>closed</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="998"/>
        <source>UDP</source>
        <translation>UDP</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1211"/>
        <source>single</source>
        <translation>single</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1049"/>
        <source>The state if the source host sends more than one packet but the destination host has never sent one back.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1218"/>
        <source>multiple</source>
        <translation>multiple</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1078"/>
        <source> The state if both hosts have sent packets.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1097"/>
        <source>ICMP</source>
        <translation>ICMP</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1134"/>
        <source>The state after an ICMP error came back in response to an ICMP packet.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1157"/>
        <source>error</source>
        <translation>error</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1167"/>
        <source>Other Protocols</source>
        <translation>その他のプロトコル</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1272"/>
        <source>Fragments</source>
        <translation>フラグメント</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1281"/>
        <source>reassembly timeout</source>
        <translation>組み換えのタイムアウト</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1288"/>
        <source>state expiration timeout</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1301"/>
        <source>seconds between purges of expired states and packet fragments.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1323"/>
        <source>seconds before an unassembled fragment is expired.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1358"/>
        <source>Adaptive scaling</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1367"/>
        <source>Timeout values can be reduced adaptively as the number of state table entries grows (see man page pf.conf(5) for details)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1386"/>
        <source>adaptive start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1402"/>
        <source>When the number of state entries exceeds this value, adaptive scaling begins.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1424"/>
        <source>adaptive end</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1440"/>
        <source>When reaching this number of state entries, all timeout val- ues become zero, effectively purging all state entries imme- diately.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1456"/>
        <source>Activate adaptive timeout scaling</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1483"/>
        <source>Installer</source>
        <translation>インストーラー</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1492"/>
        <source>Built-in installer</source>
        <translation>組み込みインストーラー</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1498"/>
        <source>Directory on the firewall where script should be installed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1521"/>
        <source>User name used to authenticate to the firewall (leave this empty if you use putty session):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1544"/>
        <source>Alternative name or address used to communicate with the firewall (also putty session name on Windows)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1567"/>
        <source>A command that installer should execute on the firewall in order to activate the policy (if this field is blank, installer runs firewall script in the directory specified above; it uses sudo if user name is not &apos;root&apos;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1592"/>
        <source>Additional command line parameters for ssh</source>
        <translation>ssh の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1622"/>
        <source>Additional command line parameters for scp</source>
        <translation>scp の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1653"/>
        <source>External install script</source>
        <translation>外部インストールスクリプト</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1667"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation>インストールスクリプトのポリシー (この項目が空の場合に使用する組み込みインストーラー):</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1706"/>
        <source>Command line options for the script:</source>
        <translation>スクリプトのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1757"/>
        <source>Prolog/Epilog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1836"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1811"/>
        <source>The following commands will be added verbatim after generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1853"/>
        <source>The following commands will be added verbatim on top of generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1866"/>
        <source>Insert prolog and epilog scripts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1883"/>
        <source>in the activation shell script (.fw file)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1888"/>
        <source>in the pf rule file (.conf file)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1932"/>
        <source>Logging</source>
        <translation>ログ記録</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="1960"/>
        <source>Log Prefix</source>
        <translation>ログ接頭語</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2024"/>
        <source>Fallback &quot;deny all&quot; rule should log blocked packets</source>
        <translation></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2032"/>
        <source>Script</source>
        <translation>スクリプト</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2100"/>
        <source>Add virtual addresses for NAT</source>
        <translation>NAT 用バーチャルアドレス群の追加</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2093"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation>ファイアウォールマシンのインターフェース構成</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2086"/>
        <source>Turn debugging on in generated script</source>
        <translation>生成スクリプトでデバッグに切り替え</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2057"/>
        <source>These options enable auxiliary sections in the generated shell script.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2131"/>
        <source>IPv6</source>
        <translation>IPv6</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2140"/>
        <source>The order in which ipv4 and ipv6 rules should be generated:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2161"/>
        <source>IPv4 before IPv6</source>
        <translation type="unfinished">IPv4 の前に IPv6</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2166"/>
        <source>IPv6 before IPv4</source>
        <translation type="unfinished">IPv6 の前に IPv4</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2193"/>
        <source>Help</source>
        <translation>ヘルプ</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2216"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2232"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="pfadvanceddialog_q.ui" line="2107"/>
        <source>Flush pf states after reloading rules</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>pixAdvancedDialog</name>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="839"/>
        <source>Error: Policy compiler for PIX is not installed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="824"/>
        <source>Compiler error</source>
        <translation>コンパイラーエラー</translation>
    </message>
    <message>
        <location filename="pixAdvancedDialog.cpp" line="919"/>
        <source>Script Editor</source>
        <translation>スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>pixAdvancedDialog_q</name>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="16"/>
        <source>PIX Firewall Settings</source>
        <translation>PIX ファイアウォール設定</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="48"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="65"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="95"/>
        <source>Compiler</source>
        <translation>コンパイラー</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="110"/>
        <source>Output file name (if left blank, the file name is constructed of the firewall object name and extension &quot;.fw&quot;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="139"/>
        <source>Policy Compiler Options</source>
        <translation>ポリシーコンパイラーオプション</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="151"/>
        <source>Generate rules assuming the firewall is part of &quot;Any&quot;. This makes a difference in rules that use services &apos;ssh&apos; and &apos;telnet&apos; since PIX uses special commands to control ssh and telnet access to the firewall machine</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="154"/>
        <source>Assume firewall is part of &apos;any&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="167"/>
        <source>PIX inspects packets with ACLs before it does NAT, while many other firewalls do NAT first and then apply ACLs. Policy compiler can emulate the latter behaviour if this options is turned on.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="170"/>
        <source>Replace NAT&apos;ted objects with their 
translations in policy rules</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="188"/>
        <source>Normally PIX does not support ouotbound ACL, however policy compiler can emulate them if this option is turned on</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="181"/>
        <source>Emulate outbound ACLs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="191"/>
        <source>Generate outbound ACLs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="198"/>
        <source>If the option is deactivated, compiler treats empty groups as an error and aborts processing the policy. If this option is activated, compiler removes all empty groups from all rule elements. If rule element becomes &apos;any&apos; after the last empty group has been removed, the whole rule will be ignored. Use this option only if you fully understand how it works!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="201"/>
        <source>Ignore empty groups in rules</source>
        <translation type="unfinished">ルールのからグループを無視する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="208"/>
        <source>In nat rules where network zone object is used in OSrc, ODst and OSrv are &apos;any&apos; and TSrc defines a global pool for the translation, replace object in OSrc with &apos;any&apos; to produce PIX command &quot;nat (interface) N 0.0.0.0 0.0.0.0&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="211"/>
        <source>Optimize &apos;default nat&apos; rules</source>
        <translation>「default nat」ルールを最適化する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="218"/>
        <source>Shadowing happens because a rule is a superset of a subsequent rule and any packets potentially matched by the subsequent rule have already been matched by the prior rule.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="221"/>
        <source>Detect rule shadowing in the policy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="237"/>
        <source>Verification of NAT rules</source>
        <translation>NAT ルールの検証中</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="249"/>
        <source>Check for duplicate nat rules</source>
        <translation>複製 NAT ルールを確認する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="256"/>
        <source>Check for overlapping global pools</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="263"/>
        <source>Check for overlapping statics</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="276"/>
        <source>Check for overlapping global
pools and statics</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="287"/>
        <source>Always permit ssh access from the management
workstation with this address:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="328"/>
        <source>Installer</source>
        <translation>インストーラー</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="337"/>
        <source>Built-in installer</source>
        <translation>組み込みインストーラー</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="343"/>
        <source>User name used to authenticate to the firewall (leave this empty if you use putty session):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="366"/>
        <source>Alternative name or address used to communicate with the firewall (also putty session name on Windows)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="391"/>
        <source>Additional command line parameters for ssh</source>
        <translation>ssh の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="421"/>
        <source>Additional command line parameters for scp</source>
        <translation>scp の追加コマンドラインパラメーター</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="452"/>
        <source>External install script</source>
        <translation>外部インストールスクリプト</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="466"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation>インストールスクリプトのポリシー (この項目が空の場合に使用する組み込みインストーラー):</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="505"/>
        <source>Command line options for the script:</source>
        <translation>スクリプトのコマンドラインオプション:</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="556"/>
        <source>Prolog/Epilog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="635"/>
        <source>Edit</source>
        <translation>編集</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="600"/>
        <source>The following commands will be added verbatim on top of generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="668"/>
        <source>The following commands will be added verbatim after generated configuration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="685"/>
        <source>Timeouts</source>
        <translation>タイムアウト</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="713"/>
        <source>Set all to defaults..</source>
        <translation>すべてを標準に設定します。</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="748"/>
        <source>xlate</source>
        <translation>xlate</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="761"/>
        <source>conn</source>
        <translation>conn</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="774"/>
        <source>udp</source>
        <translation>udp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="787"/>
        <source>rpc</source>
        <translation>rpc</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="800"/>
        <source>h323</source>
        <translation>h323</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2775"/>
        <source>sip</source>
        <translation>sip</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="826"/>
        <source>sip&amp;media</source>
        <translation>sip&amp;media</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="842"/>
        <source>unauth</source>
        <translation>unauth</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1167"/>
        <source>telnet</source>
        <translation>telnet</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1180"/>
        <source>ssh</source>
        <translation>ssh</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1193"/>
        <source>ss</source>
        <translation>ss</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1206"/>
        <source>mm</source>
        <translation>mm</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1219"/>
        <source>hh</source>
        <translation>hh</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1271"/>
        <source>half-closed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1332"/>
        <source>Inactivity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1345"/>
        <source>Absolute</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1379"/>
        <source>Inspect</source>
        <translation>監査</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1391"/>
        <source>Policy compiler generates &apos;fixup&apos; commands for PIX v6.1-6.3 and FWSM v2.3. For PIX 7.0 it generates &apos;class-map&apos; and &apos;inspect&apos; commands assigned to the &apos;policy-map&apos; under either default or custom inspection classes.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1435"/>
        <source>Enable all protocols</source>
        <translation>すべてのプロトコルを有効にする</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1442"/>
        <source>Disable all protocols</source>
        <translation>すべてのプロトコルを無効にする</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1449"/>
        <source>Skip all protocols</source>
        <translation>すべてのプロトコルを飛ばします</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1492"/>
        <source>Display generated commands</source>
        <translation>生成されたコマンドを表示する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1539"/>
        <source>ctiqbe</source>
        <translation>ctiqbe</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3379"/>
        <source>skip</source>
        <translation>飛ばす</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3384"/>
        <source>enable</source>
        <translation>有効</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3389"/>
        <source>disable</source>
        <translation>無効</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1567"/>
        <source>Computer Telephony Interface Quick Buffer Encoding (CTIQBE) protocol inspection module that supports NAT, PAT, and bi-directional NAT.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3346"/>
        <source>port:</source>
        <translation>ポート:</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1626"/>
        <source>dns</source>
        <translation>dns</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1635"/>
        <source>Based on this maximum-length configured by the user, the DNS fixup checks to see if the DNS packet length is within this limit. Every UDP DNS packet (request/response) undergoes the above check.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1648"/>
        <source>max length:</source>
        <translation>最大長:</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1713"/>
        <source>esp ike</source>
        <translation></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1722"/>
        <source>Enables PAT for Encapsulating Security Payload (ESP), single tunnel.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1771"/>
        <source>ftp</source>
        <translation>ftp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1816"/>
        <source>strict:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1864"/>
        <source>Activated support for FTP protocol and allows to change the ftp control connection port number.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1878"/>
        <source>h323 h225</source>
        <translation>h323 h225</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1887"/>
        <source>Specifies to use H.225, the ITU standard that governs H.225.0 session establishment and packetization, with H.323</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3249"/>
        <source>--</source>
        <translation>--</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="1991"/>
        <source>h323 ras</source>
        <translation>h323 ras</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2000"/>
        <source>Specifies to use RAS with H.323 to enable dissimilar communication devices to communicate with each other.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2104"/>
        <source>http</source>
        <translation>http</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2113"/>
        <source>The default port for HTTP is 80.  Use the port option to change the HTTP port, or specify a range of HTTP ports.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2217"/>
        <source>icmp error</source>
        <translation>ICMP エラー</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2226"/>
        <source>Enables NAT of ICMP error messages. This creates translations for intermediate hops based on the static or network address translation configuration on the firewall.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2275"/>
        <source>ils</source>
        <translation type="unfinished">ils</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2284"/>
        <source>Provides NAT support for Microsoft NetMeeting, SiteServer, and Active Directory products that use LightWeight Directory Access Protocol (LDAP) to exchange directory information with an for Internet Locator Service (ILS) server.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2388"/>
        <source>mgcp</source>
        <translation>mgcp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2397"/>
        <source>Enables the Media Gateway Control Protocol (MGCP) fixup.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2410"/>
        <source>Gateway Port:</source>
        <translation>ゲートウェイポート:</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2426"/>
        <source>Call Agent port:</source>
        <translation>エージェントポートを呼ぶ:</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2517"/>
        <source>pptp</source>
        <translation>pptp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2526"/>
        <source>Enables Point-to-Point Tunneling Protocol (PPTP) application inspection.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2604"/>
        <source>rsh</source>
        <translation>rsh</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2613"/>
        <source>Enables inspection of RSH protocol.</source>
        <translation>RSH プロトコルの監査を可能にします。</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2688"/>
        <source>rtsp</source>
        <translation>rtsp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2697"/>
        <source>Lets PIX Firewall pass Real Time Streaming Protocol (RTSP) packets. RTSP is used by RealAudio, RealNetworks, Apple QuickTime 4, RealPlayer, and Cisco IP/TV connections.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2784"/>
        <source>Enable or change the port assignment for the Session Initiation Protocol (SIP) for Voice over IP TCP connections.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2888"/>
        <source>sip udp</source>
        <translation>sip udp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2897"/>
        <source>Enable SIP-over-UDP application inspection.</source>
        <translation>SIP オーバー UDP アプリケーションの監査を有効にします。</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2972"/>
        <source>skinny</source>
        <translation>skinny</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="2981"/>
        <source>Enable SCCP application inspection.  SCCP protocol supports IP telephony and can coexist in an H.323 environment. An application layer ensures that all SCCP signaling and media packets can traverse the PIX Firewall and interoperate with H.323 terminals.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3085"/>
        <source>smtp</source>
        <translation>smtp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3094"/>
        <source>Enables the Mail Guard feature, which only lets mail servers receive the RFC 821, section 4.5.1, commands of HELO, MAIL, RCPT, DATA, RSET, NOOP, and QUIT. All other commands are translated into X&apos;s which are rejected by the internal server.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3198"/>
        <source>sqlnet</source>
        <translation>sqlnet</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3207"/>
        <source>Enables support for SQL*Net protocol.</source>
        <translation>SQL*Net プロトコルのサポートを有効にします。</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3311"/>
        <source>tftp</source>
        <translation>tftp</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3320"/>
        <source>Enable TFTP application inspection.</source>
        <translation>TFTP アプリケーションの監査を有効にします。</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3418"/>
        <source>Logging</source>
        <translation>ログ記録</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3468"/>
        <source>Syslog host (name or IP address):</source>
        <translation>Syslog ホスト (名前か IP アドレス):</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3481"/>
        <source>syslog facility:</source>
        <translation>syslog ファシリティ:</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3494"/>
        <source>syslog level (&apos;logging trap&apos;):</source>
        <translation>syslog レベル (&apos;logging trap&apos;):</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3513"/>
        <source>Syslog message queue size (messages):</source>
        <translation>Syslog メッセージキューの大きさ(メッセージ):</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3526"/>
        <source>PIX Firewall Version 6.3 introduces support for EMBLEM format, which is required when using the CiscoWorks Resource Manager Essentials (RME) syslog analyzer.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3529"/>
        <source>Use &apos;EMBLEM&apos; format for syslog messages</source>
        <translation>syslog で「EMBLEM「」形式
を使用する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3549"/>
        <source>Set device id for syslog messages (v6.3 and later):</source>
        <translation>syslog メッセージのデバイス ID 設定 (v6.3 以上):</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3564"/>
        <source>use address of interface</source>
        <translation>インターフェースのアドレスを使用する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3571"/>
        <source>use text string</source>
        <translation>テキスト文字列を使用する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3578"/>
        <source>use hostname</source>
        <translation>ホスト名を使用する</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3617"/>
        <source>The logging timestamp command requires that the clock command be set.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3630"/>
        <source>Enable logging timestamps on syslog file</source>
        <translation>syslog ファイルで日付の記録を有効にする</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3640"/>
        <source>Other logging destinations and levels:</source>
        <translation>その他のログ記録先とレベル:</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3649"/>
        <source>Internal buffer</source>
        <translation>内部バッファ</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3656"/>
        <source>Console</source>
        <translation>コンソール</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3689"/>
        <source>Script</source>
        <translation>スクリプト</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3716"/>
        <source>Insert comments into generated PIX configuration file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3719"/>
        <source>Comment the code</source>
        <translation>コメントコード</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3726"/>
        <source>Use ACL remarks to relate ACL commands and policy rules in the GUI</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3729"/>
        <source>Use ACL remarks</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3736"/>
        <source>Group PIX commands in the script so that similar commands appear next to each other, just like PIX does it when you use &apos;show config&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3739"/>
        <source>Group similar commands together</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3746"/>
        <source>Use manual ACL commit on FWSM</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3774"/>
        <source>Clear all access lists then install new ones. This method may interrupt access to the firewall if you manage it remotely via IPSEC tunnel. This is the way access lists were generated in older versions of Firewall Builder for PIX.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3793"/>
        <source>Do not clear access lists and object group, just generate PIX commands for the new ones. Use this optin if you have your own policy installation scripts.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3825"/>
        <source>&quot;Safety net&quot; method:

First, create temporary access list to permit connections from the management subnet specified below to the firewall and assign it to outside interface. This temporary ACL helps maintain session between management station and the firewall while access lists are reloaded in case connection comes over IPSEC tunnel. Then clear permanent lists, recreate them and assign to interfaces. This method ensures that remote access to the firewall is maintained without interruption  at a cost of slightly larger configuration.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3855"/>
        <source>Temporary access list should permit access from this address or subnet (use prefix notation to specify subnet, e.g. 192.0.2.0/24):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3968"/>
        <source>PIX Options</source>
        <translation>PIX オプション</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="3995"/>
        <source>Actively reset inbound TCP connections with RST</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4005"/>
        <source>Actively reset inbound TCP connections with RST on outside interface</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4015"/>
        <source>Force each TCP connection to linger in a shortened TIME&amp;WAIT</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4018"/>
        <source>Alt+W</source>
        <translation>Alt+W</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4025"/>
        <source>Enable the IP Frag Guard feature (deprecated in v6.3 and later).</source>
        <translation>IP フラグガード機能を有効にします(バージョン 6.3 以上では非推奨です)。</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4032"/>
        <source>Enable TCP resource control for AAA Authentication Proxy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4039"/>
        <source>Specify that when an incoming packet does a route lookup,
the incoming interface is used to determine which interface
the packet should go to, and which is the next hop
(deprecated in v6.3 and later).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4062"/>
        <source>Disable inbound embedded DNS A record fixups</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4069"/>
        <source>Disable outbound DNS A record replies</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4089"/>
        <source>maximum number of simultaneous TCP and UDP connections</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4105"/>
        <source>maximum number of embryonic connections per host</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4121"/>
        <source>Specifies the maximum number of simultaneous TCP and UDP connections for the entire subnet. The default is 0, which means unlimited connections. (Idle connections are closed after the idle timeout specified by the timeout conn command.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4137"/>
        <source>Specifies the maximum number of embryonic connections per host. An embryonic connection is a connection request that has not finished the necessary handshake between source and destination. Set a small value for slower systems, and a higher value for faster systems. The default is 0, which means unlimited embryonic connections.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4153"/>
        <source>The following parameters are used for all NAT rules:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4166"/>
        <source>(The default for both parameters is 0, which means unlimited number of connections.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4199"/>
        <source>IPv6</source>
        <translation>IPv6</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4208"/>
        <source>The order in which ipv4 and ipv6 rules should be generated:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4229"/>
        <source>IPv4 before IPv6</source>
        <translation type="unfinished">IPv4 の前に IPv6</translation>
    </message>
    <message>
        <location filename="pixadvanceddialog_q.ui" line="4234"/>
        <source>IPv6 before IPv4</source>
        <translation type="unfinished">IPv6 の前に IPv4</translation>
    </message>
</context>
<context>
    <name>pixosAdvancedDialog_q</name>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="16"/>
        <source>PIX Advanced Configuration Options</source>
        <translation>PIX 高度な設定オプション</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="44"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="68"/>
        <source>Set PIX host name using object&apos;s name</source>
        <translation>オブジェクト名を使用して PIX ホスト名を設定する</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="75"/>
        <source>Generate commands to configure addresses for interfaces</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="99"/>
        <source>NTP</source>
        <translation>NTP</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="139"/>
        <source>NTP Servers:</source>
        <translation>NTP サーバー:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="186"/>
        <source>Server 1:</source>
        <translation>サーバー 1:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="199"/>
        <source>Server 2:</source>
        <translation>サーバー 2:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="212"/>
        <source>Server 3:</source>
        <translation>サーバー 3:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="232"/>
        <source>Preffered:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="403"/>
        <source>IP address:</source>
        <translation>IP アドレス:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="262"/>
        <source>SNMP</source>
        <translation>SNMP</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="286"/>
        <source>Disable SNMP Agent</source>
        <translation>SNMP エージェントを無効にする</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="293"/>
        <source>Set SNMP communities using data from the firewall object dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="316"/>
        <source>SNMP servers</source>
        <translation>SNMP サーバー</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="366"/>
        <source>Poll</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="371"/>
        <source>Poll and Traps</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="376"/>
        <source>Traps</source>
        <translation>トラップ</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="384"/>
        <source>Enable:</source>
        <translation>有効:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="416"/>
        <source>SNMP Server 1:</source>
        <translation>SNMP サーバー1:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="429"/>
        <source>SNMP Server 2:</source>
        <translation>SNMP サーバー2:</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="445"/>
        <source>Enable sending log messages as SNMP trap notifications</source>
        <translation>ログメッセージを SNMP トラップ通知で送信を有効にする</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="453"/>
        <source>Options</source>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="477"/>
        <source>Change TCP MSS to</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="516"/>
        <source>bytes</source>
        <translation>バイト</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="582"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="pixosadvanceddialog_q.ui" line="599"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
</context>
<context>
    <name>prefsDialog_q</name>
    <message>
        <location filename="prefsdialog_q.ui" line="16"/>
        <source>Preferences</source>
        <translation>設定</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="29"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="62"/>
        <source>Working directory:</source>
        <translation>作業ディレクトリ－:</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="483"/>
        <source>Browse...</source>
        <translation>参照...</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="115"/>
        <source>Software Updates:</source>
        <translation>ソフトウェアの更新:</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="122"/>
        <source>Check for updates automatically</source>
        <translation>自動的に更新を確認する</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="142"/>
        <source>Check Now</source>
        <translation>今すぐ確認する</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="162"/>
        <source>Use http proxy while checking for updates  (host:port)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="192"/>
        <source>Objects</source>
        <translation>オブジェクト</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="217"/>
        <source>Automatically save data in dialogs when switching between objects</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="240"/>
        <source>Enable object tooltips</source>
        <translation>オブジェクトツールチップスを有効にする</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="260"/>
        <source>Tooltip delay:</source>
        <translation>ツールチップ遅延:</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="293"/>
        <source>Show deleted objects</source>
        <translation>削除されたオブジェクトを表示する</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="300"/>
        <source>Clip comments in rules</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="321"/>
        <source>Data File</source>
        <translation>データファイル</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="346"/>
        <source>Periodically save data to file every </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="375"/>
        <source>minutes</source>
        <translation>分</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="398"/>
        <source>Do not ask for the log record when checking in new file revision.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="405"/>
        <source>Enable compression of the data file</source>
        <translation>データファイルの圧縮を有効にする</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="426"/>
        <source>SSH</source>
        <translation>SSH</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="435"/>
        <source>A full path to the Secure Shell utility (remote command execution; for example ssh on Unix or plink.exe  on Windows):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="464"/>
        <source>A full path to the SCP utility (file copy over ssh; for example scp on Unix or pscp.exe on Windows):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="515"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Lucida Grande&apos;; font-size:13pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:&apos;MS Shell Dlg 2&apos;; font-size:8pt;&quot;&gt;Download plink.exe from the web site at  &lt;a href=&quot;http://www.chiark.greenend.org.uk/~sgtatham/putty/&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#0000ff;&quot;&gt;http://www.chiark.greenend.org.uk/~sgtatham/putty/&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:&apos;MS Shell Dlg 2&apos;; font-size:8pt;&quot;&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="547"/>
        <source>Labels</source>
        <translation>ラベル</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="556"/>
        <source>Use these labels to mark rules in the firewall policy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="594"/>
        <source>Red</source>
        <translation>赤色</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="601"/>
        <source>Blue</source>
        <translation>青色</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="608"/>
        <source>Yellow</source>
        <translation>黄色</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="622"/>
        <source>Orange</source>
        <translation>オレンジ色</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="657"/>
        <source>Purple</source>
        <translation>紫色</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="671"/>
        <source>Green</source>
        <translation>緑色</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="678"/>
        <source>Gray</source>
        <translation>灰色</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="717"/>
        <source>Icons</source>
        <translation>アイコン</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="720"/>
        <source>Icons settings</source>
        <translation>アイコンの設定</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="745"/>
        <source>Show icons in rules</source>
        <translation>ルールのアイコンを表示する</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="755"/>
        <source>Show text descriptions in columns &quot;Direction&quot;, &quot;Action&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="762"/>
        <source>Icon size:</source>
        <translation>アイコンの大きさ:</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="774"/>
        <source>16x16</source>
        <translation>16×16</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="781"/>
        <source>25x25</source>
        <translation>25×25</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="808"/>
        <source>Fonts</source>
        <translation>フォント</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="845"/>
        <source>Choose font for rules sets</source>
        <translation>ルールセットのフォントを選んでください</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="848"/>
        <source>Rules font </source>
        <translation>ルールのフォント</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="861"/>
        <source>rules font description</source>
        <translation>ルールフォントの説明</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="880"/>
        <source>Choose font for tree</source>
        <translation>ツリーのフォントを選んでください</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="883"/>
        <source>Tree font</source>
        <translation>ツリーのフォント</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="896"/>
        <source>tree font description</source>
        <translation>ツリーフォントの説明</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="938"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="prefsdialog_q.ui" line="954"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>printingProgressDialog_q</name>
    <message>
        <location filename="printingprogressdialog_q.ui" line="13"/>
        <source>Printing</source>
        <translation>印刷をしています</translation>
    </message>
    <message>
        <location filename="printingprogressdialog_q.ui" line="19"/>
        <source>Cancel</source>
        <translation>取り消し</translation>
    </message>
    <message>
        <location filename="printingprogressdialog_q.ui" line="61"/>
        <source>textLabel1</source>
        <translation>テキストラベル 1</translation>
    </message>
</context>
<context>
    <name>solarisAdvancedDialog_q</name>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="13"/>
        <source>Solaris: advanced settings</source>
        <translation>Solaris: 高度な設定</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="70"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="86"/>
        <source>&amp;Cancel</source>
        <translation>取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="105"/>
        <source>Options</source>
        <translation>オプション</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="129"/>
        <source>Ignore ICMP redirects</source>
        <translation>ICMP リダイレクトを無視する</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="258"/>
        <source>No change</source>
        <translation>変更なし</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="263"/>
        <source>On</source>
        <translation>オン</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="268"/>
        <source>Off</source>
        <translation>オフ</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="161"/>
        <source>Forward directed broadcasts</source>
        <translation>ダイレクトブロードキャストの転送</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="174"/>
        <source>Respond to echo broadcast</source>
        <translation>エコーブロードキャストへの応答</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="225"/>
        <source>Packet forwarding</source>
        <translation>パケット転送</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="276"/>
        <source>Forward source routed packets</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="354"/>
        <source>Path</source>
        <translation>パス</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="388"/>
        <source>ipf:</source>
        <translation>ipf:</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="401"/>
        <source>ipnat:</source>
        <translation>ipnat:</translation>
    </message>
    <message>
        <location filename="solarisadvanceddialog_q.ui" line="424"/>
        <source>Specify directory path and a file name for the following utilities on the OS your firewall machine is running. Leave these empty if you want to use default values.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:430</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Builder</source>
        <translation type="obsolete">ファイアウォールビルダー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:432</name>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:433</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Name</source>
        <translation type="obsolete">ファイアウォール名</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:434</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewalls:</source>
        <translation type="obsolete">ファイアウォール:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:435</name>
    <message>
        <location filename="" line="0"/>
        <source>Tab 1</source>
        <translation type="obsolete">タブ 1</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:436</name>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:437</name>
    <message>
        <location filename="" line="0"/>
        <source>Close</source>
        <translation type="obsolete">閉じる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:438</name>
    <message>
        <location filename="" line="0"/>
        <source>New Object File</source>
        <translation type="obsolete">新規オブジェクトファイル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:439</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;New Object File</source>
        <translation type="obsolete">新規オブジェクトファイル(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:440</name>
    <message>
        <location filename="" line="0"/>
        <source>Open</source>
        <translation type="obsolete">開く</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:441</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Open...</source>
        <translation type="obsolete">開く(&amp;O)...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:442</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+O</source>
        <translation type="obsolete">Ctrl+O</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:443</name>
    <message>
        <location filename="" line="0"/>
        <source>Save</source>
        <translation type="obsolete">保存</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:444</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Save</source>
        <translation type="obsolete">保存(&amp;S)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:445</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+S</source>
        <translation type="obsolete">Ctrl+S</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:446</name>
    <message>
        <location filename="" line="0"/>
        <source>Save As</source>
        <translation type="obsolete">名前を付けて保存</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:447</name>
    <message>
        <location filename="" line="0"/>
        <source>Save &amp;As...</source>
        <translation type="obsolete">名前を付けて保存(&amp;A)...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:449</name>
    <message>
        <location filename="" line="0"/>
        <source>Print</source>
        <translation type="obsolete">印刷</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:450</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Print...</source>
        <translation type="obsolete">印刷(&amp;P)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:451</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+P</source>
        <translation type="obsolete">Ctrl+P</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:452</name>
    <message>
        <location filename="" line="0"/>
        <source>Exit</source>
        <translation type="obsolete">終了</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:453</name>
    <message>
        <location filename="" line="0"/>
        <source>E&amp;xit</source>
        <translation type="obsolete">終了(&amp;X)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:455</name>
    <message>
        <location filename="" line="0"/>
        <source>Undo</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:456</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Undo</source>
        <translation type="obsolete">取り消し(&amp;U)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:457</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+Z</source>
        <translation type="obsolete">Ctrl+Z</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:458</name>
    <message>
        <location filename="" line="0"/>
        <source>Redo</source>
        <translation type="obsolete">やり直し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:459</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Redo</source>
        <translation type="obsolete">やり直し(&amp;R)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:460</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+Y</source>
        <translation type="obsolete">Ctrl+Y</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:461</name>
    <message>
        <location filename="" line="0"/>
        <source>Cut</source>
        <translation type="obsolete">切り取り</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:462</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cut</source>
        <translation type="obsolete">切り取り(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:463</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+X</source>
        <translation type="obsolete">Ctrl+X</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:464</name>
    <message>
        <location filename="" line="0"/>
        <source>Copy</source>
        <translation type="obsolete">コピー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:465</name>
    <message>
        <location filename="" line="0"/>
        <source>C&amp;opy</source>
        <translation type="obsolete">コピー(&amp;O)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:466</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+C</source>
        <translation type="obsolete">Ctrl+C</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:467</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste</source>
        <translation type="obsolete">貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:468</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Paste</source>
        <translation type="obsolete">貼り付け(&amp;P)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:469</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+V</source>
        <translation type="obsolete">Ctrl+V</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:472</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+F</source>
        <translation type="obsolete">Ctrl+F</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:473</name>
    <message>
        <location filename="" line="0"/>
        <source>Contents</source>
        <translation type="obsolete">内容</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:474</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Contents...</source>
        <translation type="obsolete">内容(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:476</name>
    <message>
        <location filename="" line="0"/>
        <source>Index</source>
        <translation type="obsolete">インデックス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:477</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Index...</source>
        <translation type="obsolete">インデックス...(&amp;I)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:479</name>
    <message>
        <location filename="" line="0"/>
        <source>About</source>
        <translation type="obsolete">ファイアウォールビルダーについて</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:480</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;About</source>
        <translation type="obsolete">ファイアウォールビルダーについて(&amp;A)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:482</name>
    <message>
        <location filename="" line="0"/>
        <source>New</source>
        <translation type="obsolete">新規</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:483</name>
    <message>
        <location filename="" line="0"/>
        <source>New</source>
        <translation type="obsolete">新規</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:484</name>
    <message>
        <location filename="" line="0"/>
        <source>Open</source>
        <translation type="obsolete">開く</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:485</name>
    <message>
        <location filename="" line="0"/>
        <source>Open</source>
        <translation type="obsolete">開く</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:486</name>
    <message>
        <location filename="" line="0"/>
        <source>Save</source>
        <translation type="obsolete">保存</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:487</name>
    <message>
        <location filename="" line="0"/>
        <source>Save</source>
        <translation type="obsolete">保存</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:488</name>
    <message>
        <location filename="" line="0"/>
        <source>Close</source>
        <translation type="obsolete">閉じる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:489</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Close</source>
        <translation type="obsolete">閉じる(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:490</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile</source>
        <translation type="obsolete">コンパイル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:491</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile</source>
        <translation type="obsolete">コンパイル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:492</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile rules</source>
        <translation type="obsolete">コンパイルルール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:493</name>
    <message>
        <location filename="" line="0"/>
        <source>Install</source>
        <translation type="obsolete">インストール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:494</name>
    <message>
        <location filename="" line="0"/>
        <source>Install</source>
        <translation type="obsolete">インストール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:495</name>
    <message>
        <location filename="" line="0"/>
        <source>Install firewall policy</source>
        <translation type="obsolete">ファイアウォールポリシーのインストール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:496</name>
    <message>
        <location filename="" line="0"/>
        <source>Back</source>
        <translation type="obsolete">戻る</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:497</name>
    <message>
        <location filename="" line="0"/>
        <source>Back</source>
        <translation type="obsolete">戻る</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:498</name>
    <message>
        <location filename="" line="0"/>
        <source>Move back to the previous object</source>
        <translation type="obsolete">前のオブジェクトに戻る</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:499</name>
    <message>
        <location filename="" line="0"/>
        <source>Move back to the previous object</source>
        <translation type="obsolete">前のオブジェクトに戻る</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:500</name>
    <message>
        <location filename="" line="0"/>
        <source>New Object</source>
        <translation type="obsolete">新規オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:501</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;New Object</source>
        <translation type="obsolete">新規オブジェクト(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:502</name>
    <message>
        <location filename="" line="0"/>
        <source>Create New Object</source>
        <translation type="obsolete">新規オブジェクト作成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:503</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+N</source>
        <translation type="obsolete">Ctrl+N</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:504</name>
    <message>
        <location filename="" line="0"/>
        <source>Find Object</source>
        <translation type="obsolete">オブジェクト検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:505</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Find Object</source>
        <translation type="obsolete">オブジェクト検索(&amp;F)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:506</name>
    <message>
        <location filename="" line="0"/>
        <source>Find object in the tree</source>
        <translation type="obsolete">ツリー中のオブジェクトを検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:507</name>
    <message>
        <location filename="" line="0"/>
        <source>Find object in the tree</source>
        <translation type="obsolete">ツリー中のオブジェクトを検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:508</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+F</source>
        <translation type="obsolete">Ctrl+F</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:509</name>
    <message>
        <location filename="" line="0"/>
        <source>Preferences...</source>
        <translation type="obsolete">設定...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:510</name>
    <message>
        <location filename="" line="0"/>
        <source>P&amp;references...</source>
        <translation type="obsolete">設定(&amp;E)...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:511</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit Preferences</source>
        <translation type="obsolete">設定の編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:512</name>
    <message>
        <location filename="" line="0"/>
        <source>Insert Rule</source>
        <translation type="obsolete">ルール挿入</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:513</name>
    <message>
        <location filename="" line="0"/>
        <source>Insert Rule</source>
        <translation type="obsolete">ルール挿入</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:514</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Rule Up</source>
        <translation type="obsolete">ルールを上に移動</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:515</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Rule Up</source>
        <translation type="obsolete">ルールを上に移動</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:516</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Rule Down</source>
        <translation type="obsolete">ルールを下に移動</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:517</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Rule Down</source>
        <translation type="obsolete">ルールを下に移動</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:518</name>
    <message>
        <location filename="" line="0"/>
        <source>Add Rule Below</source>
        <translation type="obsolete">下にルール追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:519</name>
    <message>
        <location filename="" line="0"/>
        <source>Add Rule Below</source>
        <translation type="obsolete">下にルール追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:520</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove Rule</source>
        <translation type="obsolete">ルール削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:521</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove Rule</source>
        <translation type="obsolete">ルール削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:522</name>
    <message>
        <location filename="" line="0"/>
        <source>Ctrl+Del</source>
        <translation type="obsolete">Ctrl+Del</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:523</name>
    <message>
        <location filename="" line="0"/>
        <source>Copy Rule</source>
        <translation type="obsolete">ルールコピー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:524</name>
    <message>
        <location filename="" line="0"/>
        <source>Copy Rule</source>
        <translation type="obsolete">ルールコピー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:525</name>
    <message>
        <location filename="" line="0"/>
        <source>Cut Rule</source>
        <translation type="obsolete">ルール切り取り</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:526</name>
    <message>
        <location filename="" line="0"/>
        <source>Cut Rule</source>
        <translation type="obsolete">ルール切り取り</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:527</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule Above</source>
        <translation type="obsolete">上にルールを貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:528</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule Above</source>
        <translation type="obsolete">上にルールを貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:529</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule Below</source>
        <translation type="obsolete">下にルールを貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:530</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule Below</source>
        <translation type="obsolete">下にルールを貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:531</name>
    <message>
        <location filename="" line="0"/>
        <source>Add File to RCS</source>
        <translation type="obsolete">RCS にファイルを追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:532</name>
    <message>
        <location filename="" line="0"/>
        <source>Add File to &amp;RCS</source>
        <translation type="obsolete">RCS にファイルを追加(&amp;R)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:533</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:534</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:535</name>
    <message>
        <location filename="" line="0"/>
        <source>Export Library To a File</source>
        <translation type="obsolete">ファイルへライブラリーをエクスポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:536</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Export Library</source>
        <translation type="obsolete">ライブラリーエクスポート(&amp;E)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:537</name>
    <message>
        <location filename="" line="0"/>
        <source>Import Library From a File</source>
        <translation type="obsolete">ファイルからライブラリーをインポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:538</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Import Library</source>
        <translation type="obsolete">ライブラリーインポート(&amp;I)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:539</name>
    <message>
        <location filename="" line="0"/>
        <source>Debug</source>
        <translation type="obsolete">デバッグ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:540</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Debug</source>
        <translation type="obsolete">デバッグ(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:541</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Properties</source>
        <translation type="obsolete">プロパティ(&amp;P)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:542</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Properties</source>
        <translation type="obsolete">プロパティ(&amp;P)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:543</name>
    <message>
        <location filename="" line="0"/>
        <source>Show File Properties</source>
        <translation type="obsolete">ファイルのプロパティを表示する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:544</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Selected Rules</source>
        <translation type="obsolete">選択したルールを移動する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:545</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Selected Rules</source>
        <translation type="obsolete">選択したルールを移動する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:546</name>
    <message>
        <location filename="" line="0"/>
        <source>Discard</source>
        <translation type="obsolete">やり直し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:547</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Discard</source>
        <translation type="obsolete">破棄する(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:548</name>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:549</name>
    <message>
        <location filename="" line="0"/>
        <source>Commit</source>
        <translation type="obsolete">コミットする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:550</name>
    <message>
        <location filename="" line="0"/>
        <source>C&amp;ommit</source>
        <translation type="obsolete">コメント(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:551</name>
    <message>
        <location filename="" line="0"/>
        <source>Commit Opened File to RCS and Continue Editing</source>
        <translation type="obsolete">開いているファイルをRCS にコミットして編集を続ける</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:552</name>
    <message>
        <location filename="" line="0"/>
        <source>Lock</source>
        <translation type="obsolete">ロックする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:553</name>
    <message>
        <location filename="" line="0"/>
        <source>Lock</source>
        <translation type="obsolete">ロックする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:554</name>
    <message>
        <location filename="" line="0"/>
        <source>Unlock</source>
        <translation type="obsolete">ロック解除する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:555</name>
    <message>
        <location filename="" line="0"/>
        <source>Unlock</source>
        <translation type="obsolete">ロック解除する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:556</name>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:557</name>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:558</name>
    <message>
        <location filename="" line="0"/>
        <source>new item</source>
        <translation type="obsolete">新規項目</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:559</name>
    <message>
        <location filename="" line="0"/>
        <source>new item</source>
        <translation type="obsolete">新規項目</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:560</name>
    <message>
        <location filename="" line="0"/>
        <source>Toolbar</source>
        <translation type="obsolete">ツールバー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:562</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;File</source>
        <translation type="obsolete">ファイル(&amp;F)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:564</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Edit</source>
        <translation type="obsolete">編集(&amp;E)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:566</name>
    <message>
        <location filename="" line="0"/>
        <source>Object</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:568</name>
    <message>
        <location filename="" line="0"/>
        <source>Rules</source>
        <translation type="obsolete">ルール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:570</name>
    <message>
        <location filename="" line="0"/>
        <source>Tools</source>
        <translation type="obsolete">ツール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/FWBMainWindow_q.cpp:572</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Help</source>
        <translation type="obsolete">ヘルプ(&amp;H)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/aboutdialog_q.cpp:135</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Builder</source>
        <translation type="obsolete">ファイアウォールビルダー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/aboutdialog_q.cpp:136</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Builder</source>
        <translation type="obsolete">ファイアウォールビルダー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/aboutdialog_q.cpp:137</name>
    <message>
        <location filename="" line="0"/>
        <source>Using libfwbuilder API v</source>
        <translation type="obsolete">試用中の libfwbuilder API バージョン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/aboutdialog_q.cpp:138</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision: </source>
        <translation type="obsolete">リビジョン: </translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/aboutdialog_q.cpp:139</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/aboutdialog_q.cpp:141</name>
    <message>
        <location filename="" line="0"/>
        <source>Copyright 2002-2006  NetCitadel, LLC</source>
        <translation type="obsolete">Copyright 2002-2006  NetCitadel, LLC</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/aboutdialog_q.cpp:142</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;p align=&quot;center&quot;&gt;&lt;a href=&quot;http://www.fwbuilder.org/&quot;&gt;http://www.fwbuilder.org&lt;/a&gt;&lt;/p&gt;</source>
        <translation type="obsolete">&lt;a href=\&quot;http://www.fwbuilder.org/\&quot;&gt;http://www.fwbuilder.org&lt;/a&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:437</name>
    <message>
        <location filename="" line="0"/>
        <source>Actions Dialog</source>
        <translation type="obsolete">操作ダイアログ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:438</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:439</name>
    <message>
        <location filename="" line="0"/>
        <source>Tag string:</source>
        <translation type="obsolete">タグ文字列:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:440</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:441</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:442</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:443</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:444</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:445</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:446</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:447</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:449</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:450</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:451</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:452</name>
    <message>
        <location filename="" line="0"/>
        <source>Custom string:</source>
        <translation type="obsolete">テキスト文字列を使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:453</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:454</name>
    <message>
        <location filename="" line="0"/>
        <source>Divert socket port number:</source>
        <translation type="obsolete">プロトコル番号:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:455</name>
    <message>
        <location filename="" line="0"/>
        <source>User-defined chain name:</source>
        <translation type="obsolete">ユーザー定義チェイン名:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:456</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:457</name>
    <message>
        <location filename="" line="0"/>
        <source>Anchor name:</source>
        <translation type="obsolete">著者名</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:459</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:460</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:461</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:462</name>
    <message>
        <location filename="" line="0"/>
        <source>interface</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:463</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:464</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:466</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:467</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:468</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:469</name>
    <message>
        <location filename="" line="0"/>
        <source>interface</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:470</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:471</name>
    <message>
        <location filename="" line="0"/>
        <source>Change inbound interface to</source>
        <translation type="obsolete">インターフェンス管理</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:472</name>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:473</name>
    <message>
        <location filename="" line="0"/>
        <source>Change outbound interface to</source>
        <translation type="obsolete">インターフェンス管理</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:474</name>
    <message>
        <location filename="" line="0"/>
        <source>Continue packet inspection</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/actionsdialog_q.cpp:475</name>
</context>
<context>
    <name>src/gui/.ui/addressrangedialog_q.cpp:162</name>
    <message>
        <location filename="" line="0"/>
        <source>Address Range</source>
        <translation type="obsolete">アドレスレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addressrangedialog_q.cpp:163</name>
    <message>
        <location filename="" line="0"/>
        <source>Address Range</source>
        <translation type="obsolete">アドレスレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addressrangedialog_q.cpp:164</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addressrangedialog_q.cpp:165</name>
    <message>
        <location filename="" line="0"/>
        <source>Range End:</source>
        <translation type="obsolete">レンジ終了</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addressrangedialog_q.cpp:166</name>
    <message>
        <location filename="" line="0"/>
        <source>Range Start:</source>
        <translation type="obsolete">レンジ開始</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addressrangedialog_q.cpp:167</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addressrangedialog_q.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:187</name>
    <message>
        <location filename="" line="0"/>
        <source>Address Table</source>
        <translation type="obsolete">アドレステーブル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>Address Table</source>
        <translation type="obsolete">アドレステーブル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:189</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:191</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:193</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile Time</source>
        <translation type="obsolete">コンパイル時間</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:194</name>
    <message>
        <location filename="" line="0"/>
        <source>Run Time</source>
        <translation type="obsolete">ランタイム</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:195</name>
    <message>
        <location filename="" line="0"/>
        <source>File name:</source>
        <translation type="obsolete">ファイル名:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:196</name>
    <message>
        <location filename="" line="0"/>
        <source>Browse</source>
        <translation type="obsolete">閲覧</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:197</name>
    <message>
        <location filename="" line="0"/>
        <source>Browse</source>
        <translation type="obsolete">閲覧</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/addresstabledialog_q.cpp:198</name>
    <message>
        <location filename="" line="0"/>
        <source>Preview</source>
        <translation type="obsolete">プレビュー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/askrulenumberdialog_q.cpp:86</name>
    <message>
        <location filename="" line="0"/>
        <source>Enter New Position For The Rule</source>
        <translation type="obsolete">ルールの&lt;新しい位置を入力する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/askrulenumberdialog_q.cpp:87</name>
    <message>
        <location filename="" line="0"/>
        <source>Enter new position for selected rules:</source>
        <translation type="obsolete">選択されたルール群の新しい位置を入力する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/askrulenumberdialog_q.cpp:88</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Move</source>
        <translation type="obsolete">移動(&amp;M)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/askrulenumberdialog_q.cpp:89</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+M</source>
        <translation type="obsolete">Alt+M</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/askrulenumberdialog_q.cpp:90</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/askrulenumberdialog_q.cpp:91</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+C</source>
        <translation type="obsolete">Alt+C</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:108</name>
    <message>
        <location filename="" line="0"/>
        <source>Orange</source>
        <translation type="obsolete">オレンジ色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:110</name>
    <message>
        <location filename="" line="0"/>
        <source>Green</source>
        <translation type="obsolete">緑色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:112</name>
    <message>
        <location filename="" line="0"/>
        <source>Purple</source>
        <translation type="obsolete">紫色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:114</name>
    <message>
        <location filename="" line="0"/>
        <source>Blue</source>
        <translation type="obsolete">青色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:116</name>
    <message>
        <location filename="" line="0"/>
        <source>Yellow</source>
        <translation type="obsolete">黄色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:118</name>
    <message>
        <location filename="" line="0"/>
        <source>Gray</source>
        <translation type="obsolete">灰色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:120</name>
    <message>
        <location filename="" line="0"/>
        <source>Red</source>
        <translation type="obsolete">赤色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/colorlabelmenuitem_q.cpp:122</name>
    <message>
        <location filename="" line="0"/>
        <source>No color</source>
        <translation type="obsolete">色なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/commenteditorpanel_q.cpp:95</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment Editor Panel</source>
        <translation type="obsolete">コメントエディタパネル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/commenteditorpanel_q.cpp:96</name>
</context>
<context>
    <name>src/gui/.ui/commenteditorpanel_q.cpp:98</name>
    <message>
        <location filename="" line="0"/>
        <source>Import from file ...</source>
        <translation type="obsolete">ファイルからインポート ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:104</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Builder</source>
        <translation type="obsolete">ファイアウォールビルダー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:105</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:106</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:107</name>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:108</name>
    <message>
        <location filename="" line="0"/>
        <source>Object</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:109</name>
    <message>
        <location filename="" line="0"/>
        <source>Parent</source>
        <translation type="obsolete">親</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:110</name>
    <message>
        <location filename="" line="0"/>
        <source>Details</source>
        <translation type="obsolete">詳細</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:111</name>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:64</name>
    <message>
        <location filename="" line="0"/>
        <source>Object</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:65</name>
    <message>
        <location filename="" line="0"/>
        <source>Parent</source>
        <translation type="obsolete">親</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/confirmdeleteobjectdialog_q.cpp:66</name>
    <message>
        <location filename="" line="0"/>
        <source>Details</source>
        <translation type="obsolete">詳細</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>Custom Service</source>
        <translation type="obsolete">カスタムサービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:178</name>
    <message>
        <location filename="" line="0"/>
        <source>Custom Service</source>
        <translation type="obsolete">カスタムサービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:179</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:181</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:182</name>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:183</name>
    <message>
        <location filename="" line="0"/>
        <source>Platform:</source>
        <translation type="obsolete">プラットフォーム:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/customservicedialog_q.cpp:184</name>
    <message>
        <location filename="" line="0"/>
        <source>Code:</source>
        <translation type="obsolete">コード:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/debugdialog_q.cpp:73</name>
    <message>
        <location filename="" line="0"/>
        <source>Debugging Info</source>
        <translation type="obsolete">デバグ情報</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/debugdialog_q.cpp:74</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Close</source>
        <translation type="obsolete">閉じる(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/debugdialog_q.cpp:75</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+C</source>
        <translation type="obsolete">Alt+C</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:688</name>
    <message>
        <location filename="" line="0"/>
        <source>Object</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:690</name>
    <message>
        <location filename="" line="0"/>
        <source>Interfaces</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:692</name>
    <message>
        <location filename="" line="0"/>
        <source>Type</source>
        <translation type="obsolete">種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:800</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:801</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:802</name>
    <message>
        <location filename="" line="0"/>
        <source>Discovery method:</source>
        <translation type="obsolete">検出方法</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:803</name>
    <message>
        <location filename="" line="0"/>
        <source>Read file in hosts format</source>
        <translation type="obsolete">hosts 形式のファイルを読み込む</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:804</name>
    <message>
        <location filename="" line="0"/>
        <source>Import DNS zone</source>
        <translation type="obsolete">DNS ゾーンをインポートをする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:805</name>
    <message>
        <location filename="" line="0"/>
        <source>Perform network discovery using SNMP</source>
        <translation type="obsolete">SNMP を用いてネットワークの検出を実行する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:806</name>
    <message>
        <location filename="" line="0"/>
        <source>Discovery Method</source>
        <translation type="obsolete">検出方法</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:807</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:808</name>
    <message>
        <location filename="" line="0"/>
        <source>File in hosts format</source>
        <translation type="obsolete">hosts 形式のファイル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:809</name>
    <message>
        <location filename="" line="0"/>
        <source>Browse ...</source>
        <translation type="obsolete">参照 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:810</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:811</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:813</name>
    <message>
        <location filename="" line="0"/>
        <source>Domain name</source>
        <translation type="obsolete">ドメイン名</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:814</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:815</name>
    <message>
        <location filename="" line="0"/>
        <source>Use long names</source>
        <translation type="obsolete">ユーザー名:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:816</name>
    <message>
        <location filename="" line="0"/>
        <source>Import DNS zone</source>
        <translation type="obsolete">DNS ゾーンをインポートをする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:817</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:818</name>
    <message>
        <location filename="" line="0"/>
        <source>Name server</source>
        <translation type="obsolete">ネームサーバー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:819</name>
    <message>
        <location filename="" line="0"/>
        <source>choose name server from the list below</source>
        <translation type="obsolete">下の一覧からネームサーバーを選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:820</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:822</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS Query options</source>
        <translation type="obsolete">ルールオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:823</name>
    <message>
        <location filename="" line="0"/>
        <source>Timeout (sec)</source>
        <translation type="obsolete">TCP FIN タイムアウト(秒)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:824</name>
    <message>
        <location filename="" line="0"/>
        <source>Retries</source>
        <translation type="obsolete">サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:825</name>
    <message>
        <location filename="" line="0"/>
        <source>Name server</source>
        <translation type="obsolete">ネームサーバー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:826</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:827</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:829</name>
    <message>
        <location filename="" line="0"/>
        <source>Enter a valid host name or address.</source>
        <translation type="obsolete">正しいホスト銘菓アドレスを入力してください。</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:830</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:831</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:832</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask:</source>
        <translation type="obsolete">ネットマスク:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:833</name>
    <message>
        <location filename="" line="0"/>
        <source>Address:</source>
        <translation type="obsolete">アドレス:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:835</name>
    <message>
        <location filename="" line="0"/>
        <source>Network discovery using SNMP</source>
        <translation type="obsolete">SNMP を用いてインターフェースを検出する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:836</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:837</name>
    <message>
        <location filename="" line="0"/>
        <source>Run network scan recursively</source>
        <translation type="obsolete">ネットワークの再起スキャンを実行する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:838</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:839</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:840</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:841</name>
    <message>
        <location filename="" line="0"/>
        <source>Include virtual addresses</source>
        <translation type="obsolete">NAT 用バーチャルアドレス群の追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:842</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:843</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:844</name>
    <message>
        <location filename="" line="0"/>
        <source>Network scan options</source>
        <translation type="obsolete">ネットワークゾーン:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:845</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:846</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP query parameters:</source>
        <translation type="obsolete">フラグメント</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:847</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP &apos;read&apos; community string:</source>
        <translation type="obsolete">SNMP &apos;read&apos; コミュニティ文字列</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:848</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:849</name>
    <message>
        <location filename="" line="0"/>
        <source>timeout (sec):</source>
        <translation type="obsolete">タイムアウト(秒):</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:850</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:851</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS parameters:</source>
        <translation type="obsolete">DNS パラメーター:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:852</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:853</name>
    <message>
        <location filename="" line="0"/>
        <source>timeout (sec) :</source>
        <translation type="obsolete">タイムアウト(秒) :</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:854</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:856</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:858</name>
    <message>
        <location filename="" line="0"/>
        <source>Process name</source>
        <translation type="obsolete">処理名</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:860</name>
    <message>
        <location filename="" line="0"/>
        <source>Stop</source>
        <translation type="obsolete">停止</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:861</name>
    <message>
        <location filename="" line="0"/>
        <source>Save scan log to file</source>
        <translation type="obsolete">スキャンログをファイルに保存する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:862</name>
    <message>
        <location filename="" line="0"/>
        <source>Process log:</source>
        <translation type="obsolete">処理ログ:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:863</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:864</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:865</name>
    <message>
        <location filename="" line="0"/>
        <source>Select All</source>
        <translation type="obsolete">すべて選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:866</name>
    <message>
        <location filename="" line="0"/>
        <source>Filter ...</source>
        <translation type="obsolete">フィルター ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:867</name>
    <message>
        <location filename="" line="0"/>
        <source>Unselect All</source>
        <translation type="obsolete">すべて選択をやめる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:868</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove Filter</source>
        <translation type="obsolete">フィルターを削除する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:869</name>
    <message>
        <location filename="" line="0"/>
        <source>-&gt;</source>
        <translation type="obsolete">→</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:870</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;-</source>
        <translation type="obsolete">←</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:871</name>
    <message>
        <location filename="" line="0"/>
        <source>Select All</source>
        <translation type="obsolete">すべて選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:872</name>
    <message>
        <location filename="" line="0"/>
        <source>Unselect All</source>
        <translation type="obsolete">すべて選択をやめる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:873</name>
    <message>
        <location filename="" line="0"/>
        <source>Networks</source>
        <translation type="obsolete">ネットワーク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:874</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:875</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove Filter</source>
        <translation type="obsolete">フィルターを削除する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:876</name>
    <message>
        <location filename="" line="0"/>
        <source>Select All</source>
        <translation type="obsolete">すべて選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:877</name>
    <message>
        <location filename="" line="0"/>
        <source>Filter ...</source>
        <translation type="obsolete">フィルター ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:878</name>
    <message>
        <location filename="" line="0"/>
        <source>Unselect All</source>
        <translation type="obsolete">すべて選択をやめる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:879</name>
    <message>
        <location filename="" line="0"/>
        <source>-&gt;</source>
        <translation type="obsolete">→</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:880</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;-</source>
        <translation type="obsolete">←</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:881</name>
    <message>
        <location filename="" line="0"/>
        <source>Select All</source>
        <translation type="obsolete">すべて選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:882</name>
    <message>
        <location filename="" line="0"/>
        <source>Unselect All</source>
        <translation type="obsolete">すべて選択をやめる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:883</name>
    <message>
        <location filename="" line="0"/>
        <source>Objects</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:884</name>
    <message>
        <location filename="" line="0"/>
        <source>Unselect All</source>
        <translation type="obsolete">すべて選択をやめる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:885</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove Filter</source>
        <translation type="obsolete">フィルターを削除する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:886</name>
    <message>
        <location filename="" line="0"/>
        <source>Filter ...</source>
        <translation type="obsolete">フィルター ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:887</name>
    <message>
        <location filename="" line="0"/>
        <source>Select All</source>
        <translation type="obsolete">すべて選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:888</name>
    <message>
        <location filename="" line="0"/>
        <source>Change type of selected objects:</source>
        <translation type="obsolete">選択されたオブジェクトの種類の変更:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:889</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:890</name>
    <message>
        <location filename="" line="0"/>
        <source>Host</source>
        <translation type="obsolete">ホスト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:891</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:892</name>
    <message>
        <location filename="" line="0"/>
        <source>Object</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:893</name>
    <message>
        <location filename="" line="0"/>
        <source>Interfaces</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:894</name>
    <message>
        <location filename="" line="0"/>
        <source>Type</source>
        <translation type="obsolete">種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:895</name>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:896</name>
    <message>
        <location filename="" line="0"/>
        <source>Adjust Object types</source>
        <translation type="obsolete">オブジェクトの種類の調整</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:897</name>
    <message>
        <location filename="" line="0"/>
        <source>Select target library</source>
        <translation type="obsolete">ターゲットライブラリーを選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:898</name>
    <message>
        <location filename="" line="0"/>
        <source>Target library</source>
        <translation type="obsolete">ターゲットライブラリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:899</name>
    <message>
        <location filename="" line="0"/>
        <source>Adding new objects to library ...</source>
        <translation type="obsolete">ライブラリーに新しいオブジェクトを追加しています ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/discoverydruid_q.cpp:900</name>
    <message>
        <location filename="" line="0"/>
        <source>Creatnig objects</source>
        <translation type="obsolete">新規オブジェクト作成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:164</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS Name</source>
        <translation type="obsolete">DNS 名</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:165</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS Name</source>
        <translation type="obsolete">DNS 名</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:166</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile Time</source>
        <translation type="obsolete">コンパイル時間</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:169</name>
    <message>
        <location filename="" line="0"/>
        <source>Run Time</source>
        <translation type="obsolete">ランタイム</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:170</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/dnsnamedialog_q.cpp:172</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS Record:</source>
        <translation type="obsolete">DNS レコード:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/execdialog_q.cpp:90</name>
    <message>
        <location filename="" line="0"/>
        <source>Executing external command</source>
        <translation type="obsolete">外部コマンドの実行</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/execdialog_q.cpp:91</name>
    <message>
        <location filename="" line="0"/>
        <source>Save log to file</source>
        <translation type="obsolete">ファイルにログを保存する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/execdialog_q.cpp:92</name>
    <message>
        <location filename="" line="0"/>
        <source>Stop</source>
        <translation type="obsolete">停止</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/execdialog_q.cpp:93</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Close</source>
        <translation type="obsolete">閉じる(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/execdialog_q.cpp:94</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+C</source>
        <translation type="obsolete">Alt+C</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:109</name>
    <message>
        <location filename="" line="0"/>
        <source>File Properties</source>
        <translation type="obsolete">ファイルプロパティ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:110</name>
    <message>
        <location filename="" line="0"/>
        <source>Location:</source>
        <translation type="obsolete">場所:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:111</name>
    <message>
        <location filename="" line="0"/>
        <source>RO</source>
        <translation type="obsolete">RO</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:112</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision Control:</source>
        <translation type="obsolete">リビジョン制御</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:113</name>
    <message>
        <location filename="" line="0"/>
        <source>Time of last modification:</source>
        <translation type="obsolete">最終修正の時間</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:114</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision:</source>
        <translation type="obsolete">リビジョン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:115</name>
    <message>
        <location filename="" line="0"/>
        <source>Locked by user:</source>
        <translation type="obsolete">ユーザーがロック中:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:116</name>
    <message>
        <location filename="" line="0"/>
        <source>location</source>
        <translation type="obsolete">場所</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:117</name>
    <message>
        <location filename="" line="0"/>
        <source>lastModified</source>
        <translation type="obsolete">最終更新日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:118</name>
    <message>
        <location filename="" line="0"/>
        <source>rev</source>
        <translation type="obsolete">リビジョン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:119</name>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:120</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision history:</source>
        <translation type="obsolete">リビジョン履歴:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:121</name>
    <message>
        <location filename="" line="0"/>
        <source>OK</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filepropdialog_q.cpp:122</name>
    <message>
        <location filename="" line="0"/>
        <source>Print</source>
        <translation type="obsolete">印刷</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:139</name>
    <message>
        <location filename="" line="0"/>
        <source>Filter</source>
        <translation type="obsolete">フィルター</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:140</name>
    <message>
        <location filename="" line="0"/>
        <source>Save</source>
        <translation type="obsolete">保存</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:141</name>
    <message>
        <location filename="" line="0"/>
        <source>Load</source>
        <translation type="obsolete">読み込み</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:142</name>
    <message>
        <location filename="" line="0"/>
        <source>Ok</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:143</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:144</name>
    <message>
        <location filename="" line="0"/>
        <source>Match</source>
        <translation type="obsolete">一致</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:146</name>
    <message>
        <location filename="" line="0"/>
        <source>all</source>
        <translation type="obsolete">すべて</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:147</name>
    <message>
        <location filename="" line="0"/>
        <source>any</source>
        <translation type="obsolete">どれか</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:148</name>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:150</name>
    <message>
        <location filename="" line="0"/>
        <source>+</source>
        <translation type="obsolete">+</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:151</name>
    <message>
        <location filename="" line="0"/>
        <source>Add a new pattern</source>
        <translation type="obsolete">パターンを追加する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:152</name>
    <message>
        <location filename="" line="0"/>
        <source>Target</source>
        <translation type="obsolete">ターゲット</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:153</name>
    <message>
        <location filename="" line="0"/>
        <source>Type</source>
        <translation type="obsolete">種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:154</name>
    <message>
        <location filename="" line="0"/>
        <source>Pattern</source>
        <translation type="obsolete">パターン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:155</name>
    <message>
        <location filename="" line="0"/>
        <source>Case sensitive</source>
        <translation type="obsolete">英大小文字の区別</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:156</name>
    <message>
        <location filename="" line="0"/>
        <source>-</source>
        <translation type="obsolete">-</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:157</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove a pattern</source>
        <translation type="obsolete">パターンを削除する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:87</name>
    <message>
        <location filename="" line="0"/>
        <source>Target</source>
        <translation type="obsolete">ターゲット</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:89</name>
    <message>
        <location filename="" line="0"/>
        <source>Type</source>
        <translation type="obsolete">種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/filterdialog_q.cpp:91</name>
    <message>
        <location filename="" line="0"/>
        <source>Pattern</source>
        <translation type="obsolete">パターン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:126</name>
    <message>
        <location filename="" line="0"/>
        <source>Find Object</source>
        <translation type="obsolete">オブジェクト検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:127</name>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:128</name>
    <message>
        <location filename="" line="0"/>
        <source>Search in policy rules</source>
        <translation type="obsolete">ポリシールール群から検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:129</name>
    <message>
        <location filename="" line="0"/>
        <source>Search in the tree</source>
        <translation type="obsolete">ツリーから検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:130</name>
    <message>
        <location filename="" line="0"/>
        <source>Find</source>
        <translation type="obsolete">検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:131</name>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:133</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:134</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP/UDP port</source>
        <translation type="obsolete">TCP/UDP ポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:135</name>
    <message>
        <location filename="" line="0"/>
        <source>Protocol number</source>
        <translation type="obsolete">プロトコル番号</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:136</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP type</source>
        <translation type="obsolete">ICMP の種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/finddialog_q.cpp:137</name>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:171</name>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:172</name>
    <message>
        <location filename="" line="0"/>
        <source> Find object</source>
        <translation type="obsolete">オブジェクト検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:174</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:175</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:176</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP/UDP port</source>
        <translation type="obsolete">TCP/UDP ポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>Protocol number</source>
        <translation type="obsolete">プロトコル番号</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:178</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP type</source>
        <translation type="obsolete">ICMP の種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:179</name>
    <message>
        <location filename="" line="0"/>
        <source> Replace object </source>
        <translation type="obsolete">このオブジェクトと置換</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>Replace &amp;&amp; Find</source>
        <translation type="obsolete">置換と検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:182</name>
    <message>
        <location filename="" line="0"/>
        <source>Next</source>
        <translation type="obsolete">次へ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:183</name>
    <message>
        <location filename="" line="0"/>
        <source>Replace all</source>
        <translation type="obsolete">すべて置換する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:184</name>
    <message>
        <location filename="" line="0"/>
        <source>Replace</source>
        <translation type="obsolete">置換する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:185</name>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:187</name>
    <message>
        <location filename="" line="0"/>
        <source>Tree only</source>
        <translation type="obsolete">ツリーのみ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>Tree and policy of all firewalls</source>
        <translation type="obsolete">すべてのファイアウォールのポリシーとツリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:189</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy of all firewalls</source>
        <translation type="obsolete">すべてのファイアウォールのポリシー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>policy of the opened firewall</source>
        <translation type="obsolete">開いているファイアウォールのポリシー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findobjectwidget_q.cpp:191</name>
    <message>
        <location filename="" line="0"/>
        <source>Close</source>
        <translation type="obsolete">閉じる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:112</name>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:113</name>
    <message>
        <location filename="" line="0"/>
        <source>Object:</source>
        <translation type="obsolete">オブジェクト:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:114</name>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:115</name>
    <message>
        <location filename="" line="0"/>
        <source>Parent Object</source>
        <translation type="obsolete">親オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:116</name>
    <message>
        <location filename="" line="0"/>
        <source>Details</source>
        <translation type="obsolete">詳細</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:117</name>
    <message>
        <location filename="" line="0"/>
        <source>Find</source>
        <translation type="obsolete">検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:118</name>
    <message>
        <location filename="" line="0"/>
        <source>Close</source>
        <translation type="obsolete">閉じる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:61</name>
    <message>
        <location filename="" line="0"/>
        <source>Parent Object</source>
        <translation type="obsolete">親オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/findwhereusedwidget_q.cpp:62</name>
    <message>
        <location filename="" line="0"/>
        <source>Details</source>
        <translation type="obsolete">詳細</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:199</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:200</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:201</name>
    <message>
        <location filename="" line="0"/>
        <source>Host OS Settings ...</source>
        <translation type="obsolete">ホスト OS 設定 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:202</name>
    <message>
        <location filename="" line="0"/>
        <source>Inactive firewall</source>
        <translation type="obsolete">新規ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:203</name>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:204</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Settings ...</source>
        <translation type="obsolete">ファイアウォール設定 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:205</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:206</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:207</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:208</name>
    <message>
        <location filename="" line="0"/>
        <source>Platform:</source>
        <translation type="obsolete">プラットフォーム:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:209</name>
    <message>
        <location filename="" line="0"/>
        <source>Version:</source>
        <translation type="obsolete">バージョン:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/firewalldialog_q.cpp:210</name>
    <message>
        <location filename="" line="0"/>
        <source>Host OS:</source>
        <translation type="obsolete">ホスト OS:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:185</name>
    <message>
        <location filename="" line="0"/>
        <source>FreeBSD: advanced settings</source>
        <translation type="obsolete">FreeBSD: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:186</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:190</name>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:191</name>
    <message>
        <location filename="" line="0"/>
        <source>Generate ICMP redirects</source>
        <translation type="obsolete">ICMP リダイレクト生成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:192</name>
    <message>
        <location filename="" line="0"/>
        <source>Packet forwarding</source>
        <translation type="obsolete">パケット転送</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:194</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:195</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:196</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:198</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:199</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:200</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:202</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:203</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:204</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:205</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:206</name>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:207</name>
    <message>
        <location filename="" line="0"/>
        <source>ipnat:</source>
        <translation type="obsolete">ipnat:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:208</name>
    <message>
        <location filename="" line="0"/>
        <source>sysctl:</source>
        <translation type="obsolete">sysctl:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:209</name>
    <message>
        <location filename="" line="0"/>
        <source>ipf:</source>
        <translation type="obsolete">ipf:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:210</name>
    <message>
        <location filename="" line="0"/>
        <source>ipfw:</source>
        <translation type="obsolete">ipfw:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/freebsdadvanceddialog_q.cpp:211</name>
    <message>
        <location filename="" line="0"/>
        <source>Path</source>
        <translation type="obsolete">パス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/fwobjectdroparea_q.cpp:49</name>
</context>
<context>
    <name>src/gui/.ui/groupobjectdialog_q.cpp:186</name>
    <message>
        <location filename="" line="0"/>
        <source>Group</source>
        <translation type="obsolete">グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/groupobjectdialog_q.cpp:187</name>
    <message>
        <location filename="" line="0"/>
        <source>Group</source>
        <translation type="obsolete">グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/groupobjectdialog_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/groupobjectdialog_q.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>I</source>
        <translation type="obsolete">I</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/groupobjectdialog_q.cpp:191</name>
    <message>
        <location filename="" line="0"/>
        <source>L</source>
        <translation type="obsolete">L</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/groupobjectdialog_q.cpp:192</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/groupobjectdialog_q.cpp:193</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/hostdialog_q.cpp:144</name>
    <message>
        <location filename="" line="0"/>
        <source>Host</source>
        <translation type="obsolete">ホスト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/hostdialog_q.cpp:145</name>
    <message>
        <location filename="" line="0"/>
        <source>Host</source>
        <translation type="obsolete">ホスト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/hostdialog_q.cpp:146</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC matching</source>
        <translation type="obsolete">MAC 一致</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/hostdialog_q.cpp:147</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/hostdialog_q.cpp:148</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/hostdialog_q.cpp:149</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:167</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP</source>
        <translation type="obsolete">ICMP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP Service</source>
        <translation type="obsolete">ICMP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:169</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:170</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:172</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP Type:</source>
        <translation type="obsolete">ICMP の種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:173</name>
    <message>
        <location filename="" line="0"/>
        <source>any</source>
        <translation type="obsolete">どれか</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:174</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP Code:</source>
        <translation type="obsolete">ICMP コード:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/icmpservicedialog_q.cpp:175</name>
    <message>
        <location filename="" line="0"/>
        <source>any</source>
        <translation type="obsolete">どれか</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:130</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:131</name>
    <message>
        <location filename="" line="0"/>
        <source>Progress</source>
        <translation type="obsolete">処理中</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:219</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:220</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile status</source>
        <translation type="obsolete">コンパイル状態</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:221</name>
    <message>
        <location filename="" line="0"/>
        <source>Install status</source>
        <translation type="obsolete">インストール状態</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:259</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Builder: Policy Installer</source>
        <translation type="obsolete">ファイアウォールビルダー: ポリシーインストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:260</name>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:261</name>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:262</name>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:263</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile</source>
        <translation type="obsolete">コンパイル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:264</name>
    <message>
        <location filename="" line="0"/>
        <source>Install</source>
        <translation type="obsolete">インストール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:265</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:266</name>
    <message>
        <location filename="" line="0"/>
        <source>Library</source>
        <translation type="obsolete">ライブラリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:267</name>
    <message>
        <location filename="" line="0"/>
        <source>Last Modified</source>
        <translation type="obsolete">最終更新日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:268</name>
    <message>
        <location filename="" line="0"/>
        <source>Last Compiled</source>
        <translation type="obsolete">最終コンパイル日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:269</name>
    <message>
        <location filename="" line="0"/>
        <source>Last Installed</source>
        <translation type="obsolete">最終インストール日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:270</name>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:271</name>
    <message>
        <location filename="" line="0"/>
        <source>None</source>
        <translation type="obsolete">なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:272</name>
    <message>
        <location filename="" line="0"/>
        <source>Show selected</source>
        <translation type="obsolete">選択の表示</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:274</name>
    <message>
        <location filename="" line="0"/>
        <source>Stop</source>
        <translation type="obsolete">停止</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:275</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:276</name>
    <message>
        <location filename="" line="0"/>
        <source>Progress</source>
        <translation type="obsolete">処理中</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:277</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewalls:</source>
        <translation type="obsolete">ファイアウォール:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:278</name>
    <message>
        <location filename="" line="0"/>
        <source>firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:279</name>
    <message>
        <location filename="" line="0"/>
        <source>Progress:</source>
        <translation type="obsolete">処理中:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:281</name>
    <message>
        <location filename="" line="0"/>
        <source>Show Details</source>
        <translation type="obsolete">詳細を表示する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:282</name>
    <message>
        <location filename="" line="0"/>
        <source>Process log</source>
        <translation type="obsolete">処理ログ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:283</name>
    <message>
        <location filename="" line="0"/>
        <source>Save log to file</source>
        <translation type="obsolete">ファイルにログを保存する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:285</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:286</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile status</source>
        <translation type="obsolete">コンパイル状態</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:287</name>
    <message>
        <location filename="" line="0"/>
        <source>Install status</source>
        <translation type="obsolete">インストール状態</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:77</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile</source>
        <translation type="obsolete">コンパイル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:79</name>
    <message>
        <location filename="" line="0"/>
        <source>Install</source>
        <translation type="obsolete">インストール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:81</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:83</name>
    <message>
        <location filename="" line="0"/>
        <source>Library</source>
        <translation type="obsolete">ライブラリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:85</name>
    <message>
        <location filename="" line="0"/>
        <source>Last Modified</source>
        <translation type="obsolete">最終更新日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:87</name>
    <message>
        <location filename="" line="0"/>
        <source>Last Compiled</source>
        <translation type="obsolete">最終コンパイル日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instdialog_q.cpp:89</name>
    <message>
        <location filename="" line="0"/>
        <source>Last Installed</source>
        <translation type="obsolete">最終インストール日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:270</name>
    <message>
        <location filename="" line="0"/>
        <source>Install options</source>
        <translation type="obsolete">インストールのオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:271</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:272</name>
    <message>
        <location filename="" line="0"/>
        <source>Enter authentication information below and click &apos;Next&apos;</source>
        <translation type="obsolete">下に認証情報を入力し、「次へ」をクリックしてください</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:273</name>
    <message>
        <location filename="" line="0"/>
        <source>Password or passphrase:</source>
        <translation type="obsolete">パスワードかパスフレーズ:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:274</name>
    <message>
        <location filename="" line="0"/>
        <source>User name:</source>
        <translation type="obsolete">ユーザー名:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:275</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:276</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:277</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:278</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:279</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:280</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:281</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable password:</source>
        <translation type="obsolete">有効なパスワード:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:282</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:283</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:284</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:285</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:286</name>
    <message>
        <location filename="" line="0"/>
        <source>min</source>
        <translation type="obsolete">最小</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:287</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:288</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:289</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:290</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:291</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove comments from configuration</source>
        <translation type="obsolete">設定からコメントを削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:292</name>
    <message>
        <location filename="" line="0"/>
        <source>Compress script</source>
        <translation type="obsolete">スクリプト圧縮</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:293</name>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:294</name>
    <message>
        <location filename="" line="0"/>
        <source>Ok</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/instoptionsdialog_q.cpp:295</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:210</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:211</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:212</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:213</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:214</name>
    <message>
        <location filename="" line="0"/>
        <source>Label:</source>
        <translation type="obsolete">ラベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:215</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:216</name>
    <message>
        <location filename="" line="0"/>
        <source>Security level:</source>
        <translation type="obsolete">セキュリティレベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:217</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:220</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:223</name>
    <message>
        <location filename="" line="0"/>
        <source>Network zone:</source>
        <translation type="obsolete">ネットワークゾーン:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:224</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:225</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:228</name>
    <message>
        <location filename="" line="0"/>
        <source>Management interface</source>
        <translation type="obsolete">インターフェンス管理</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:229</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:230</name>
    <message>
        <location filename="" line="0"/>
        <source>This interface is external (insecure)</source>
        <translation type="obsolete">このインターフェースは外部です (セキュアではありません)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:231</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:232</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:233</name>
    <message>
        <location filename="" line="0"/>
        <source>Regular interface</source>
        <translation type="obsolete">レギュラーインターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:234</name>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:235</name>
    <message>
        <location filename="" line="0"/>
        <source>Unnumbered interface</source>
        <translation type="obsolete">アンナンバードインターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/interfacedialog_q.cpp:236</name>
    <message>
        <location filename="" line="0"/>
        <source>Bridge port</source>
        <translation type="obsolete">ブリッジポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:521</name>
    <message>
        <location filename="" line="0"/>
        <source>ipf: advanced settings</source>
        <translation type="obsolete">ipf: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:522</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:524</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:526</name>
    <message>
        <location filename="" line="0"/>
        <source>Use raudio proxy in NAT rules</source>
        <translation type="obsolete">NAT ルール中で paudio を使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:527</name>
    <message>
        <location filename="" line="0"/>
        <source>Use h323 proxy in NAT rules</source>
        <translation type="obsolete">NAT ルール中で h323 プロキシを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:528</name>
    <message>
        <location filename="" line="0"/>
        <source>Use ipsec proxy in NAT rules</source>
        <translation type="obsolete">NAT ルール中で ipsec プロキシを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:529</name>
    <message>
        <location filename="" line="0"/>
        <source>Use ftp proxy in NAT rules</source>
        <translation type="obsolete">NAT ルール中で ftp プロキシを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:530</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:531</name>
    <message>
        <location filename="" line="0"/>
        <source>Use PPTP proxy in NAT rules</source>
        <translation type="obsolete">NAT ルール中で ftp プロキシを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:532</name>
    <message>
        <location filename="" line="0"/>
        <source>Use IRC proxy in NAT rules for DCC</source>
        <translation type="obsolete">NAT ルール中で ftp プロキシを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:533</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:534</name>
    <message>
        <location filename="" line="0"/>
        <source>Protocol Helpers</source>
        <translation type="obsolete">プロトコルヘッダー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:535</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler:</source>
        <translation type="obsolete">コンパイラー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:536</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:537</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:540</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:541</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:542</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:543</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:544</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:545</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:546</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:547</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:548</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:551</name>
    <message>
        <location filename="" line="0"/>
        <source>Default action on &apos;Reject&apos;:</source>
        <translation type="obsolete">&apos;Reject(拒否)&apos;のデフォルトアクション:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:552</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the compiler:</source>
        <translation type="obsolete">コンパイラーのコマンドラインオプション:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:553</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:554</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler</source>
        <translation type="obsolete">コンパイラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:555</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:556</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation type="obsolete">ポリシーインストールスクリプト (もしこのフィールドがブランクの時に使用する組み込みインストーラー)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:557</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the script:</source>
        <translation type="obsolete">スクリプトのコマンドラインオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:558</name>
    <message>
        <location filename="" line="0"/>
        <source>Built-in installer</source>
        <translation type="obsolete">組み込みインストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:559</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:560</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:561</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:562</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:563</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:564</name>
    <message>
        <location filename="" line="0"/>
        <source>Installer</source>
        <translation type="obsolete">インストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:566</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:567</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:570</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:572</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:573</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:574</name>
    <message>
        <location filename="" line="0"/>
        <source>Log facility:</source>
        <translation type="obsolete">ログファシリティ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:575</name>
    <message>
        <location filename="" line="0"/>
        <source>Log level:</source>
        <translation type="obsolete">ログレベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:576</name>
    <message>
        <location filename="" line="0"/>
        <source>Log packet body</source>
        <translation type="obsolete">ログパケット本体</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:577</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:578</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging</source>
        <translation type="obsolete">ログ記録</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:579</name>
    <message>
        <location filename="" line="0"/>
        <source>Add virtual addresses for NAT</source>
        <translation type="obsolete">NAT 用バーチャルアドレス群の追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:580</name>
    <message>
        <location filename="" line="0"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation type="obsolete">ファイアウォールマシンのインターフェース構成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:581</name>
    <message>
        <location filename="" line="0"/>
        <source>Turn debugging on in generated script</source>
        <translation type="obsolete">生成スクリプトでデバグを切り替え</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:582</name>
    <message>
        <location filename="" line="0"/>
        <source>Optimization</source>
        <translation type="obsolete">最適化</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:583</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:584</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:585</name>
</context>
<context>
    <name>src/gui/.ui/ipfadvanceddialog_q.cpp:586</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Options</source>
        <translation type="obsolete">スクリプトオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:344</name>
    <message>
        <location filename="" line="0"/>
        <source>ipfw: advanced settings</source>
        <translation type="obsolete">ipfw: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:345</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:347</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:349</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:350</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:351</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:352</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:353</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the compiler:</source>
        <translation type="obsolete">コンパイラーのコマンドラインオプション:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:354</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler:</source>
        <translation type="obsolete">コンパイラー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:355</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:356</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:359</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler</source>
        <translation type="obsolete">コンパイラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:360</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:361</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation type="obsolete">ポリシーインストールスクリプト (もしこのフィールドがブランクの時に使用する組み込みインストーラー)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:362</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the script:</source>
        <translation type="obsolete">スクリプトのコマンドラインオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:363</name>
    <message>
        <location filename="" line="0"/>
        <source>Built-in installer</source>
        <translation type="obsolete">組み込みインストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:364</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:365</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:366</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:367</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:368</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:369</name>
    <message>
        <location filename="" line="0"/>
        <source>Installer</source>
        <translation type="obsolete">インストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:371</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:373</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:375</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:376</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:378</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:379</name>
    <message>
        <location filename="" line="0"/>
        <source>Add virtual addresses for NAT</source>
        <translation type="obsolete">NAT 用バーチャルアドレス群の追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:380</name>
    <message>
        <location filename="" line="0"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation type="obsolete">ファイアウォールマシンのインターフェース構成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:381</name>
    <message>
        <location filename="" line="0"/>
        <source>Turn debugging on in generated script</source>
        <translation type="obsolete">生成スクリプトでデバグを切り替え</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:382</name>
</context>
<context>
    <name>src/gui/.ui/ipfwadvanceddialog_q.cpp:383</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Options</source>
        <translation type="obsolete">スクリプトオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:208</name>
    <message>
        <location filename="" line="0"/>
        <source>IP</source>
        <translation type="obsolete">IP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:209</name>
    <message>
        <location filename="" line="0"/>
        <source>IP Service</source>
        <translation type="obsolete">IP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:210</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:212</name>
    <message>
        <location filename="" line="0"/>
        <source>all fragments</source>
        <translation type="obsolete">全フラグメント</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:213</name>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:214</name>
    <message>
        <location filename="" line="0"/>
        <source>timestamp</source>
        <translation type="obsolete">タイムスタンプ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:215</name>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:216</name>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:217</name>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:218</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:219</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:220</name>
    <message>
        <location filename="" line="0"/>
        <source>Protocol number:</source>
        <translation type="obsolete">プロトコル番号:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipservicedialog_q.cpp:221</name>
    <message>
        <location filename="" line="0"/>
        <source>( 0 - any protocol )</source>
        <translation type="obsolete">( 0 - 任意のプロトコル )</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:589</name>
    <message>
        <location filename="" line="0"/>
        <source>iptables: advanced settings</source>
        <translation type="obsolete">iptables: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:590</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:592</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:594</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the compiler:</source>
        <translation type="obsolete">コンパイラーのコマンドラインオプション:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:595</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:598</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:599</name>
    <message>
        <location filename="" line="0"/>
        <source>Bridging firewall</source>
        <translation type="obsolete">ブリッジ中のファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:600</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:601</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:602</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:603</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:604</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:605</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:606</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler:</source>
        <translation type="obsolete">コンパイラー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:607</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:608</name>
    <message>
        <location filename="" line="0"/>
        <source>Default action on &apos;Reject&apos;:</source>
        <translation type="obsolete">&apos;Reject(拒否)&apos;のデフォルトアクション:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:609</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:611</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:612</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler</source>
        <translation type="obsolete">コンパイラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:613</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:614</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation type="obsolete">ポリシーインストールスクリプト (もしこのフィールドがブランクの時に使用する組み込みインストーラー)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:615</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the script:</source>
        <translation type="obsolete">スクリプトのコマンドラインオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:616</name>
    <message>
        <location filename="" line="0"/>
        <source>Built-in installer</source>
        <translation type="obsolete">組み込みインストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:617</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:618</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:619</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:620</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:621</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:622</name>
    <message>
        <location filename="" line="0"/>
        <source>Installer</source>
        <translation type="obsolete">インストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:624</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:626</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:629</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:630</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:631</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:633</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:634</name>
    <message>
        <location filename="" line="0"/>
        <source>after interface configuration</source>
        <translation type="obsolete">インターフェース設定の後</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:635</name>
    <message>
        <location filename="" line="0"/>
        <source>after policy reset</source>
        <translation type="obsolete">ポリシーリセットの後</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:636</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:638</name>
    <message>
        <location filename="" line="0"/>
        <source>use ULOG</source>
        <translation type="obsolete">ULOG を使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:639</name>
    <message>
        <location filename="" line="0"/>
        <source>use LOG</source>
        <translation type="obsolete">LOG を使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:640</name>
    <message>
        <location filename="" line="0"/>
        <source>log TCP seq. numbers</source>
        <translation type="obsolete">TCP シーケンス番号をログ記録する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:641</name>
    <message>
        <location filename="" line="0"/>
        <source>log IP options</source>
        <translation type="obsolete">IP オプションをログ記録する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:642</name>
    <message>
        <location filename="" line="0"/>
        <source>use numeric syslog levels</source>
        <translation type="obsolete">使用する syslog レベルの数値</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:643</name>
    <message>
        <location filename="" line="0"/>
        <source>Log level:</source>
        <translation type="obsolete">ログレベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:644</name>
    <message>
        <location filename="" line="0"/>
        <source>log TCP options</source>
        <translation type="obsolete">ログ TCP オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:645</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:646</name>
    <message>
        <location filename="" line="0"/>
        <source>queue threshold:</source>
        <translation type="obsolete">キュー閾値:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:647</name>
    <message>
        <location filename="" line="0"/>
        <source>netlink group:</source>
        <translation type="obsolete">ネットリンクグループ:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:648</name>
    <message>
        <location filename="" line="0"/>
        <source>Log prefix:</source>
        <translation type="obsolete">ログ接頭語:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:649</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging limit:</source>
        <translation type="obsolete">ログ記録制限:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:650</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:652</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging</source>
        <translation type="obsolete">ログ記録</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:653</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:654</name>
    <message>
        <location filename="" line="0"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation type="obsolete">ファイアウォールマシンのインターフェース構成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:655</name>
    <message>
        <location filename="" line="0"/>
        <source>Turn debugging on in generated script</source>
        <translation type="obsolete">生成スクリプトでデバグを切り替え</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:656</name>
    <message>
        <location filename="" line="0"/>
        <source>Verify interfaces before loading firewall policy</source>
        <translation type="obsolete">ファイアウォールポリシーをロードする前にインターフェースを確認</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:657</name>
    <message>
        <location filename="" line="0"/>
        <source>Load modules</source>
        <translation type="obsolete">ロードモジュール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:658</name>
    <message>
        <location filename="" line="0"/>
        <source>Add virtual addresses for NAT</source>
        <translation type="obsolete">NAT 用バーチャルアドレス群の追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:659</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:660</name>
</context>
<context>
    <name>src/gui/.ui/iptadvanceddialog_q.cpp:661</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Options</source>
        <translation type="obsolete">スクリプトオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:164</name>
    <message>
        <location filename="" line="0"/>
        <source>IPv4</source>
        <translation type="obsolete">IPv4</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:165</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:166</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:167</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:169</name>
    <message>
        <location filename="" line="0"/>
        <source>Address:</source>
        <translation type="obsolete">アドレス:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:170</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask:</source>
        <translation type="obsolete">ネットマスク:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ipv4dialog_q.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS Lookup...</source>
        <translation type="obsolete">DNS 照合...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/libexport_q.cpp:105</name>
    <message>
        <location filename="" line="0"/>
        <source>Export</source>
        <translation type="obsolete">エクスポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/libexport_q.cpp:106</name>
</context>
<context>
    <name>src/gui/.ui/libexport_q.cpp:108</name>
    <message>
        <location filename="" line="0"/>
        <source>New Item</source>
        <translation type="obsolete">新規項目</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/libexport_q.cpp:109</name>
</context>
<context>
    <name>src/gui/.ui/libexport_q.cpp:110</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose libraries to be exported:</source>
        <translation type="obsolete">エクスポートするライブラリーを選択:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/libexport_q.cpp:111</name>
    <message>
        <location filename="" line="0"/>
        <source>Ok</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/libexport_q.cpp:112</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/librarydialog_q.cpp:136</name>
    <message>
        <location filename="" line="0"/>
        <source>Library</source>
        <translation type="obsolete">ライブラリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/librarydialog_q.cpp:137</name>
    <message>
        <location filename="" line="0"/>
        <source>Library</source>
        <translation type="obsolete">ライブラリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/librarydialog_q.cpp:138</name>
    <message>
        <location filename="" line="0"/>
        <source>Color:</source>
        <translation type="obsolete">色:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/librarydialog_q.cpp:139</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/librarydialog_q.cpp:141</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:187</name>
    <message>
        <location filename="" line="0"/>
        <source>Linksys/Sveasoft: advanced settings</source>
        <translation type="obsolete">Linksys/Sveasoft: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:192</name>
    <message>
        <location filename="" line="0"/>
        <source>modprobe:</source>
        <translation type="obsolete">modprobe:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:193</name>
    <message>
        <location filename="" line="0"/>
        <source>logger:</source>
        <translation type="obsolete">logger:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:194</name>
    <message>
        <location filename="" line="0"/>
        <source>ip:</source>
        <translation type="obsolete">ip:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:195</name>
    <message>
        <location filename="" line="0"/>
        <source>lsmod</source>
        <translation type="obsolete">lsmod</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:196</name>
    <message>
        <location filename="" line="0"/>
        <source>iptables:</source>
        <translation type="obsolete">iptables:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:197</name>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:198</name>
    <message>
        <location filename="" line="0"/>
        <source>Path</source>
        <translation type="obsolete">パス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:199</name>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:203</name>
    <message>
        <location filename="" line="0"/>
        <source>prompt 1</source>
        <translation type="obsolete">プロンプト1</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:204</name>
    <message>
        <location filename="" line="0"/>
        <source>prompt 2</source>
        <translation type="obsolete">プロンプト2</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:205</name>
    <message>
        <location filename="" line="0"/>
        <source>Use default prompts</source>
        <translation type="obsolete">デフォルトプロンプトを使う</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linksysadvanceddialog_q.cpp:206</name>
    <message>
        <location filename="" line="0"/>
        <source>Prompts</source>
        <translation type="obsolete">プロンプト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:364</name>
    <message>
        <location filename="" line="0"/>
        <source>Linux 2.4: advanced settings</source>
        <translation type="obsolete">Linux 2.4: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:365</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:367</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:370</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:371</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:372</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:374</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:375</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:376</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:378</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:379</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:380</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:382</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:383</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:384</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:386</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:387</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:388</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:390</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:391</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:392</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:394</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:395</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:396</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:398</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:399</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:400</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:402</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:403</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:404</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:405</name>
    <message>
        <location filename="" line="0"/>
        <source>Packet forwarding</source>
        <translation type="obsolete">パケット転送</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:406</name>
    <message>
        <location filename="" line="0"/>
        <source>Kernel anti-spoofing protection</source>
        <translation type="obsolete">カーネルの spoofing 対策保護</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:407</name>
    <message>
        <location filename="" line="0"/>
        <source>Ignore broadcast pings</source>
        <translation type="obsolete">ブロードキャストピングの無視</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:408</name>
    <message>
        <location filename="" line="0"/>
        <source>Ignore all pings</source>
        <translation type="obsolete">全ピングの無視</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:409</name>
    <message>
        <location filename="" line="0"/>
        <source>Accept source route</source>
        <translation type="obsolete">ソースルートの許可</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:410</name>
    <message>
        <location filename="" line="0"/>
        <source>Accept ICMP redirects</source>
        <translation type="obsolete">ICMP リダイレクトの受理</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:411</name>
    <message>
        <location filename="" line="0"/>
        <source>Ignore bogus ICMP errors</source>
        <translation type="obsolete">bogus ICMP エラーを無視</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:412</name>
    <message>
        <location filename="" line="0"/>
        <source>Allow dynamic addresses</source>
        <translation type="obsolete">ダイナミックアドレス群の許可</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:413</name>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:414</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:415</name>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:417</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:418</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:419</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:421</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:422</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:423</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:425</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:426</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:427</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:429</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:430</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:431</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:433</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:434</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:435</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:437</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:438</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:439</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:440</name>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:441</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP window scaling</source>
        <translation type="obsolete">TCP ウィンドウスケーリング</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:442</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP ECN</source>
        <translation type="obsolete">TCP ECN</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:443</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP SYN cookies</source>
        <translation type="obsolete">TCP SYN クッキー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:444</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP keepalive time (sec)</source>
        <translation type="obsolete">TCP 生存時間 (秒)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:445</name>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:446</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP timestamps</source>
        <translation type="obsolete">TCP タイムスタンプ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:447</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP FIN timeout (sec)</source>
        <translation type="obsolete">TCP FIN タイムアウト(秒)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:448</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP</source>
        <translation type="obsolete">TCP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:449</name>
    <message>
        <location filename="" line="0"/>
        <source>modprobe:</source>
        <translation type="obsolete">modprobe:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:450</name>
    <message>
        <location filename="" line="0"/>
        <source>logger:</source>
        <translation type="obsolete">logger:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:451</name>
    <message>
        <location filename="" line="0"/>
        <source>ip:</source>
        <translation type="obsolete">ip:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:452</name>
    <message>
        <location filename="" line="0"/>
        <source>lsmod</source>
        <translation type="obsolete">lsmod</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:453</name>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:454</name>
    <message>
        <location filename="" line="0"/>
        <source>iptables:</source>
        <translation type="obsolete">iptables:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:455</name>
    <message>
        <location filename="" line="0"/>
        <source>iptables-restore:</source>
        <translation type="obsolete">iptables-restore:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/linux24advanceddialog_q.cpp:456</name>
    <message>
        <location filename="" line="0"/>
        <source>Path</source>
        <translation type="obsolete">パス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/longtextdialog_q.cpp:94</name>
</context>
<context>
    <name>src/gui/.ui/longtextdialog_q.cpp:95</name>
    <message>
        <location filename="" line="0"/>
        <source>Continue</source>
        <translation type="obsolete">続ける</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/longtextdialog_q.cpp:96</name>
    <message>
        <location filename="" line="0"/>
        <source>this is the error text</source>
        <translation type="obsolete">これはエラーテキストです</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:163</name>
    <message>
        <location filename="" line="0"/>
        <source>MacOS X: advanced settings</source>
        <translation type="obsolete">Mac OS X: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:164</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:166</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>Generate ICMP redirects</source>
        <translation type="obsolete">ICMP リダイレクト生成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:169</name>
    <message>
        <location filename="" line="0"/>
        <source>Packet forwarding</source>
        <translation type="obsolete">パケット転送</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:172</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:173</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:175</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:176</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:179</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:181</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:182</name>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:183</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:184</name>
    <message>
        <location filename="" line="0"/>
        <source>ipfw:</source>
        <translation type="obsolete">ipfw:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:185</name>
    <message>
        <location filename="" line="0"/>
        <source>sysctl:</source>
        <translation type="obsolete">sysctl:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:186</name>
</context>
<context>
    <name>src/gui/.ui/macosxadvanceddialog_q.cpp:187</name>
    <message>
        <location filename="" line="0"/>
        <source>Path</source>
        <translation type="obsolete">パス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/metriceditorpanel_q.cpp:76</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/metriceditorpanel_q.cpp:78</name>
    <message>
        <location filename="" line="0"/>
        <source>textLabel2</source>
        <translation type="obsolete">テキストラベル 2</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:154</name>
    <message>
        <location filename="" line="0"/>
        <source>NAT Rule Options</source>
        <translation type="obsolete">NAT ルールオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:155</name>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:156</name>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:157</name>
    <message>
        <location filename="" line="0"/>
        <source>Pool type</source>
        <translation type="obsolete">プールの種類</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:158</name>
    <message>
        <location filename="" line="0"/>
        <source>default</source>
        <translation type="obsolete">標準</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:159</name>
    <message>
        <location filename="" line="0"/>
        <source>bitmask</source>
        <translation type="obsolete">ネットマスク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:160</name>
    <message>
        <location filename="" line="0"/>
        <source>random</source>
        <translation type="obsolete">ランダム</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:161</name>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:162</name>
</context>
<context>
    <name>src/gui/.ui/natruleoptionsdialog_q.cpp:163</name>
</context>
<context>
    <name>src/gui/.ui/networkdialog_q.cpp:163</name>
    <message>
        <location filename="" line="0"/>
        <source>Network</source>
        <translation type="obsolete">ネットワーク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/networkdialog_q.cpp:164</name>
    <message>
        <location filename="" line="0"/>
        <source>Network</source>
        <translation type="obsolete">ネットワーク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/networkdialog_q.cpp:165</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/networkdialog_q.cpp:166</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/networkdialog_q.cpp:167</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/networkdialog_q.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask:</source>
        <translation type="obsolete">ネットマスク:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/networkdialog_q.cpp:169</name>
    <message>
        <location filename="" line="0"/>
        <source>Address:</source>
        <translation type="obsolete">アドレス:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:169</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:170</name>
    <message>
        <location filename="" line="0"/>
        <source>Label</source>
        <translation type="obsolete">ラベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:172</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask</source>
        <translation type="obsolete">ネットマスク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:173</name>
    <message>
        <location filename="" line="0"/>
        <source>Dyn</source>
        <translation type="obsolete">Dyn</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:174</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC</source>
        <translation type="obsolete">MAC</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:320</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:321</name>
    <message>
        <location filename="" line="0"/>
        <source>Label</source>
        <translation type="obsolete">ラベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:322</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:323</name>
    <message>
        <location filename="" line="0"/>
        <source>Security Level</source>
        <translation type="obsolete">セキュリティレベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:485</name>
    <message>
        <location filename="" line="0"/>
        <source>New Firewall</source>
        <translation type="obsolete">新規ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:486</name>
    <message>
        <location filename="" line="0"/>
        <source>Enter the name of the new object below:</source>
        <translation type="obsolete">新しいオブジェクトの名前を入力してください:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:487</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose firewall software it is running:</source>
        <translation type="obsolete">実行中のファイアウォールソフトウェアの選択してください:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:488</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose OS the new firewall runs on:</source>
        <translation type="obsolete">新規ファイアウォールを実行する OS を選択してください:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:489</name>
    <message>
        <location filename="" line="0"/>
        <source>Use preconfigured template firewall objects</source>
        <translation type="obsolete">あらかじめ設定されたテンプレートオブジェクトを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:491</name>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:493</name>
    <message>
        <location filename="" line="0"/>
        <source>Configure interfaces manually</source>
        <translation type="obsolete">インターフェースを手動設定する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:494</name>
    <message>
        <location filename="" line="0"/>
        <source>Use SNMP to discover interfaces of the firewall</source>
        <translation type="obsolete">SNMP を使用し、ファイアウォールのインターフェースを発見する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:495</name>
    <message>
        <location filename="" line="0"/>
        <source>Discover Interfaces using SNMP</source>
        <translation type="obsolete">SNMP を用いてインターフェースを発見する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:496</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP &apos;read&apos; community string:</source>
        <translation type="obsolete">SNMP &apos;read&apos; コミュニティ文字列</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:498</name>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:499</name>
    <message>
        <location filename="" line="0"/>
        <source>Click &apos;Next&apos; when done.</source>
        <translation type="obsolete">「Next」をクリックすると処理します。</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:500</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:501</name>
    <message>
        <location filename="" line="0"/>
        <source>Label</source>
        <translation type="obsolete">ラベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:502</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:503</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask</source>
        <translation type="obsolete">ネットマスク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:504</name>
    <message>
        <location filename="" line="0"/>
        <source>Dyn</source>
        <translation type="obsolete">Dyn</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:505</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC</source>
        <translation type="obsolete">MAC</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:506</name>
    <message>
        <location filename="" line="0"/>
        <source>Label:</source>
        <translation type="obsolete">ラベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:507</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:508</name>
    <message>
        <location filename="" line="0"/>
        <source>Update</source>
        <translation type="obsolete">更新</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:509</name>
    <message>
        <location filename="" line="0"/>
        <source>Add</source>
        <translation type="obsolete">追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:511</name>
    <message>
        <location filename="" line="0"/>
        <source>Bridge port</source>
        <translation type="obsolete">ブリッジポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:512</name>
    <message>
        <location filename="" line="0"/>
        <source>Unnumbered interface</source>
        <translation type="obsolete">アンナンバードインターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:513</name>
    <message>
        <location filename="" line="0"/>
        <source>Dynamic address</source>
        <translation type="obsolete">ダイナミックアドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:514</name>
    <message>
        <location filename="" line="0"/>
        <source>Regular interface</source>
        <translation type="obsolete">レギュラーインターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:515</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:516</name>
    <message>
        <location filename="" line="0"/>
        <source>Address:</source>
        <translation type="obsolete">アドレス:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:517</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask:</source>
        <translation type="obsolete">ネットマスク:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:518</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC:</source>
        <translation type="obsolete">MAC:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:520</name>
    <message>
        <location filename="" line="0"/>
        <source>up</source>
        <translation type="obsolete">上</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:521</name>
    <message>
        <location filename="" line="0"/>
        <source>down</source>
        <translation type="obsolete">下</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:522</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:523</name>
    <message>
        <location filename="" line="0"/>
        <source>Label</source>
        <translation type="obsolete">ラベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:524</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:525</name>
    <message>
        <location filename="" line="0"/>
        <source>Security Level</source>
        <translation type="obsolete">セキュリティレベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:526</name>
    <message>
        <location filename="" line="0"/>
        <source>Click &apos;Finish&apos; when done.</source>
        <translation type="obsolete">「Finish」をクリックすると処理します。</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:527</name>
</context>
<context>
    <name>src/gui/.ui/newfirewalldialog_q.cpp:529</name>
</context>
<context>
    <name>src/gui/.ui/newgroupdialog_q.cpp:100</name>
    <message>
        <location filename="" line="0"/>
        <source>Create a group</source>
        <translation type="obsolete">新規グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newgroupdialog_q.cpp:101</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newgroupdialog_q.cpp:96</name>
    <message>
        <location filename="" line="0"/>
        <source>New Group</source>
        <translation type="obsolete">新規グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newgroupdialog_q.cpp:97</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newgroupdialog_q.cpp:98</name>
    <message>
        <location filename="" line="0"/>
        <source>Group Name:</source>
        <translation type="obsolete">グループ名:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newgroupdialog_q.cpp:99</name>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:185</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:186</name>
    <message>
        <location filename="" line="0"/>
        <source>Label</source>
        <translation type="obsolete">ラベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:187</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask</source>
        <translation type="obsolete">ネットマスク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:189</name>
    <message>
        <location filename="" line="0"/>
        <source>Dyn</source>
        <translation type="obsolete">Dyn</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC</source>
        <translation type="obsolete">MAC</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:376</name>
    <message>
        <location filename="" line="0"/>
        <source>New Host</source>
        <translation type="obsolete">新規ホスト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:377</name>
    <message>
        <location filename="" line="0"/>
        <source>Enter the name of the new object below:</source>
        <translation type="obsolete">新しいオブジェクトの名前を入力してください:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:378</name>
    <message>
        <location filename="" line="0"/>
        <source>Use preconfigured template host objects</source>
        <translation type="obsolete">事前定義されたホストオブジェクトテンプレートを使う</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:380</name>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:382</name>
    <message>
        <location filename="" line="0"/>
        <source>Configure interfaces manually</source>
        <translation type="obsolete">インターフェースを手動設定する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:383</name>
    <message>
        <location filename="" line="0"/>
        <source>Use SNMP to discover interfaces of the host</source>
        <translation type="obsolete">ホストのインタフェースを発見するためにSNMPを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:384</name>
    <message>
        <location filename="" line="0"/>
        <source>Discover Interfaces using SNMP</source>
        <translation type="obsolete">SNMP を用いてインターフェースを発見する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:385</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP &apos;read&apos; community string:</source>
        <translation type="obsolete">SNMP &apos;read&apos; コミュニティ文字列</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:387</name>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:388</name>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:389</name>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:390</name>
    <message>
        <location filename="" line="0"/>
        <source>Click &apos;Next&apos; when done.</source>
        <translation type="obsolete">「Next」をクリックすると処理します。</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:391</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:392</name>
    <message>
        <location filename="" line="0"/>
        <source>Label:</source>
        <translation type="obsolete">ラベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:393</name>
    <message>
        <location filename="" line="0"/>
        <source>Address:</source>
        <translation type="obsolete">アドレス:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:394</name>
    <message>
        <location filename="" line="0"/>
        <source>Unnumbered interface</source>
        <translation type="obsolete">アンナンバードインターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:395</name>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:396</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:397</name>
    <message>
        <location filename="" line="0"/>
        <source>Label</source>
        <translation type="obsolete">ラベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:398</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:399</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask</source>
        <translation type="obsolete">ネットマスク</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:400</name>
    <message>
        <location filename="" line="0"/>
        <source>Dyn</source>
        <translation type="obsolete">Dyn</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:401</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC</source>
        <translation type="obsolete">MAC</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:402</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC:</source>
        <translation type="obsolete">MAC:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:403</name>
    <message>
        <location filename="" line="0"/>
        <source>Dynamic address</source>
        <translation type="obsolete">ダイナミックアドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:404</name>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:405</name>
    <message>
        <location filename="" line="0"/>
        <source>Netmask:</source>
        <translation type="obsolete">ネットマスク:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:406</name>
    <message>
        <location filename="" line="0"/>
        <source>Add</source>
        <translation type="obsolete">追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:407</name>
    <message>
        <location filename="" line="0"/>
        <source>Update</source>
        <translation type="obsolete">更新</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:408</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/newhostdialog_q.cpp:410</name>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:145</name>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:146</name>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:147</name>
    <message>
        <location filename="" line="0"/>
        <source>Current Object </source>
        <translation type="obsolete">現在のオブジェクト </translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:149</name>
    <message>
        <location filename="" line="0"/>
        <source>Keep current object</source>
        <translation type="obsolete">現在のオブジェクトを保持</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:150</name>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:152</name>
    <message>
        <location filename="" line="0"/>
        <source>New Object</source>
        <translation type="obsolete">新規オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:154</name>
    <message>
        <location filename="" line="0"/>
        <source>Replace with this object</source>
        <translation type="obsolete">このオブジェクトと置換</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objconflictresolutiondialog_q.cpp:155</name>
</context>
<context>
    <name>src/gui/.ui/objectmanipulator_q.cpp:107</name>
    <message>
        <location filename="" line="0"/>
        <source>Tree of Objects</source>
        <translation type="obsolete">オブジェクトのツリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objectmanipulator_q.cpp:110</name>
    <message>
        <location filename="" line="0"/>
        <source>Back</source>
        <translation type="obsolete">戻る</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objectmanipulator_q.cpp:111</name>
    <message>
        <location filename="" line="0"/>
        <source>Go back to the previous object</source>
        <translation type="obsolete">前のオブジェクトに戻る</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objectmanipulator_q.cpp:113</name>
    <message>
        <location filename="" line="0"/>
        <source>New Object</source>
        <translation type="obsolete">新規オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/objectmanipulator_q.cpp:114</name>
    <message>
        <location filename="" line="0"/>
        <source>Create New Object</source>
        <translation type="obsolete">新規オブジェクト作成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>OpenBSD: advanced settings</source>
        <translation type="obsolete">OpenBSD: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:172</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:174</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:176</name>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable directed broadcast</source>
        <translation type="obsolete">ダイレクトブロードキャストを入力する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:179</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:181</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:183</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:184</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:185</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:186</name>
    <message>
        <location filename="" line="0"/>
        <source>Packet forwarding</source>
        <translation type="obsolete">パケット転送</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:189</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:192</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:193</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:194</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:196</name>
    <message>
        <location filename="" line="0"/>
        <source>Generate ICMP redirects</source>
        <translation type="obsolete">ICMP リダイレクト生成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:197</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:198</name>
    <message>
        <location filename="" line="0"/>
        <source>pfctl:</source>
        <translation type="obsolete">pfctl:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:199</name>
    <message>
        <location filename="" line="0"/>
        <source>sysctl:</source>
        <translation type="obsolete">sysctl:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:200</name>
</context>
<context>
    <name>src/gui/.ui/openbsdadvanceddialog_q.cpp:201</name>
    <message>
        <location filename="" line="0"/>
        <source>Path</source>
        <translation type="obsolete">パス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:102</name>
    <message>
        <location filename="" line="0"/>
        <source>Page Setup</source>
        <translation type="obsolete">ページ設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:103</name>
    <message>
        <location filename="" line="0"/>
        <source>start each section on a new page</source>
        <translation type="obsolete">各セクションを新規ページで開始する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:104</name>
    <message>
        <location filename="" line="0"/>
        <source>print header on every page</source>
        <translation type="obsolete">各ページにヘッダーを印刷する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:105</name>
    <message>
        <location filename="" line="0"/>
        <source>print legend</source>
        <translation type="obsolete">凡例を印刷する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:106</name>
    <message>
        <location filename="" line="0"/>
        <source>print objects used in rules</source>
        <translation type="obsolete">ツリー中のオブジェクトを検索</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:107</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:108</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+O</source>
        <translation type="obsolete">Alt+O</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:109</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:110</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+C</source>
        <translation type="obsolete">Alt+C</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:111</name>
    <message>
        <location filename="" line="0"/>
        <source>Scale tables: </source>
        <translation type="obsolete">テーブルスケール: </translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:113</name>
    <message>
        <location filename="" line="0"/>
        <source>50%</source>
        <translation type="obsolete">50%</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:114</name>
    <message>
        <location filename="" line="0"/>
        <source>75%</source>
        <translation type="obsolete">75%</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:115</name>
    <message>
        <location filename="" line="0"/>
        <source>100%</source>
        <translation type="obsolete">100%</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:116</name>
    <message>
        <location filename="" line="0"/>
        <source>150%</source>
        <translation type="obsolete">150%</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pagesetupdialog_q.cpp:117</name>
    <message>
        <location filename="" line="0"/>
        <source>200%</source>
        <translation type="obsolete">200%</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1000</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1001</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1002</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1003</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1004</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1005</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1006</name>
    <message>
        <location filename="" line="0"/>
        <source>Timeouts</source>
        <translation type="obsolete">タイムアウト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1007</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1008</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation type="obsolete">ポリシーインストールスクリプト (もしこのフィールドがブランクの時に使用する組み込みインストーラー)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1009</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the script:</source>
        <translation type="obsolete">スクリプトのコマンドラインオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1010</name>
    <message>
        <location filename="" line="0"/>
        <source>Built-in installer</source>
        <translation type="obsolete">組み込みインストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1011</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1012</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1013</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1014</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1015</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1016</name>
    <message>
        <location filename="" line="0"/>
        <source>Installer</source>
        <translation type="obsolete">インストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1018</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1020</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1022</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1023</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1025</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1026</name>
    <message>
        <location filename="" line="0"/>
        <source>Log Prefix</source>
        <translation type="obsolete">ログ接頭語</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1027</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1028</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging</source>
        <translation type="obsolete">ログ記録</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1029</name>
    <message>
        <location filename="" line="0"/>
        <source>Add virtual addresses for NAT</source>
        <translation type="obsolete">NAT 用バーチャルアドレス群の追加</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1030</name>
    <message>
        <location filename="" line="0"/>
        <source>Configure Interfaces of the firewall machine</source>
        <translation type="obsolete">ファイアウォールマシンのインターフェース構成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1031</name>
    <message>
        <location filename="" line="0"/>
        <source>Turn debugging on in generated script</source>
        <translation type="obsolete">生成スクリプトでデバグを切り替え</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1032</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:1033</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Options</source>
        <translation type="obsolete">スクリプトオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:917</name>
    <message>
        <location filename="" line="0"/>
        <source>pf: advanced settings</source>
        <translation type="obsolete">pf: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:918</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:920</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:922</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler:</source>
        <translation type="obsolete">コンパイラー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:923</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the compiler:</source>
        <translation type="obsolete">コンパイラーのコマンドラインオプション:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:924</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:927</name>
    <message>
        <location filename="" line="0"/>
        <source>state table size: </source>
        <translation type="obsolete">ステートテーブルの大きさ: </translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:928</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:929</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:930</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:931</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:932</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:933</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:934</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:935</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:937</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:938</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:939</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:940</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:941</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:942</name>
    <message>
        <location filename="" line="0"/>
        <source>Optimization:</source>
        <translation type="obsolete">最適化:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:943</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler</source>
        <translation type="obsolete">コンパイラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:944</name>
    <message>
        <location filename="" line="0"/>
        <source>Enforce Minimum TTL:</source>
        <translation type="obsolete">TTL 最小値の強化</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:945</name>
    <message>
        <location filename="" line="0"/>
        <source>Enforce Maximum MSS:</source>
        <translation type="obsolete">MSS 最大値の強化</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:946</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:947</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:948</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:949</name>
    <message>
        <location filename="" line="0"/>
        <source>Clear DF bit</source>
        <translation type="obsolete">DF ビット解除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:950</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:951</name>
    <message>
        <location filename="" line="0"/>
        <source>Use random ID</source>
        <translation type="obsolete">ランダム ID の使用</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:952</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:954</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:955</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:956</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:957</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:958</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:959</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:960</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:961</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:962</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP</source>
        <translation type="obsolete">TCP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:963</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:964</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:965</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:966</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:967</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:968</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:969</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:970</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:971</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:972</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:973</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:974</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:975</name>
    <message>
        <location filename="" line="0"/>
        <source>UDP</source>
        <translation type="obsolete">UDP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:976</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:977</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:978</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:979</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:980</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:981</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:982</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP</source>
        <translation type="obsolete">ICMP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:983</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:984</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:985</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:986</name>
    <message>
        <location filename="" line="0"/>
        <source>error</source>
        <translation type="obsolete">error</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:987</name>
    <message>
        <location filename="" line="0"/>
        <source>Other Protocols</source>
        <translation type="obsolete">その他のプロトコル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:988</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:989</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:990</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:991</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:992</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:993</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:994</name>
    <message>
        <location filename="" line="0"/>
        <source>Fragments</source>
        <translation type="obsolete">フラグメント</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:995</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:996</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:997</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:998</name>
</context>
<context>
    <name>src/gui/.ui/pfadvanceddialog_q.cpp:999</name>
</context>
<context>
    <name>src/gui/.ui/physaddressdialog_q.cpp:149</name>
    <message>
        <location filename="" line="0"/>
        <source>physAddress</source>
        <translation type="obsolete">物理アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/physaddressdialog_q.cpp:150</name>
    <message>
        <location filename="" line="0"/>
        <source>MAC Address</source>
        <translation type="obsolete">新規 MAC アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/physaddressdialog_q.cpp:151</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/physaddressdialog_q.cpp:152</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/physaddressdialog_q.cpp:153</name>
    <message>
        <location filename="" line="0"/>
        <source>Physical address (MAC):</source>
        <translation type="obsolete">物理アドレス (MAC):</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/physaddressdialog_q.cpp:154</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1800</name>
    <message>
        <location filename="" line="0"/>
        <source>PIX Firewall Settings</source>
        <translation type="obsolete">PIX ファイアウォール設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1801</name>
    <message>
        <location filename="" line="0"/>
        <source>OK</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1802</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1803</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy Compiler Options</source>
        <translation type="obsolete">ポリシーコンパイラーオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1804</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1805</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1806</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1807</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1808</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1810</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1811</name>
    <message>
        <location filename="" line="0"/>
        <source>Optimize &apos;default nat&apos; rules</source>
        <translation type="obsolete">&apos;default nat&apos; ルールを最適化</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1812</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1813</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1814</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1815</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1816</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1817</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1818</name>
    <message>
        <location filename="" line="0"/>
        <source>Verification of NAT rules</source>
        <translation type="obsolete">NAT ルールの検証中</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1819</name>
    <message>
        <location filename="" line="0"/>
        <source>Check for duplicate nat rules</source>
        <translation type="obsolete">複製 NAT ルールの検証中</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1820</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1821</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1822</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1824</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1827</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler Options</source>
        <translation type="obsolete">コンパイラーオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1828</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1829</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1830</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1831</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1832</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1833</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1834</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1835</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1836</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1837</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1838</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1839</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1842</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1846</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Options</source>
        <translation type="obsolete">スクリプトオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1847</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1848</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy install script (using built-in installer if this field is blank):</source>
        <translation type="obsolete">ポリシーインストールスクリプト (もしこのフィールドがブランクの時に使用する組み込みインストーラー)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1849</name>
    <message>
        <location filename="" line="0"/>
        <source>Command line options for the script:</source>
        <translation type="obsolete">スクリプトのコマンドラインオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1850</name>
    <message>
        <location filename="" line="0"/>
        <source>Built-in installer</source>
        <translation type="obsolete">組み込みインストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1851</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1852</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1853</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1854</name>
    <message>
        <location filename="" line="0"/>
        <source>Installer</source>
        <translation type="obsolete">インストーラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1856</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1857</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1860</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1862</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1863</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1864</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1865</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1866</name>
    <message>
        <location filename="" line="0"/>
        <source>conn</source>
        <translation type="obsolete">conn</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1867</name>
    <message>
        <location filename="" line="0"/>
        <source>udp</source>
        <translation type="obsolete">udp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1868</name>
    <message>
        <location filename="" line="0"/>
        <source>rpc</source>
        <translation type="obsolete">rpc</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1869</name>
    <message>
        <location filename="" line="0"/>
        <source>h323</source>
        <translation type="obsolete">h323</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1870</name>
    <message>
        <location filename="" line="0"/>
        <source>sip</source>
        <translation type="obsolete">sip</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1871</name>
    <message>
        <location filename="" line="0"/>
        <source>sip&amp;media</source>
        <translation type="obsolete">sip&amp;media</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1872</name>
    <message>
        <location filename="" line="0"/>
        <source>unauth</source>
        <translation type="obsolete">unauth</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1873</name>
    <message>
        <location filename="" line="0"/>
        <source>telnet</source>
        <translation type="obsolete">telnet</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1874</name>
    <message>
        <location filename="" line="0"/>
        <source>ssh</source>
        <translation type="obsolete">ssh</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1875</name>
    <message>
        <location filename="" line="0"/>
        <source>ss</source>
        <translation type="obsolete">ss</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1876</name>
    <message>
        <location filename="" line="0"/>
        <source>mm</source>
        <translation type="obsolete">mm</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1877</name>
    <message>
        <location filename="" line="0"/>
        <source>hh</source>
        <translation type="obsolete">hh</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1878</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1880</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1881</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1884</name>
    <message>
        <location filename="" line="0"/>
        <source>Timeouts</source>
        <translation type="obsolete">タイムアウト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1885</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1887</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable all protocols</source>
        <translation type="obsolete">全プロトコルを有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1888</name>
    <message>
        <location filename="" line="0"/>
        <source>Disable all protocols</source>
        <translation type="obsolete">全プロトコルを無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1889</name>
    <message>
        <location filename="" line="0"/>
        <source>Skip all protocols</source>
        <translation type="obsolete">全プロトコルをスキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1890</name>
    <message>
        <location filename="" line="0"/>
        <source>Display generated commands</source>
        <translation type="obsolete">&apos;clear&apos; コマンドを生成</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1892</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1893</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1894</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1895</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1896</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1897</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1898</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1899</name>
    <message>
        <location filename="" line="0"/>
        <source>max length:</source>
        <translation type="obsolete">最大長:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1901</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1902</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1903</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1904</name>
    <message>
        <location filename="" line="0"/>
        <source>dns</source>
        <translation type="obsolete">dns</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1905</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1907</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1908</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1909</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1910</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1912</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1913</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1915</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1916</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1917</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1918</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1919</name>
    <message>
        <location filename="" line="0"/>
        <source>ftp</source>
        <translation type="obsolete">ftp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1920</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1921</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1922</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1924</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1925</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1926</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1927</name>
    <message>
        <location filename="" line="0"/>
        <source>h323 h225</source>
        <translation type="obsolete">h323 h225</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1928</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1929</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1930</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1932</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1933</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1934</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1935</name>
    <message>
        <location filename="" line="0"/>
        <source>h323 ras</source>
        <translation type="obsolete">h323 ras</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1936</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1937</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1938</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1940</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1941</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1942</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1943</name>
    <message>
        <location filename="" line="0"/>
        <source>http</source>
        <translation type="obsolete">http</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1944</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1946</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1947</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1948</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1949</name>
    <message>
        <location filename="" line="0"/>
        <source>icmp error</source>
        <translation type="obsolete">ICMP エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1950</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1951</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1952</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1954</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1955</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1956</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1957</name>
    <message>
        <location filename="" line="0"/>
        <source>ils</source>
        <translation type="obsolete">ils</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1958</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1959</name>
    <message>
        <location filename="" line="0"/>
        <source>Gateway Port:</source>
        <translation type="obsolete">ゲートウェイポート</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1960</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1962</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1963</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1964</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1965</name>
    <message>
        <location filename="" line="0"/>
        <source>mgcp</source>
        <translation type="obsolete">mgcp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1966</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1967</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1969</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1970</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1971</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1972</name>
    <message>
        <location filename="" line="0"/>
        <source>pptp</source>
        <translation type="obsolete">pptp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1973</name>
    <message>
        <location filename="" line="0"/>
        <source>Enables inspection of RSH protocol.</source>
        <translation type="obsolete">RSHプロトコルの検査を可能にする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1974</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1976</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1977</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1978</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1979</name>
    <message>
        <location filename="" line="0"/>
        <source>rsh</source>
        <translation type="obsolete">rsh</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1980</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1981</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1983</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1984</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1985</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1986</name>
    <message>
        <location filename="" line="0"/>
        <source>rtsp</source>
        <translation type="obsolete">rtsp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1987</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1988</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1989</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1991</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1992</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1993</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1994</name>
    <message>
        <location filename="" line="0"/>
        <source>sip</source>
        <translation type="obsolete">sip</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1995</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1996</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1998</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:1999</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2000</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2001</name>
    <message>
        <location filename="" line="0"/>
        <source>sip udp</source>
        <translation type="obsolete">sip udp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2002</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2003</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2004</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2006</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2007</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2008</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2009</name>
    <message>
        <location filename="" line="0"/>
        <source>skinny</source>
        <translation type="obsolete">skinny</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2010</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2011</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2012</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2014</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2015</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2016</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2017</name>
    <message>
        <location filename="" line="0"/>
        <source>smtp</source>
        <translation type="obsolete">smtp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2018</name>
    <message>
        <location filename="" line="0"/>
        <source>Enables support for SQL*Net protocol.</source>
        <translation type="obsolete">SQL*Net プロトコルのサポートを有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2019</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2020</name>
    <message>
        <location filename="" line="0"/>
        <source>--</source>
        <translation type="obsolete">--</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2022</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2023</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2024</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2025</name>
    <message>
        <location filename="" line="0"/>
        <source>sqlnet</source>
        <translation type="obsolete">sqlnet</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2026</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable TFTP application inspection.</source>
        <translation type="obsolete">TFTP アプリケーション検査を有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2027</name>
    <message>
        <location filename="" line="0"/>
        <source>port:</source>
        <translation type="obsolete">ポート:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2029</name>
    <message>
        <location filename="" line="0"/>
        <source>skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2030</name>
    <message>
        <location filename="" line="0"/>
        <source>enable</source>
        <translation type="obsolete">有効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2031</name>
    <message>
        <location filename="" line="0"/>
        <source>disable</source>
        <translation type="obsolete">無効</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2032</name>
    <message>
        <location filename="" line="0"/>
        <source>tftp</source>
        <translation type="obsolete">tftp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2033</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2034</name>
    <message>
        <location filename="" line="0"/>
        <source>Syslog</source>
        <translation type="obsolete">Syslog</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2035</name>
    <message>
        <location filename="" line="0"/>
        <source>Syslog host (name or IP address):</source>
        <translation type="obsolete">Syslog ホスト (名前か IP アドレス):</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2036</name>
    <message>
        <location filename="" line="0"/>
        <source>syslog facility:</source>
        <translation type="obsolete">syslog ファシリティ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2037</name>
    <message>
        <location filename="" line="0"/>
        <source>syslog level (&apos;logging trap&apos;):</source>
        <translation type="obsolete">syslog レベル (&apos;logging trap&apos;)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2038</name>
    <message>
        <location filename="" line="0"/>
        <source>Syslog message queue size (messages):</source>
        <translation type="obsolete">Syslog メッセージキューサイズ(メッセージ):</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2039</name>
    <message>
        <location filename="" line="0"/>
        <source>Use &apos;EMBLEM&apos; format for syslog messages</source>
        <translation type="obsolete">syslog で`書式 &apos;EMBLEM&apos; を使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2040</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2041</name>
    <message>
        <location filename="" line="0"/>
        <source>Set device id for syslog messages (v6.3 and later):</source>
        <translation type="obsolete">syslog メッセージのデバイス ID 設定 (v6.3 かそれ以上)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2042</name>
    <message>
        <location filename="" line="0"/>
        <source>use address of interface</source>
        <translation type="obsolete">インターフェースのアドレスを使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2043</name>
    <message>
        <location filename="" line="0"/>
        <source>use text string</source>
        <translation type="obsolete">テキスト文字列を使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2044</name>
    <message>
        <location filename="" line="0"/>
        <source>use hostname</source>
        <translation type="obsolete">ホスト名を使用する</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2045</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2046</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2047</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2048</name>
    <message>
        <location filename="" line="0"/>
        <source>Internal buffer</source>
        <translation type="obsolete">内部バッファ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2049</name>
    <message>
        <location filename="" line="0"/>
        <source>Console</source>
        <translation type="obsolete">コンソール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2050</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging</source>
        <translation type="obsolete">ログ記録</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2051</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2053</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2055</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2056</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+W</source>
        <translation type="obsolete">Alt+W</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2057</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable the IP Frag Guard feature (deprecated in v6.3 and later).</source>
        <translation type="obsolete">IP フラグガード機能有効 (deprecated in v6.3 and later)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2058</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2059</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2063</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2064</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2065</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2066</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2067</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2068</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2069</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2070</name>
</context>
<context>
    <name>src/gui/.ui/pixadvanceddialog_q.cpp:2071</name>
    <message>
        <location filename="" line="0"/>
        <source>PIX Options</source>
        <translation type="obsolete">PIX オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:274</name>
    <message>
        <location filename="" line="0"/>
        <source>PIX Advanced Configuration Options</source>
        <translation type="obsolete">PIX 高度な設定オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:275</name>
    <message>
        <location filename="" line="0"/>
        <source>Set PIX host name using object&apos;s name</source>
        <translation type="obsolete">オブジェクト名を使用して PIX ホスト名を設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:276</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:277</name>
    <message>
        <location filename="" line="0"/>
        <source>General</source>
        <translation type="obsolete">一般</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:278</name>
    <message>
        <location filename="" line="0"/>
        <source>NTP Servers:</source>
        <translation type="obsolete">NTP サーバー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:281</name>
    <message>
        <location filename="" line="0"/>
        <source>Server 1:</source>
        <translation type="obsolete">サーバー 1:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:282</name>
    <message>
        <location filename="" line="0"/>
        <source>Server 2:</source>
        <translation type="obsolete">サーバー 2:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:283</name>
    <message>
        <location filename="" line="0"/>
        <source>Server 3:</source>
        <translation type="obsolete">サーバー 3:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:285</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:286</name>
    <message>
        <location filename="" line="0"/>
        <source>IP address:</source>
        <translation type="obsolete">IP アドレス:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:287</name>
    <message>
        <location filename="" line="0"/>
        <source>NTP</source>
        <translation type="obsolete">NTP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:288</name>
    <message>
        <location filename="" line="0"/>
        <source>Disable SNMP Agent</source>
        <translation type="obsolete">SNMP エージェントを無効にする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:289</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:290</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP servers</source>
        <translation type="obsolete">SNMP サーバー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:292</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:293</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:294</name>
    <message>
        <location filename="" line="0"/>
        <source>Traps</source>
        <translation type="obsolete">トラップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:296</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:297</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:298</name>
    <message>
        <location filename="" line="0"/>
        <source>Traps</source>
        <translation type="obsolete">トラップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:299</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable:</source>
        <translation type="obsolete">有効:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:300</name>
    <message>
        <location filename="" line="0"/>
        <source>IP address:</source>
        <translation type="obsolete">IP アドレス:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:301</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP Server 1:</source>
        <translation type="obsolete">SNMP サーバー1:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:302</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP Server 2:</source>
        <translation type="obsolete">SNMP サーバー2:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:303</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable sending log messages as SNMP trap notifications</source>
        <translation type="obsolete">ログメッセージを SNMP トラップ通知で送信を有効にする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:304</name>
    <message>
        <location filename="" line="0"/>
        <source>SNMP</source>
        <translation type="obsolete">SNMP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:305</name>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:306</name>
    <message>
        <location filename="" line="0"/>
        <source>bytes</source>
        <translation type="obsolete">バイト</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:307</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:308</name>
    <message>
        <location filename="" line="0"/>
        <source>OK</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/pixosadvanceddialog_q.cpp:309</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:208</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:211</name>
    <message>
        <location filename="" line="0"/>
        <source>Load</source>
        <translation type="obsolete">読み込み</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:212</name>
    <message>
        <location filename="" line="0"/>
        <source>File Path</source>
        <translation type="obsolete">ファイルパス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:362</name>
    <message>
        <location filename="" line="0"/>
        <source>Preferences</source>
        <translation type="obsolete">設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:363</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:365</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:367</name>
    <message>
        <location filename="" line="0"/>
        <source>minutes</source>
        <translation type="obsolete">分</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:368</name>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:369</name>
    <message>
        <location filename="" line="0"/>
        <source>Tooltip delay:</source>
        <translation type="obsolete">ツールチップ遅延</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:370</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable object tooltips</source>
        <translation type="obsolete">オブジェクトツールチップスを有効にする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:371</name>
    <message>
        <location filename="" line="0"/>
        <source>Show deleted objects</source>
        <translation type="obsolete">削除されたオブジェクトを表示</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:372</name>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:373</name>
    <message>
        <location filename="" line="0"/>
        <source>On startup: </source>
        <translation type="obsolete">起動時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:375</name>
    <message>
        <location filename="" line="0"/>
        <source>Load standard objects</source>
        <translation type="obsolete">標準オブジェクトを読み込む</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:376</name>
    <message>
        <location filename="" line="0"/>
        <source>Load last edited file</source>
        <translation type="obsolete">最終編集ファイルを読み込む</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:377</name>
    <message>
        <location filename="" line="0"/>
        <source>Expand all branches in the object tree</source>
        <translation type="obsolete">オブジェクトツリーの全ての分岐を展開</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:378</name>
    <message>
        <location filename="" line="0"/>
        <source>Working directory:</source>
        <translation type="obsolete">作業ディレクトリ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:379</name>
    <message>
        <location filename="" line="0"/>
        <source>Browse...</source>
        <translation type="obsolete">閲覧...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:380</name>
    <message>
        <location filename="" line="0"/>
        <source>General</source>
        <translation type="obsolete">一般</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:381</name>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:382</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision Control</source>
        <translation type="obsolete">リビジョン制御</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:383</name>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:384</name>
    <message>
        <location filename="" line="0"/>
        <source>Browse...</source>
        <translation type="obsolete">閲覧...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:385</name>
    <message>
        <location filename="" line="0"/>
        <source>SSH</source>
        <translation type="obsolete">SSH</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:386</name>
    <message>
        <location filename="" line="0"/>
        <source>Add...</source>
        <translation type="obsolete">追加...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:387</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:388</name>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:389</name>
    <message>
        <location filename="" line="0"/>
        <source>Available libraries:</source>
        <translation type="obsolete">有効なライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:390</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:391</name>
    <message>
        <location filename="" line="0"/>
        <source>Load</source>
        <translation type="obsolete">読み込み</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:392</name>
    <message>
        <location filename="" line="0"/>
        <source>File Path</source>
        <translation type="obsolete">ファイルパス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:393</name>
    <message>
        <location filename="" line="0"/>
        <source>Libraries</source>
        <translation type="obsolete">ライブラリー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:394</name>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:396</name>
    <message>
        <location filename="" line="0"/>
        <source>Red</source>
        <translation type="obsolete">赤色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:397</name>
    <message>
        <location filename="" line="0"/>
        <source>Blue</source>
        <translation type="obsolete">青色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:398</name>
    <message>
        <location filename="" line="0"/>
        <source>Yellow</source>
        <translation type="obsolete">黄色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:400</name>
    <message>
        <location filename="" line="0"/>
        <source>Orange</source>
        <translation type="obsolete">オレンジ色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:405</name>
    <message>
        <location filename="" line="0"/>
        <source>Purple</source>
        <translation type="obsolete">紫色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:407</name>
    <message>
        <location filename="" line="0"/>
        <source>Green</source>
        <translation type="obsolete">緑色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:408</name>
    <message>
        <location filename="" line="0"/>
        <source>Gray</source>
        <translation type="obsolete">灰色</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/prefsdialog_q.cpp:409</name>
    <message>
        <location filename="" line="0"/>
        <source>Labels</source>
        <translation type="obsolete">ラベル</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/printingprogressdialog_q.cpp:72</name>
    <message>
        <location filename="" line="0"/>
        <source>Printing</source>
        <translation type="obsolete">印刷をしています</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/printingprogressdialog_q.cpp:73</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/printingprogressdialog_q.cpp:74</name>
    <message>
        <location filename="" line="0"/>
        <source>textLabel1</source>
        <translation type="obsolete">テキストラベル 1</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:120</name>
    <message>
        <location filename="" line="0"/>
        <source>RCSFilePreview</source>
        <translation type="obsolete">RCS ファイルプレビュー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:121</name>
    <message>
        <location filename="" line="0"/>
        <source>Open read-only</source>
        <translation type="obsolete">読み込み専用で開く</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:122</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision</source>
        <translation type="obsolete">リビジョン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:123</name>
    <message>
        <location filename="" line="0"/>
        <source>Date</source>
        <translation type="obsolete">日付</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:124</name>
    <message>
        <location filename="" line="0"/>
        <source>Author</source>
        <translation type="obsolete">著者</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:125</name>
    <message>
        <location filename="" line="0"/>
        <source>Locked by</source>
        <translation type="obsolete">ロックした人</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:126</name>
    <message>
        <location filename="" line="0"/>
        <source>RCS log:</source>
        <translation type="obsolete">RCS ログ:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:48</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision</source>
        <translation type="obsolete">リビジョン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:51</name>
    <message>
        <location filename="" line="0"/>
        <source>Date</source>
        <translation type="obsolete">日付</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:54</name>
    <message>
        <location filename="" line="0"/>
        <source>Author</source>
        <translation type="obsolete">著者</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilepreview_q.cpp:57</name>
    <message>
        <location filename="" line="0"/>
        <source>Locked by</source>
        <translation type="obsolete">ロックした人</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilesavedialog_q.cpp:100</name>
</context>
<context>
    <name>src/gui/.ui/rcsfilesavedialog_q.cpp:101</name>
    <message>
        <location filename="" line="0"/>
        <source>Check file &amp;in</source>
        <translation type="obsolete">チェックファイル入力(&amp;I)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilesavedialog_q.cpp:102</name>
    <message>
        <location filename="" line="0"/>
        <source>Alt+I</source>
        <translation type="obsolete">Alt+I</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilesavedialog_q.cpp:103</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilesavedialog_q.cpp:105</name>
    <message>
        <location filename="" line="0"/>
        <source>Checking file %1 into RCS</source>
        <translation type="obsolete">RCS にファイル %1 をチェックインしています</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilesavedialog_q.cpp:106</name>
    <message>
        <location filename="" line="0"/>
        <source>Log record for this revision: </source>
        <translation type="obsolete">このリビジョンのログレコード</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/rcsfilesavedialog_q.cpp:99</name>
    <message>
        <location filename="" line="0"/>
        <source>Log record for the new revision</source>
        <translation type="obsolete">新規リビジョンのログレコード</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/routingruleoptionsdialog_q.cpp:118</name>
    <message>
        <location filename="" line="0"/>
        <source>Routing Rule Options</source>
        <translation type="obsolete">ルーティングのルールオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/routingruleoptionsdialog_q.cpp:119</name>
</context>
<context>
    <name>src/gui/.ui/routingruleoptionsdialog_q.cpp:120</name>
</context>
<context>
    <name>src/gui/.ui/routingruleoptionsdialog_q.cpp:121</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:663</name>
    <message>
        <location filename="" line="0"/>
        <source>Rule Options for ipt</source>
        <translation type="obsolete">ipt 用ルールオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:664</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:665</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:666</name>
    <message>
        <location filename="" line="0"/>
        <source>Stateless rule</source>
        <translation type="obsolete">ステートレスルール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:667</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:668</name>
    <message>
        <location filename="" line="0"/>
        <source>General</source>
        <translation type="obsolete">一般</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:671</name>
    <message>
        <location filename="" line="0"/>
        <source>alert</source>
        <translation type="obsolete">alert</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:672</name>
    <message>
        <location filename="" line="0"/>
        <source>crit</source>
        <translation type="obsolete">crit</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:673</name>
    <message>
        <location filename="" line="0"/>
        <source>error</source>
        <translation type="obsolete">error</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:674</name>
    <message>
        <location filename="" line="0"/>
        <source>warning</source>
        <translation type="obsolete">warning</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:675</name>
    <message>
        <location filename="" line="0"/>
        <source>notice</source>
        <translation type="obsolete">notice</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:676</name>
    <message>
        <location filename="" line="0"/>
        <source>info</source>
        <translation type="obsolete">info</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:677</name>
    <message>
        <location filename="" line="0"/>
        <source>debug</source>
        <translation type="obsolete">debug</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:678</name>
    <message>
        <location filename="" line="0"/>
        <source>Log prefix:</source>
        <translation type="obsolete">ログ接頭語:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:679</name>
    <message>
        <location filename="" line="0"/>
        <source>Log level:</source>
        <translation type="obsolete">ログレベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:680</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:681</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging</source>
        <translation type="obsolete">ログ記録</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:682</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:683</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:684</name>
    <message>
        <location filename="" line="0"/>
        <source>Burst:</source>
        <translation type="obsolete">バースト:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:687</name>
    <message>
        <location filename="" line="0"/>
        <source>/day</source>
        <translation type="obsolete">/日</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:688</name>
    <message>
        <location filename="" line="0"/>
        <source>/hour</source>
        <translation type="obsolete">/時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:689</name>
    <message>
        <location filename="" line="0"/>
        <source>/minute</source>
        <translation type="obsolete">/分</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:690</name>
    <message>
        <location filename="" line="0"/>
        <source>/second</source>
        <translation type="obsolete">/秒</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:691</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:692</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:693</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:694</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:695</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:696</name>
    <message>
        <location filename="" line="0"/>
        <source>connlimit</source>
        <translation type="obsolete">ログ記録制限:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:697</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:700</name>
    <message>
        <location filename="" line="0"/>
        <source>/day</source>
        <translation type="obsolete">/日</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:701</name>
    <message>
        <location filename="" line="0"/>
        <source>/hour</source>
        <translation type="obsolete">/時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:702</name>
    <message>
        <location filename="" line="0"/>
        <source>/minute</source>
        <translation type="obsolete">/分</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:703</name>
    <message>
        <location filename="" line="0"/>
        <source>/second</source>
        <translation type="obsolete">/秒</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:704</name>
    <message>
        <location filename="" line="0"/>
        <source>Rate:</source>
        <translation type="obsolete">日付:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:705</name>
    <message>
        <location filename="" line="0"/>
        <source>Burst:</source>
        <translation type="obsolete">バースト:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:707</name>
    <message>
        <location filename="" line="0"/>
        <source>dstip</source>
        <translation type="obsolete">smtp</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:708</name>
    <message>
        <location filename="" line="0"/>
        <source>srcip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:709</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:710</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:711</name>
    <message>
        <location filename="" line="0"/>
        <source>Mode:</source>
        <translation type="obsolete">モード:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:712</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:713</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:714</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:715</name>
    <message>
        <location filename="" line="0"/>
        <source>Stateless rule</source>
        <translation type="obsolete">ステートレスルール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:716</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:717</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:718</name>
    <message>
        <location filename="" line="0"/>
        <source>General</source>
        <translation type="obsolete">一般</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:719</name>
    <message>
        <location filename="" line="0"/>
        <source>Log facility:</source>
        <translation type="obsolete">ログファシリティ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:720</name>
    <message>
        <location filename="" line="0"/>
        <source>Log level:</source>
        <translation type="obsolete">ログレベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:721</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging</source>
        <translation type="obsolete">ログ記録</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:722</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:723</name>
    <message>
        <location filename="" line="0"/>
        <source>Stateless rule</source>
        <translation type="obsolete">ステートレスルール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:724</name>
    <message>
        <location filename="" line="0"/>
        <source>General</source>
        <translation type="obsolete">一般</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:725</name>
    <message>
        <location filename="" line="0"/>
        <source>Log prefix:</source>
        <translation type="obsolete">ログ接頭語:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:726</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging</source>
        <translation type="obsolete">ログ記録</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:727</name>
    <message>
        <location filename="" line="0"/>
        <source>Activate source tracking</source>
        <translation type="obsolete">ソース追跡を有効にする</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:728</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:729</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:730</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:731</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:732</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:733</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:734</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:735</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:736</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:737</name>
    <message>
        <location filename="" line="0"/>
        <source>/</source>
        <translation type="obsolete">/</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:738</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:739</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:740</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:741</name>
    <message>
        <location filename="" line="0"/>
        <source>sec</source>
        <translation type="obsolete">秒</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:742</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:743</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:744</name>
    <message>
        <location filename="" line="0"/>
        <source>Limits</source>
        <translation type="obsolete">制限</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:745</name>
    <message>
        <location filename="" line="0"/>
        <source>Stateless rule</source>
        <translation type="obsolete">ステートレスルール</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:746</name>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:747</name>
    <message>
        <location filename="" line="0"/>
        <source>These options are only valid for PIX running software v6.3 or later</source>
        <translation type="obsolete">これらのオプションは実行中の PIX ソフトウェアバージョン 6.3 以上で有効です。</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:748</name>
    <message>
        <location filename="" line="0"/>
        <source>completely disable logging for this rule</source>
        <translation type="obsolete">このルールのログ記の無効化を
完了しました</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:749</name>
    <message>
        <location filename="" line="0"/>
        <source>Log level:</source>
        <translation type="obsolete">ログレベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/ruleoptionsdialog_q.cpp:750</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging interval:</source>
        <translation type="obsolete">ログ記録間隔:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpleinteditor_q.cpp:83</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpleinteditor_q.cpp:84</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpleinteditor_q.cpp:86</name>
    <message>
        <location filename="" line="0"/>
        <source>OK</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpletexteditor_q.cpp:86</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpletexteditor_q.cpp:88</name>
    <message>
        <location filename="" line="0"/>
        <source>OK</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpletexteditor_q.cpp:89</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpletexteditor_q.cpp:90</name>
    <message>
        <location filename="" line="0"/>
        <source>Import from file ...</source>
        <translation type="obsolete">ファイルからインポート ...</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpletextview_q.cpp:91</name>
    <message>
        <location filename="" line="0"/>
        <source>Text viewer</source>
        <translation type="obsolete">テキストビュアー</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpletextview_q.cpp:92</name>
    <message>
        <location filename="" line="0"/>
        <source>Object Name</source>
        <translation type="obsolete">オブジェクト名</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/simpletextview_q.cpp:93</name>
    <message>
        <location filename="" line="0"/>
        <source>Close</source>
        <translation type="obsolete">閉じる</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:181</name>
    <message>
        <location filename="" line="0"/>
        <source>Solaris: advanced settings</source>
        <translation type="obsolete">Solaris: 高度な設定</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:182</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;OK</source>
        <translation type="obsolete">&amp;OK</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:184</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:186</name>
    <message>
        <location filename="" line="0"/>
        <source>Ignore ICMP redirects</source>
        <translation type="obsolete">ICMP リダイレクトを無視</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:189</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:191</name>
    <message>
        <location filename="" line="0"/>
        <source>Forward directed broadcasts</source>
        <translation type="obsolete">ダイレクトブロードキャストの転送</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:192</name>
    <message>
        <location filename="" line="0"/>
        <source>Respond to echo broadcast</source>
        <translation type="obsolete">エコーブロードキャストへの応答</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:194</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:195</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:196</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:198</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:199</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:200</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:201</name>
    <message>
        <location filename="" line="0"/>
        <source>Packet forwarding</source>
        <translation type="obsolete">パケット転送</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:203</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:204</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:205</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:207</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:208</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:209</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:210</name>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:211</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:212</name>
    <message>
        <location filename="" line="0"/>
        <source>ipf:</source>
        <translation type="obsolete">ipf:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:213</name>
    <message>
        <location filename="" line="0"/>
        <source>ipnat:</source>
        <translation type="obsolete">ipnat:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:214</name>
</context>
<context>
    <name>src/gui/.ui/solarisadvanceddialog_q.cpp:215</name>
    <message>
        <location filename="" line="0"/>
        <source>Path</source>
        <translation type="obsolete">パス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tagservicedialog_q.cpp:142</name>
</context>
<context>
    <name>src/gui/.ui/tagservicedialog_q.cpp:143</name>
    <message>
        <location filename="" line="0"/>
        <source>Tag Service</source>
        <translation type="obsolete">タグサービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tagservicedialog_q.cpp:144</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tagservicedialog_q.cpp:145</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tagservicedialog_q.cpp:146</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tagservicedialog_q.cpp:147</name>
    <message>
        <location filename="" line="0"/>
        <source>Code:</source>
        <translation type="obsolete">コード:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:349</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP</source>
        <translation type="obsolete">TCP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:350</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP Service</source>
        <translation type="obsolete">TCP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:351</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:352</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP Flags</source>
        <translation type="obsolete">TCP フラグ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:354</name>
    <message>
        <location filename="" line="0"/>
        <source>A</source>
        <translation type="obsolete">A</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:359</name>
    <message>
        <location filename="" line="0"/>
        <source>U</source>
        <translation type="obsolete">U</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:362</name>
    <message>
        <location filename="" line="0"/>
        <source>Mask:</source>
        <translation type="obsolete">マスク:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:364</name>
    <message>
        <location filename="" line="0"/>
        <source>Settings:</source>
        <translation type="obsolete">設定:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:365</name>
    <message>
        <location filename="" line="0"/>
        <source>S</source>
        <translation type="obsolete">S</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:367</name>
    <message>
        <location filename="" line="0"/>
        <source>F</source>
        <translation type="obsolete">F</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:368</name>
    <message>
        <location filename="" line="0"/>
        <source>P</source>
        <translation type="obsolete">P</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:369</name>
    <message>
        <location filename="" line="0"/>
        <source>R</source>
        <translation type="obsolete">R</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:374</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:375</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:376</name>
    <message>
        <location filename="" line="0"/>
        <source>Source Port Range</source>
        <translation type="obsolete">送信元ポートレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:377</name>
    <message>
        <location filename="" line="0"/>
        <source>Start:</source>
        <translation type="obsolete">開始:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:378</name>
    <message>
        <location filename="" line="0"/>
        <source>End:</source>
        <translation type="obsolete">終了:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:379</name>
    <message>
        <location filename="" line="0"/>
        <source>Destination Port Range</source>
        <translation type="obsolete">送信先ポートレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:380</name>
    <message>
        <location filename="" line="0"/>
        <source>Start:</source>
        <translation type="obsolete">開始:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/tcpservicedialog_q.cpp:381</name>
    <message>
        <location filename="" line="0"/>
        <source>End:</source>
        <translation type="obsolete">終了:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:235</name>
    <message>
        <location filename="" line="0"/>
        <source>Time</source>
        <translation type="obsolete">時間</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:236</name>
    <message>
        <location filename="" line="0"/>
        <source>Time Interval</source>
        <translation type="obsolete">時間間隔</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:237</name>
    <message>
        <location filename="" line="0"/>
        <source>Activate a rule on:</source>
        <translation type="obsolete">ルールの活性化日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:238</name>
    <message>
        <location filename="" line="0"/>
        <source>Day of week (0-6):</source>
        <translation type="obsolete">曜日数 (0-6):</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:239</name>
    <message>
        <location filename="" line="0"/>
        <source>Date:</source>
        <translation type="obsolete">日付:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:240</name>
    <message>
        <location filename="" line="0"/>
        <source>Time:</source>
        <translation type="obsolete">時間:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:241</name>
    <message>
        <location filename="" line="0"/>
        <source>Deactivate a rule on:</source>
        <translation type="obsolete">ルールの非活性化日時</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:242</name>
    <message>
        <location filename="" line="0"/>
        <source>Day of week (0-6):</source>
        <translation type="obsolete">曜日数 (0-6):</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:243</name>
    <message>
        <location filename="" line="0"/>
        <source>Date:</source>
        <translation type="obsolete">日付:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:244</name>
    <message>
        <location filename="" line="0"/>
        <source>Time:</source>
        <translation type="obsolete">時間:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:245</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:246</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/timedialog_q.cpp:247</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:221</name>
    <message>
        <location filename="" line="0"/>
        <source>UDP</source>
        <translation type="obsolete">UDP</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:222</name>
    <message>
        <location filename="" line="0"/>
        <source>UDP Service</source>
        <translation type="obsolete">UDP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:223</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment:</source>
        <translation type="obsolete">コメント:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:224</name>
    <message>
        <location filename="" line="0"/>
        <source>Source Port Range</source>
        <translation type="obsolete">送信元ポートレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:225</name>
    <message>
        <location filename="" line="0"/>
        <source>Start:</source>
        <translation type="obsolete">開始:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:226</name>
    <message>
        <location filename="" line="0"/>
        <source>End:</source>
        <translation type="obsolete">終了:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:227</name>
    <message>
        <location filename="" line="0"/>
        <source>Destination Port Range</source>
        <translation type="obsolete">送信先ポートレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:228</name>
    <message>
        <location filename="" line="0"/>
        <source>Start:</source>
        <translation type="obsolete">開始:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:229</name>
    <message>
        <location filename="" line="0"/>
        <source>End:</source>
        <translation type="obsolete">終了:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:230</name>
    <message>
        <location filename="" line="0"/>
        <source>Library:</source>
        <translation type="obsolete">ライブラリー:</translation>
    </message>
</context>
<context>
    <name>src/gui/.ui/udpservicedialog_q.cpp:231</name>
    <message>
        <location filename="" line="0"/>
        <source>Name:</source>
        <translation type="obsolete">名前:</translation>
    </message>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:122</name>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:123</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:222</name>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:223</name>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:226</name>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:227</name>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:97</name>
</context>
<context>
    <name>src/gui/ActionsDialog.cpp:98</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/AddressRangeDialog.cpp:107</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation type="obsolete">不当な IP アドレス &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>src/gui/AddressRangeDialog.cpp:108</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/AddressRangeDialog.cpp:118</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation type="obsolete">不当な IP アドレス &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>src/gui/AddressRangeDialog.cpp:119</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/ColorLabelMenuItem.cpp:48</name>
    <message>
        <location filename="" line="0"/>
        <source>no color</source>
        <translation type="obsolete">色なし</translation>
    </message>
</context>
<context>
    <name>src/gui/CommentEditorPanel.cpp:75</name>
</context>
<context>
    <name>src/gui/CommentEditorPanel.cpp:80</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose file that contains PIX commands</source>
        <translation type="obsolete">PIX コマンドを含むファイルを選択してください。</translation>
    </message>
</context>
<context>
    <name>src/gui/CommentEditorPanel.cpp:88</name>
    <message>
        <location filename="" line="0"/>
        <source>Could not open file %1</source>
        <translation type="obsolete">ファイル %1 を開くことが出来ません。</translation>
    </message>
</context>
<context>
    <name>src/gui/ConfirmDeleteObjectDialog.cpp:157</name>
    <message>
        <location filename="" line="0"/>
        <source>NAT</source>
        <translation type="obsolete">NAT</translation>
    </message>
</context>
<context>
    <name>src/gui/ConfirmDeleteObjectDialog.cpp:160</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy</source>
        <translation type="obsolete">ポリシー</translation>
    </message>
</context>
<context>
    <name>src/gui/ConfirmDeleteObjectDialog.cpp:163</name>
    <message>
        <location filename="" line="0"/>
        <source>Routing</source>
        <translation type="obsolete">ルーティング</translation>
    </message>
</context>
<context>
    <name>src/gui/ConfirmDeleteObjectDialog.cpp:166</name>
    <message>
        <location filename="" line="0"/>
        <source>Unknown rule set</source>
        <translation type="obsolete">不明なルールセット</translation>
    </message>
</context>
<context>
    <name>src/gui/ConfirmDeleteObjectDialog.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>/Rule%1</source>
        <translation type="obsolete">ルール %1</translation>
    </message>
</context>
<context>
    <name>src/gui/ConfirmDeleteObjectDialog.cpp:182</name>
    <message>
        <location filename="" line="0"/>
        <source>Type: </source>
        <translation type="obsolete">種類: </translation>
    </message>
</context>
<context>
    <name>src/gui/ConfirmDeleteObjectDialog.cpp:203</name>
</context>
<context>
    <name>src/gui/DialogFactory.cpp:158</name>
</context>
<context>
    <name>src/gui/DialogFactory.cpp:181</name>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1086</name>
    <message>
        <location filename="" line="0"/>
        <source>Adding objects ...</source>
        <translation type="obsolete">オブジェクトを追加しています ...</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1158</name>
    <message>
        <location filename="" line="0"/>
        <source>Adding objects ...</source>
        <translation type="obsolete">オブジェクトを追加しています ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1159</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1362</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Prepare objects ...</source>
        <translation type="obsolete">オブジェクトの準備 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1507</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Copying results ...</source>
        <translation type="obsolete">結果をコピーしています ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1548</name>
    <message>
        <location filename="" line="0"/>
        <source>Copying results ...</source>
        <translation type="obsolete">結果をコピーしています ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1549</name>
    <message>
        <location filename="" line="0"/>
        <source>Cancel</source>
        <translation type="obsolete">取り消し</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1675</name>
    <message>
        <location filename="" line="0"/>
        <source>Discovery error</source>
        <translation type="obsolete">検出エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1722</name>
    <message>
        <location filename="" line="0"/>
        <source>Discovery error</source>
        <translation type="obsolete">検出エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1838</name>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:1917</name>
    <message>
        <location filename="" line="0"/>
        <source>Empty community string</source>
        <translation type="obsolete">コミュニティ文字列が空です</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:2132</name>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:2143</name>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:616</name>
    <message>
        <location filename="" line="0"/>
        <source>Hosts file parsing ...</source>
        <translation type="obsolete">ホストファイルを解析しています ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:625</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS zone transfer ...</source>
        <translation type="obsolete">DNS ゾーン転送 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:635</name>
    <message>
        <location filename="" line="0"/>
        <source>Network discovery using SNMP ...</source>
        <translation type="obsolete">SNMP を用いたネットワーク検出 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:645</name>
    <message>
        <location filename="" line="0"/>
        <source>Import configuration from file ...</source>
        <translation type="obsolete">ファイルから構成のインポート ...</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:790</name>
    <message>
        <location filename="" line="0"/>
        <source>Discovery error</source>
        <translation type="obsolete">検出エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/DiscoveryDruid.cpp:791</name>
    <message>
        <location filename="" line="0"/>
        <source>Could not open file %1</source>
        <translation type="obsolete">ファイル %1 を開くことが出来ません。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWBSettings.cpp:150</name>
    <message>
        <location filename="" line="0"/>
        <source>Working directory %1 does not exist and could not be created.
Ignoring this setting.</source>
        <translation type="obsolete">作業ディレクトリ %1 が存在しない上に作成できませんでした。
この設定は無効になります。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWBTree.cpp:399</name>
    <message>
        <location filename="" line="0"/>
        <source>New Library</source>
        <translation type="obsolete">新規ライブラリー</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectDropArea.cpp:103</name>
    <message>
        <location filename="" line="0"/>
        <source>Drop object here.</source>
        <translation type="obsolete">ここにオブジェクトをドロップします。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectDropArea.cpp:141</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste</source>
        <translation type="obsolete">貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectDropArea.cpp:143</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:102</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS record: </source>
        <translation type="obsolete">DNS レコード: </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:106</name>
    <message>
        <location filename="" line="0"/>
        <source>Address Table: </source>
        <translation type="obsolete">アドレステーブル: </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:157</name>
    <message>
        <location filename="" line="0"/>
        <source> objects</source>
        <translation type="obsolete"> オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:173</name>
    <message>
        <location filename="" line="0"/>
        <source>protocol: %1</source>
        <translation type="obsolete">プロトコル: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>type: %1</source>
        <translation type="obsolete">種類: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:179</name>
    <message>
        <location filename="" line="0"/>
        <source>code: %1</source>
        <translation type="obsolete">コード: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:238</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Library:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:243</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Object Id:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;オブジェクト ID:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:248</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Object Type:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;オブジェクトタイプ:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:252</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Object Name:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&lt;&gt;オブジェクト名:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:274</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;DNS record:&lt;/b&gt;</source>
        <translation type="obsolete">&lt;b&gt;DNS レコード:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:277</name>
    <message>
        <location filename="" line="0"/>
        <source>Run-time</source>
        <translation type="obsolete">ランタイム</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Compile-time</source>
        <translation type="obsolete">コンパイルタイム</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:282</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Table file:&lt;/b&gt;</source>
        <translation type="obsolete">&lt;b&lt;&gt;テーブルファイル:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:285</name>
    <message>
        <location filename="" line="0"/>
        <source>Run-time</source>
        <translation type="obsolete">ランタイム</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Compile-time</source>
        <translation type="obsolete">コンパイルタイム</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:320</name>
    <message>
        <location filename="" line="0"/>
        <source>%1 objects&lt;br&gt;
</source>
        <translation type="obsolete">%1 オブジェクト&lt;br&gt;
</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:385</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Path:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;パス:&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:444</name>
    <message>
        <location filename="" line="0"/>
        <source>protocol </source>
        <translation type="obsolete">プロトコル </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:449</name>
    <message>
        <location filename="" line="0"/>
        <source>type: </source>
        <translation type="obsolete">種類: </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:451</name>
    <message>
        <location filename="" line="0"/>
        <source>code: </source>
        <translation type="obsolete">コード: </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:471</name>
    <message>
        <location filename="" line="0"/>
        <source>Pattern: &quot;%1&quot;</source>
        <translation type="obsolete">パターン: 「%1」</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:605</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Action   :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;操作     :&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:608</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Parameter:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;パラメーター:&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:631</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Log prefix    :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ログ接頭語    :&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:637</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Log Level     :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ログレベル    :&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:644</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Netlink group :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ネットリンクグループ:&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:650</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Limit Value   :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;制限値        :&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:656</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:663</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Limit burst   :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:670</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:676</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;li&gt;&lt;b&gt;Stateless&lt;/b&gt;&lt;/li&gt; </source>
        <translation type="obsolete">&lt;li&gt;&lt;b&gt;ステートレス&lt;/b&gt;&lt;/li&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:685</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Log facility:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ログファシリティ:&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:692</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Log level   :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ログレベル  :&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:700</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:706</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;li&gt;&lt;b&gt;Stateless&lt;/b&gt;&lt;/li&gt; </source>
        <translation type="obsolete">&lt;li&gt;&lt;b&gt;ステートレス&lt;/b&gt;&lt;/li&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:712</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:722</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Log prefix :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ログ接頭語 :&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:728</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Max state  :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:735</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;li&gt;&lt;b&gt;Stateless&lt;/b&gt;&lt;/li&gt; </source>
        <translation type="obsolete">&lt;li&gt;&lt;b&gt;ステートレス&lt;/b&gt;&lt;/li&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:741</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:744</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:747</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Max src states:&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:758</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;li&gt;&lt;b&gt;Stateless&lt;/b&gt;&lt;/li&gt; </source>
        <translation type="obsolete">&lt;li&gt;&lt;b&gt;ステートレス&lt;/b&gt;&lt;/li&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:767</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:775</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Log level   :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ログレベル  :&lt;/b&gt; </translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:781</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Log interval  :&lt;/b&gt; </source>
        <translation type="obsolete">&lt;b&gt;ライブラリー:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:788</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;li&gt;&lt;b&gt;Disable logging for this rule&lt;/b&gt;&lt;/li&gt; </source>
        <translation type="obsolete">このルールのログ記の無効化を
完了しました</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:820</name>
    <message>
        <location filename="" line="0"/>
        <source>bitmask</source>
        <translation type="obsolete">ネットマスク</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:821</name>
    <message>
        <location filename="" line="0"/>
        <source>random</source>
        <translation type="obsolete">ランダム</translation>
    </message>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:822</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:823</name>
</context>
<context>
    <name>src/gui/FWObjectPropertiesFactory.cpp:825</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1080</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Builder 2 uses file extension &apos;.fwb&apos; and 
needs to rename old data file &apos;%1&apos; to &apos;%2&apos;,
but file &apos;%3&apos; already exists.
Choose a different name for the new file.</source>
        <translation type="obsolete">ファイアウォールビルダー2 はファイル拡張子 &apos;.fwb&apos; を使用します。
あなたのデータファイルは &apos;%1&apos; は &apos;%2&apos; に名称変更する必要があります。&lt;
しかしファイル &apos;%3&apos; は既に存在します。
違う新規ファイル名を選んでください。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1082</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1086</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose name and location for the new file</source>
        <translation type="obsolete">新規ファイルの名前とフォルダーを選択</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1094</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1095</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1109</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall Builder 2 uses file extension &apos;.fwb&apos;. Your data file &apos;%1&apos; 
has been renamed &apos;%2&apos;</source>
        <translation type="obsolete">ファイアウォールビルダー2 はファイル拡張子 &apos;.fwb&apos; を使用します。あなたのデータファイルは &apos;%1&apos; は &apos;%2&apos; に名称変更されました。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1111</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1124</name>
    <message>
        <location filename="" line="0"/>
        <source>(read-only)</source>
        <translation type="obsolete">(読み込み専用)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1140</name>
    <message>
        <location filename="" line="0"/>
        <source>Exception: %1</source>
        <translation type="obsolete">例外: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1142</name>
    <message>
        <location filename="" line="0"/>
        <source>Failed transformation : %1</source>
        <translation type="obsolete">変換に失敗しました : %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1144</name>
    <message>
        <location filename="" line="0"/>
        <source>XML element : %1</source>
        <translation type="obsolete">XML 要素 : %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1148</name>
    <message>
        <location filename="" line="0"/>
        <source>Error loading file:
%1</source>
        <translation type="obsolete">ファイルの読み込みエラー:
%1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1149</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1154</name>
    <message>
        <location filename="" line="0"/>
        <source>Error loading file:
%1</source>
        <translation type="obsolete">ファイルの読み込みエラー:
%1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1155</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1167</name>
    <message>
        <location filename="" line="0"/>
        <source>Building object tree...</source>
        <translation type="obsolete">オブジェクトツリーを構築しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1172</name>
    <message>
        <location filename="" line="0"/>
        <source>Indexing...</source>
        <translation type="obsolete">インデックスを作成しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1198</name>
    <message>
        <location filename="" line="0"/>
        <source>Checking file %1 in RCS</source>
        <translation type="obsolete">RCS にファイル %1 をチェックインしています</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1223</name>
    <message>
        <location filename="" line="0"/>
        <source>Error checking in file %1:
%2</source>
        <translation type="obsolete">ファイル %1 の検査中のエラーです:%2</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1225</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1311</name>
    <message>
        <location filename="" line="0"/>
        <source>File is read-only</source>
        <translation type="obsolete">ファイルは読み込み専用です。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1317</name>
    <message>
        <location filename="" line="0"/>
        <source>Error saving file %1: %2</source>
        <translation type="obsolete">ファイル %1 の保存エラー: %2</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1319</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1360</name>
    <message>
        <location filename="" line="0"/>
        <source>Duplicate library &apos;%1&apos;</source>
        <translation type="obsolete">ライブラリー %1 の複製</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1361</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1382</name>
    <message>
        <location filename="" line="0"/>
        <source>Error loading file %1:
%2</source>
        <translation type="obsolete">ファイル %1 の読み込みエラー:
%2</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1384</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1396</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose a file to import</source>
        <translation type="obsolete">インポートするファイルを選んでください。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1414</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1427</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose the first file</source>
        <translation type="obsolete">1 つ目のファイルを選んでください</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1434</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose the second file</source>
        <translation type="obsolete">2 つ目のファイルを選んでください</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1455</name>
    <message>
        <location filename="" line="0"/>
        <source>Error loading file %1:
%2</source>
        <translation type="obsolete">ファイル %1 の読み込みエラー:
%2</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1457</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1473</name>
    <message>
        <location filename="" line="0"/>
        <source>Error loading file %1:
%2</source>
        <translation type="obsolete">ファイル %1 の読み込みエラー:
%2</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1475</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1497</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1510</name>
    <message>
        <location filename="" line="0"/>
        <source>TXT Files (*.txt);;All Files (*)</source>
        <translation type="obsolete">FWB ファイル (*.fwb);;すべてのファイル (*)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1512</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose name and location for the report file</source>
        <translation type="obsolete">新規ファイルの名前とフォルダーを選択</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1537</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1538</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1548</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1550</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1665</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1674</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1685</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1695</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1710</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:175</name>
    <message>
        <location filename="" line="0"/>
        <source>No firewalls defined</source>
        <translation type="obsolete">ファイアウォールは未定義です</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1751</name>
    <message>
        <location filename="" line="0"/>
        <source>File is read-only</source>
        <translation type="obsolete">ファイルは読み込み専用です。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1755</name>
    <message>
        <location filename="" line="0"/>
        <source>Error saving file %1: %2</source>
        <translation type="obsolete">ファイル %1 の保存エラー: %2</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1781</name>
    <message>
        <location filename="" line="0"/>
        <source>Please select a library you want to export.</source>
        <translation type="obsolete">エクスポートしたいライブラリーを選択してください。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1806</name>
    <message>
        <location filename="" line="0"/>
        <source>The file %1 already exists.
Do you want to overwrite it ?</source>
        <translation type="obsolete">ファイル %1 は既に存在します。
本当に上書きしますか?</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:1808</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Yes</source>
        <translation type="obsolete">はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;No</source>
        <translation type="obsolete">いいえ(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2000</name>
    <message>
        <location filename="" line="0"/>
        <source>%1</source>
        <translation type="obsolete">%1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2012</name>
    <message>
        <location filename="" line="0"/>
        <source>Building branch policy view &apos;%1&apos;...</source>
        <translation type="obsolete">ポリシービュー「%1」の分岐を構築しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2082</name>
    <message>
        <location filename="" line="0"/>
        <source>Building policy view...</source>
        <translation type="obsolete">ポリシービューを構築しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2088</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy</source>
        <translation type="obsolete">ポリシー</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2111</name>
    <message>
        <location filename="" line="0"/>
        <source>Building NAT view...</source>
        <translation type="obsolete">NAT ビューを構築しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2116</name>
    <message>
        <location filename="" line="0"/>
        <source>NAT</source>
        <translation type="obsolete">NAT</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2126</name>
    <message>
        <location filename="" line="0"/>
        <source>Building routing view...</source>
        <translation type="obsolete">ルーティングビューを構築しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:2131</name>
    <message>
        <location filename="" line="0"/>
        <source>Routing</source>
        <translation type="obsolete">ルーティング</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:379</name>
    <message>
        <location filename="" line="0"/>
        <source>Some objects have been modified but not saved.
Do you want to save changes now ?</source>
        <translation type="obsolete">いくつかのオブジェクトの修正が保存されていません。
変更を保存しますか?</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:381</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Save</source>
        <translation type="obsolete">保存(&amp;S)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Discard</source>
        <translation type="obsolete">破棄する(&amp;D)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:436</name>
    <message>
        <location filename="" line="0"/>
        <source>FWB Files (*.fwb);;All Files (*)</source>
        <translation type="obsolete">FWB ファイル (*.fwb);;すべてのファイル (*)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:447</name>
    <message>
        <location filename="" line="0"/>
        <source>The file %1 already exists.
Do you want to overwrite it ?</source>
        <translation type="obsolete">ファイル %1 は既に存在します。
本当に上書きしますか?</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:449</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Yes</source>
        <translation type="obsolete">はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;No</source>
        <translation type="obsolete">いいえ(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:483</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose name and location for the new file</source>
        <translation type="obsolete">新規ファイルの名前とフォルダーを選択</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:585</name>
    <message>
        <location filename="" line="0"/>
        <source>Saving data to file...</source>
        <translation type="obsolete">ファイルにデータを保存しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:617</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose name and location for the file</source>
        <translation type="obsolete">ファイルの名前とフォルダーを選択する</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:674</name>
</context>
<context>
    <name>src/gui/FWWindow.cpp:679</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Discard changes</source>
        <translation type="obsolete">変更のやり直し(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:680</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:723</name>
    <message>
        <location filename="" line="0"/>
        <source>File %1 has been added to RCS.</source>
        <translation type="obsolete">ファイル %1 を RCS に追加しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:724</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:732</name>
    <message>
        <location filename="" line="0"/>
        <source>Error adding file to RCS:
%1</source>
        <translation type="obsolete">RCS へのファイルの追加エラー:
%1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:733</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:739</name>
    <message>
        <location filename="" line="0"/>
        <source>(read-only)</source>
        <translation type="obsolete">(読み込み専用)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:798</name>
    <message>
        <location filename="" line="0"/>
        <source>Loading system objects...</source>
        <translation type="obsolete">システムオブジェクトを読み込んでいます...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:857</name>
    <message>
        <location filename="" line="0"/>
        <source>Error loading file:
%1</source>
        <translation type="obsolete">ファイルの読み込みエラー:
%1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:858</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:908</name>
    <message>
        <location filename="" line="0"/>
        <source>Loading system objects...</source>
        <translation type="obsolete">システムオブジェクトを読み込んでいます...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:916</name>
    <message>
        <location filename="" line="0"/>
        <source>Reading and parsing data file...</source>
        <translation type="obsolete">データファイルの読み込みと解析をしています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindow.cpp:986</name>
    <message>
        <location filename="" line="0"/>
        <source>Merging with system objects...</source>
        <translation type="obsolete">システムオブジェクトと結合しています...</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:100</name>
    <message>
        <location filename="" line="0"/>
        <source>IP Service</source>
        <translation type="obsolete">IP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:101</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP Service</source>
        <translation type="obsolete">ICMP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:102</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP Service</source>
        <translation type="obsolete">TCP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:103</name>
    <message>
        <location filename="" line="0"/>
        <source>UDP Service</source>
        <translation type="obsolete">UDP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:104</name>
    <message>
        <location filename="" line="0"/>
        <source>Group of services</source>
        <translation type="obsolete">サービスのグループ</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:105</name>
    <message>
        <location filename="" line="0"/>
        <source>Time Interval</source>
        <translation type="obsolete">時間間隔</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:281</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall name: %1</source>
        <translation type="obsolete">* ファイアウォール名: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:282</name>
    <message>
        <location filename="" line="0"/>
        <source>Platform: </source>
        <translation type="obsolete">プラットフォーム: </translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:283</name>
    <message>
        <location filename="" line="0"/>
        <source>Version: </source>
        <translation type="obsolete">バージョン: </translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:284</name>
    <message>
        <location filename="" line="0"/>
        <source>Host OS: </source>
        <translation type="obsolete">ホスト OS: </translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:290</name>
    <message>
        <location filename="" line="0"/>
        <source>Global Policy</source>
        <translation type="obsolete">全体ポリシー</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:343</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface %1</source>
        <translation type="obsolete">インターフェース %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:371</name>
    <message>
        <location filename="" line="0"/>
        <source>NAT</source>
        <translation type="obsolete">NAT</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:398</name>
    <message>
        <location filename="" line="0"/>
        <source>Routing</source>
        <translation type="obsolete">ルーティング</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:538</name>
    <message>
        <location filename="" line="0"/>
        <source>Legend</source>
        <translation type="obsolete">凡例</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:629</name>
    <message>
        <location filename="" line="0"/>
        <source>Objects</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:851</name>
    <message>
        <location filename="" line="0"/>
        <source>Groups</source>
        <translation type="obsolete">グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:894</name>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:916</name>
    <message>
        <location filename="" line="0"/>
        <source>Printing aborted</source>
        <translation type="obsolete">印刷を中断しました</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:919</name>
    <message>
        <location filename="" line="0"/>
        <source>Printing aborted</source>
        <translation type="obsolete">印刷を中断しました</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:92</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall</source>
        <translation type="obsolete">ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:920</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:923</name>
    <message>
        <location filename="" line="0"/>
        <source>Printing completed</source>
        <translation type="obsolete">印刷を完了しました</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:927</name>
    <message>
        <location filename="" line="0"/>
        <source>Printing aborted</source>
        <translation type="obsolete">印刷を中断しました</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:93</name>
    <message>
        <location filename="" line="0"/>
        <source>Host</source>
        <translation type="obsolete">ホスト</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:94</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:95</name>
    <message>
        <location filename="" line="0"/>
        <source>Addres Range</source>
        <translation type="obsolete">アドレスレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:96</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:97</name>
    <message>
        <location filename="" line="0"/>
        <source>Network</source>
        <translation type="obsolete">ネットワーク</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:98</name>
    <message>
        <location filename="" line="0"/>
        <source>Group of objects</source>
        <translation type="obsolete">オブジェクトのツリー</translation>
    </message>
</context>
<context>
    <name>src/gui/FWWindowPrint.cpp:99</name>
    <message>
        <location filename="" line="0"/>
        <source>Custom Service</source>
        <translation type="obsolete">カスタムサービス</translation>
    </message>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:102</name>
    <message>
        <location filename="" line="0"/>
        <source>Filter error</source>
        <translation type="obsolete">フィルターエラー</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Invalid RegExp.</source>
        <translation type="obsolete">不正な正規表現です。</translation>
    </message>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:404</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:405</name>
    <message>
        <location filename="" line="0"/>
        <source>Address</source>
        <translation type="obsolete">アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:408</name>
    <message>
        <location filename="" line="0"/>
        <source>Contains</source>
        <translation type="obsolete">内容</translation>
    </message>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:409</name>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:410</name>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:411</name>
    <message>
        <location filename="" line="0"/>
        <source>Ends with</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:412</name>
</context>
<context>
    <name>src/gui/FilterDialog.cpp:413</name>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:316</name>
    <message>
        <location filename="" line="0"/>
        <source>Search hit the end of the policy rules.</source>
        <translation type="obsolete">検索のヒットがオブジェクトツリーの最後です。</translation>
    </message>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:317</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue at top</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Stop</source>
        <translation type="obsolete">停止(&amp;S)</translation>
    </message>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:324</name>
    <message>
        <location filename="" line="0"/>
        <source>Search hit the end of the object tree.</source>
        <translation type="obsolete">検索のヒットがオブジェクトツリーの最後です。</translation>
    </message>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:325</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue at top</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Stop</source>
        <translation type="obsolete">停止(&amp;S)</translation>
    </message>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:354</name>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:364</name>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:372</name>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:466</name>
    <message>
        <location filename="" line="0"/>
        <source>Replaced %1 objects.</source>
        <translation type="obsolete">このオブジェクトと置換</translation>
    </message>
</context>
<context>
    <name>src/gui/FindObjectWidget.cpp:585</name>
</context>
<context>
    <name>src/gui/FindWhereUsedWidget.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>NAT</source>
        <translation type="obsolete">NAT</translation>
    </message>
</context>
<context>
    <name>src/gui/FindWhereUsedWidget.cpp:174</name>
    <message>
        <location filename="" line="0"/>
        <source>Policy</source>
        <translation type="obsolete">ポリシー</translation>
    </message>
</context>
<context>
    <name>src/gui/FindWhereUsedWidget.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>Routing</source>
        <translation type="obsolete">ルーティング</translation>
    </message>
</context>
<context>
    <name>src/gui/FindWhereUsedWidget.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>Unknown rule set</source>
        <translation type="obsolete">不明なルールセット</translation>
    </message>
</context>
<context>
    <name>src/gui/FindWhereUsedWidget.cpp:182</name>
    <message>
        <location filename="" line="0"/>
        <source>/Rule%1</source>
        <translation type="obsolete">ルール %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FindWhereUsedWidget.cpp:196</name>
    <message>
        <location filename="" line="0"/>
        <source>Type: </source>
        <translation type="obsolete">種類: </translation>
    </message>
</context>
<context>
    <name>src/gui/FirewallDialog.cpp:314</name>
    <message>
        <location filename="" line="0"/>
        <source>FWBuilder API error: %1</source>
        <translation type="obsolete">FWBuilder API エラー: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FirewallDialog.cpp:315</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/FirewallDialog.cpp:339</name>
    <message>
        <location filename="" line="0"/>
        <source>FWBuilder API error: %1</source>
        <translation type="obsolete">FWBuilder API エラー: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/FirewallDialog.cpp:340</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:144</name>
    <message>
        <location filename="" line="0"/>
        <source>Name</source>
        <translation type="obsolete">名前</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:145</name>
    <message>
        <location filename="" line="0"/>
        <source>Properties</source>
        <translation type="obsolete">プロパティ</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:675</name>
    <message>
        <location filename="" line="0"/>
        <source>Open</source>
        <translation type="obsolete">開く</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:677</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:680</name>
    <message>
        <location filename="" line="0"/>
        <source>Copy</source>
        <translation type="obsolete">コピー</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:681</name>
    <message>
        <location filename="" line="0"/>
        <source>Cut</source>
        <translation type="obsolete">切り取り</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:682</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste</source>
        <translation type="obsolete">貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/GroupObjectDialog.cpp:683</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/IPv4Dialog.cpp:145</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation type="obsolete">不当な IP アドレス &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>src/gui/IPv4Dialog.cpp:146</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/IPv4Dialog.cpp:159</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal netmask &apos;%1&apos;</source>
        <translation type="obsolete">不当なネットマスク 「%1」</translation>
    </message>
</context>
<context>
    <name>src/gui/IPv4Dialog.cpp:160</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/IPv4Dialog.cpp:263</name>
</context>
<context>
    <name>src/gui/IPv4Dialog.cpp:270</name>
    <message>
        <location filename="" line="0"/>
        <source>DNS lookup failed for name of the address object &apos;%1&apos;.</source>
        <translation type="obsolete">アドレスオブジェクト &apos;%1&apos; の名前を DNS 解決に失敗しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/InstallFirewallViewItem.cpp:31</name>
    <message>
        <location filename="" line="0"/>
        <source>Failure</source>
        <translation type="obsolete">失敗</translation>
    </message>
</context>
<context>
    <name>src/gui/InstallFirewallViewItem.cpp:44</name>
    <message>
        <location filename="" line="0"/>
        <source>Success</source>
        <translation type="obsolete">成功</translation>
    </message>
</context>
<context>
    <name>src/gui/InterfaceDialog.cpp:193</name>
    <message>
        <location filename="" line="0"/>
        <source>Group: </source>
        <translation type="obsolete">グループ: </translation>
    </message>
</context>
<context>
    <name>src/gui/InterfaceDialog.cpp:211</name>
    <message>
        <location filename="" line="0"/>
        <source>Network: </source>
        <translation type="obsolete">ネットワーク: </translation>
    </message>
</context>
<context>
    <name>src/gui/LibraryDialog.cpp:151</name>
    <message>
        <location filename="" line="0"/>
        <source>Pick the color for this library</source>
        <translation type="obsolete">このライブラリーの色を選択してください。</translation>
    </message>
</context>
<context>
    <name>src/gui/NetworkDialog.cpp:108</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal IP address &apos;%1&apos;</source>
        <translation type="obsolete">不当な IP アドレス &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>src/gui/NetworkDialog.cpp:109</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/NetworkDialog.cpp:119</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal netmask &apos;%1&apos;</source>
        <translation type="obsolete">不当なネットマスク 「%1」</translation>
    </message>
</context>
<context>
    <name>src/gui/NetworkDialog.cpp:120</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:117</name>
    <message>
        <location filename="" line="0"/>
        <source>Object &apos;%1&apos; has been deleted</source>
        <translation type="obsolete">オブジェクト「%1」を削除しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:118</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:141</name>
    <message>
        <location filename="" line="0"/>
        <source>Object &apos;%1&apos; has been deleted</source>
        <translation type="obsolete">オブジェクト「%1」を削除しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:142</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:176</name>
    <message>
        <location filename="" line="0"/>
        <source>Object &apos;%1&apos; in the objects tree</source>
        <translation type="obsolete">検索のヒットがオブジェクトツリーの最後です。</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:178</name>
    <message>
        <location filename="" line="0"/>
        <source>Object &apos;%1&apos; in file %2</source>
        <translation type="obsolete">ファイル %2 中の オブジェクト「%1」</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>Object &apos;%1&apos; in file %2</source>
        <translation type="obsolete">ファイル %2 中の オブジェクト「%1」</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:297</name>
    <message>
        <location filename="" line="0"/>
        <source>Next</source>
        <translation type="obsolete">次へ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:299</name>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:300</name>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:63</name>
    <message>
        <location filename="" line="0"/>
        <source>Keep current object</source>
        <translation type="obsolete">現在のオブジェクトを保持</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjConflictResolutionDialog.cpp:64</name>
    <message>
        <location filename="" line="0"/>
        <source>Replace with this object</source>
        <translation type="obsolete">このオブジェクトと置換</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectEditor.cpp:437</name>
</context>
<context>
    <name>src/gui/ObjectEditor.cpp:438</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue editing</source>
        <translation type="obsolete">編集を続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectEditor.cpp:439</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Discard changes</source>
        <translation type="obsolete">変更のやり直し(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectEditor.cpp:465</name>
    <message>
        <location filename="" line="0"/>
        <source>This object has been modified but not saved.
Do you want to save it ?</source>
        <translation type="obsolete">いくつかのオブジェクトの修正が保存されていません。
変更を保存しますか?</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectEditor.cpp:466</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Save</source>
        <translation type="obsolete">保存(&amp;S)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Discard</source>
        <translation type="obsolete">破棄する(&amp;D)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue editing</source>
        <translation type="obsolete">編集を続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1002</name>
    <message>
        <location filename="" line="0"/>
        <source>New TagService</source>
        <translation type="obsolete">新規 TCP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1006</name>
    <message>
        <location filename="" line="0"/>
        <source>New Group</source>
        <translation type="obsolete">新規グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1010</name>
    <message>
        <location filename="" line="0"/>
        <source>New Time Interval</source>
        <translation type="obsolete">新規時間間隔</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1014</name>
    <message>
        <location filename="" line="0"/>
        <source>Find</source>
        <translation type="obsolete">検索</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1015</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1027</name>
    <message>
        <location filename="" line="0"/>
        <source>Group</source>
        <translation type="obsolete">グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1035</name>
    <message>
        <location filename="" line="0"/>
        <source>Compile</source>
        <translation type="obsolete">コンパイル</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1036</name>
    <message>
        <location filename="" line="0"/>
        <source>Install</source>
        <translation type="obsolete">インストール</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1043</name>
    <message>
        <location filename="" line="0"/>
        <source>Lock</source>
        <translation type="obsolete">ロックする</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1045</name>
    <message>
        <location filename="" line="0"/>
        <source>Unlock</source>
        <translation type="obsolete">ロック解除する</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1054</name>
    <message>
        <location filename="" line="0"/>
        <source>dump</source>
        <translation type="obsolete">ダンプ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1087</name>
    <message>
        <location filename="" line="0"/>
        <source>Undelete...</source>
        <translation type="obsolete">削除の復活...</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:145</name>
    <message>
        <location filename="" line="0"/>
        <source>Object Manipulator</source>
        <translation type="obsolete">オブジェクトマニピュレータ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1576</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:161</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Library</source>
        <translation type="obsolete">新規ライブラリー(&amp;L)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:164</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Firewall</source>
        <translation type="obsolete">新規ファイアウォール(&amp;F)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:165</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Host</source>
        <translation type="obsolete">新規ホスト(&amp;H)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:166</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Interface</source>
        <translation type="obsolete">新規インターフェース(&amp;I)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:168</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Network</source>
        <translation type="obsolete">新規ネットワーク(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:169</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Address</source>
        <translation type="obsolete">新規アドレス(&amp;A)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:170</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;DNS Name</source>
        <translation type="obsolete">新規 DNS 名(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>New A&amp;ddress Table</source>
        <translation type="obsolete">新規アドレステーブル(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:172</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address &amp;Range</source>
        <translation type="obsolete">新規アドレスレンジ(&amp;R)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:173</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Object Group</source>
        <translation type="obsolete">新規オブジェクトグループ(&amp;O)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1743</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:175</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Custom Service</source>
        <translation type="obsolete">新規カスタムサービス(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1752</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Yes</source>
        <translation type="obsolete">はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;No</source>
        <translation type="obsolete">いいえ(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:176</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;IP Service</source>
        <translation type="obsolete">新規 IP サービス(&amp;I)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>New IC&amp;MP Service</source>
        <translation type="obsolete">新規 ICMP サービス(&amp;M)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:178</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;TCP Service</source>
        <translation type="obsolete">新規 TCP サービス(&amp;T)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:179</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;UDP Service</source>
        <translation type="obsolete">新規 UDP サービス(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;TagService</source>
        <translation type="obsolete">新規タグサービス(&amp;T)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:181</name>
    <message>
        <location filename="" line="0"/>
        <source>New &amp;Service Group</source>
        <translation type="obsolete">新規サービスグループ(&amp;S)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1823</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:1828</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Yes</source>
        <translation type="obsolete">はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;No</source>
        <translation type="obsolete">いいえ(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:183</name>
    <message>
        <location filename="" line="0"/>
        <source>New Ti&amp;me Interval</source>
        <translation type="obsolete">新規時間間隔(&amp;M)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:230</name>
    <message>
        <location filename="" line="0"/>
        <source> ( read only )</source>
        <translation type="obsolete">( 読み込み専用 )</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2343</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2470</name>
    <message>
        <location filename="" line="0"/>
        <source>New Interface</source>
        <translation type="obsolete">新規インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2473</name>
    <message>
        <location filename="" line="0"/>
        <source>New Interface</source>
        <translation type="obsolete">新規インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2491</name>
    <message>
        <location filename="" line="0"/>
        <source>New Network</source>
        <translation type="obsolete">新規ネットワーク</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2515</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address</source>
        <translation type="obsolete">新規アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2531</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address</source>
        <translation type="obsolete">新規アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2546</name>
    <message>
        <location filename="" line="0"/>
        <source>New DNS Name</source>
        <translation type="obsolete">新規 DNS 名</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2557</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address Table</source>
        <translation type="obsolete">新規アドレステーブル</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2591</name>
    <message>
        <location filename="" line="0"/>
        <source>New TagService</source>
        <translation type="obsolete">新規 TCP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2624</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address Range</source>
        <translation type="obsolete">新規アドレスレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2635</name>
    <message>
        <location filename="" line="0"/>
        <source>New Object Group</source>
        <translation type="obsolete">新規オブジェクトグループ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2647</name>
    <message>
        <location filename="" line="0"/>
        <source>New Custom Service</source>
        <translation type="obsolete">新規カスタムサービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2658</name>
    <message>
        <location filename="" line="0"/>
        <source>New IP Service</source>
        <translation type="obsolete">新規 IP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2669</name>
    <message>
        <location filename="" line="0"/>
        <source>New ICMP Service</source>
        <translation type="obsolete">新規 ICMP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2680</name>
    <message>
        <location filename="" line="0"/>
        <source>New TCP Service</source>
        <translation type="obsolete">新規 TCP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2691</name>
    <message>
        <location filename="" line="0"/>
        <source>New UDP Service</source>
        <translation type="obsolete">新規 UDP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2702</name>
    <message>
        <location filename="" line="0"/>
        <source>New Service Group</source>
        <translation type="obsolete">新規サービスグループ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2714</name>
    <message>
        <location filename="" line="0"/>
        <source>New Time Interval</source>
        <translation type="obsolete">新規時間間隔</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:2821</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:498</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:510</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Yes</source>
        <translation type="obsolete">はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;No</source>
        <translation type="obsolete">いいえ(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:527</name>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:539</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Yes</source>
        <translation type="obsolete">はい(&amp;Y)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;No</source>
        <translation type="obsolete">いいえ(&amp;N)</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:840</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:874</name>
    <message>
        <location filename="" line="0"/>
        <source>place in library %1</source>
        <translation type="obsolete">ライブラリー %1 の中の場所</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:883</name>
    <message>
        <location filename="" line="0"/>
        <source>to library %1</source>
        <translation type="obsolete">ライブラリー %1 へ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:893</name>
    <message>
        <location filename="" line="0"/>
        <source>place here</source>
        <translation type="obsolete">ここの場所</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:896</name>
    <message>
        <location filename="" line="0"/>
        <source>Duplicate ...</source>
        <translation type="obsolete">複製 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:901</name>
    <message>
        <location filename="" line="0"/>
        <source>Move ...</source>
        <translation type="obsolete">移動 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:904</name>
    <message>
        <location filename="" line="0"/>
        <source>Move ...</source>
        <translation type="obsolete">移動 ...</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:912</name>
    <message>
        <location filename="" line="0"/>
        <source>Copy</source>
        <translation type="obsolete">コピー</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:914</name>
    <message>
        <location filename="" line="0"/>
        <source>Cut</source>
        <translation type="obsolete">切り取り</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:916</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste</source>
        <translation type="obsolete">貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:921</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:933</name>
    <message>
        <location filename="" line="0"/>
        <source>Add Interface</source>
        <translation type="obsolete">新規インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:938</name>
    <message>
        <location filename="" line="0"/>
        <source>Add IP Address</source>
        <translation type="obsolete">新規 IP アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:940</name>
    <message>
        <location filename="" line="0"/>
        <source>Add MAC Address</source>
        <translation type="obsolete">新規 MAC アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:945</name>
    <message>
        <location filename="" line="0"/>
        <source>New Firewall</source>
        <translation type="obsolete">新規ファイアウォール</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:950</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address</source>
        <translation type="obsolete">新規アドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:955</name>
    <message>
        <location filename="" line="0"/>
        <source>New DNS Name</source>
        <translation type="obsolete">新規 DNS 名</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:961</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address Table</source>
        <translation type="obsolete">新規アドレステーブル</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:966</name>
    <message>
        <location filename="" line="0"/>
        <source>New Address Range</source>
        <translation type="obsolete">新規アドレスレンジ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:970</name>
    <message>
        <location filename="" line="0"/>
        <source>New Host</source>
        <translation type="obsolete">新規ホスト</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:974</name>
    <message>
        <location filename="" line="0"/>
        <source>New Network</source>
        <translation type="obsolete">新規ネットワーク</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:978</name>
    <message>
        <location filename="" line="0"/>
        <source>New Group</source>
        <translation type="obsolete">新規グループ</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:982</name>
    <message>
        <location filename="" line="0"/>
        <source>New Custom Service</source>
        <translation type="obsolete">新規カスタムサービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:986</name>
    <message>
        <location filename="" line="0"/>
        <source>New IP Service</source>
        <translation type="obsolete">新規 IP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:990</name>
    <message>
        <location filename="" line="0"/>
        <source>New ICMP Service</source>
        <translation type="obsolete">新規 ICMP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:994</name>
    <message>
        <location filename="" line="0"/>
        <source>New TCP Service</source>
        <translation type="obsolete">新規 TCP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectManipulator.cpp:998</name>
    <message>
        <location filename="" line="0"/>
        <source>New UDP Service</source>
        <translation type="obsolete">新規 UDP サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/ObjectTreeView.cpp:115</name>
    <message>
        <location filename="" line="0"/>
        <source>Object</source>
        <translation type="obsolete">オブジェクト</translation>
    </message>
</context>
<context>
    <name>src/gui/PrefsDialog.cpp:178</name>
    <message>
        <location filename="" line="0"/>
        <source>Pick the color</source>
        <translation type="obsolete">色を摘出</translation>
    </message>
</context>
<context>
    <name>src/gui/PrefsDialog.cpp:226</name>
    <message>
        <location filename="" line="0"/>
        <source>Find working directory</source>
        <translation type="obsolete">作業ディレクトリの検索</translation>
    </message>
</context>
<context>
    <name>src/gui/PrefsDialog.cpp:235</name>
    <message>
        <location filename="" line="0"/>
        <source>Find Secure Shell utility</source>
        <translation type="obsolete">セキュアシェルユーティリティの検索</translation>
    </message>
</context>
<context>
    <name>src/gui/PrefsDialog.cpp:286</name>
    <message>
        <location filename="" line="0"/>
        <source>Find add-on library</source>
        <translation type="obsolete">アドオンライブラリーの検索</translation>
    </message>
</context>
<context>
    <name>src/gui/PrintingProgressDialog.cpp:48</name>
    <message>
        <location filename="" line="0"/>
        <source>Printing (page %1/%2)</source>
        <translation type="obsolete">印刷中 (ページ %1/%2)</translation>
    </message>
</context>
<context>
    <name>src/gui/PrintingProgressDialog.cpp:50</name>
    <message>
        <location filename="" line="0"/>
        <source>Printing page %1</source>
        <translation type="obsolete">印刷中 (ページ %1)</translation>
    </message>
</context>
<context>
    <name>src/gui/PrintingProgressDialog.cpp:67</name>
    <message>
        <location filename="" line="0"/>
        <source>Aborting print operation</source>
        <translation type="obsolete">印刷操作を中断しています</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:1038</name>
    <message>
        <location filename="" line="0"/>
        <source>Fatal error running rcsdiff for file %1</source>
        <translation type="obsolete">致命的なエラーです。ファイル %1 のrcsdiff を実行中です。</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:502</name>
    <message>
        <location filename="" line="0"/>
        <source>Error checking file out: %1</source>
        <translation type="obsolete">ファイル取り出しの検査中エラー: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:503</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:565</name>
</context>
<context>
    <name>src/gui/RCS.cpp:694</name>
    <message>
        <location filename="" line="0"/>
        <source>Error creating temporary file </source>
        <translation type="obsolete">一時ファイルの作成に失敗しました </translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:695</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:707</name>
    <message>
        <location filename="" line="0"/>
        <source>Error writing to temporary file </source>
        <translation type="obsolete">維持値ファイルの書き込みに失敗しました </translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:708</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:724</name>
    <message>
        <location filename="" line="0"/>
        <source>Error checking file out: %1</source>
        <translation type="obsolete">ファイル取り出しの検査中エラー: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:725</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:739</name>
    <message>
        <location filename="" line="0"/>
        <source>File is opened and locked by %1.
You can only open it read-only.</source>
        <translation type="obsolete">ファイルはオープン済みで %1 にロックされています。それは読み込み専用でのみ開くことが出来ます。</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:752</name>
</context>
<context>
    <name>src/gui/RCS.cpp:755</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Cancel</source>
        <translation type="obsolete">取り消し(&amp;C)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>Open &amp;read-only</source>
        <translation type="obsolete">読み込み専用で開く(&amp;O)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Open and continue editing</source>
        <translation type="obsolete">読み込んで編集を続ける(&amp;O)</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:807</name>
    <message>
        <location filename="" line="0"/>
        <source>Error checking file out: %1</source>
        <translation type="obsolete">ファイル取り出しの検査中エラー: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:808</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/RCS.cpp:998</name>
    <message>
        <location filename="" line="0"/>
        <source>Fatal error running rlog for %1</source>
        <translation type="obsolete">致命的なエラーで、%1 の rlog を実行中です。</translation>
    </message>
</context>
<context>
    <name>src/gui/RCSFilePreview.cpp:137</name>
    <message>
        <location filename="" line="0"/>
        <source>File is not in RCS</source>
        <translation type="obsolete">ファイルが RCS にありません。</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1457</name>
    <message>
        <location filename="" line="0"/>
        <source>Insert Rule</source>
        <translation type="obsolete">ルール挿入</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1459</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule</source>
        <translation type="obsolete">ルール貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1473</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule</source>
        <translation type="obsolete">ルール貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1603</name>
    <message>
        <location filename="" line="0"/>
        <source>Parameters</source>
        <translation type="obsolete">フラグメント</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1620</name>
    <message>
        <location filename="" line="0"/>
        <source>Inbound</source>
        <translation type="obsolete">内向き</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1624</name>
    <message>
        <location filename="" line="0"/>
        <source>Outbound</source>
        <translation type="obsolete">外向き</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1628</name>
    <message>
        <location filename="" line="0"/>
        <source>Both</source>
        <translation type="obsolete">両方</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1637</name>
    <message>
        <location filename="" line="0"/>
        <source>Rule Options</source>
        <translation type="obsolete">ルールオプション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1644</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging On</source>
        <translation type="obsolete">記録する</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1648</name>
    <message>
        <location filename="" line="0"/>
        <source>Logging Off</source>
        <translation type="obsolete">記録しない</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1660</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1663</name>
    <message>
        <location filename="" line="0"/>
        <source>Copy</source>
        <translation type="obsolete">コピー</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1665</name>
    <message>
        <location filename="" line="0"/>
        <source>Cut</source>
        <translation type="obsolete">切り取り</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1666</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste</source>
        <translation type="obsolete">貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1669</name>
    <message>
        <location filename="" line="0"/>
        <source>Delete</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1672</name>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1674</name>
    <message>
        <location filename="" line="0"/>
        <source>Reveal in tree</source>
        <translation type="obsolete">ツリーから検索</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1677</name>
    <message>
        <location filename="" line="0"/>
        <source>Negate</source>
        <translation type="obsolete">否定</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1717</name>
    <message>
        <location filename="" line="0"/>
        <source>Insert Rule</source>
        <translation type="obsolete">ルール挿入</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1725</name>
    <message>
        <location filename="" line="0"/>
        <source>Rules: %1-%2</source>
        <translation type="obsolete">ルール: %1-%2</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1728</name>
    <message>
        <location filename="" line="0"/>
        <source>Rule: %1</source>
        <translation type="obsolete">ルール: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1733</name>
    <message>
        <location filename="" line="0"/>
        <source>Color Label:</source>
        <translation type="obsolete">色ラベル:</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1745</name>
    <message>
        <location filename="" line="0"/>
        <source>Insert Rule</source>
        <translation type="obsolete">ルール挿入</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1747</name>
    <message>
        <location filename="" line="0"/>
        <source>Add Rule Below</source>
        <translation type="obsolete">下にルール追加</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1750</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove Rule</source>
        <translation type="obsolete">ルール削除</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1751</name>
    <message>
        <location filename="" line="0"/>
        <source>Remove Rules</source>
        <translation type="obsolete">複数ルール削除</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1754</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Rule</source>
        <translation type="obsolete">ルール移動</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1755</name>
    <message>
        <location filename="" line="0"/>
        <source>Move Rules</source>
        <translation type="obsolete">複数ルール移動</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1761</name>
    <message>
        <location filename="" line="0"/>
        <source>Copy Rule</source>
        <translation type="obsolete">ルールコピー</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1763</name>
    <message>
        <location filename="" line="0"/>
        <source>Cut Rule</source>
        <translation type="obsolete">ルール切り取り</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1765</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule Above</source>
        <translation type="obsolete">上にルールを貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1767</name>
    <message>
        <location filename="" line="0"/>
        <source>Paste Rule Below</source>
        <translation type="obsolete">下にルールを貼り付け</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1774</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable Rule</source>
        <translation type="obsolete">ルール有効</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1775</name>
    <message>
        <location filename="" line="0"/>
        <source>Enable Rules</source>
        <translation type="obsolete">ルール群有効</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1779</name>
    <message>
        <location filename="" line="0"/>
        <source>Disable Rule</source>
        <translation type="obsolete">ルール無効</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1780</name>
    <message>
        <location filename="" line="0"/>
        <source>Disable Rules</source>
        <translation type="obsolete">ルール群無効</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1789</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1793</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:1797</name>
    <message>
        <location filename="" line="0"/>
        <source>Edit</source>
        <translation type="obsolete">編集</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:206</name>
    <message>
        <location filename="" line="0"/>
        <source>A Rule Set</source>
        <translation type="obsolete">ルールセット</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3306</name>
    <message>
        <location filename="" line="0"/>
        <source>Source</source>
        <translation type="obsolete">ソース</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3309</name>
    <message>
        <location filename="" line="0"/>
        <source>Destination</source>
        <translation type="obsolete">ディスティネーション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3312</name>
    <message>
        <location filename="" line="0"/>
        <source>Service</source>
        <translation type="obsolete">サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3315</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3318</name>
    <message>
        <location filename="" line="0"/>
        <source>Direction</source>
        <translation type="obsolete">方向</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3321</name>
    <message>
        <location filename="" line="0"/>
        <source>Action</source>
        <translation type="obsolete">操作</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3326</name>
    <message>
        <location filename="" line="0"/>
        <source>Time</source>
        <translation type="obsolete">時間</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3332</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3336</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment</source>
        <translation type="obsolete">コメント</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3396</name>
    <message>
        <location filename="" line="0"/>
        <source>Source</source>
        <translation type="obsolete">ソース</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3399</name>
    <message>
        <location filename="" line="0"/>
        <source>Destination</source>
        <translation type="obsolete">ディスティネーション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3402</name>
    <message>
        <location filename="" line="0"/>
        <source>Service</source>
        <translation type="obsolete">サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3405</name>
    <message>
        <location filename="" line="0"/>
        <source>Direction</source>
        <translation type="obsolete">方向</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3408</name>
    <message>
        <location filename="" line="0"/>
        <source>Action</source>
        <translation type="obsolete">操作</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3413</name>
    <message>
        <location filename="" line="0"/>
        <source>Time</source>
        <translation type="obsolete">時間</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3419</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3423</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment</source>
        <translation type="obsolete">コメント</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3481</name>
    <message>
        <location filename="" line="0"/>
        <source>Original Src</source>
        <translation type="obsolete">オリジナルソース</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3484</name>
    <message>
        <location filename="" line="0"/>
        <source>Original Dst</source>
        <translation type="obsolete">オリジナルディスティネーション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3487</name>
    <message>
        <location filename="" line="0"/>
        <source>Original Srv</source>
        <translation type="obsolete">オリジナルサービス</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3490</name>
    <message>
        <location filename="" line="0"/>
        <source>Translated Src</source>
        <translation type="obsolete">転送元ソース</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3493</name>
    <message>
        <location filename="" line="0"/>
        <source>Translated Dst</source>
        <translation type="obsolete">転送先</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3496</name>
    <message>
        <location filename="" line="0"/>
        <source>Translated Srv</source>
        <translation type="obsolete">転送サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3499</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3502</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment</source>
        <translation type="obsolete">コメント</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3559</name>
    <message>
        <location filename="" line="0"/>
        <source>Destination</source>
        <translation type="obsolete">ディスティネーション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3562</name>
    <message>
        <location filename="" line="0"/>
        <source>Gateway</source>
        <translation type="obsolete">ゲートウェイ</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3565</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface</source>
        <translation type="obsolete">インターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3568</name>
    <message>
        <location filename="" line="0"/>
        <source>Metric</source>
        <translation type="obsolete">サービス</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3571</name>
    <message>
        <location filename="" line="0"/>
        <source>Options</source>
        <translation type="obsolete">オプション</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:3574</name>
    <message>
        <location filename="" line="0"/>
        <source>Comment</source>
        <translation type="obsolete">コメント</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:621</name>
    <message>
        <location filename="" line="0"/>
        <source>Outbound </source>
        <translation type="obsolete">外向き</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:707</name>
    <message>
        <location filename="" line="0"/>
        <source>Original</source>
        <translation type="obsolete">オリジナル</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:708</name>
    <message>
        <location filename="" line="0"/>
        <source>Default</source>
        <translation type="obsolete">削除</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:711</name>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:712</name>
    <message>
        <location filename="" line="0"/>
        <source>Any</source>
        <translation type="obsolete">すべて</translation>
    </message>
</context>
<context>
    <name>src/gui/RuleSetView.cpp:720</name>
    <message>
        <location filename="" line="0"/>
        <source>Any</source>
        <translation type="obsolete">すべて</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:136</name>
    <message>
        <location filename="" line="0"/>
        <source>
*** Fatal error :</source>
        <translation type="obsolete">
*** 致命的なエラー :</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:170</name>
    <message>
        <location filename="" line="0"/>
        <source>Logged in</source>
        <translation type="obsolete">ログイン</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>Switching to enable mode...</source>
        <translation type="obsolete">有効モードに切り替え中...
</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:205</name>
    <message>
        <location filename="" line="0"/>
        <source>New RSA key</source>
        <translation type="obsolete">新規 RSA キー</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:206</name>
    <message>
        <location filename="" line="0"/>
        <source>Yes</source>
        <translation type="obsolete">はい</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>No</source>
        <translation type="obsolete">いいえ</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:252</name>
    <message>
        <location filename="" line="0"/>
        <source>In enable mode.</source>
        <translation type="obsolete">有効モード中です。</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:347</name>
    <message>
        <location filename="" line="0"/>
        <source>Can not open file %1</source>
        <translation type="obsolete">ファイル %1 を開くことが出来ません。</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:387</name>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:424</name>
    <message>
        <location filename="" line="0"/>
        <source>Rule %1</source>
        <translation type="obsolete">ルール %1</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:450</name>
    <message>
        <location filename="" line="0"/>
        <source>End</source>
        <translation type="obsolete">終了:</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:532</name>
    <message>
        <location filename="" line="0"/>
        <source>Making backup copy of the firewall configuration</source>
        <translation type="obsolete">ファイアウォール設定のバックアップコピー作成中</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:596</name>
    <message>
        <location filename="" line="0"/>
        <source>*** Clearing unused access lists</source>
        <translation type="obsolete">*** 未使用アクセス一覧の消去中</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:661</name>
    <message>
        <location filename="" line="0"/>
        <source>*** Clearing unused object groups</source>
        <translation type="obsolete">*** 未使用オブジェクトグループの消去中</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:681</name>
    <message>
        <location filename="" line="0"/>
        <source>*** End </source>
        <translation type="obsolete">*** 終了</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:692</name>
    <message>
        <location filename="" line="0"/>
        <source>Reading current firewall configuration</source>
        <translation type="obsolete">現在のファイアウォール設定の読み込み中</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:717</name>
    <message>
        <location filename="" line="0"/>
        <source>Generating configuration diff</source>
        <translation type="obsolete">設定の差分を生成中</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:732</name>
    <message>
        <location filename="" line="0"/>
        <source>Fork failed for %1</source>
        <translation type="obsolete">%1 のフォークに失敗しました</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:738</name>
    <message>
        <location filename="" line="0"/>
        <source>Not enough memory.</source>
        <translation type="obsolete">メモリがありません。</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:743</name>
    <message>
        <location filename="" line="0"/>
        <source>Too many opened file descriptors in the system.</source>
        <translation type="obsolete">システムでファイルディスクリプタを開きすぎています。</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:769</name>
    <message>
        <location filename="" line="0"/>
        <source>Empty configuration diff</source>
        <translation type="obsolete">設定の差分がありません</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHPIX.cpp:783</name>
</context>
<context>
    <name>src/gui/SSHSession.cpp:180</name>
    <message>
        <location filename="" line="0"/>
        <source>Failed to start ssh</source>
        <translation type="obsolete">ssh の開始に失敗しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHSession.cpp:500</name>
    <message>
        <location filename="" line="0"/>
        <source>ERROR</source>
        <translation type="obsolete">エラー</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>OK</source>
        <translation type="obsolete">OK</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHSession.cpp:502</name>
    <message>
        <location filename="" line="0"/>
        <source>SSH session terminated, exit status: %1</source>
        <translation type="obsolete">SSH 接続が切断しました。終了ステータス: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHSession.cpp:90</name>
</context>
<context>
    <name>src/gui/SSHUnx.cpp:152</name>
    <message>
        <location filename="" line="0"/>
        <source>Logged in</source>
        <translation type="obsolete">ログイン</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHUnx.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>New RSA key</source>
        <translation type="obsolete">新規 RSA キー</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHUnx.cpp:178</name>
    <message>
        <location filename="" line="0"/>
        <source>Yes</source>
        <translation type="obsolete">はい</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>No</source>
        <translation type="obsolete">いいえ</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHUnx.cpp:237</name>
    <message>
        <location filename="" line="0"/>
        <source>Done</source>
        <translation type="obsolete">終了しました</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHUnx.cpp:249</name>
    <message>
        <location filename="" line="0"/>
        <source>Error in SSH</source>
        <translation type="obsolete">SSH のエラー</translation>
    </message>
</context>
<context>
    <name>src/gui/SSHUnx.cpp:96</name>
    <message>
        <location filename="" line="0"/>
        <source>
*** Fatal error :</source>
        <translation type="obsolete">
*** 致命的なエラー :</translation>
    </message>
</context>
<context>
    <name>src/gui/SimpleTextEditor.cpp:66</name>
</context>
<context>
    <name>src/gui/SimpleTextEditor.cpp:71</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose file</source>
        <translation type="obsolete">インポートするファイルを選んでください。</translation>
    </message>
</context>
<context>
    <name>src/gui/SimpleTextEditor.cpp:79</name>
    <message>
        <location filename="" line="0"/>
        <source>Could not open file %1</source>
        <translation type="obsolete">ファイル %1 を開くことが出来ません。</translation>
    </message>
</context>
<context>
    <name>src/gui/StartWizard.cpp:106</name>
    <message>
        <location filename="" line="0"/>
        <source>File %1 is read-only, you can not save changes to it.</source>
        <translation type="obsolete">ファイル %1 は読み込み専用です。変更を保存することが出来ませんでした。</translation>
    </message>
</context>
<context>
    <name>src/gui/StartWizard.cpp:157</name>
    <message>
        <location filename="" line="0"/>
        <source>Error adding file to RCS:
%1</source>
        <translation type="obsolete">RCS へのファイルの追加エラー:
%1</translation>
    </message>
</context>
<context>
    <name>src/gui/StartWizard.cpp:171</name>
    <message>
        <location filename="" line="0"/>
        <source>Error opening file:
%1</source>
        <translation type="obsolete">ファイルを開くことが出来ません:
%1</translation>
    </message>
</context>
<context>
    <name>src/gui/StartWizard.cpp:99</name>
    <message>
        <location filename="" line="0"/>
        <source>Choose name and location for the new file</source>
        <translation type="obsolete">新規ファイルの名前とフォルダーを選択</translation>
    </message>
</context>
<context>
    <name>src/gui/TCPServiceDialog.cpp:176</name>
</context>
<context>
    <name>src/gui/TCPServiceDialog.cpp:177</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue editing</source>
        <translation type="obsolete">編集を続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/TCPServiceDialog.cpp:184</name>
</context>
<context>
    <name>src/gui/TCPServiceDialog.cpp:185</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue editing</source>
        <translation type="obsolete">編集を続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:68</name>
    <message>
        <location filename="" line="0"/>
        <source>(M/D/Y)</source>
        <translation type="obsolete">日付 (月/日/年):</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:69</name>
    <message>
        <location filename="" line="0"/>
        <source>(M/D/Y)</source>
        <translation type="obsolete">日付 (月/日/年):</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:73</name>
    <message>
        <location filename="" line="0"/>
        <source>(D/M/Y)</source>
        <translation type="obsolete">日付 (日/月/年):</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:74</name>
    <message>
        <location filename="" line="0"/>
        <source>(D/M/Y)</source>
        <translation type="obsolete">日付 (日/月/年):</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:78</name>
    <message>
        <location filename="" line="0"/>
        <source>(Y/M/D)</source>
        <translation type="obsolete">日付 (年/日/月):</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:79</name>
    <message>
        <location filename="" line="0"/>
        <source>(Y/M/D)</source>
        <translation type="obsolete">日付 (年/日/月):</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:83</name>
    <message>
        <location filename="" line="0"/>
        <source>(Y/D/M)</source>
        <translation type="obsolete">日付 (年/日/月)</translation>
    </message>
</context>
<context>
    <name>src/gui/TimeDialog.cpp:84</name>
    <message>
        <location filename="" line="0"/>
        <source>(Y/D/M)</source>
        <translation type="obsolete">日付 (年/日/月)</translation>
    </message>
</context>
<context>
    <name>src/gui/UDPServiceDialog.cpp:118</name>
</context>
<context>
    <name>src/gui/UDPServiceDialog.cpp:119</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue editing</source>
        <translation type="obsolete">編集を続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/UDPServiceDialog.cpp:126</name>
</context>
<context>
    <name>src/gui/UDPServiceDialog.cpp:127</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue editing</source>
        <translation type="obsolete">編集を続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/aboutdialog_q.ui.h:14</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision: %1 ( Build: %2 )</source>
        <translation type="obsolete">リビジョン: %1 ( ビルド: %2 )</translation>
    </message>
</context>
<context>
    <name>src/gui/aboutdialog_q.ui.h:16</name>
    <message>
        <location filename="" line="0"/>
        <source>Using Firewall Builder API %1</source>
        <translation type="obsolete">試用中の libfwbuilder API バージョン</translation>
    </message>
</context>
<context>
    <name>src/gui/aboutdialog_q.ui.h:19</name>
</context>
<context>
    <name>src/gui/aboutdialog_q.ui.h:20</name>
</context>
<context>
    <name>src/gui/execDialog.cpp:101</name>
    <message>
        <location filename="" line="0"/>
        <source>Error: Failed to start program</source>
        <translation type="obsolete">エラー: プログラムの開始に失敗しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/filePropDialog.cpp:62</name>
    <message>
        <location filename="" line="0"/>
        <source>Opened read-only</source>
        <translation type="obsolete">読み込み専用で開いています</translation>
    </message>
</context>
<context>
    <name>src/gui/filePropDialog.cpp:80</name>
    <message>
        <location filename="" line="0"/>
        <source>Revision %1</source>
        <translation type="obsolete">リビジョン %1</translation>
    </message>
</context>
<context>
    <name>src/gui/findDialog.cpp:269</name>
    <message>
        <location filename="" line="0"/>
        <source>Search hit the end of the object tree.</source>
        <translation type="obsolete">検索のヒットがオブジェクトツリーの最後です。</translation>
    </message>
</context>
<context>
    <name>src/gui/findDialog.cpp:270</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue at top</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Stop</source>
        <translation type="obsolete">停止(&amp;S)</translation>
    </message>
</context>
<context>
    <name>src/gui/freebsdAdvancedDialog.cpp:62</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/freebsdAdvancedDialog.cpp:65</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/freebsdAdvancedDialog.cpp:68</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/instBatchOptionsDialog.cpp:47</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:1043</name>
    <message>
        <location filename="" line="0"/>
        <source>Can not open file %1</source>
        <translation type="obsolete">ファイル %1 を開くことが出来ません。</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1146</name>
    <message>
        <location filename="" line="0"/>
        <source>
Copying %1 -&gt; %2:%3
</source>
        <translation type="obsolete">
%1 から %2:%3 へコピーしています
</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1180</name>
    <message>
        <location filename="" line="0"/>
        <source>Running command &apos;%1&apos;
</source>
        <translation type="obsolete">コマンド &apos;%1&apos; を実行しています
</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1190</name>
    <message>
        <location filename="" line="0"/>
        <source>Success</source>
        <translation type="obsolete">成功</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1191</name>
    <message>
        <location filename="" line="0"/>
        <source>Success</source>
        <translation type="obsolete">成功</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1195</name>
    <message>
        <location filename="" line="0"/>
        <source>Error</source>
        <translation type="obsolete">エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1196</name>
    <message>
        <location filename="" line="0"/>
        <source>Error</source>
        <translation type="obsolete">エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1225</name>
    <message>
        <location filename="" line="0"/>
        <source>Done
</source>
        <translation type="obsolete">実行しました
</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1288</name>
    <message>
        <location filename="" line="0"/>
        <source>Activating new policy
</source>
        <translation type="obsolete">新規ポリシーの有効化中
</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:130</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:1435</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:145</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:1450</name>
    <message>
        <location filename="" line="0"/>
        <source>Error: Failed to start program</source>
        <translation type="obsolete">エラー: プログラムの開始に失敗しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1473</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:1476</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1497</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall isn&apos;t compiled.</source>
        <translation type="obsolete">ファイアウォールを構築しませんでした。</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:159</name>
    <message>
        <location filename="" line="0"/>
        <source>Unknown operation.</source>
        <translation type="obsolete">不明な操作です。</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1592</name>
    <message>
        <location filename="" line="0"/>
        <source>Firewall platform is not specified in this object.
Can&apos;t compile firewall policy.</source>
        <translation type="obsolete">ファイアウォールプラットフォームがこのオブジェクトで指定されていません。
ファイアウォールポリシーをコンパイルできません。</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1594</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1793</name>
    <message>
        <location filename="" line="0"/>
        <source>Error: Terminating install sequence
</source>
        <translation type="obsolete">エラー: インストールシーケンス終了
</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1829</name>
    <message>
        <location filename="" line="0"/>
        <source>Success</source>
        <translation type="obsolete">成功</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1834</name>
    <message>
        <location filename="" line="0"/>
        <source>Error</source>
        <translation type="obsolete">エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1858</name>
    <message>
        <location filename="" line="0"/>
        <source>Success</source>
        <translation type="obsolete">成功</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1859</name>
    <message>
        <location filename="" line="0"/>
        <source>Success</source>
        <translation type="obsolete">成功</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1863</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:1869</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:1870</name>
    <message>
        <location filename="" line="0"/>
        <source>Error</source>
        <translation type="obsolete">エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1871</name>
    <message>
        <location filename="" line="0"/>
        <source>Error</source>
        <translation type="obsolete">エラー</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1899</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiling ...</source>
        <translation type="obsolete">コンパイル中...</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:190</name>
    <message>
        <location filename="" line="0"/>
        <source>Show details</source>
        <translation type="obsolete">詳細を表示する</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1904</name>
    <message>
        <location filename="" line="0"/>
        <source>Failure</source>
        <translation type="obsolete">失敗</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1913</name>
    <message>
        <location filename="" line="0"/>
        <source>Recompile</source>
        <translation type="obsolete">再コンパイル</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:196</name>
    <message>
        <location filename="" line="0"/>
        <source>Hide details</source>
        <translation type="obsolete">詳細を隠す</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:1999</name>
    <message>
        <location filename="" line="0"/>
        <source>Batch policy rules compilation</source>
        <translation type="obsolete">ポリシールール群から検索</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2028</name>
    <message>
        <location filename="" line="0"/>
        <source>Stop</source>
        <translation type="obsolete">停止</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2052</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiling ...</source>
        <translation type="obsolete">コンパイル中...</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2057</name>
    <message>
        <location filename="" line="0"/>
        <source>Failure</source>
        <translation type="obsolete">失敗</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2122</name>
    <message>
        <location filename="" line="0"/>
        <source>Error: Failed to start program</source>
        <translation type="obsolete">エラー: プログラムの開始に失敗しました。</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2171</name>
    <message>
        <location filename="" line="0"/>
        <source>Stop</source>
        <translation type="obsolete">停止</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2182</name>
    <message>
        <location filename="" line="0"/>
        <source>Install firewall: </source>
        <translation type="obsolete">ファイアウォールインストール:</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2192</name>
    <message>
        <location filename="" line="0"/>
        <source>Installing firewalls</source>
        <translation type="obsolete">ファイアウォールのインストール</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2223</name>
    <message>
        <location filename="" line="0"/>
        <source>Installing ...</source>
        <translation type="obsolete">インストール中...</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2265</name>
    <message>
        <location filename="" line="0"/>
        <source>Failure</source>
        <translation type="obsolete">失敗</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2277</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:2284</name>
    <message>
        <location filename="" line="0"/>
        <source>Failure</source>
        <translation type="obsolete">失敗</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2298</name>
    <message>
        <location filename="" line="0"/>
        <source>Failure</source>
        <translation type="obsolete">失敗</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2310</name>
    <message>
        <location filename="" line="0"/>
        <source>Failure</source>
        <translation type="obsolete">失敗</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2407</name>
    <message>
        <location filename="" line="0"/>
        <source>Show selected</source>
        <translation type="obsolete">選択の表示</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:2413</name>
    <message>
        <location filename="" line="0"/>
        <source>Show all</source>
        <translation type="obsolete">すべて表示する</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:300</name>
    <message>
        <location filename="" line="0"/>
        <source>Unsupported exception</source>
        <translation type="obsolete">未サポートの例外</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:340</name>
    <message>
        <location filename="" line="0"/>
        <source>&lt;b&gt;Summary:&lt;/b&gt;</source>
        <translation type="obsolete">&lt;b&gt;要約:&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:343</name>
    <message>
        <location filename="" line="0"/>
        <source>* firewall name : %1</source>
        <translation type="obsolete">* ファイアウォール名: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:345</name>
    <message>
        <location filename="" line="0"/>
        <source>* user name : %1</source>
        <translation type="obsolete">* ユーザー名: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:347</name>
    <message>
        <location filename="" line="0"/>
        <source>* management address : %1</source>
        <translation type="obsolete">* 管理アドレス: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:349</name>
    <message>
        <location filename="" line="0"/>
        <source>* platform : %1</source>
        <translation type="obsolete">* プラットフォーム: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:351</name>
    <message>
        <location filename="" line="0"/>
        <source>* host OS : %1</source>
        <translation type="obsolete">* ホスト OS: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:353</name>
    <message>
        <location filename="" line="0"/>
        <source>* Loading configuration from file %1</source>
        <translation type="obsolete">* ファイル %1 から設定を読み込んでいます</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:358</name>
    <message>
        <location filename="" line="0"/>
        <source>* Incremental install</source>
        <translation type="obsolete">* インクリメンタルインストール</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:363</name>
    <message>
        <location filename="" line="0"/>
        <source>* Configuration diff will be saved in file %1</source>
        <translation type="obsolete">* 設定の差分をファイル %1 に保存するでしょう</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:368</name>
    <message>
        <location filename="" line="0"/>
        <source>* Commands will not be executed on the firewall</source>
        <translation type="obsolete">* コマンドはファイアウォール上で実行されないでしょう</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:373</name>
    <message>
        <location filename="" line="0"/>
        <source>* firewall name : %1</source>
        <translation type="obsolete">* ファイアウォール名: %1</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:670</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:677</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:684</name>
</context>
<context>
    <name>src/gui/instDialog.cpp:732</name>
    <message>
        <location filename="" line="0"/>
        <source>File %1 not found.</source>
        <translation type="obsolete">ファイル %1 が見つかりません。</translation>
    </message>
</context>
<context>
    <name>src/gui/instDialog.cpp:733</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/instOptionsDialog.cpp:66</name>
    <message>
        <location filename="" line="0"/>
        <source>Install options for firewall &apos;%1&apos;</source>
        <translation type="obsolete">ファイアウォールポリシーのインストール</translation>
    </message>
</context>
<context>
    <name>src/gui/ipfAdvancedDialog.cpp:170</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/ipfAdvancedDialog.cpp:179</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/ipfwAdvancedDialog.cpp:144</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/ipfwAdvancedDialog.cpp:153</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/iptAdvancedDialog.cpp:204</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/iptAdvancedDialog.cpp:213</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/linksysAdvancedDialog.cpp:68</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/linksysAdvancedDialog.cpp:71</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/linksysAdvancedDialog.cpp:74</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/linux24AdvancedDialog.cpp:62</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/linux24AdvancedDialog.cpp:65</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/linux24AdvancedDialog.cpp:68</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/listOfLibraries.cpp:142</name>
</context>
<context>
    <name>src/gui/listOfLibraries.cpp:148</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/listOfLibraries.cpp:186</name>
    <message>
        <location filename="" line="0"/>
        <source>Error loading file %1:
%2</source>
        <translation type="obsolete">ファイル %1 の読み込みエラー:
%2</translation>
    </message>
</context>
<context>
    <name>src/gui/listOfLibraries.cpp:188</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/listOfLibraries.cpp:214</name>
    <message>
        <location filename="" line="0"/>
        <source>Duplicate library &apos;%1&apos;</source>
        <translation type="obsolete">ライブラリー %1 の複製</translation>
    </message>
</context>
<context>
    <name>src/gui/listOfLibraries.cpp:215</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/macosxAdvancedDialog.cpp:62</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/macosxAdvancedDialog.cpp:65</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/macosxAdvancedDialog.cpp:68</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:100</name>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:229</name>
    <message>
        <location filename="" line="0"/>
        <source>Missing SNMP community string.</source>
        <translation type="obsolete">SNMP コミュニティ文字列が足りません。</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:248</name>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:422</name>
    <message>
        <location filename="" line="0"/>
        <source>dynamic</source>
        <translation type="obsolete">ダイナミック</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:503</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface: %1 (%2)</source>
        <translation type="obsolete">インターフェース: %1 (%2)</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:511</name>
    <message>
        <location filename="" line="0"/>
        <source>Dynamic address</source>
        <translation type="obsolete">ダイナミックアドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:513</name>
    <message>
        <location filename="" line="0"/>
        <source>Unnumbered interface</source>
        <translation type="obsolete">アンナンバードインターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:515</name>
    <message>
        <location filename="" line="0"/>
        <source>Bridge port</source>
        <translation type="obsolete">ブリッジポート</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:555</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal address &apos;%1/%2&apos;</source>
        <translation type="obsolete">不当なアドレス 「%1/%2」</translation>
    </message>
</context>
<context>
    <name>src/gui/newFirewallDialog.cpp:99</name>
</context>
<context>
    <name>src/gui/newHostDialog.cpp:222</name>
    <message>
        <location filename="" line="0"/>
        <source>Missing SNMP community string.</source>
        <translation type="obsolete">SNMP コミュニティ文字列が足りません。</translation>
    </message>
</context>
<context>
    <name>src/gui/newHostDialog.cpp:241</name>
</context>
<context>
    <name>src/gui/newHostDialog.cpp:433</name>
    <message>
        <location filename="" line="0"/>
        <source>Interface: %1 (%2)</source>
        <translation type="obsolete">インターフェース: %1 (%2)</translation>
    </message>
</context>
<context>
    <name>src/gui/newHostDialog.cpp:441</name>
    <message>
        <location filename="" line="0"/>
        <source>Dynamic address</source>
        <translation type="obsolete">ダイナミックアドレス</translation>
    </message>
</context>
<context>
    <name>src/gui/newHostDialog.cpp:443</name>
    <message>
        <location filename="" line="0"/>
        <source>Unnumbered interface</source>
        <translation type="obsolete">アンナンバードインターフェース</translation>
    </message>
</context>
<context>
    <name>src/gui/newHostDialog.cpp:480</name>
    <message>
        <location filename="" line="0"/>
        <source>Illegal address &apos;%1/%2&apos;</source>
        <translation type="obsolete">不当なアドレス 「%1/%2」</translation>
    </message>
</context>
<context>
    <name>src/gui/openbsdAdvancedDialog.cpp:62</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/openbsdAdvancedDialog.cpp:65</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/openbsdAdvancedDialog.cpp:68</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/pfAdvancedDialog.cpp:100</name>
    <message>
        <location filename="" line="0"/>
        <source>Conservative</source>
        <translation type="obsolete">コンサバティブ</translation>
    </message>
</context>
<context>
    <name>src/gui/pfAdvancedDialog.cpp:102</name>
</context>
<context>
    <name>src/gui/pfAdvancedDialog.cpp:104</name>
    <message>
        <location filename="" line="0"/>
        <source>Normal</source>
        <translation type="obsolete">通常</translation>
    </message>
</context>
<context>
    <name>src/gui/pfAdvancedDialog.cpp:278</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/pfAdvancedDialog.cpp:287</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/pfAdvancedDialog.cpp:98</name>
    <message>
        <location filename="" line="0"/>
        <source>Aggressive</source>
        <translation type="obsolete">アグレッシブ</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:130</name>
    <message>
        <location filename="" line="0"/>
        <source>0 - System Unusable</source>
        <translation type="obsolete">0 - システム未使用</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:135</name>
    <message>
        <location filename="" line="0"/>
        <source>1 - Take Immediate Action</source>
        <translation type="obsolete">1 - すぐさま処理をする</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:140</name>
    <message>
        <location filename="" line="0"/>
        <source>2 - Critical Condition</source>
        <translation type="obsolete">2 - 危険な状態</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:145</name>
    <message>
        <location filename="" line="0"/>
        <source>3 - Error Message</source>
        <translation type="obsolete">3 - エラーメッセージ</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:150</name>
    <message>
        <location filename="" line="0"/>
        <source>4 - Warning Message</source>
        <translation type="obsolete">4 - 警告メッセージ</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:155</name>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:160</name>
    <message>
        <location filename="" line="0"/>
        <source>6 - Informational</source>
        <translation type="obsolete">6 - 情報</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:165</name>
    <message>
        <location filename="" line="0"/>
        <source>7 - Debug Message</source>
        <translation type="obsolete">7 - デバグメッセージ</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:679</name>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:703</name>
    <message>
        <location filename="" line="0"/>
        <source>Compiler error</source>
        <translation type="obsolete">コンパイラーエラー</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:717</name>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:786</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/pixAdvancedDialog.cpp:795</name>
    <message>
        <location filename="" line="0"/>
        <source>Script Editor</source>
        <translation type="obsolete">スクリプトエディター</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:101</name>
    <message>
        <location filename="" line="0"/>
        <source>ftp</source>
        <translation type="obsolete">ftp</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:103</name>
    <message>
        <location filename="" line="0"/>
        <source>local0</source>
        <translation type="obsolete">local0</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:105</name>
    <message>
        <location filename="" line="0"/>
        <source>local1</source>
        <translation type="obsolete">local1</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:107</name>
    <message>
        <location filename="" line="0"/>
        <source>local2</source>
        <translation type="obsolete">local2</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:109</name>
    <message>
        <location filename="" line="0"/>
        <source>local3</source>
        <translation type="obsolete">local3</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:111</name>
    <message>
        <location filename="" line="0"/>
        <source>local4</source>
        <translation type="obsolete">local4</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:113</name>
    <message>
        <location filename="" line="0"/>
        <source>local5</source>
        <translation type="obsolete">local5</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:115</name>
    <message>
        <location filename="" line="0"/>
        <source>local6</source>
        <translation type="obsolete">local6</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:117</name>
    <message>
        <location filename="" line="0"/>
        <source>local7</source>
        <translation type="obsolete">local7</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:122</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP admin prohibited</source>
        <translation type="obsolete">ICMP 管理は禁止されました。</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:124</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP host prohibited</source>
        <translation type="obsolete">ICMP ホストは禁止されました。</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:126</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP host unreachable</source>
        <translation type="obsolete">ICMP ホストに到達しません。</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:128</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP net prohibited</source>
        <translation type="obsolete">ICMP ネットは禁止されました。</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:130</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP net unreachable</source>
        <translation type="obsolete">ICMP ネットに到達しません。</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:132</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP port unreachable</source>
        <translation type="obsolete">ICMP ポートに到達しません。</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:134</name>
    <message>
        <location filename="" line="0"/>
        <source>ICMP protocol unreachable</source>
        <translation type="obsolete">ICMP プロトコルに到達しません。</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:136</name>
    <message>
        <location filename="" line="0"/>
        <source>TCP RST</source>
        <translation type="obsolete">TCP RST</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:139</name>
    <message>
        <location filename="" line="0"/>
        <source>None</source>
        <translation type="obsolete">なし</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:141</name>
</context>
<context>
    <name>src/gui/platforms.cpp:143</name>
</context>
<context>
    <name>src/gui/platforms.cpp:145</name>
</context>
<context>
    <name>src/gui/platforms.cpp:148</name>
    <message>
        <location filename="" line="0"/>
        <source>None</source>
        <translation type="obsolete">なし</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:150</name>
    <message>
        <location filename="" line="0"/>
        <source>Random</source>
        <translation type="obsolete">ランダム</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:152</name>
    <message>
        <location filename="" line="0"/>
        <source>Source Hash</source>
        <translation type="obsolete">ソースハッシュ</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:154</name>
    <message>
        <location filename="" line="0"/>
        <source>Round Robin</source>
        <translation type="obsolete">ラウンドロビン</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:157</name>
</context>
<context>
    <name>src/gui/platforms.cpp:159</name>
    <message>
        <location filename="" line="0"/>
        <source>after interface configuration</source>
        <translation type="obsolete">インターフェース設定の後</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:161</name>
    <message>
        <location filename="" line="0"/>
        <source>after policy reset</source>
        <translation type="obsolete">ポリシーリセットの後</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:164</name>
</context>
<context>
    <name>src/gui/platforms.cpp:167</name>
</context>
<context>
    <name>src/gui/platforms.cpp:170</name>
</context>
<context>
    <name>src/gui/platforms.cpp:173</name>
</context>
<context>
    <name>src/gui/platforms.cpp:176</name>
</context>
<context>
    <name>src/gui/platforms.cpp:181</name>
    <message>
        <location filename="" line="0"/>
        <source>/day</source>
        <translation type="obsolete">/日</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:183</name>
    <message>
        <location filename="" line="0"/>
        <source>/hour</source>
        <translation type="obsolete">/時</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:185</name>
    <message>
        <location filename="" line="0"/>
        <source>/minute</source>
        <translation type="obsolete">/分</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:187</name>
    <message>
        <location filename="" line="0"/>
        <source>/second</source>
        <translation type="obsolete">/秒</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:392</name>
    <message>
        <location filename="" line="0"/>
        <source>- any -</source>
        <translation type="obsolete">- 任意 -</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:393</name>
    <message>
        <location filename="" line="0"/>
        <source>1.2.5 or earlier</source>
        <translation type="obsolete">1.2.5 以上</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:394</name>
    <message>
        <location filename="" line="0"/>
        <source>1.2.6 to 1.2.8</source>
        <translation type="obsolete">1.2.6 から 1.2.8</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:395</name>
    <message>
        <location filename="" line="0"/>
        <source>1.2.9 to 1.2.11</source>
        <translation type="obsolete">1.2.9 から 1.2.11</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:396</name>
    <message>
        <location filename="" line="0"/>
        <source>1.3.0 or later</source>
        <translation type="obsolete">1.3.0 以上</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:413</name>
    <message>
        <location filename="" line="0"/>
        <source>3.x</source>
        <translation type="obsolete">3.x</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:414</name>
    <message>
        <location filename="" line="0"/>
        <source>3.7 to 3.9</source>
        <translation type="obsolete">3.7 から 3.9</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:415</name>
    <message>
        <location filename="" line="0"/>
        <source>4.x</source>
        <translation type="obsolete">4.x</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:536</name>
    <message>
        <location filename="" line="0"/>
        <source>Accept</source>
        <translation type="obsolete">受理</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:538</name>
    <message>
        <location filename="" line="0"/>
        <source>Deny</source>
        <translation type="obsolete">破棄</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:540</name>
    <message>
        <location filename="" line="0"/>
        <source>Reject</source>
        <translation type="obsolete">拒否</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:542</name>
</context>
<context>
    <name>src/gui/platforms.cpp:544</name>
</context>
<context>
    <name>src/gui/platforms.cpp:546</name>
    <message>
        <location filename="" line="0"/>
        <source>Skip</source>
        <translation type="obsolete">スキップ</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:548</name>
    <message>
        <location filename="" line="0"/>
        <source>Continue</source>
        <translation type="obsolete">続ける</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:550</name>
    <message>
        <location filename="" line="0"/>
        <source>Modify</source>
        <translation type="obsolete">修正</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:552</name>
</context>
<context>
    <name>src/gui/platforms.cpp:554</name>
    <message>
        <location filename="" line="0"/>
        <source>Custom</source>
        <translation type="obsolete">カスタム</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:557</name>
    <message>
        <location filename="" line="0"/>
        <source>Branch</source>
        <translation type="obsolete">分岐</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:558</name>
    <message>
        <location filename="" line="0"/>
        <source>Chain</source>
        <translation type="obsolete">連結</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:559</name>
</context>
<context>
    <name>src/gui/platforms.cpp:563</name>
    <message>
        <location filename="" line="0"/>
        <source>Accounting</source>
        <translation type="obsolete">アカウント</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:564</name>
    <message>
        <location filename="" line="0"/>
        <source>Count</source>
        <translation type="obsolete">回数</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:568</name>
    <message>
        <location filename="" line="0"/>
        <source>Tag</source>
        <translation type="obsolete">タグ</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:569</name>
    <message>
        <location filename="" line="0"/>
        <source>Mark</source>
        <translation type="obsolete">マスク</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:573</name>
    <message>
        <location filename="" line="0"/>
        <source>Pipe</source>
        <translation type="obsolete">パイプ</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:574</name>
    <message>
        <location filename="" line="0"/>
        <source>Queue</source>
        <translation type="obsolete">キュー</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:577</name>
    <message>
        <location filename="" line="0"/>
        <source>Routing</source>
        <translation type="obsolete">ルーティング</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:61</name>
    <message>
        <location filename="" line="0"/>
        <source>alert</source>
        <translation type="obsolete">alert</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:63</name>
    <message>
        <location filename="" line="0"/>
        <source>crit</source>
        <translation type="obsolete">crit</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:65</name>
    <message>
        <location filename="" line="0"/>
        <source>error</source>
        <translation type="obsolete">error</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:67</name>
    <message>
        <location filename="" line="0"/>
        <source>warning</source>
        <translation type="obsolete">warning</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:69</name>
    <message>
        <location filename="" line="0"/>
        <source>notice</source>
        <translation type="obsolete">notice</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:71</name>
    <message>
        <location filename="" line="0"/>
        <source>info</source>
        <translation type="obsolete">info</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:73</name>
    <message>
        <location filename="" line="0"/>
        <source>debug</source>
        <translation type="obsolete">debug</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:79</name>
    <message>
        <location filename="" line="0"/>
        <source>kern</source>
        <translation type="obsolete">kern</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:81</name>
    <message>
        <location filename="" line="0"/>
        <source>user</source>
        <translation type="obsolete">user</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:83</name>
    <message>
        <location filename="" line="0"/>
        <source>mail</source>
        <translation type="obsolete">mail</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:85</name>
    <message>
        <location filename="" line="0"/>
        <source>daemon</source>
        <translation type="obsolete">daemon</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:87</name>
    <message>
        <location filename="" line="0"/>
        <source>auth</source>
        <translation type="obsolete">auth</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:89</name>
    <message>
        <location filename="" line="0"/>
        <source>syslog</source>
        <translation type="obsolete">syslog</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:91</name>
    <message>
        <location filename="" line="0"/>
        <source>lpr</source>
        <translation type="obsolete">lpr</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:93</name>
    <message>
        <location filename="" line="0"/>
        <source>news</source>
        <translation type="obsolete">news</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:95</name>
    <message>
        <location filename="" line="0"/>
        <source>uucp</source>
        <translation type="obsolete">uucp</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:97</name>
    <message>
        <location filename="" line="0"/>
        <source>cron</source>
        <translation type="obsolete">cron</translation>
    </message>
</context>
<context>
    <name>src/gui/platforms.cpp:99</name>
    <message>
        <location filename="" line="0"/>
        <source>authpriv</source>
        <translation type="obsolete">authpriv</translation>
    </message>
</context>
<context>
    <name>src/gui/printerStream.cpp:147</name>
    <message>
        <location filename="" line="0"/>
        <source>Page %1</source>
        <translation type="obsolete">ページ %1</translation>
    </message>
</context>
<context>
    <name>src/gui/solarisAdvancedDialog.cpp:62</name>
    <message>
        <location filename="" line="0"/>
        <source>No change</source>
        <translation type="obsolete">変更なし</translation>
    </message>
</context>
<context>
    <name>src/gui/solarisAdvancedDialog.cpp:65</name>
    <message>
        <location filename="" line="0"/>
        <source>On</source>
        <translation type="obsolete">オン</translation>
    </message>
</context>
<context>
    <name>src/gui/solarisAdvancedDialog.cpp:68</name>
    <message>
        <location filename="" line="0"/>
        <source>Off</source>
        <translation type="obsolete">オフ</translation>
    </message>
</context>
<context>
    <name>src/gui/upgradePredicate.h:45</name>
</context>
<context>
    <name>src/gui/upgradePredicate.h:53</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Upgrade</source>
        <translation type="obsolete">アップグレード(&amp;U)</translation>
    </message>
</context>
<context>
    <name>src/gui/upgradePredicate.h:54</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Do not load the file</source>
        <translation type="obsolete">ファイル %1 を読み込まない(&amp;D)</translation>
    </message>
</context>
<context>
    <name>src/gui/utils.cpp:197</name>
</context>
<context>
    <name>src/gui/utils.cpp:198</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue</source>
        <translation type="obsolete">続ける(&amp;C)</translation>
    </message>
</context>
<context>
    <name>src/gui/utils.cpp:219</name>
    <message>
        <location filename="" line="0"/>
        <source>Object with name &apos;%1&apos; already exists, please choose different name.</source>
        <translation type="obsolete">オブジェクト名 &apos;%1&apos; は既に存在します。別の名前を洗濯してください。</translation>
    </message>
</context>
<context>
    <name>src/gui/utils.cpp:221</name>
    <message>
        <location filename="" line="0"/>
        <source>&amp;Continue editing</source>
        <translation type="obsolete">編集を続ける(&amp;C)</translation>
    </message>
</context>
</TS>
