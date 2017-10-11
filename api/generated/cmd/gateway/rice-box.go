package cmdGwService

import (
	"github.com/GeertJohan/go.rice/embedded"
	"time"
)

func init() {

	// define files
	file2 := &embedded.EmbeddedFile{
		Filename:    "cmd.swagger.json",
		FileModTime: time.Unix(1507267888, 0),
		Content:     string("{\n  \"swagger\": \"2.0\",\n  \"info\": {\n    \"title\": \"Service name\",\n    \"version\": \"version not set\"\n  },\n  \"schemes\": [\n    \"http\",\n    \"https\"\n  ],\n  \"consumes\": [\n    \"application/json\"\n  ],\n  \"produces\": [\n    \"application/json\"\n  ],\n  \"paths\": {\n    \"/cluster\": {\n      \"get\": {\n        \"operationId\": \"AutoListCluster\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdClusterList\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"description\": \"Name of the object, unique within a Namespace for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Tenant\",\n            \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Namespace\",\n            \"description\": \"Namespace of the object, for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.ResourceVersion\",\n            \"description\": \"Resource version in the object store. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.UUID\",\n            \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"LabelSelector\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"FieldSelector\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"PrefixWatch\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"boolean\",\n            \"format\": \"boolean\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      }\n    },\n    \"/cluster/{O.Name}\": {\n      \"get\": {\n        \"operationId\": \"AutoGetCluster\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdCluster\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.Kind\",\n            \"description\": \"Kind represents the type of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.APIVersion\",\n            \"description\": \"APIVersion defines the version of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Tenant\",\n            \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Namespace\",\n            \"description\": \"Namespace of the object, for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.ResourceVersion\",\n            \"description\": \"Resource version in the object store. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.UUID\",\n            \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.QuorumNodes\",\n            \"description\": \"Current lifecycle phase of the node.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"array\",\n            \"items\": {\n              \"type\": \"string\"\n            }\n          },\n          {\n            \"name\": \"Spec.VirtualIP\",\n            \"description\": \"List of current node conditions.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.NTPServers\",\n            \"description\": \"Nics holds a list of Mac addresses each uniquely identifying\\na SmartNIC subsystem that is part of the Node.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"array\",\n            \"items\": {\n              \"type\": \"string\"\n            }\n          },\n          {\n            \"name\": \"Spec.DNSSubDomain\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.AutoAdmitNICs\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"boolean\",\n            \"format\": \"boolean\"\n          },\n          {\n            \"name\": \"Status.Leader\",\n            \"description\": \"Type indicates a certain node condition.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      },\n      \"delete\": {\n        \"operationId\": \"AutoDeleteCluster\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdCluster\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      },\n      \"put\": {\n        \"operationId\": \"AutoUpdateCluster\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdCluster\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"body\",\n            \"in\": \"body\",\n            \"required\": true,\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdCluster\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      }\n    },\n    \"/nodes\": {\n      \"get\": {\n        \"operationId\": \"AutoListNode\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdNodeList\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"description\": \"Name of the object, unique within a Namespace for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Tenant\",\n            \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Namespace\",\n            \"description\": \"Namespace of the object, for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.ResourceVersion\",\n            \"description\": \"Resource version in the object store. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.UUID\",\n            \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"LabelSelector\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"FieldSelector\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"PrefixWatch\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"boolean\",\n            \"format\": \"boolean\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      },\n      \"post\": {\n        \"operationId\": \"AutoAddNode\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdNode\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"body\",\n            \"in\": \"body\",\n            \"required\": true,\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdNode\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      }\n    },\n    \"/nodes/{O.Name}\": {\n      \"get\": {\n        \"operationId\": \"AutoGetNode\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdNode\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.Kind\",\n            \"description\": \"Kind represents the type of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.APIVersion\",\n            \"description\": \"APIVersion defines the version of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Tenant\",\n            \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Namespace\",\n            \"description\": \"Namespace of the object, for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.ResourceVersion\",\n            \"description\": \"Resource version in the object store. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.UUID\",\n            \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.Roles\",\n            \"description\": \"Type indicates a certain NIC condition.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"array\",\n            \"items\": {\n              \"type\": \"string\"\n            }\n          },\n          {\n            \"name\": \"Status.Phase\",\n            \"description\": \"Mac address of the Port, which is key identifier of the port.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Status.Nics\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"array\",\n            \"items\": {\n              \"type\": \"string\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      },\n      \"delete\": {\n        \"operationId\": \"AutoDeleteNode\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdNode\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      },\n      \"put\": {\n        \"operationId\": \"AutoUpdateNode\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdNode\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"body\",\n            \"in\": \"body\",\n            \"required\": true,\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdNode\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      }\n    },\n    \"/smartnics\": {\n      \"get\": {\n        \"operationId\": \"AutoListSmartNIC\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdSmartNICList\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"description\": \"Name of the object, unique within a Namespace for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Tenant\",\n            \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Namespace\",\n            \"description\": \"Namespace of the object, for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.ResourceVersion\",\n            \"description\": \"Resource version in the object store. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.UUID\",\n            \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"LabelSelector\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"FieldSelector\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"PrefixWatch\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"boolean\",\n            \"format\": \"boolean\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      }\n    },\n    \"/smartnics/{O.Name}\": {\n      \"get\": {\n        \"operationId\": \"AutoGetSmartNIC\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdSmartNIC\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.Kind\",\n            \"description\": \"Kind represents the type of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.APIVersion\",\n            \"description\": \"APIVersion defines the version of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Tenant\",\n            \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Namespace\",\n            \"description\": \"Namespace of the object, for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.ResourceVersion\",\n            \"description\": \"Resource version in the object store. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.UUID\",\n            \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.Phase\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Status.SerialNum\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Status.PrimaryMacAddress\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Status.NodeName\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      },\n      \"delete\": {\n        \"operationId\": \"AutoDeleteSmartNIC\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdSmartNIC\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      },\n      \"put\": {\n        \"operationId\": \"AutoUpdateSmartNIC\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdSmartNIC\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"body\",\n            \"in\": \"body\",\n            \"required\": true,\n            \"schema\": {\n              \"$ref\": \"#/definitions/cmdSmartNIC\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"CmdV1\"\n        ]\n      }\n    }\n  },\n  \"definitions\": {\n    \"apiListMeta\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"ResourceVersion\": {\n          \"type\": \"string\",\n          \"description\": \"Resource version of object store at the time of list generation.\"\n        }\n      },\n      \"description\": \"ListMeta contains the metadata for list of objects.\"\n    },\n    \"apiListWatchOptions\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"O\": {\n          \"$ref\": \"#/definitions/apiObjectMeta\"\n        },\n        \"LabelSelector\": {\n          \"type\": \"string\"\n        },\n        \"FieldSelector\": {\n          \"type\": \"string\"\n        },\n        \"PrefixWatch\": {\n          \"type\": \"boolean\",\n          \"format\": \"boolean\"\n        }\n      }\n    },\n    \"apiObjectMeta\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Name\": {\n          \"type\": \"string\",\n          \"description\": \"Name of the object, unique within a Namespace for scoped objects.\"\n        },\n        \"Tenant\": {\n          \"type\": \"string\",\n          \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\"\n        },\n        \"Namespace\": {\n          \"type\": \"string\",\n          \"description\": \"Namespace of the object, for scoped objects.\"\n        },\n        \"ResourceVersion\": {\n          \"type\": \"string\",\n          \"description\": \"Resource version in the object store. This can only be set by the server.\"\n        },\n        \"UUID\": {\n          \"type\": \"string\",\n          \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\"\n        },\n        \"Labels\": {\n          \"type\": \"object\",\n          \"additionalProperties\": {\n            \"type\": \"string\"\n          },\n          \"description\": \"Labels are arbitrary (key,value) pairs associated with any object.\"\n        }\n      },\n      \"description\": \"ObjectMeta contains metadata that all objects stored in kvstore must have.\"\n    },\n    \"apiTypeMeta\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Kind\": {\n          \"type\": \"string\",\n          \"description\": \"Kind represents the type of the API object.\"\n        },\n        \"APIVersion\": {\n          \"type\": \"string\",\n          \"description\": \"APIVersion defines the version of the API object.\"\n        }\n      },\n      \"description\": \"TypeMeta contains the metadata about kind and version for all API objects.\"\n    },\n    \"cmdAutoMsgClusterWatchHelper\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Type\": {\n          \"type\": \"string\"\n        },\n        \"Object\": {\n          \"$ref\": \"#/definitions/cmdCluster\"\n        }\n      },\n      \"description\": \"Cluster represents a full cluster venice and workload nodes\\n\\nEntity responsible \\u0026 scenarios involved in managing this object:\\n\\n     Create:\\n         o NetOps-admin\\n             - initial cluster creation\\n     Modify:\\n         o NetOps-admin\\n             - update spec attributes\\n         o CMD\\n             - update status attributes\\n     Delete:\\n         o NetOps-admin\\n             - TBD\",\n      \"title\": \"--------------------------------- CLUSTER ---------------------------------------------\"\n    },\n    \"cmdAutoMsgNodeWatchHelper\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Type\": {\n          \"type\": \"string\",\n          \"description\": \"QuorumNodes contains the list of hostnames for nodes configured to be quorum\\nnodes in the cluster.\"\n        },\n        \"Object\": {\n          \"$ref\": \"#/definitions/cmdNode\",\n          \"description\": \"VirtualIP is the IP address for managing the cluster. It will be hosted by\\nthe winner of election between quorum nodes.\"\n        }\n      },\n      \"description\": \"ClusterSpec contains the configuration of the cluster.\"\n    },\n    \"cmdAutoMsgSmartNICWatchHelper\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Type\": {\n          \"type\": \"string\",\n          \"description\": \"Leader contains the node name of the cluster leader.\"\n        },\n        \"Object\": {\n          \"$ref\": \"#/definitions/cmdSmartNIC\"\n        }\n      },\n      \"description\": \"ClusterStatus contains the current state of the Cluster.\"\n    },\n    \"cmdCluster\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\"\n        },\n        \"O\": {\n          \"$ref\": \"#/definitions/apiObjectMeta\"\n        },\n        \"Spec\": {\n          \"$ref\": \"#/definitions/cmdClusterSpec\",\n          \"description\": \"Spec contains the configuration of the node.\"\n        },\n        \"Status\": {\n          \"$ref\": \"#/definitions/cmdClusterStatus\",\n          \"description\": \"Status contains the current state of the node.\"\n        }\n      },\n      \"description\": \"Node is representation of a single node in the system.\\n\\nEntity responsible \\u0026 scenarios involved in managing this object:\\n\\n     Create:\\n         o NetOps-admin\\n             - initial node creation for Baremetal node\\n         o CMD\\n             - auto created when Hypervisor Node and NIC are\\n               discovered via Orchestrator interface, NIC registration\\n     Modify:\\n         o NetOps-admin\\n             - update spec for Baremetal node\\n         o CMD\\n             - update spec attributes for Hypervisor node\\n             - update status attributes\\n     Delete:\\n         o NetOps-admin\\n             - when Baremetal node is decommissioned\\n         o CMD\\n             - TBD\",\n      \"title\": \"---------------------------------- NODE -------------------------------------------\"\n    },\n    \"cmdClusterList\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\",\n          \"description\": \"Roles is of list of roles a node can be configured with.\"\n        },\n        \"ListMeta\": {\n          \"$ref\": \"#/definitions/apiListMeta\"\n        },\n        \"Items\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdCluster\"\n          }\n        }\n      },\n      \"description\": \"NodeSpec contains the configuration of the node.\"\n    },\n    \"cmdClusterSpec\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"QuorumNodes\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"type\": \"string\"\n          },\n          \"description\": \"Current lifecycle phase of the node.\"\n        },\n        \"VirtualIP\": {\n          \"type\": \"string\",\n          \"title\": \"List of current node conditions\"\n        },\n        \"NTPServers\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"type\": \"string\"\n          },\n          \"title\": \"Nics holds a list of Mac addresses each uniquely identifying\\na SmartNIC subsystem that is part of the Node\"\n        },\n        \"DNSSubDomain\": {\n          \"type\": \"string\"\n        },\n        \"AutoAdmitNICs\": {\n          \"type\": \"boolean\",\n          \"format\": \"boolean\"\n        }\n      },\n      \"description\": \"NodeStatus contains the current state of the node.\"\n    },\n    \"cmdClusterStatus\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Leader\": {\n          \"type\": \"string\",\n          \"title\": \"Type indicates a certain node condition\"\n        }\n      },\n      \"description\": \"NodeCondition describes the state of a Node at a certain point.\"\n    },\n    \"cmdNode\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\"\n        },\n        \"O\": {\n          \"$ref\": \"#/definitions/apiObjectMeta\",\n          \"title\": \"Object name is Serial-Number of the SmartNIC\"\n        },\n        \"Spec\": {\n          \"$ref\": \"#/definitions/cmdNodeSpec\",\n          \"description\": \"SmartNICSpec contains the configuration of the network adapter.\"\n        },\n        \"Status\": {\n          \"$ref\": \"#/definitions/cmdNodeStatus\",\n          \"description\": \"SmartNICStatus contains the current state of the network adapter.\"\n        }\n      },\n      \"description\": \"SmartNIC represents the Naples I/O subsystem\\n\\nEntity responsible \\u0026 scenarios involved in managing this object:\\n\\n     Create:\\n         o CMD\\n             - created as part of NIC registration, Admittance\\n     Modify:\\n         o CMD\\n             - update spec attributes\\n             - update status attributes\\n     Delete:\\n         o CMD\\n             - aging out stale or rejected NICs (TBD)\\n         o NetOps, SecOps\\n             - Decomission a NIC (TBD)\",\n      \"title\": \"------------------------------------ SMART NIC  -------------------------------------------\"\n    },\n    \"cmdNodeCondition\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Type\": {\n          \"type\": \"string\",\n          \"description\": \"Current phase of the SmartNIC.\\nWhen auto-admission is enabled, Phase will be set to NIC_ADMITTED\\nby CMD for validated NICs.\\nWhen auto-admission is not enabled, Phase will be set to NIC_PENDING\\nby CMD for validated NICs since it requires manual approval.\\nTo admit the NIC as a part of manual admission, user is expected to\\nset the Phase to NIC_ADMITTED for the NICs that are in NIC_PENDING\\nstate. Note : Whitelist mode is not supported yet.\"\n        },\n        \"Status\": {\n          \"type\": \"string\",\n          \"title\": \"Ports holds a list of Port Specs\"\n        },\n        \"LastTransitionTime\": {\n          \"type\": \"string\",\n          \"format\": \"int64\"\n        },\n        \"Reason\": {\n          \"type\": \"string\"\n        },\n        \"Message\": {\n          \"type\": \"string\"\n        }\n      },\n      \"title\": \"SmartNICSpec contains configuration of the SmartNIC (Naples I/O subsystem)\"\n    },\n    \"cmdNodeList\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\",\n          \"title\": \"List of current NIC conditions\"\n        },\n        \"ListMeta\": {\n          \"$ref\": \"#/definitions/apiListMeta\",\n          \"title\": \"Serial number\"\n        },\n        \"Items\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdNode\"\n          },\n          \"title\": \"Primary MAC address, which is MAC address of the primary PF exposed by SmartNIC\"\n        }\n      },\n      \"title\": \"SmartNICStatus contains current status of a SmartNIC\"\n    },\n    \"cmdNodeSpec\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Roles\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"type\": \"string\"\n          },\n          \"title\": \"Type indicates a certain NIC condition\"\n        }\n      },\n      \"description\": \"SmartNICCondition describes the state of a SmartNIC at a certain point.\"\n    },\n    \"cmdNodeStatus\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Phase\": {\n          \"type\": \"string\",\n          \"title\": \"Mac address of the Port, which is key identifier of the port\"\n        },\n        \"Conditions\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdNodeCondition\"\n          }\n        },\n        \"Nics\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"type\": \"string\"\n          }\n        }\n      },\n      \"title\": \"PortSpec contains configuration of a port in SmartNIC\"\n    },\n    \"cmdPortCondition\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Type\": {\n          \"type\": \"string\",\n          \"title\": \"Mac address of the Port, which is key identifier of the port\"\n        },\n        \"Status\": {\n          \"type\": \"string\",\n          \"title\": \"LinkSpeed of the Port\"\n        },\n        \"LastTransitionTime\": {\n          \"type\": \"string\",\n          \"format\": \"int64\",\n          \"title\": \"List of current Port conditions\"\n        },\n        \"Reason\": {\n          \"type\": \"string\"\n        },\n        \"Message\": {\n          \"type\": \"string\"\n        }\n      },\n      \"title\": \"PortStatus contains current status of a Port\"\n    },\n    \"cmdPortSpec\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"MacAddress\": {\n          \"type\": \"string\",\n          \"title\": \"Type indicates a certain Port condition\"\n        }\n      },\n      \"description\": \"PortCondition describes the state of a Port at a certain point.\"\n    },\n    \"cmdPortStatus\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"MacAddress\": {\n          \"type\": \"string\"\n        },\n        \"LinkSpeed\": {\n          \"type\": \"string\"\n        },\n        \"Conditions\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdPortCondition\"\n          }\n        }\n      }\n    },\n    \"cmdSmartNIC\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\"\n        },\n        \"O\": {\n          \"$ref\": \"#/definitions/apiObjectMeta\"\n        },\n        \"Spec\": {\n          \"$ref\": \"#/definitions/cmdSmartNICSpec\"\n        },\n        \"Status\": {\n          \"$ref\": \"#/definitions/cmdSmartNICStatus\"\n        }\n      }\n    },\n    \"cmdSmartNICCondition\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Type\": {\n          \"type\": \"string\"\n        },\n        \"Status\": {\n          \"type\": \"string\"\n        },\n        \"LastTransitionTime\": {\n          \"type\": \"string\",\n          \"format\": \"int64\"\n        },\n        \"Reason\": {\n          \"type\": \"string\"\n        },\n        \"Message\": {\n          \"type\": \"string\"\n        }\n      }\n    },\n    \"cmdSmartNICList\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\"\n        },\n        \"ListMeta\": {\n          \"$ref\": \"#/definitions/apiListMeta\"\n        },\n        \"Items\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdSmartNIC\"\n          }\n        }\n      }\n    },\n    \"cmdSmartNICSpec\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Phase\": {\n          \"type\": \"string\"\n        },\n        \"Ports\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdPortSpec\"\n          }\n        }\n      }\n    },\n    \"cmdSmartNICStatus\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Conditions\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdSmartNICCondition\"\n          }\n        },\n        \"SerialNum\": {\n          \"type\": \"string\"\n        },\n        \"PrimaryMacAddress\": {\n          \"type\": \"string\"\n        },\n        \"NodeName\": {\n          \"type\": \"string\"\n        },\n        \"Ports\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/cmdPortStatus\"\n          }\n        }\n      }\n    }\n  }\n}\n"),
	}

	// define dirs
	dir1 := &embedded.EmbeddedDir{
		Filename:   "",
		DirModTime: time.Unix(1507246998, 0),
		ChildFiles: []*embedded.EmbeddedFile{
			file2, // "cmd.swagger.json"

		},
	}

	// link ChildDirs
	dir1.ChildDirs = []*embedded.EmbeddedDir{}

	// register embeddedBox
	embedded.RegisterEmbeddedBox(`../../../../../sw/api/generated/cmd/swagger`, &embedded.EmbeddedBox{
		Name: `../../../../../sw/api/generated/cmd/swagger`,
		Time: time.Unix(1507246998, 0),
		Dirs: map[string]*embedded.EmbeddedDir{
			"": dir1,
		},
		Files: map[string]*embedded.EmbeddedFile{
			"cmd.swagger.json": file2,
		},
	})
}
