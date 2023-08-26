#include <iostream>
#include <cstdlib>
#include <string>
#include <pcap.h>
#include "PacketLimiter.h"

int main(int argc, char* argv[])
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <rate_mbps> <input_pcap> <output_pcap>" << std::endl;
        return 1;
    }

    // Parse command-line arguments
    double rateMbps = std::stod(argv[1]);
    const char* inputPcap = argv[2];
    const char* outputPcap = argv[3];

    // Create an instance of the PacketLimiter class with the specified rate
    try {
        PacketLimiter packetLimiter(rateMbps);
        // Open the input pcap file for reading
        pcap_t* pcapHandle;
        char errbuf[PCAP_ERRBUF_SIZE];
        pcapHandle = pcap_open_offline(inputPcap, errbuf);

        if (pcapHandle == nullptr) {
            std::cerr << "Error opening input pcap file: " << errbuf << std::endl;
            return 1;
        }

        // Create a pcap dumper for the processed packets
        pcap_dumper_t* pcapDumper;
        pcapDumper = pcap_dump_open(pcapHandle, "processed.pcap");
        if (pcapDumper == nullptr) {
            std::cerr << "Error creating output pcap file." << std::endl;
            pcap_close(pcapHandle);
            return 1;
        }

        // Create a pcap dumper for the missed packets
        pcap_t* missedPcapHandle;
        missedPcapHandle = pcap_open_dead(DLT_EN10MB, 65535);
        pcap_dumper_t* missedPcapDumper;
        missedPcapDumper = pcap_dump_open(missedPcapHandle, outputPcap);

        // Process each packet in the input pcap file
        struct pcap_pkthdr header; //Structure provided by the pcap library that contains information about the captured packet, such as timestamp, length, and so on
        const u_char* packet_data;
        int packetsProcessed = 0;
        int packetsMissed = 0;
        int totalPackets = 0;
        while ((packet_data = pcap_next(pcapHandle, &header)) != nullptr) {
            totalPackets++;
            // Try to consume the packet using the packetLimiter
            if (packetLimiter.tryConsume(header.len)) {
                // Packet consumed within the rate, dump to processed pcap file
                pcap_dump((u_char*)pcapDumper, &header, packet_data);
                packetsProcessed++;
            }
            else {
                // Packet not consumed within the rate, dump to missed pcap file
                pcap_dump((u_char*)missedPcapDumper, &header, packet_data);
                packetsMissed++;
            }
        }

        // Close pcap dumpers and handles
        pcap_dump_close(pcapDumper);
        pcap_dump_close(missedPcapDumper);
        pcap_close(pcapHandle);
        pcap_close(missedPcapHandle);

        // Print completion message
        std::cout << "Rate limited pcap generation complete." << std::endl;
        std::cout << "Total number of packets: " << totalPackets << std::endl;
        std::cout << "Packets processed: " << packetsProcessed << std::endl;
        std::cout << "Packets missed: " << packetsMissed << std::endl;
    }
    catch (const std::invalid_argument& e) {
        // Print error meessage if there is incorrect value for rate or packet size
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

