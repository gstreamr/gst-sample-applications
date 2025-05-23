# Sample applications for Qualcomm® Linux platforms

Sample applications for Qualcomm® Linux repository provide sample applications to demonstrate the capability of Qualcomm Linux platforms.

## Build Sample Applications
### Generate the eSDK containing the cross-compiler tool chain

Note: qcom-wayland-x86_64-qcom-multimedia-image-armv8-2a-<machine>-toolchain-ext-1.3-ver.1.1 is required to build sample applications.

Follow these [build procedures](https://github.com/quic-yocto/meta-qcom-qim-product-sdk) to build the Qualcomm Intelligent Multimedia Product SDK (QIMP SDK).

Run the following command from the build directory to generate the eSDK:

```console
bitbake -c populate_sdk_ext qcom-multimedia-image
```

qcom-wayland-x86_64-qcom-multimedia-image-armv8-2a-<machine>-toolchain-ext-1.3-ver.1.1 is generated at <WORKSPACE DIR>/build-base/tmp-glibc/deploy/sdk.

Extract the toolchain using 
```console
./qcom-wayland-x86_64-qcom-multimedia-image-armv8-2a-<machine>-toolchain-ext-1.3-ver.1.1

**## Build Standalone Sample App using makefile.**

Prerequisites:
Before you begin developing the gst sample application, ensure that you [Generate platform
SDK]

1. Run the following command to go to the directory where the platform SDK was installed.
cd <installation directory of platfom SDK>
2. Run the following command to set up the source environment:
   source environment-setup-armv8-2a-qcom-linux

3. git clone https://github.com/gstreamr/gst-sample-applications
4. cd gst-sample-application
5. export SDKTARGETSYSROOT=<path to installation directory of platfom SDK>/tmp/sysroots
   e.g.: export SDKTARGETSYSROOT=/local/mnt/workspace/Platform_eSDK/tmp/sysroots

6. export MACHINE=<machine>
   e.g export MACHINE=qcs615-adp-air

7. export GST_APP_NAME=<appname> 
   e.g.: export GST_APP_NAME=gst-test-app
8. Do run make
9. Application executable will be generated and push the bin to target device and validate the usecase.
