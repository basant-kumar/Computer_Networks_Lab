# ################################################
# Make a simulator (scheduler)
  set ns [new Simulator]

# ################################################
#Define different colors for data flows (for NAM)
  $ns color 1 Blue
  $ns color 2 Red

# ################################################
# Open the NAM trace file
  set file2 [open out.nam w]
  $ns namtrace-all $file2

# ################################################
# Open the Window trace file
  set winfile [open reno w]

# ################################################
# Define a 'finish' procedure
  proc finish {} {

     global ns file2
     $ns flush-trace
     close $file2

    # exec nam out.nam &
     exit 0
  }


# ################################################
# Create this configuration:
#
#            0                 4
#    2Mb/10ms \  0.3Mb/100ms  / 0.5Mb/40ms
#              2 ----------- 3
#    2Mb/10ms /               \ 0.5Mb/30ms
#            1                 5
#
#  TCP1:  0 -> 4
# ################################################

# ################################################
# Create the nodes:
 set n0 [$ns node]
 set n1 [$ns node]
 set n2 [$ns node]
 set n3 [$ns node]
 set n4 [$ns node]
 set n5 [$ns node]

# ################################################
# Create the links:
  $ns duplex-link $n0 $n2 2Mb 10ms DropTail
  $ns duplex-link $n1 $n2 2Mb 10ms DropTail
  $ns simplex-link $n2 $n3 0.3Mb 200ms DropTail
  $ns simplex-link $n3 $n2 0.3Mb 200ms DropTail
  $ns duplex-link $n3 $n4 0.5Mb 40ms DropTail
  $ns duplex-link $n3 $n5 0.5Mb 30ms DropTail

# ################################################
#Monitor the queue for link (n0-n1). (for NAM)
$ns duplex-link-op $n2 $n3 queuePos 0.1


# ################################################
# Give node position (for NAM)
  $ns duplex-link-op  $n0 $n2 orient right-down
  $ns duplex-link-op  $n1 $n2 orient right-up
  $ns simplex-link-op $n2 $n3 orient right
  $ns simplex-link-op $n3 $n2 orient left
  $ns duplex-link-op  $n3 $n4 orient right-up
  $ns duplex-link-op  $n3 $n5 orient right-down


# ########################################################
# Set Queue Size of link (n2-n3) to 10 (default is 50 ?)
  $ns queue-limit $n2 $n3 10

# ########################################################
# Setup a TCP connection
  set tcp [new Agent/TCP/Newreno]
  $ns attach-agent $n0 $tcp

# set sink [new Agent/TCPSink/DelAck]
  set sink [new Agent/TCPSink]
  $ns attach-agent $n4 $sink

  $ns connect $tcp $sink
  $tcp set fid_ 1
  $tcp set window_ 8000
  $tcp set packetSize_ 552

# ########################################################
# Setup a FTP over TCP connection
  set ftp [new Application/FTP]
  $ftp attach-agent $tcp
  $ftp set type_ FTP


# ########################################################
# Schedule start/stop times
  $ns at 0.1 "$ftp start"
  $ns at 100.0 "$ftp stop"

# ####################################################################
# plotWindow(tcpSource,file): write CWND from $tcpSource
#			      to output file $file every 0.1 sec
  proc plotWindow {tcpSource file} {
     global ns

     set time 0.1
     set now [$ns now]
     set cwnd [$tcpSource set cwnd_]
     set wnd [$tcpSource set window_]
     puts $file "$now $cwnd"
     $ns at [expr $now+$time] "plotWindow $tcpSource $file" 
  }

# ####################################################################
# Start plotWindow
  $ns at 0.1 "plotWindow $tcp $winfile"

# ####################################################################
# Set simulation end time
  $ns at 125.0 "finish"


set trace_file [open  "newreno.tr"  w]

$ns  trace-queue  $n2  $n3  $trace_file

# ####################################################################
# Run !!!!
  $ns run
