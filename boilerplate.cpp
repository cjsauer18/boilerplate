

{
  "apiVersion": "v1",
  "kind": "Pod",
  "metadata": {
    "name": "example-pod",
    "namespace": "common-namespace",
    "labels": {
      "app": "example-app",
      "unique-id": "unique-identifier-12345"
    },
    "annotations": {
      "description": "This is an example pod with unique metadata."
    }
  },
  "spec": {
    "containers": []  // This will be replaced with the container spec
  }
}


{
  "name": "example-container",
  "image": "example-image:latest",
  "ports": [
    {
      "containerPort": 8080
    }
  ]
}


// code ot find pods by a unique identifier. 

#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

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
            return pod["metadata"]["name"];
        }
    }
    return "";
}

// Function to delete (stop) a pod
void deletePod(const std::string& apiServer, const std::string& token, const std::string& namespaceName, const std::string& podName) {
    std::string url = apiServer + "/api/v1/namespaces/" + namespaceName + "/pods/" + podName;
    makeHTTPRequest(url, token, "DELETE");
    std::cout << "Pod " << podName << " deleted successfully" << std::endl;
}

// Main function to demonstrate usage
int main() {
    std::string apiServer = "https://127.0.0.1:16443";  // For MicroK8s running locally
    std::string token = "<YOUR_BEARER_TOKEN>";  // Replace with your actual token
    std::string namespaceName = "default";
    std::string labelSelector = "app=example-app";  // Label selector to list pods of a specific application

    // List the pods
    json pods = listPods(apiServer, token, namespaceName, labelSelector);

    // Find the pod by unique identifier
    std::string uniqueId = "unique-identifier-12345";
    std::string podName = findPodByUniqueId(pods, uniqueId);
    if (!podName.empty()) {
        std::cout << "Found pod with unique ID: " << podName << std::endl;

        // Delete the pod
        deletePod(apiServer, token, namespaceName, podName);
    } else {
        std::cout << "Pod with unique ID not found" << std::endl;
    }

    return 0;
}




// Function to delete (stop) a pod
void deletePod(const std::string& apiServer, const std::string& token, const std::string& namespaceName, const std::string& podName) {
    std::string url = apiServer + "/api/v1/namespaces/" + namespaceName + "/pods/" + podName;
    makeHTTPRequest(url, token, "DELETE");
    std::cout << "Pod " << podName << " deleted successfully" << std::endl;
}

// Function to create a pod with a new configuration
void createPod(const std::string& apiServer, const std::string& token, const json& podSpec) {
    std::string url = apiServer + "/api/v1/namespaces/" + podSpec["metadata"]["namespace"].get<std::string>() + "/pods";
    std::string body = podSpec.dump();
    makeHTTPRequest(url, token, "POST", body);
    std::cout << "Pod created successfully" << std::endl;
}

// Function to update a pod configuration (stop and start with new config)
void updatePodConfig(const std::string& apiServer, const std::string& token, const json& podSpec) {
    std::string namespaceName = podSpec["metadata"]["namespace"];
    std::string podName = podSpec["metadata"]["name"];
    deletePod(apiServer, token, namespaceName, podName);
    createPod(apiServer, token, podSpec);
}
