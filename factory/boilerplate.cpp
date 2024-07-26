#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <nholmann/json.hpp>

using json = nlohmann::json;



// Callback function for libcurl to write the response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to make an HTTP request using libcurl
std::string makeHTTPRequest(const std::string& url, const std::string& token, const std::string& method, const std::string& body = "") {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());

        if (method == "POST" || method == "PUT") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Only for local development
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // Only for local development

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return readBuffer;
}

// Function to read JSON from a file
json readJSONFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    json jsonData;
    file >> jsonData;
    return jsonData;
}

// Function to create a network policy
void createNetworkPolicy(const std::string& apiServer, const std::string& token, const json& policySpec) {
    std::string url = apiServer + "/apis/networking.k8s.io/v1/namespaces/" + policySpec["metadata"]["namespace"].get<std::string>() + "/networkpolicies";
    std::string body = policySpec.dump();
    makeHTTPRequest(url, token, "POST", body);
    std::cout << "Network policy created successfully" << std::endl;
}

// Function to create a pod
void createPod(const std::string& apiServer, const std::string& token, const json& podSpec) {
    std::string url = apiServer + "/api/v1/namespaces/" + podSpec["metadata"]["namespace"].get<std::string>() + "/pods";
    std::string body = podSpec.dump();
    makeHTTPRequest(url, token, "POST", body);
    std::cout << "Pod created successfully" << std::endl;
}

// Function to list pods by label
json listPods(const std::string& apiServer, const std::string& token, const std::string& namespaceName, const std::string& labelSelector) {
    std::string url = apiServer + "/api/v1/namespaces/" + namespaceName + "/pods?labelSelector=" + labelSelector;
    std::string response = makeHTTPRequest(url, token, "GET");
    return json::parse(response);
}

// Function to find a pod by unique identifier
std::string findPodByUniqueId(const json& pods, const std::string& uniqueId) {
    for (const auto& pod : pods["items"]) {
        if (pod["metadata"]["labels"]["unique-id"] == uniqueId) {
            return pod["metadata"]["name"]; // this pod may have a new pod name givcen to it.
        }
    }
    return "";
}

// Function to delete a pod
void deletePod(const std::string& apiServer, const std::string& token, const std::string& namespaceName, const std::string& podName) {
    std::string url = apiServer + "/api/v1/namespaces/" + namespaceName + "/pods/" + podName;
    makeHTTPRequest(url, token, "DELETE");
    std::cout << "Pod " << podName << " deleted successfully" << std::endl;
}

// Function to create a pod manifest by combining pod template and container spec
json createPodManifest(const json& podTemplate, const json& containerSpec) {
    json podManifest = podTemplate;
    podManifest["spec"]["containers"].push_back(containerSpec);
    return podManifest;
}

// Function to promote authorization by moving a pod to the privileged namespace
void promoteAuthorization(const std::string& apiServer, const std::string& token, const std::string& namespaceName, const std::string& uniqueId) {
    // List pods and find the one with the unique ID
    json pods = listPods(apiServer, token, namespaceName, "app=common-app");
    std::string podNameToUpdate = findPodByUniqueId(pods, uniqueId);
    if (!podNameToUpdate.empty()) {
        std::cout << "Found pod with unique ID: " << podNameToUpdate << std::endl;

        // Delete the existing pod
        deletePod(apiServer, token, namespaceName, podNameToUpdate);

        // Create a new pod in the privileged namespace with the same container spec
        json podTemplate = readJSONFromFile("pod-template.json");
        json containerSpec = readJSONFromFile("container-spec.json");
        podTemplate["metadata"]["namespace"] = "privileged-namespace";
        json podManifest = createConfiguredComponent(podTemplate, containerSpec, uniqueId);
        createPod(apiServer, token, podManifest);
    } else {
        std::cout << "Pod with unique ID not found" << std::endl;
    }
}
// Function to list all namespaces
std::vector<std::string> listNamespaces(const std::string& apiServer, const std::string& token) {
    std::string url = apiServer + "/api/v1/namespaces";
    std::string response = makeHTTPRequest(url, token, "GET");
    json namespaces = json::parse(response);
    std::vector<std::string> namespaceList;
    for (const auto& ns : namespaces["items"]) {
        namespaceList.push_back(ns["metadata"]["name"]);
    }
    return namespaceList;
}

// Function to search for a pod with a unique ID across all namespaces
std::string findPodNamespace(const std::string& apiServer, const std::string& token, const std::string& uniqueId) {
    std::vector<std::string> namespaces = listNamespaces(apiServer, token);
    for (const auto& ns : namespaces) {
        std::string url = apiServer + "/api/v1/namespaces/" + ns + "/pods?labelSelector=unique-id=" + uniqueId;
        std::string response = makeHTTPRequest(url, token, "GET");
        json pods = json::parse(response);
        if (!pods["items"].empty()) {
            return ns;
        }
    }
    return "";
}


json configureContainerSpec(json containerSpec, int& id, std::string& name){
  containerSpec["name"] = "common-app-" + std::to_string(id);
 // containerSpec[""]
 return containerSpec;
}

json configurePodSpec(json podSpec, int&id){
  podSpec["metadata"]["name"] = "common-pod-" + std::to_string(id);
  podSpec["metadata"]["labels"]["unique-id"] =  std::to_string(id);
  podSpec["metadata"]["labels"]["app"] = "common-app"; // generic name for content of pod. standardized as common app in this example model 
  return podSpec;
}

json createConfiguredComponent(json podSpec, json containerSpec, std::string & id){
  json podManifest = podSpec;
  containerSpec["name"] = "common-app-" + id;
  podManifest["metadata"]["name"] = "common-pod-" + id;
  podManifest["metadata"]["labels"]["unique-id"] = id;
  podManifest["metadata"]["labels"]["app"] = "common-app"; // generic name for content of pod. standardized as common app in this example model 
  podManifest["spec"]["containers"].push_back(containerSpec);
  return podManifest;
}


// Main function to demonstrate usage
int main() {
    std::string apiServer = "https://127.0.0.1:16443";  // For MicroK8s running locally
    std::string token = "<YOUR_BEARER_TOKEN>";  // Replace with your actual token

    // Read JSON data from files
    json privilegedPolicy = readJSONFromFile("privileged-policy.json");
    json defaultPolicy = readJSONFromFile("default-policy.json");
    json podTemplate = readJSONFromFile("pod-template.json");
    json containerSpec = readJSONFromFile("container-spec.json");

    // Create network policies
    createNetworkPolicy(apiServer, token, privilegedPolicy);
    createNetworkPolicy(apiServer, token, defaultPolicy);

    // Create an example pod in the default namespace
    
    json podManifestApp1 = createConfiguredComponent(podSpec, containerSpec, 1);
    createPod(apiServer, token, podManifestApp1);
    json podManifestApp2 = createConfiguredComponent(podSpec, containerSpec, 2);
    createPod(apiServer, token, podManifestApp2);
    json podManifestApp3 = createConfiguredComponent(podSpec, containerSpec, 3);
    createPod(apiServer, token, podManifestApp3);

    while (true) {
        std::cout << "Choose an option:\n";
        std::cout << "1. Add a pod\n";
        std::cout << "2. Remove a pod\n";
        std::cout << "3. Promote/Demote authorization of a pod\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        
        int choice;
        std::cin >> choice;

        if (choice == 4) {
            break;
        }

        std::cout << "Enter the pod identifier (integer): ";
        int podIdentifier;
        std::cin >> podIdentifier;
        std::string uniqueId = std::to_string(podIdentifier);

        switch (choice) {
            case 1: {
                // Add a pod
                json podTemplate = readJSONFromFile("pod-template.json");
                json containerSpec = readJSONFromFile("container-spec.json");
                json newPodManifest = createConfiguredComponent(podTemplate, containerSpec, uniqueId);
                createPod(apiServer, token, newPodManifest);
                break;
            }
            case 2: {
                // Remove a pod
                std::string namespaceName = findPodNamespace(apiServer, token, uniqueId); // Find namespace of pod to identify delete target
                std::string podName = "common-pod-" + uniqueId;
                deletePod(apiServer, token, namespaceName, podName);
                break;
            }
            case 3: {
                // Promote authorization of a pod
                // Get current network policy
                // Add condition to check if policy is demoting or promoting. 
                std::string currentNamespace = findPodNamespace(apiServer, token, uniqueId);
                std::string newNamespace = "";
                if (namespaceName = "priveleged-namespace"){
                  newNamespace = "default-namespace";
                  promoteAuthorization(apiServer, token, newNamespace, uniqueId);
                } else if (namespaceName = "default-namespace"){
                  newNamespace = "priveleged-namespace";
                  promoteAuthorization(apiServer, token, newNamespace, uniqueId);
                }  // Change as needed
                break;
            }
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }


    // Promote authorization by moving the pod to the privileged namespace
    promoteAuthorization(apiServer, token, "default-namespace",  podManifestApp1["metadata"]["labels"]["unique-id"], podManifestApp1["spec"]["containers"]);

    return 0;
}
