#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <memory>

using json = nlohmann::json;

// Base class
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
