Commsignia V2X Remote C SDK
======================================

Commsignia V2X Remote C SDK is a library to access low level
functionality of the Commsignia V2X stack. It handles connecting to the stack
through a proprietary TCP protocol and sends RPC commands.

# Components

- SDK includes in `include`
- SDK libraries in `lib`
- API reference in `doc`
- examples in `examples`

# Building the examples

[The SDK examples](@ref examples)  can be built with _CMake_ or using the included _Makefile_.

## Building with CMake

Use the following commands to build the examples:
```
# build_dir: Build directory
# sdk_dir: SDK root directory
cd $build_dir
cmake $sdk_dir
make
```

For example building the sdk in `upl_sdk` on the path `upl_sdk/build`:
```
cd upl_sdk/build
cmake ..
make
```

## Building with Makefile

Use the following commands to build the examples:
```
# sdk_dir: SDK root directory
cd $sdk_dir/examples
make
```

# Including in CMake projects

The `cms_v2x_sdk.cmake` file defines _CMake interface libraries_
to use the SDK in your own project. All you have to do is
- set the `CMS_V2X_SDK_DIR` CMake variable to the SDK root path
- include the `cms_v2x_sdk.cmake` file in your `CMakeLists.txt`
- link your target against
  - `cms_v2x_sdk-stat` for using the static lib
  - `cms_v2x_sdk-shr` for using the shared lib

E.g.
```
set (CMS_V2X_SDK_DIR ${PROJECT_SOURCE_DIR}/sdk)
include (${CMS_V2X_SDK_DIR}/cms_v2x_sdk.cmake)
target_link_libraries (my_target PRIVATE cms_v2x_sdk-stat)
```

# Copyright

(C) Commsignia Ltd. - All Rights Reserved.

Unauthorised copying via any medium is strictly prohibited.
Proprietary and confidential.
