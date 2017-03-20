
# Sliding-Window  ns script

# sliding window protocol in normal situation
# features : labeling, annotation, nam-graph, and window size monitoring

set ns [new Simulator]

$ns color 1 red

$ns trace-all [open sliding-window.tr w]
$ns namtrace-all [open sliding-window.nam w]

# build topology with 6 nodes
proc build_topology { ns } {

 global node_

 set node_(s1) [$ns node]
        set node_(s2) [$ns node]
        set node_(r1) [$ns node]
        set node_(r2) [$ns node]
        set node_(s3) [$ns node]
        set node_(s4) [$ns node]

        $node_(s2) color "red"
        $node_(s4) color "red"

        $node_(r1) color "blue"
        $node_(r2) color "blue"
 $node_(r1) shape "rectangular"
        $node_(r2) shape "rectangular"

        $ns at 0.0 "$node_(s1) label Sliding-W-sender"
        $ns at 0.0 "$node_(s2) label CBR-sender"
        $ns at 0.0 "$node_(s3) label Sliding-W-receiver"
        $ns at 0.0 "$node_(s4) label CBR-receiver"

 $ns duplex-link $node_(s1) $node_(r1) 0.5Mb 50ms DropTail
        $ns duplex-link $node_(s2) $node_(r1) 0.5Mb 50ms DropTail
        $ns duplex-link $node_(r1) $node_(r2) 0.5Mb 50ms DropTail
        $ns duplex-link $node_(r2) $node_(s3) 0.5Mb 50ms DropTail
        $ns duplex-link $node_(r2) $node_(s4) 0.5Mb 50ms DropTail

 $ns queue-limit $node_(r1) $node_(r2) 100
        $ns queue-limit $node_(r2) $node_(r1) 100

 $ns duplex-link-op $node_(s1) $node_(r1) orient right-down
        $ns duplex-link-op $node_(s2) $node_(r1) orient right-up
        $ns duplex-link-op $node_(r1) $node_(r2) orient right
        $ns duplex-link-op $node_(r2) $node_(s3) orient right-up
        $ns duplex-link-op $node_(r2) $node_(s4) orient right-down

        $ns duplex-link-op $node_(r1) $node_(r2) queuePos 0.5
        $ns duplex-link-op $node_(r2) $node_(r1) queuePos 0.5

}

build_topology $ns

Agent/TCP set nam_tracevar_ true

### sliding-window protocol between s1 and s3 (Black)

set tcp [$ns create-connection TCP $node_(s1) TCPSink $node_(s3) 0]
$tcp set windowInit_ 4   #initial value of cwnd, i.e. congestion window
$tcp set maxcwnd_ 4      #maximum congestion window size
$tcp set class_ 0

set ftp [$tcp attach-app FTP]

$ns add-agent-trace $tcp tcp
$ns monitor-agent-trace $tcp
$tcp tracevar cwnd_

### CBR traffic between s2 and s4 (Red)
set cbr [$ns create-connection CBR $node_(s2) Null $node_(s4) 1]
$cbr set packetSize_ 500
$cbr set interval_ 0.05
$cbr set class_ 1

proc finish {} {

 global ns
 $ns flush-trace

 puts "filtering..."
 exec tclsh ../ns-allinone-2.35/nam-1.07/bin/namfilter.tcl sliding-window.nam
 puts "running nam..."
 exec nam sliding-window.nam &
 exit 0
}

### set operations
$ns at 0.1 "$ftp start"
$ns at 1.7 "$ftp stop"
$ns at 0.1 "$cbr start"
$ns at 1.7 "$cbr stop"
$ns at 2.0 "finish"

### add annotations
$ns at 0.0 "$ns trace-annotate \"Normal operation of <Sliding Window> with window size, 4\""
$ns at 0.1 "$ns trace-annotate \"FTP starts at 0.1\""
$ns at 0.1 "$ns trace-annotate \"CBR starts at 0.1\""

$ns at 0.11 "$ns trace-annotate \"Send Packet_0,1,2,3 : window size, 4\""
$ns at 0.32 "$ns trace-annotate \"Ack_0,1,2,3\""
$ns at 0.46 "$ns trace-annotate \"Send Packet_4,5,6,7 : window size, 4\""
$ns at 0.66 "$ns trace-annotate \"Ack_4,5,6,7\""

$ns at 0.81 "$ns trace-annotate \"Send Packet_8,9,10,11   : window size, 4\""
$ns at 1.00 "$ns trace-annotate \"Ack_8,9,10,11\""
$ns at 1.16 "$ns trace-annotate \"Send Packet_12,13,14,15  : window size, 4\""
$ns at 1.35 "$ns trace-annotate \"Ack_12,13,14,15\""

$ns at 1.50 "$ns trace-annotate \"Send Packet_16,17,18,19   : window size, 4\""
$ns at 1.71 "$ns trace-annotate \"Ack_16,17,18,19\""

$ns at 1.8 "$ns trace-annotate \"FTP stops at 1.7\""
$ns at 1.8 "$ns trace-annotate \"CBR stops at 1.7\""

$ns run
  