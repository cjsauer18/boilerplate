#include <string>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class Configuration {
public:
    std::string server_domain;
    std::string token;
    std::string url_prefix;
    std::string pod_url;
    std::string namespace_url;
    std::string networkpolicy_url;

    void loadConfig(const std::string& configFilePath) {
        std::ifstream configFile(configFilePath);
        json configJson;
        configFile >> configJson;

        server_domain = configJson.value("server_domain", "");
        token = configJson.value("token", "");
        url_prefix = configJson.value("url_prefix", "");
        pod_url = configJson.value("pod_url", "/pods");
        namespace_url = configJson.value("namespace_url", "/namespaces");
        networkpolicy_url = configJson.value("networkpolicy_url", "/networkpolicies");
    }
};
