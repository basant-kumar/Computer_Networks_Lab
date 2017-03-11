#define a simulator object
set ns [new Simulator]

#define different colors for data flows
$ns color 1 Red
$ns color 2 Green
$ns color 3 Blue
$ns color 4 Yellow

#open the NAM trace file
set nf [ open out.nam w]
$ns namtrace-all $nf

#define a finish procedure
proc finish {} {
	global ns nf
	$ns flush-trace
	#close the NAM trace file	
	close $nf
	#execute NAM on the trace file
	exec nam out.nam &
	exit 0
}

#create five nodes
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#create links between the nodes
$ns duplex-link $n1 $n2 1Mb 20ms DropTail #2-3
$ns duplex-link $n2 $n3 2Mb  30ms DropTail #1-2
$ns duplex-link $n3 $n4 2Mb 50ms DropTail #0-1
$ns duplex-link $n4 $n5 4Mb 40ms DropTail #4-3

#set queue size of link
$ns queue-limit $n1 $n2 10
$ns queue-limit $n4 $n5 6

#give node position (for nam)
$ns duplex-link-op $n1 $n2 orient right
$ns duplex-link-op $n2 $n3 orient right
$ns duplex-link-op $n3 $n4 orient right
$ns duplex-link-op $n4 $n5 orient right



#monitor the queue for link
$ns duplex-link-op $n1 $n2 queuePos 0.5
$ns duplex-link-op $n4 $n5 queuePos 1.5

#setup tcp1 connection
set tcp1 [new Agent/TCP]
$tcp1 set class_ 2
$ns attach-agent $n2 $tcp1
set sink1 [new Agent/TCPSink]
$ns attach-agent $n1 $sink1
$ns connect $tcp1 $sink1
$tcp1 set fid_ 1

#setup tcp2 connection
set tcp2 [new Agent/TCP]
$tcp2 set class_ 2
$ns attach-agent $n2 $tcp2
set sink2 [new Agent/TCPSink]
$ns attach-agent $n3 $sink2
$ns connect $tcp2 $sink2
$tcp2 set fid_ 2

#setup FTP over TCP1 connection
set ftp1 [new Application/FTP]
$ftp1 attach-agent $tcp1
$ftp1 set type_ FTP

#setup FTP over TC2 connection
set ftp2 [new Application/FTP]
$ftp2 attach-agent $tcp2
$ftp2 set type_ FTP

#setup udp1 connection
set udp1 [new Agent/UDP]
$ns attach-agent $n4 $udp1
set null1 [new Agent/Null]
$ns attach-agent $n3 $null1
$ns connect $udp1 $null1
$udp1 set fid_ 3

#setup udp2 connection
set udp2 [new Agent/UDP]
$ns attach-agent $n4 $udp2
set null2 [new Agent/Null]
$ns attach-agent $n5 $null2
$ns connect $udp2 $null2
$udp2 set fid_ 4

#setup CBR over udp1 connection
set cbr1 [new Application/Traffic/CBR]
$cbr1 attach-agent $udp1
$cbr1 set type_ CBR
$cbr1 set packet_size_ 1000
$cbr1 set rate_ 1mb
$cbr1 set random_ false

#setup CBR over udp2 connection
set cbr2 [new Application/Traffic/CBR]
$cbr2 attach-agent $udp2
$cbr2 set type_ CBR
$cbr2 set packet_size_ 1000
$cbr2 set rate_ 1mb
$cbr2 set random_ false

#schedule events for the cbr and ftp agents
$ns at 0.1 "$cbr1 start"
$ns at 0.5 "$ftp1 start"
$ns at 1.0 "$cbr2 start"
$ns at 1.5 "$ftp2 start"
$ns at 4.0 "$cbr1 stop"
$ns at 4.5 "$ftp1 stop"
$ns at 5.0 "$cbr2 stop"
$ns at 5.3 "$ftp2 stop"

#call the finish procedure to end
$ns at 5.5 "finish"

#print CBR packet size and interval
puts "CBR1 packet size =[$cbr1 set packet_size_]"
puts "CBR1 interval = [$cbr1 set interval_]"
puts "CBR2 packet size =[$cbr2 set packet_size_]"
puts "CBR2 interval = [$cbr2 set interval_]"

#run the simulation
$ns run


