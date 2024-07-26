#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "KubernetesController.h"  // Include the KubernetesController header
#include "ElementFactory.h"  // Include the ElementFactory header

class ControllerProcessor {
public:
    ControllerProcessor(const std::string& configFilePath) {
        controller = std::make_unique<KubernetesController>(configFilePath);
        loadConfig(configFilePath);
    }

    void deploy(const Payload& payload) {
        std::cout << "Deploying to " << payload.url_extension << " with data: " << std::endl;
        payload.element->printInfo();
        controller->createElement(payload.element->kind);
    }

    void loadManifests(const std::string& directoryPath) {
        // Assume we have a function to list files in a directory
        std::vector<std::string> files = listFiles(directoryPath);

        for (const auto& file : files) {
            std::ifstream fileStream(directoryPath + "/" + file);
            std::string jsonData((std::istreambuf_iterator<char>(fileStream)),
                                  std::istreambuf_iterator<char>());
            Payload payload = ElementFactory::createPayload(jsonData);
            deploy(payload);
        }
    }

private:
    std::unique_ptr<KubernetesController> controller;
    std::unordered_map<std::string, std::string> config;

    void loadConfig(const std::string& configFilePath) {
        std::ifstream configFile(configFilePath);
        json configJson;
        configFile >> configJson;
        for (json::iterator it = configJson.begin(); it != configJson.end(); ++it) {
            config[it.key()] = it.value();
        }
    }

    std::vector<std::string> listFiles(const std::string& directoryPath) {
        // Dummy implementation to list files
        // In real implementation, this should list files in the directoryPath
        return {"pod1.json", "pod2.json"}; // Example file names
    }
};
