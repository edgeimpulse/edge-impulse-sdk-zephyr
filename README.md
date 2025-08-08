# Edge Impulse SDK Zephyr module

Portable library for digital signal processing and machine learning inferencing. This repository contains the device implementation in C++ for both processing and learning blocks in Edge Impulse.

## Import the Edge Impulse SDK
There are different ways to import the [Edge Impulse SDK Zepyhr module](https://github.com/edgeimpulse/edge-impulse-sdk-zephyr):
1. Update the `west.yml` of your Zephyr repo adding the lines below for the Edge Impulse SDK then call `west update` to downlaod the SDK into your Zepyr repo.

2. If your application is using a local manifest `west.yml`, update it adding the lines below.

Here's the lines to add the EI-SDK:
```
    - name: edge-impulse-sdk-zephyr
      path: modules/edge-impulse-sdk-zephyr
      revision: ${SDK_VERSION}
      url: https://github.com/edgeimpulse/edge-impulse-sdk-zephyr
```

The ${SDK_VERSION} depends on the model, and it needs to be the same version to be sure to avoid compilation error.
The version can be found in the `west.yml` that is deployed with the Zephyr model deployment. 

Check the [Zephyr module documentation](https://docs.zephyrproject.org/latest/develop/modules.html) for best practice.