#! /bin/sh
#

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
NAME=fwbuilder
DESC="Firewall Builder"
DEFAULT=/etc/default/fwbuilder
IPTABLES=/sbin/iptables

test -f $DEFAULT || exit 0
grep -s -q 'START_FWBUILDER=yes' $DEFAULT || exit 0

SCRIPT_DIR=$(grep -s "^[[:space:]]*FWBSCRIPT_DIR" $DEFAULT | cut -d "=" -f 2)
SCRIPT="$SCRIPT_DIR/$(hostname -s).fw"

stopfw() {
	#Set accept for default tables
	$IPTABLES -P OUTPUT  ACCEPT
	$IPTABLES -P INPUT   ACCEPT
	$IPTABLES -P FORWARD ACCEPT

	#Flush tables
	$IPTABLES -F
	$IPTABLES -F -t nat
	$IPTABLES -F -t mangle
	$IPTABLES -X
	$IPTABLES -X -t nat
	$IPTABLES -X -t mangle
}

test -x $SCRIPT || exit 0
test -x $IPTABLES || exit 0

set -e

case "$1" in
  start)
		echo -n "Starting $DESC: "
		$SCRIPT 2>/dev/null
		echo "$NAME."
	;;
  stop)
		echo -n "Stopping $DESC: "
		stopfw
		echo "$NAME."
	;;
  restart|force-reload|reload)
	#
	#	Firewall Builder generated script flushes tables prior
	#	to setting up new tables so safe to just re-execute
	#
		echo -n "Restarting $DESC: "
		$SCRIPT 2>/dev/null
		echo "$NAME."
	;;
	listfilter)
		$IPTABLES -L -n -v
	;;
	listnat)
		$IPTABLES -t nat -L -n -v
	;;
	listmangle)
		$IPTABLES -t mangle -L -n -v
	;;
  *)
		N=/etc/init.d/$NAME
		echo "Usage: $N {start|stop|restart|reload|force-reload|listfilter|listnat|listmangle}" >&2
		exit 1
	;;
esac

exit 0
