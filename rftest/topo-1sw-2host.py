""" rftestT topology

authors: Marcelo Nascimento (marcelon@cpqd.com.br)
         Allan Vidal (allanv@cqpd.com.br)

Four switches connected in mesh topology plus a host for each switch:

       h1 --- sA ---- sB --- h2
               |  \    |
               |   \   |
               |    \  |
               |     \ |
       h3 --- sC ---- sD --- h4

"""

from mininet.topo import Topo

class rftestT(Topo):
    "RouteFlow Demo Setup"

    def __init__( self, enable_all = True ):
        "Create custom topo."

        Topo.__init__( self )

        h1 = self.addHost("h1",
                          ip="192.168.124.2/24",
                          defaultRoute="gw 192.168.124.1")

        h2 = self.addHost("h2",
                          ip="192.168.125.2/24",
                          defaultRoute="gw 192.168.125.1")



        sA = self.addSwitch("s5")

        self.addLink(h1, sA)
        self.addLink(h2, sA)


topos = { 'rftestT': ( lambda: rftestT() ) }
