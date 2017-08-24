#Make a NS simulator 
set ns [new Simulator]        

#open the NAM trace file
set nf [ open out.nam w]
$ns namtrace-all $nf

#define different colors for data flows
$ns color 1 Red
$ns color 2 Green
$ns color 3 Blue
$ns color 4 Yellow

#define a finish procedure
proc finish {} {
	global ns nf
	$ns flush-trace
	#close the NAM trace file	
	close $nf
	#execute NAM on the trace file
	#exec nam out.nam &
	exit 0
}
# Create the nodes:
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

# Create the links:
$ns duplex-link $n0 $n2   1Mb  10ms DropTail
$ns duplex-link $n1 $n2   1Mb  10ms DropTail
$ns duplex-link $n2 $n3 0.25Mb 10ms DropTail
$ns duplex-link $n3 $n4 1Mb  10ms DropTail
$ns duplex-link $n3 $n5 1Mb  10ms DropTail



#give node position (for nam)
$ns duplex-link-op $n0 $n2 orient right-up
$ns duplex-link-op $n1 $n2 orient right-down
$ns duplex-link-op $n2 $n3 orient right
$ns duplex-link-op $n3 $n4 orient right-up
$ns duplex-link-op $n3 $n5 orient right-down



#monitor the queue for link
$ns duplex-link-op $n2 $n3 queuePos 0.5

# ########################################################
# Set Queue Size of link (n2-n3) to 10
  $ns queue-limit $n2 $n3 10


# Add a TCP sending module to node n0
set tcp1 [new Agent/TCP/Reno]
$ns attach-agent $n0 $tcp1

# ########################################################
$tcp1 set window_ 8000
$tcp1 set packetSize_ 552
$tcp1 set fid_ 1


# Add a TCP receiving module to node n4
set sink1 [new Agent/TCPSink]
$ns attach-agent $n3 $sink1

# Direct traffic from "tcp1" to "sink1"
$ns connect $tcp1 $sink1

# Setup a FTP traffic generator on "tcp1"
set ftp1 [new Application/FTP]
$ftp1 attach-agent $tcp1
$ftp1 set type_ FTP               

# Schedule start/stop times
$ns at 0.1   "$ftp1 start"
$ns at 100.0 "$ftp1 stop"

# Set simulation end time
$ns at 125.0 "finish"            


##################################################
## Obtain Trace date at destination (n4)
##################################################

set trace_file [open  "out.tr"  w]

$ns  trace-queue  $n2  $n3  $trace_file



# Run simulation !!!!
$ns run
