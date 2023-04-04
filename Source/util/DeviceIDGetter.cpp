/*
  ==============================================================================

    DeviceIDGetter.cpp
    Created: 31 Mar 2023 3:05:54pm
    Author:  andre

  ==============================================================================
*/

#include "DeviceIDGetter.h"

std::string getSystemId()
{
    std::string system_info;

#if defined(_WIN32)
    // Windows-specific system info
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    DWORD serial;
    GetVolumeInformation("C:\\", NULL, 0, &serial, NULL, NULL, NULL, 0);
    system_info += std::to_string(si.dwNumberOfProcessors) + "-" + std::to_string(serial);
#elif defined(__linux__)
    // Linux-specific system info
    utsname name;
    if (uname(&name) == -1)
        return "";
    system_info += name.nodename + "-" + std::to_string(gethostid());
#elif defined(__APPLE__) && defined(__MACH__)
    std::ostringstream system_info_ss;
    char mac_address[IFNAMSIZ];
    unsigned char hash[SHA_DIGEST_LENGTH];
    void* tmp_addr_ptr = nullptr;
    size_t len = 0;
    int success = 0;

    // Get the host name and concatenate with the model identifier
    char host_name[1024];
    success = gethostname(host_name, 1024);
    if (success == 0) {
        system_info_ss << host_name;
    }

    // Get the model identifier
    char model_identifier[256];
    len = sizeof(model_identifier);
    success = sysctlbyname("hw.model", &model_identifier, &len, NULL, 0);
    if (success == 0) {
        system_info_ss << "-" << model_identifier;
    }

    // Get the MAC address of the first Ethernet interface
    struct ifaddrs* if_list = nullptr;
    struct ifaddrs* if_list_head = nullptr;
    if (getifaddrs(&if_list_head) == 0) {
        if_list = if_list_head;
        while (if_list != nullptr) {
            if (if_list->ifa_addr->sa_family == AF_LINK) {
                struct sockaddr_dl* sdl = (struct sockaddr_dl*)if_list->ifa_addr;
                tmp_addr_ptr = sdl->sdl_data + sdl->sdl_nlen;
                len = sdl->sdl_alen;
                memcpy(mac_address, tmp_addr_ptr, len);
                mac_address[len] = '\0';
                if (strncmp(if_list->ifa_name, "en", 2) == 0) {
                    break;
                }
            }
            if_list = if_list->ifa_next;
        }
        freeifaddrs(if_list_head);
    }

    system_info = system_info_ss.str();
#endif
    system_info += SALT;
    // Hash the system info using SHA-1
    //unsigned char hash[SHA_DIGEST_LENGTH];
    //SHA1((const unsigned char*)system_info.c_str(), system_info.length(), hash);
    SHA256 hash(String(system_info).toUTF8());

    // Convert the hash to a string representation
    std::string system_id = hash.toHexString().toStdString();
    system_id = system_id.substr(0, 15);
    for (auto& c : system_id) {
        c = std::toupper(c);
    }

    return system_id;
}