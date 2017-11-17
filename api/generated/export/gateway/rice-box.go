package exportGwService

import (
	"github.com/GeertJohan/go.rice/embedded"
	"time"
)

func init() {

	// define files
	file2 := &embedded.EmbeddedFile{
		Filename:    "export.swagger.json",
		FileModTime: time.Unix(1510213563, 0),
		Content:     string("{\n  \"swagger\": \"2.0\",\n  \"info\": {\n    \"title\": \"Service name\",\n    \"version\": \"version not set\"\n  },\n  \"schemes\": [\n    \"http\",\n    \"https\"\n  ],\n  \"consumes\": [\n    \"application/json\"\n  ],\n  \"produces\": [\n    \"application/json\"\n  ],\n  \"paths\": {\n    \"/{O.Tenant}/exportPolicy\": {\n      \"post\": {\n        \"operationId\": \"AutoAddExportPolicy\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/exportExportPolicy\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Tenant\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"body\",\n            \"in\": \"body\",\n            \"required\": true,\n            \"schema\": {\n              \"$ref\": \"#/definitions/exportExportPolicy\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"ExportPolicyV1\"\n        ]\n      }\n    },\n    \"/{O.Tenant}/exportPolicy/{O.Name}\": {\n      \"get\": {\n        \"operationId\": \"AutoGetExportPolicy\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/exportExportPolicy\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Tenant\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.Kind\",\n            \"description\": \"Kind represents the type of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"T.APIVersion\",\n            \"description\": \"APIVersion defines the version of the API object.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Namespace\",\n            \"description\": \"Namespace of the object, for scoped objects.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.ResourceVersion\",\n            \"description\": \"Resource version in the object store. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.UUID\",\n            \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.CreationTime.time\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\",\n            \"format\": \"date-time\"\n          },\n          {\n            \"name\": \"O.ModTime.time\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\",\n            \"format\": \"date-time\"\n          },\n          {\n            \"name\": \"Spec.ExportInterval\",\n            \"description\": \"UserName is the login id to be used towards the external entity.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.Format\",\n            \"description\": \"Password is one time specified, not visibile on read operations\\nOnly valid when UserName is defined\\nTBD: need to add (venice.secret) = \\\"true\\\" support for this.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.CollectorIpAddress\",\n            \"description\": \"External entity supports bearer tokens for authentication and authorization\\nToken refresh is not supported using OAuth2\\nTBD: need to add (venice.secret) = \\\"true\\\" support for this.\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.CollectorPort\",\n            \"description\": \"CertData holds PEM-encoded bytes (typically read from a client certificate file).\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.Credentials.UserName\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.Credentials.Password\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.Credentials.BearerToken\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"Spec.Credentials.CertData\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\",\n            \"format\": \"byte\"\n          },\n          {\n            \"name\": \"Spec.Credentials.KeyData\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\",\n            \"format\": \"byte\"\n          },\n          {\n            \"name\": \"Spec.Credentials.CaData\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"string\",\n            \"format\": \"byte\"\n          },\n          {\n            \"name\": \"Status.MonitoringPolicies\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"array\",\n            \"items\": {\n              \"type\": \"string\"\n            }\n          },\n          {\n            \"name\": \"Status.EventPolicies\",\n            \"in\": \"query\",\n            \"required\": false,\n            \"type\": \"array\",\n            \"items\": {\n              \"type\": \"string\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"ExportPolicyV1\"\n        ]\n      },\n      \"delete\": {\n        \"operationId\": \"AutoDeleteExportPolicy\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/exportExportPolicy\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Tenant\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          }\n        ],\n        \"tags\": [\n          \"ExportPolicyV1\"\n        ]\n      },\n      \"put\": {\n        \"operationId\": \"AutoUpdateExportPolicy\",\n        \"responses\": {\n          \"200\": {\n            \"description\": \"\",\n            \"schema\": {\n              \"$ref\": \"#/definitions/exportExportPolicy\"\n            }\n          }\n        },\n        \"parameters\": [\n          {\n            \"name\": \"O.Tenant\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"O.Name\",\n            \"in\": \"path\",\n            \"required\": true,\n            \"type\": \"string\"\n          },\n          {\n            \"name\": \"body\",\n            \"in\": \"body\",\n            \"required\": true,\n            \"schema\": {\n              \"$ref\": \"#/definitions/exportExportPolicy\"\n            }\n          }\n        ],\n        \"tags\": [\n          \"ExportPolicyV1\"\n        ]\n      }\n    }\n  },\n  \"definitions\": {\n    \"apiListMeta\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"ResourceVersion\": {\n          \"type\": \"string\",\n          \"description\": \"Resource version of object store at the time of list generation.\"\n        }\n      },\n      \"description\": \"ListMeta contains the metadata for list of objects.\"\n    },\n    \"apiListWatchOptions\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"O\": {\n          \"$ref\": \"#/definitions/apiObjectMeta\"\n        },\n        \"LabelSelector\": {\n          \"type\": \"string\"\n        },\n        \"FieldSelector\": {\n          \"type\": \"string\"\n        },\n        \"PrefixWatch\": {\n          \"type\": \"boolean\",\n          \"format\": \"boolean\"\n        }\n      }\n    },\n    \"apiObjectMeta\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Name\": {\n          \"type\": \"string\",\n          \"description\": \"Name of the object, unique within a Namespace for scoped objects.\"\n        },\n        \"Tenant\": {\n          \"type\": \"string\",\n          \"description\": \"Tenant is global namespace isolation for various objects. This can be automatically\\nfilled in many cases based on the tenant a user, who created the object, belongs go.\"\n        },\n        \"Namespace\": {\n          \"type\": \"string\",\n          \"description\": \"Namespace of the object, for scoped objects.\"\n        },\n        \"ResourceVersion\": {\n          \"type\": \"string\",\n          \"description\": \"Resource version in the object store. This can only be set by the server.\"\n        },\n        \"UUID\": {\n          \"type\": \"string\",\n          \"description\": \"UUID is the unique identifier for the object. This can only be set by the server.\"\n        },\n        \"Labels\": {\n          \"type\": \"object\",\n          \"additionalProperties\": {\n            \"type\": \"string\"\n          },\n          \"description\": \"Labels are arbitrary (key,value) pairs associated with any object.\"\n        },\n        \"CreationTime\": {\n          \"$ref\": \"#/definitions/apiTimestamp\",\n          \"title\": \"CreationTime is the creation time of Object\"\n        },\n        \"ModTime\": {\n          \"$ref\": \"#/definitions/apiTimestamp\",\n          \"title\": \"ModTime is the Last Modification time of Object\"\n        }\n      },\n      \"description\": \"ObjectMeta contains metadata that all objects stored in kvstore must have.\"\n    },\n    \"apiTimestamp\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"time\": {\n          \"type\": \"string\",\n          \"format\": \"date-time\"\n        }\n      }\n    },\n    \"apiTypeMeta\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Kind\": {\n          \"type\": \"string\",\n          \"description\": \"Kind represents the type of the API object.\"\n        },\n        \"APIVersion\": {\n          \"type\": \"string\",\n          \"description\": \"APIVersion defines the version of the API object.\"\n        }\n      },\n      \"description\": \"TypeMeta contains the metadata about kind and version for all API objects.\"\n    },\n    \"exportAutoMsgExportPolicyWatchHelper\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"Type\": {\n          \"type\": \"string\",\n          \"title\": \"Export Interval defines how often to push the records to an external or internal collector\\nThe value is specified as a string format to be '10s', '20m', '20mins', '10secs', '10seconds'\"\n        },\n        \"Object\": {\n          \"$ref\": \"#/definitions/exportExportPolicy\",\n          \"title\": \"Area describes an area for which the monitoring policy is specified\"\n        }\n      }\n    },\n    \"exportExportPolicy\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\",\n          \"title\": \"list of monitoring policies that refer to this collection policy\"\n        },\n        \"O\": {\n          \"$ref\": \"#/definitions/apiObjectMeta\",\n          \"title\": \"list of event policies that refer to this collection policy\"\n        },\n        \"Spec\": {\n          \"$ref\": \"#/definitions/exportExportPolicySpec\"\n        },\n        \"Status\": {\n          \"$ref\": \"#/definitions/exportExportPolicyStatus\"\n        }\n      }\n    },\n    \"exportExportPolicyList\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"T\": {\n          \"$ref\": \"#/definitions/apiTypeMeta\"\n        },\n        \"ListMeta\": {\n          \"$ref\": \"#/definitions/apiListMeta\"\n        },\n        \"Items\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"$ref\": \"#/definitions/exportExportPolicy\"\n          },\n          \"description\": \"Spec contains the configuration of the export policy.\"\n        }\n      }\n    },\n    \"exportExportPolicySpec\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"ExportInterval\": {\n          \"type\": \"string\",\n          \"title\": \"UserName is the login id to be used towards the external entity\"\n        },\n        \"Format\": {\n          \"type\": \"string\",\n          \"title\": \"Password is one time specified, not visibile on read operations\\nOnly valid when UserName is defined\\nTBD: need to add (venice.secret) = \\\"true\\\" support for this\"\n        },\n        \"CollectorIpAddress\": {\n          \"type\": \"string\",\n          \"title\": \"External entity supports bearer tokens for authentication and authorization\\nToken refresh is not supported using OAuth2\\nTBD: need to add (venice.secret) = \\\"true\\\" support for this\"\n        },\n        \"CollectorPort\": {\n          \"type\": \"string\",\n          \"description\": \"CertData holds PEM-encoded bytes (typically read from a client certificate file).\"\n        },\n        \"Credentials\": {\n          \"$ref\": \"#/definitions/exportExternalCred\",\n          \"title\": \"KeyData holds PEM-encoded bytes (typically read from a client certificate key file).\\nTBD: need to add (venice.secret) = \\\"true\\\" support for this\"\n        }\n      },\n      \"title\": \"------------------------ ExternalCred Object ----------------------------\\nExternalCred defines credentails required to access an external entity, such as\\na stats collector, compute orchestration entity, or a syslog server.\\nExternal entity may support a variety of methods, like username/password,\\nTLS Client authentication, or Bearer Token based authentication. User is\\nexpected to configure one of the methods\"\n    },\n    \"exportExportPolicyStatus\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"MonitoringPolicies\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"type\": \"string\"\n          }\n        },\n        \"EventPolicies\": {\n          \"type\": \"array\",\n          \"items\": {\n            \"type\": \"string\"\n          }\n        }\n      }\n    },\n    \"exportExternalCred\": {\n      \"type\": \"object\",\n      \"properties\": {\n        \"UserName\": {\n          \"type\": \"string\"\n        },\n        \"Password\": {\n          \"type\": \"string\"\n        },\n        \"BearerToken\": {\n          \"type\": \"string\"\n        },\n        \"CertData\": {\n          \"type\": \"string\",\n          \"format\": \"byte\"\n        },\n        \"KeyData\": {\n          \"type\": \"string\",\n          \"format\": \"byte\"\n        },\n        \"CaData\": {\n          \"type\": \"string\",\n          \"format\": \"byte\"\n        }\n      }\n    }\n  }\n}\n"),
	}

	// define dirs
	dir1 := &embedded.EmbeddedDir{
		Filename:   "",
		DirModTime: time.Unix(1510000343, 0),
		ChildFiles: []*embedded.EmbeddedFile{
			file2, // "export.swagger.json"

		},
	}

	// link ChildDirs
	dir1.ChildDirs = []*embedded.EmbeddedDir{}

	// register embeddedBox
	embedded.RegisterEmbeddedBox(`../../../../../sw/api/generated/export/swagger`, &embedded.EmbeddedBox{
		Name: `../../../../../sw/api/generated/export/swagger`,
		Time: time.Unix(1510000343, 0),
		Dirs: map[string]*embedded.EmbeddedDir{
			"": dir1,
		},
		Files: map[string]*embedded.EmbeddedFile{
			"export.swagger.json": file2,
		},
	})
}
