RouteFlow
Copyright (C) 2012 CPqD

=== Welcome ===
Welcome to the RouteFlow remote virtual routing platform.  This distribution
includes all the software you need to build, install, and deploy RouteFlow in
your OpenFlow network.

This version of RouteFlow is a beta developers' release intended to evaluate
RouteFlow for providing virtualized IP routing services on one or more OpenFlow
switches.

RouteFlow relies on the following technologies:
- NOX/POX and OpenFlow v1.0 as the communication protocol for controlling 
  switches.
- Open vSwitch to provide the connectivity within the virtual environment where
  Linux virtual machines may run the Quagga routing engine.
- MongoDB as a central database and IPC.

Please be aware of NOX, POX, OpenFlow, Open vSwitch, Quagga, MongoDB, jQuery,
JIT and RouteFlow licenses and terms.


=== Overview ===
RouteFlow is a distribution composed by three basic applications: RFClient,
RFServer and RFProxy.

- RFClient is the module running as a daemon in the Virtual Machine (VM)
responsible for detecting changes in the Linux ARP and routing tables.
Routing information are send to the RFServer when they're updated.

- RFServer is a standalone application that manages the VMs running the
RFClient daemons. The RFServer keeps the mapping between the RFClient VM
instances and interfaces and the corresponding switches and ports. It connects 
to RFProxy to instruct it about when to configure flows and also to configure 
the Open vSwitch to maintain the connectivity in the virtual environment formed 
by the set of VMs.

- RFProxy is an application (for NOX and POX) responsible for the interactions 
with the OpenFlow switches (identified by datapaths) via the OpenFlow protocol. 
It listens to instructions from the RFServer and notifies it about events in
the network.
  We recommend running POX when you are experimenting and testing your network.
You can also use NOX though if you need or want (for production maybe).

There is also a library of common functions (rflib). It has implementations of 
the IPC, utilities like custom types for IP and MAC addresses manipulation and 
OpenFlow message creation.

Additionally, there are two extra modules: rfweb, an web interface for
RouteFlow and jsonflowagent, an SNMP agent.


== RouteFlow architecture ===
+--------VM---------+
| Quagga | rfclient |
+-------------------+
         \
M:1      \ RFProtocol
         \
+-------------------+
|     rfserver      |
+-------------------+
         \
1:1      \ RFProtocol
         \
+-------------------+
|      rfproxy      |
|-------------------|
|      NOX/POX      |
+-------------------+
         \
1:N      \ OpenFlow Protocol
         \
+-------------------+
|  OpenFlow Switch  |
+-------------------+


=== Building ===
These instructions are tested on Ubuntu 11.04.

** Open vSwitch **
1) Install the dependencies:
$ sudo apt-get install build-essential linux-headers-generic

2) Download Open vSwitch 1.4.1, extract it to a folder and browse to it:
$ wget http://openvswitch.org/releases/openvswitch-1.4.1.tar.gz
$ tar xzf openvswitch-1.4.1.tar.gz
$ cd openvswitch-1.4.1

3) Configure it as a kernel module, then compile and install
$ ./configure --with-linux=/lib/modules/`uname -r`/build
$ make
$ sudo make install

4) Install the modules in your system:
$ sudo mkdir /lib/modules/`uname -r`/kernel/net/ovs
$ sudo cp datapath/linux/*.ko /lib/modules/`uname -r`/kernel/net/ovs/
$ sudo depmod -a
$ sudo modprobe openvswitch_mod

Optionally, you may choose to skip the steps above and load the module manually
every time:

$ sudo insmod datapath/linux/openvswitch_mod.ko

--- Note ---
	If for some reason the bridge module is loaded in your system, the command
	above will fail with an invalid module format error. If you don't need the
	default bridge support in your system, use the modprobe -r bridge command to
	unload it and try the modprobe command again. If you need bridge support,
	you can get some help from the INSTALL.bridge file instructions in the
	Open vSwitch distribution directory.

	To avoid automatic loading of the bridge module (which would conflict with
	openvswitch_mod), let's blacklist it. Access the /etc/modprobe.d/ directory
	and create a new bridge.conf file:

	$ cd /etc/modprobe.d
	$ sudo vi bridge.conf

	Insert the following lines in the editor, save and close:

	# avoid conflicts with the openvswitch module
	blacklist bridge

5) Edit /etc/modules to configure the automatic loading of the openvswitch_mod
   module:

	$ sudo vi /etc/modules

	Insert the following line at the end of the file, save and close:

	openvswitch_mod

	To be sure that everything is OK, reboot your computer. Log in and try the
	following command. If the "openvswitch_mod" line is shown like in the
	example below, you're ready to go.

	$ sudo lsmod | grep openvswitch_mod
	openvswitch_mod        68247  0

6) Initialize the configuration database:
$ sudo mkdir -p /usr/local/etc/openvswitch
$ sudo ovsdb-tool create /usr/local/etc/openvswitch/conf.db vswitchd/vswitch.ovsschema


** MongoDB **
1) Install the dependencies:
$ sudo apt-get install git-core build-essential scons libboost-dev \
  libboost-program-options-dev libboost-thread-dev libboost-filesystem-dev \
  python-pip

3) Download and extract MongoDB v2.0.5
$ wget http://downloads.mongodb.org/src/mongodb-src-r2.0.5.tar.gz
$ tar zxf mongodb-src-r2.0.5.tar.gz
$ cd mongodb-src-r2.0.5

3) There's a conflict with a constant name in NOX and MongoDB. It has
been fixed, but is not part of version 2.0.5 yet. So, we need to fix it
applying the changes listed in this commit:
https://github.com/mongodb/mongo/commit/a1e68969d48bbb47c893870f6428048a602faf90

4) Then compile and install MongoDB:
$ scons all
$ sudo scons --full install --prefix=/usr --sharedclient
$ sudo pip install pymongo


** NOX install instructions **
These instructions are only necessary if you want to run RouteFlow using NOX.
The version of the NOX controller we are using does not compile under newer 
versions of Ubuntu (11.10, 12.04). You can use POX, which doesn't require 
compiling.

1) Install the dependencies:
$ sudo apt-get install autoconf automake g++ libtool swig make git-core \
  libboost-dev libboost-test-dev libboost-filesystem-dev libssl-dev \
  libpcap-dev python-twisted python-simplejson python-dev

2) TwistedPython, one of the dependencies of the NOX controller bundled with
the RouteFlow distribution, got an update that made it stop working.
To get around this issue, edit the following file in your machine:
$ sudo vi /usr/lib/python2.6/dist-packages/twisted/internet/base.py

Insert the method _handleSigchld at the end of the file, just before the last
statement (the one that reads "__all__ = []"):

[other lines above... (supressed) ]
            except:
                log.msg("Unexpected error in main loop.")
                log.err()
            else:
                log.msg('Main loop terminated.')

    def _handleSigchld(self, signum, frame, _threadSupport=platform.supportsThreads()):
        from twisted.internet.process import reapAllProcesses
        if _threadSupport:
            self.callFromThread(reapAllProcesses)
        else:
            self.callLater(0, reapAllProcesses)

__all__ = []

Save the file and you're ready to go.

NOX will be compiled with the RouteFlow distribution in the steps ahead.


** RouteFlow **
1) Install the dependencies:
$ sudo apt-get install build-essential iproute-dev swig1.3

2) Checkout the RouteFlow distribution:
$ git clone git://github.com/CPqD/RouteFlow.git

3) You can compile all RouteFlow applications by running the following command
in the project root:
$ make all

You can also compile them individually:
$ make rfclient
$ make nox

4) That's it, everything is compiled! After the build, you can run tests 1 and
2. The setup to run them is described in the section "Running".


=== Running ===
The folder rftest contains all that is needed to create and run two test cases.

First, create the LXC containers that will run as virtual machines:
   $ sudo ./create

The containers will have a default root/root user/password combination. You 
should change that if you plan to deploy RouteFlow.

In the tests below, you can choose to run with either NOX or POX by changing
the command line arguments.

rftest1
   $ sudo ./rftest1 --nox
   To stop the script, press CTRL+C.
    
   You can then log in to the LXC container b1 and try to ping b2:
   $ sudo lxc-console -n b1
   And inside b1:
   # ping 172.31.2.2

   For more details on this test, see:
   http://sites.google.com/site/routeflow/documents/first-routeflow

rftest2
   $ sudo ./rftest2 --pox
   To stop the script, press CTRL+C.

   This test should be run with a Mininet simulated network:
   http://yuba.stanford.edu/foswiki/bin/view/OpenFlow/Mininet

   Once you have a Mininet setup, copy the files in rftest to the VM:
   $ scp topo-4sw-4host.py openflow@[Mininet address]:/home/openflow/mininet/custom
   $ scp ipconf openflow@[Mininet address]:/home/openflow/

   Then start the network:
   $ sudo mn --custom ~/mininet/custom/topo-4sw-4host.py --topo=rftopo" \
   --controller=remote --ip=[Controller address] --port=6633"

   Inside Mininet, load the address configuration:
   mininet> source ipconf

   Wait for the network to converge (it should take a few seconds), and try to
   ping:
   mininet> pingall
   
   By default, this test will use the virtual machines (LXC containers) created 
   by the "create" script mentioned above. You can use other virtualization 
   technologies. If you have experience with or questions about setting up 
   RouteFlow on a particular technology, contact us! See the section "Support".

   For more details on this test, see:
   http://sites.google.com/site/routeflow/documents/tutorial2-four-routers-with-ospf


=== Web interface ===
The module rfweb provides an web application to inspect the network, showing
topology, status and statistics. The application is written in Python using the
WSGI specification: http://wsgi.readthedocs.org/en/latest/

The web interface only works when running under POX.

It's possible to run the application in several servers, and a simple server is
provided in rfweb_server.py. This server is very simple, and you probably don't
want to use it for anything more serious than testing and playing around:

$ python rfweb_server.py

We've also tested the application with gunicorn (http://gunicorn.org/). You can
run rfweb on it using the following command:

$ gunicorn -w 4 -b 0.0.0.0:8080 rfweb:application
(Runs four workers, listening on all interfaces on port 8080)

Then to access the main page of the web interface (adapt the address to your
setup), go to:
http://localhost:8080/index.html


=== Support ===
RouteFlow has a discussion list. You can send your questions on:
https://groups.google.com/group/routeflow-discuss/topics


=== Known Bugs ===
- rftest*: When closing the tests, segfaults happen, to no effect.

- RouteFlow: when all datapaths go down and come back after a short while, 
  bogus routes are installed on them caused by delayed updates from RFClients.

- See also: 
  http://bugs.openflowhub.org/browse/ROUTEFLOW
  https://github.com/CPqD/RouteFlow/issues


=== TODO (and features expected in upcoming versions) ===
- Tests and instructions for other virtualization environments

- Hooks into Quagga Zebra to reflect link up/down events and extract additional 
  route / label information

- Create headers for RFClient.cc

- Let the RFServer order the RFClient to set the VM's non-administrative
  interfaces to the same MAC Address

- Create a verbose mode for RFServer

- Configuration arguments for RFServer

- Port RFProxy to Trema, Floodlight

- Add TTL-decrement action (if supported by the datapath devices)

- Explore integration opportunities with FlowVisor

- Libvirt: Virtualization-independence to accomodate alternative virtualization
  environments via unified virtualization API provided by libvirt
    - provide on-demand start-up of VMs via libvirt upon interactions (e.g. CLI)
      with RFServer)

- Routing Protocol Optimization:
    - Separate topology discovery and maintenance from state distribution
    - Dynamic virtual topology maintenance, with selective routing protocol
      messages delivery to the Datapath (e.g., HELLOs).
    - Improve the scenario where routing protocol messages are kept in the
      virtual domain and topology is mantained through a  Topology Discovery
      controller application.
    - Hooks into Quagga Zebra to reflect link up/down events

- Resiliency & Scalability:
    - Physically distribute the virtualization environment via mutliple OVS
      providing the connectivity of the virtual control network
    - Improve resiliency: Have a "stand-by" environment to take over in case of
      failure of the master RFServer / Virtualized Control Plane

- For smaller changes, see TODO markings in the files.