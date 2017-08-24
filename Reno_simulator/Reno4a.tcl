#Make a NS simulator 
set ns [new Simulator]        

# Define a 'finish' procedure
proc finish {} {
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
$ns duplex-link $n0 $n2   2Mb  10ms DropTail
$ns duplex-link $n1 $n2   2Mb  10ms DropTail
$ns duplex-link $n2 $n3 0.3Mb 200ms DropTail
$ns duplex-link $n3 $n4 0.5Mb  40ms DropTail
$ns duplex-link $n3 $n5 0.5Mb  30ms DropTail


# ########################################################
# Set Queue Size of link (n2-n3) to 10 (default is 50 ?)
  $ns queue-limit $n2 $n3 10


# Add a TCP sending module to node n0
set tcp1 [new Agent/TCP/Reno]
$ns attach-agent $n0 $tcp1

# ########################################################
$tcp1 set window_ 8000
$tcp1 set packetSize_ 552


# Add a TCP receiving module to node n4
set sink1 [new Agent/TCPSink]
$ns attach-agent $n4 $sink1

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

source TraceApp.ns

proc plotThroughput {tcpSink outfile} {
   global ns

   set now [$ns now]
   set nbytes [$tcpSink set bytes_]
   $tcpSink set bytes_ 0

   set time_incr 1.0

   set throughput [expr ($nbytes * 8.0 / 1000000) / $time_incr]

###Print TIME throughput for  gnuplot to plot progressing on throughput
   puts  $outfile  "$now $throughput"

   $ns at [expr $now+$time_incr] "plotThroughput $tcpSink  $outfile"
}



set trace_file [open  "out2.tput"  w]
set traceapp [new TraceApp]	  ;# Create a TraceApp object

$traceapp attach-agent $sink1     ;# Attach traceapp to TCPSink

$ns  at  0.0  "$traceapp  start"  ;# Start the traceapp object

$ns  at  0.0  "plotThroughput $traceapp  $trace_file"


# Run simulation !!!!
$ns run
