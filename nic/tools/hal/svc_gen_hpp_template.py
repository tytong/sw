//:: import os
//:: import importlib
//:: import sys
//:: hdr_def = fileName.replace('_pb2.py', '').upper()
// {C} Copyright 2017 Pensando Systems Inc. All rights reserved

#ifndef __HAL_SVC_${hdr_def}_HPP__
#define __HAL_SVC_${hdr_def}_HPP__

#include "nic/include/base.h"
#include "grpc++/grpc++.h"
#include "nic/gen/proto/hal/types.pb.h" 
//:: if 'WS_TOP' not in os.environ:
//::     # This should have been set, before invoking the template.
//::     assert False
//:: #endif
//:: # Add the proto directory to the path.
//:: ws_top = os.environ['WS_TOP']
//:: fullpath = ws_top + '/nic/gen/proto/hal/'
//:: sys.path.insert(0, fullpath)
//::
//:: def convert_to_snake_case(name, fileName):
//::     import re
//::     import os
//::     global ws_top
//::     s1 = re.sub('([A-Z])(.)', r'\1_\2', name[::-1], 1).lower()
//::     s1 = s1[::-1]
//::     s2 = re.sub('([a-z0-9])([A-Z])', r'\1_\2', name).lower()
//::     hal_src_path = ws_top + '/nic/hal/src/'
//::     for file_name in os.listdir(hal_src_path):
//::         if file_name.endswith('.hpp') and fileName.replace('_pb2.py', '') in file_name:
//::             if s1 in open(os.path.join(hal_src_path, file_name)).read():
//::                 return s1
//::             #endif
//::             if s2 in open(os.path.join(hal_src_path, file_name)).read():
//::                 return s2
//::             #endif
//::             print ('******************************* could not find method ' + s1 + ' or ' + s2 + ' in ' + file_name + ' proto: ' + fileName)
//::         #endif
//::     #endfor
//:: #enddef
//::
//:: fileModule = importlib.import_module(fileName[:-3])
//:: includeFileName = fileName[:-7]
#include "nic/gen/proto/hal/${includeFileName}.grpc.pb.h"
//::     # Remove the _pb2.py from file and store it for now.
//:: for service in fileModule.DESCRIPTOR.services_by_name.items():
//::     pkg = fileModule.DESCRIPTOR.package.lower()

using grpc::ServerContext;
using grpc::Status;
using ${pkg}::${service[0]};

class ${service[0]}ServiceImpl final : public ${service[0]}::Service {
public:
//::     for method in service[1].methods_by_name.items():
//::         hal_name = convert_to_snake_case(method[0], fileName)
//::         input_name = pkg+'::'+method[1].input_type.name
//::         output_name = pkg+'::'+method[1].output_type.name
    Status ${method[0]}(ServerContext *context,
                        const ${input_name} *req,
                        ${output_name} *rsp) override;

//::     #endfor
};

//:: #endfor

#endif   // __HAL_SVC_${hdr_def}_HPP__

