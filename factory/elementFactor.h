#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <memory>

using json = nlohmann::json;

// Base class for Kubernetes elements
class Element {
public:
    virtual ~Element() = default;
    virtual void fromJson(const json& j) = 0;
    virtual void printInfo() const = 0;

    std::string kind;
    std::string name;
    std::string namespace_;
    std::string creationTimestamp;
    std::unordered_map<std::string, std::string> labels;
    std::unordered_map<std::string, std::string> annotations;
};

// Derived class for Pod
class Pod : public Element {
public:
    void fromJson(const json& j) override {
        kind = j.value("kind", "");
        name = j["metadata"].value("name", "");
        namespace_ = j["metadata"].value("namespace", "");
        creationTimestamp = j["metadata"].value("creationTimestamp", "");
        if (j["metadata"].contains("labels")) {
            labels = j["metadata"]["labels"].get<std::unordered_map<std::string, std::string>>();
        }
        if (j["metadata"].contains("annotations")) {
            annotations = j["metadata"]["annotations"].get<std::unordered_map<std::string, std::string>>();
        }
    }

    void printInfo() const override {
        std::cout << "Kind: " << kind << ", Name: " << name << ", Namespace: " << namespace_
                  << ", CreationTimestamp: " << creationTimestamp << ", Labels: ";
        for (const auto& label : labels) {
            std::cout << label.first << "=" << label.second << " ";
        }
        std::cout << ", Annotations: ";
        for (const auto& annotation : annotations) {
            std::cout << annotation.first << "=" << annotation.second << " ";
        }
        std::cout << std::endl;
    }
};

// Derived class for Service
class Service : public Element {
public:
    void fromJson(const json& j) override {
        kind = j.value("kind", "");
        name = j["metadata"].value("name", "");
        namespace_ = j["metadata"].value("namespace", "");
        creationTimestamp = j["metadata"].value("creationTimestamp", "");
        if (j["metadata"].contains("labels")) {
            labels = j["metadata"]["labels"].get<std::unordered_map<std::string, std::string>>();
        }
        if (j["metadata"].contains("annotations")) {
            annotations = j["metadata"]["annotations"].get<std::unordered_map<std::string, std::string>>();
        }
    }

    void printInfo() const override {
        std::cout << "Kind: " << kind << ", Name: " << name << ", Namespace: " << namespace_
                  << ", CreationTimestamp: " << creationTimestamp << ", Labels: ";
        for (const auto& label : labels) {
            std::cout << label.first << "=" << label.second << " ";
        }
        std::cout << ", Annotations: ";
        for (const auto& annotation : annotations) {
            std::cout << annotation.first << "=" << annotation.second << " ";
        }
        std::cout << std::endl;
    }
};

// Derived class for Namespace
class Namespace : public Element {
public:
    void fromJson(const json& j) override {
        kind = j.value("kind", "");
        name = j["metadata"].value("name", "");
        creationTimestamp = j["metadata"].value("creationTimestamp", "");
        if (j["metadata"].contains("labels")) {
            labels = j["metadata"]["labels"].get<std::unordered_map<std::string, std::string>>();
        }
        if (j["metadata"].contains("annotations")) {
            annotations = j["metadata"]["annotations"].get<std::unordered_map<std::string, std::string>>();
        }
    }

    void printInfo() const override {
        std::cout << "Kind: " << kind << ", Name: " << name 
                  << ", CreationTimestamp: " << creationTimestamp << ", Labels: ";
        for (const auto& label : labels) {
            std::cout << label.first << "=" << label.second << " ";
        }
        std::cout << ", Annotations: ";
        for (const auto& annotation : annotations) {
            std::cout << annotation.first << "=" << annotation.second << " ";
        }
        std::cout << std::endl;
    }
};

// Derived class for NetworkPolicy
class NetworkPolicy : public Element {
public:
    void fromJson(const json& j) override {
        kind = j.value("kind", "");
        name = j["metadata"].value("name", "");
        namespace_ = j["metadata"].value("namespace", "");
        creationTimestamp = j["metadata"].value("creationTimestamp", "");
        if (j["metadata"].contains("labels")) {
            labels = j["metadata"]["labels"].get<std::unordered_map<std::string, std::string>>();
        }
        if (j["metadata"].contains("annotations")) {
            annotations = j["metadata"]["annotations"].get<std::unordered_map<std::string, std::string>>();
        }
    }

    void printInfo() const override {
        std::cout << "Kind: " << kind << ", Name: " << name << ", Namespace: " << namespace_
                  << ", CreationTimestamp: " << creationTimestamp << ", Labels: ";
        for (const auto& label : labels) {
            std::cout << label.first << "=" << label.second << " ";
        }
        std::cout << ", Annotations: ";
        for (const auto& annotation : annotations) {
            std::cout << annotation.first << "=" << annotation.second << " ";
        }
        std::cout << std::endl;
    }
};

// Payload class
class Payload {
public:
    Payload(std::unique_ptr<Element> elem) : element(std::move(elem)) {
        // Construct URL extension
        url_extension = "/" + element->kind + "s";
        if (!element->namespace_.empty()) {
            url_extension = "/namespaces/" + element->namespace_ + url_extension;
        }
    }

    std::unique_ptr<Element> element;
    std::string url_extension;
};

// Factory class
class ElementFactory {
public:
    static std::unique_ptr<Element> createElement(const std::string& jsonData) {
        json j = json::parse(jsonData);
        std::string kind = j.value("kind", "");
        
        if (kind == "Pod") {
            auto element = std::make_unique<Pod>();
            element->fromJson(j);
            return element;
        } else if (kind == "Service") {
            auto element = std::make_unique<Service>();
            element->fromJson(j);
            return element;
        } else if (kind == "Namespace") {
            auto element = std::make_unique<Namespace>();
            element->fromJson(j);
            return element;
        } else if (kind == "NetworkPolicy") {
            auto element = std::make_unique<NetworkPolicy>();
            element->fromJson(j);
            return element;
        }
        throw std::invalid_argument("Unknown kind: " + kind);
    }

    static std::vector<std::unique_ptr<Element>> createElementList(const std::string& jsonData) {
        std::vector<std::unique_ptr<Element>> elements;
        json j = json::parse(jsonData);
        for (const auto& item : j["items"]) {
            std::string kind = item.value("kind", "");
            if (kind == "Pod") {
                auto element = std::make_unique<Pod>();
                element->fromJson(item);
                elements.push_back(std::move(element));
            } else if (kind == "Service") {
                auto element = std::make_unique<Service>();
                element->fromJson(item);
                elements.push_back(std::move(element));
            } else if (kind == "Namespace") {
                auto element = std::make_unique<Namespace>();
                element->fromJson(item);
                elements.push_back(std::move(element));
            } else if (kind == "NetworkPolicy") {
                auto element = std::make_unique<NetworkPolicy>();
                element->fromJson(item);
                elements.push_back(std::move(element));
            }
        }
        return elements;
    }

    static Payload createPayload(const std::string& jsonData) {
        auto element = createElement(jsonData);
        return Payload(std::move(element));
    }
};
