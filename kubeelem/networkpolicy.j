{
  "apiVersion": "networking.k8s.io/v1",
  "kind": "NetworkPolicy",
  "metadata": {
    "name": "allow-privileged-to-default",
    "namespace": "default"
  },
  "spec": {
    "podSelector": {
      "matchLabels": {
        "app": "my-app"
      }
    },
    "policyTypes": ["Ingress"],
    "ingress": [
      {
        "from": [
          {
            "namespaceSelector": {
              "matchLabels": {
                "name": "privileged-namespace"
              }
            }
          }
        ]
      }
    ]
  }
}
