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

//    n10  n11  n12
//     |    |    |____  
//     10.1.1.0 wifi   |
//                     |  10.1.2.0 p2p
//                     |                
//                     |                p2p 10.1.4.0
//   10.1.6.0 |-n1 n2 n3 n4----------------------n5 n6 n7 n8
//      p2p   | |  |  |  |                        |  |  |  | 
//            |---------- 10.1.3.0 csma          wifi 10.1.5.0
//            |n9

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
  p2pNodes2.Add(csmaNodes.Get(2));
  p2pNodes2.Create (1);

  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices2;
  p2pDevices2 = pointToPoint2.Install (p2pNodes2);

  //p2p 10.1.6.0
  NodeContainer p2pNodes3;
  p2pNodes3.Add(csmaNodes.Get(0));
  p2pNodes3.Create (1);

  PointToPointHelper pointToPoint3;
  pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint3.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices3;
  p2pDevices3 = pointToPoint3.Install (p2pNodes3);


  //wifi 1 10.1.1.0
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (4);
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

  //wifi-2 10.1.5.0
  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (4);
  NodeContainer wifiApNode2 = p2pNodes2.Get(1);

  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2;
  phy2.SetChannel (channel2.Create ());

  WifiHelper wifi2;
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac2;
  Ssid ssid2 = Ssid ("ns-3-ssid");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices2;
  staDevices2 = wifi2.Install (phy2, mac2, wifiStaNodes2);

  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifiApNode2);

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


  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install (wifiApNode2);

  // empiler 
  InternetStackHelper stack;

  stack.Install(p2pNodes.Get(0));
  stack.Install(csmaNodes);
  stack.Install(wifiStaNodes);

  stack.Install(p2pNodes2.Get(1));
  stack.Install(wifiStaNodes2);
  
  stack.Install(p2pNodes3.Get(1));


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

  address.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3;
  p2pInterfaces3 = address.Assign (p2pDevices3); 

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces;
  wifiInterfaces = address.Assign (staDevices);
  address.Assign (apDevices);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces2;
  wifiInterfaces2 = address.Assign (staDevices2);
  address.Assign (apDevices2);

  //application 
  Ptr<Socket> App1 = Socket::CreateSocket (wifiStaNodes.Get (1), TypeId::LookupByName ("ns3::UdpSocketFactory"));
  uint16_t port1 = 8801;
  Ipv4Address addr1 ("10.1.1.2");
  InetSocketAddress Add1 = InetSocketAddress (addr1, port1);
  App1->Bind (Add1);
  App1->SetRecvCallback (MakeCallback (&Recevoir_app1));

  Ptr<Socket> App2 = Socket::CreateSocket (wifiStaNodes2.Get (1), TypeId::LookupByName ("ns3::UdpSocketFactory"));
  uint16_t port2 = 8805;
  Ipv4Address addr2 ("10.1.5.2");
  InetSocketAddress Add2 = InetSocketAddress (addr2, port2);
  App2->Bind (Add2);
  App2->SetRecvCallback (MakeCallback (&Recevoir_app2));

  Ptr<Socket> App3 = Socket::CreateSocket (csmaNodes.Get (1), TypeId::LookupByName ("ns3::UdpSocketFactory"));
  uint16_t port3 = 8803;
  Ipv4Address addr3 ("10.1.3.2");
  InetSocketAddress Add3 = InetSocketAddress (addr3, port3);
  App3->Bind (Add3);
  App3->SetRecvCallback (MakeCallback (&Recevoir_app3));
 
  Simulator::Schedule (Seconds (2.1),&Envoyer, App1, addr2, port2, 1);
  Simulator::Schedule (Seconds (2.1),&Envoyer, App1, addr3, port3, 2); 
 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

void Envoyer (Ptr<Socket> sock, Ipv4Address dstaddr, uint16_t port, uint16_t i)
{
  if (i == 1) {
    std::cout << "APP 1 envoie INFO 1 a " << dstaddr << " sur le port " << port << "\n";
    Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*>("INFO 1"), 10);
    sock->SendTo (p, 0, InetSocketAddress (dstaddr, port));
  }

  if (i == 2) {
    std::cout << "APP 1 envoie INFO 2 a " << dstaddr << " sur le port " << port << "\n\n";
    Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*>("INFO 2"), 10);
    sock->SendTo (p, 0, InetSocketAddress (dstaddr, port));
  }
}

void Recevoir_app2 (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
 InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("APP 2 recois :" << buf << "\t Size: "<<packet->GetSize ()<< " bytes \t from: " << address.GetIpv4 ());
 
 
 Ptr<Packet>  p= Create<Packet> (reinterpret_cast<const uint8_t*>("ACK-2"),10);
 socket->SendTo (p, 0, InetSocketAddress ("10.1.3.2",8803));
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

void Recevoir_app3 (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
 InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("APP 3 recois :" << buf << "\t Size: "<<packet->GetSize ()<< " bytes \t from: " << address.GetIpv4 ());

 std::cout << "\n";

}


