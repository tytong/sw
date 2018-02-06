ProtoObject: acl_pb2
Service: Acl
enabled : True
graphEnabled : True
objects:
    - object:
        name : Acl
        key_handle : AclKeyHandle
        ignore:
        create:
            api      : AclCreate
            request  : AclRequestMsg
            response : AclResponseMsg
            pre_cb   : callback://acl/PreCreateCb
            post_cb  : None
        update:
            api      : AclUpdate
            request  : AclRequestMsg
            response : AclResponseMsg
            pre_cb   : callback://acl/PreCreateCb
            post_cb  : None
        delete:
            api      : AclDelete
            request  : AclDeleteRequestMsg
            response : AclDeleteResponseMsg
            pre_cb   : None
            post_cb  : None
        get:
            api      : AclGet
            request  : AclGetRequestMsg
            response : AclGetResponseMsg
            pre_cb   : None
            post_cb  : None

