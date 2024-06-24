int main() {
    std::string podJson = R"({
        "kind": "Pod",
        "metadata": {
            "name": "mypod",
            "namespace": "default",
            "creationTimestamp": "2023-04-01T12:34:56Z",
            "labels": {
                "app": "myapp"
            },
            "annotations": {
                "description": "my pod"
            }
        }
    })";

    std::string serviceJson = R"({
        "kind": "Service",
        "metadata": {
            "name": "myservice",
            "namespace": "default",
            "creationTimestamp": "2023-04-01T12:34:56Z",
            "labels": {
                "app": "myserviceapp"
            },
            "annotations": {
                "description": "my service"
            }
        }
    })";

    std::string podListJson = R"({
        "items": [
            {
                "kind": "Pod",
                "metadata": {
                    "name": "mypod1",
                    "namespace": "default",
                    "creationTimestamp": "2023-04-01T12:34:56Z",
                    "labels": {
                        "app": "myapp1"
                    },
                    "annotations": {
                        "description": "my pod 1"
                    }
                }
            },
            {
                "kind": "Pod",
                "metadata": {
                    "name": "mypod2",
                    "namespace": "default",
                    "creationTimestamp": "2023-04-01T12:35:56Z",
                    "labels": {
                        "app": "myapp2"
                    },
                    "annotations": {
                        "description": "my pod 2"
                    }
                }
            }
        ]
    })";

    try {
        auto pod = ElementFactory::createElement(podJson);
        auto service = ElementFactory::createElement(serviceJson);
        auto podList = ElementFactory::createElementList(podListJson);

        // Print information about the elements
        pod->printInfo();
        service->printInfo();
        for (const auto& p : podList) {
            p->printInfo();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
