#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"
#include "ns3/animation-interface.h"


//    n11  n12  n13
//     |    |    |______                                 wifi 10.1.6.0
//     10.1.1.0 wifi   |                                n5    n6   n7
//                     |  10.1.2.0 p2p                   |    |    |
//                     |                                 |
//                     |                                 |
//                     |        p2p 10.1.4.0    n8  n9  n10
//              n1 n2 n3 n4----------------------|   |   |      
//              |  |  |  |                       --------      
//              ---------- 10.1.3.0 csma          10.1.5.0 csma
//                 |
//                 |10.1.7.0
//                 |  p2p
//                 |
//     n14  n15  n16
//      |    |    |  
//     10.1.8.0 wifi

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("stationwifi");
void Envoyer (Ptr<Socket> sock, Ipv4Address dstaddr, uint16_t port, uint16_t i);

void Recevoir_app1 (Ptr<Socket> socket);
void Recevoir_app2 (Ptr<Socket> socket);
void Recevoir_app3 (Ptr<Socket> socket);

int main ()
{

  LogComponentEnable ("stationwifi", LOG_LEVEL_INFO);

  //p2p 10.1.2.0
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);


  //csma 10.1.3.0
  NodeContainer csmaNodes;
  csmaNodes.Add(p2pNodes.Get(1));
  csmaNodes.Create (3);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  //p2p 10.1.4.0
  NodeContainer p2pNodes2;
  p2pNodes2.Add(csmaNodes.Get(3));
  p2pNodes2.Create (1);

  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices2;
  p2pDevices2 = pointToPoint2.Install (p2pNodes2);


  //csma 10.1.5.0
  NodeContainer csmaNodes2;
  csmaNodes2.Add(p2pNodes2.Get(1));
  csmaNodes2.Create (2);

  CsmaHelper csma2;
  csma2.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma2.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices2;
  csmaDevices2 = csma2.Install (csmaNodes2);


  //---------------p2p 10.1.4.0
  NodeContainer p2pNodes3;
  p2pNodes3.Add(csmaNodes.Get(1));
  p2pNodes3.Create (1);

  PointToPointHelper pointToPoint3;
  pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint3.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices3;
  p2pDevices3 = pointToPoint3.Install (p2pNodes3);
//-----------------------------------------------------


  //wifi 1 10.1.1.0
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (3);
  NodeContainer wifiApNode = p2pNodes.Get(0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);


  //wifi 1 10.1.6.0
  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (3);
  NodeContainer wifiApNode2 = csmaNodes2.Get(2);

  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2;
  phy2.SetChannel (channel2.Create ());

  WifiHelper wifi2;
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac2;
  Ssid ssid2 = Ssid ("ns-3-ssid");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices2;
  staDevices2 = wifi2.Install (phy2, mac2, wifiStaNodes2);

  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifiApNode2);

  //wifi 1 10.1.6.0
  NodeContainer wifiStaNodes3;
  wifiStaNodes3.Create (3);
  NodeContainer wifiApNode3 = p2pNodes3.Get(1);

  YansWifiChannelHelper channel3 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy3;
  phy3.SetChannel (channel3.Create ());

  WifiHelper wifi3;
  wifi3.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac3;
  Ssid ssid3 = Ssid ("ns-3-ssid");
  mac3.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid3),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices3;
  staDevices3 = wifi3.Install (phy3, mac3, wifiStaNodes3);

  mac3.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid3));

  NetDeviceContainer apDevices3;
  apDevices3 = wifi3.Install (phy3, mac3, wifiApNode3);

  // mobilité 
  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);
  mobility.Install (wifiStaNodes2);
  mobility.Install (wifiStaNodes3);


  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install (wifiApNode2);
  mobility.Install (wifiApNode3);


  // empiler 
  InternetStackHelper stack;

  stack.Install(p2pNodes.Get(0));
  stack.Install(wifiStaNodes);
  stack.Install(csmaNodes);
  stack.Install(csmaNodes2);

  stack.Install(wifiStaNodes2);

  stack.Install(p2pNodes3.Get(1));
  stack.Install(wifiStaNodes3);


  // @ IP 
  Ipv4AddressHelper address;

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices); 

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2;
  p2pInterfaces2 = address.Assign (p2pDevices2); 

  address.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces2;
  csmaInterfaces2 = address.Assign (csmaDevices2);

  address.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3;
  p2pInterfaces3 = address.Assign (p2pDevices3);  


  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces;
  wifiInterfaces = address.Assign (staDevices);
  address.Assign (apDevices);


  address.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces2;
  wifiInterfaces2 = address.Assign (staDevices2);
  address.Assign (apDevices2);

  address.SetBase ("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces3;
  wifiInterfaces3 = address.Assign (staDevices3);
  address.Assign (apDevices3); 


  //application 
  Ptr<Socket> App1 = Socket::CreateSocket (wifiStaNodes.Get (1), TypeId::LookupByName ("ns3::UdpSocketFactory"));
  uint16_t port1 = 8801;
  Ipv4Address addr1 ("10.1.1.2");
  InetSocketAddress Add1 = InetSocketAddress (addr1, port1);
  App1->Bind (Add1);
  App1->SetRecvCallback (MakeCallback (&Recevoir_app1));

  Ptr<Socket> App2 = Socket::CreateSocket (wifiStaNodes2.Get (1), TypeId::LookupByName ("ns3::UdpSocketFactory"));
  uint16_t port2 = 8806;
  Ipv4Address addr2 ("10.1.6.2");
  InetSocketAddress Add2 = InetSocketAddress (addr2, port2);
  App2->Bind (Add2);
  App2->SetRecvCallback (MakeCallback (&Recevoir_app2));

  Ptr<Socket> App3 = Socket::CreateSocket (wifiStaNodes3.Get (1), TypeId::LookupByName ("ns3::UdpSocketFactory"));
  uint16_t port3 = 8808;
  Ipv4Address addr3 ("10.1.8.2");
  InetSocketAddress Add3 = InetSocketAddress (addr3, port3);
  App3->Bind (Add3);
  App3->SetRecvCallback (MakeCallback (&Recevoir_app3));
 
  Simulator::Schedule (Seconds (2.1),&Envoyer, App1, addr2, port2, 1);
  Simulator::Schedule (Seconds (2.3),&Envoyer, App1, addr3, port3, 2); 


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;

}

void Envoyer (Ptr<Socket> sock, Ipv4Address dstaddr, uint16_t port, uint16_t i)
{
  if (i == 1) {
    std::cout << "APP 1 envoie REQ1 a " << dstaddr << " sur le port " << port << "\n";
    Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*>("REQ 1"), 10);
    sock->SendTo (p, 0, InetSocketAddress (dstaddr, port));
  }

  if (i == 2) {
    std::cout << "APP 1 envoie REQ2 a " << dstaddr << " sur le port " << port << "\n\n";
    Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*>("REQ2"), 10);
    sock->SendTo (p, 0, InetSocketAddress (dstaddr, port));
  }
}



void Recevoir_app3 (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
 InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("APP 3 recois :" << buf << "\t Size: "<<packet->GetSize ()<< " bytes \t from: " << address.GetIpv4 ());
 
 
 Ptr<Packet>  p= Create<Packet> (reinterpret_cast<const uint8_t*>("INFO3"),10);
 socket->SendTo (p, 0, InetSocketAddress ("10.1.6.2",8806));
 std::cout << "\n";


}

void Recevoir_app1 (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
 InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("APP 1 recois :" << buf << "\t Size: "<<packet->GetSize ()<< " bytes \t from: " << address.GetIpv4 ());
  std::cout << "\n";

}


void Recevoir_app2 (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
 InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("APP 2 recois :" << buf << "\t Size: "<<packet->GetSize ()<< " bytes \t from: " << address.GetIpv4 ());
  std::cout << "\n";


}
