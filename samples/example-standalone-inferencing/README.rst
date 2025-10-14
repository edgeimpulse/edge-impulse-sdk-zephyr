.. zephyr:code-sample:: example-standalone-inferencing-zephyr
   :name: Edge Impulse Example: standalone inferencing (Zephyr module)

   Runs inference on a static sample using the Edge Impulse SDK Zephyr module.

Overview
********

This runs an exported impulse on most Zephyr development boards using Edge Impulse SDK Zephyr module and the Zephyr model deployment.

This project differs from `_example-standalone-inferencing-zephyr`_ because it uses the `Edge Impulse SDK Zephyr module`_ and the Zephyr library deployment for the model instead of copying the C++ library export.

Initialize this repo
********************

To initialize this repo, run:
.. code-block:: console
    west init -m https://github.com/edgeimpulse/example-standalone-inferencing-zephyr-module
    cd example-standalone-inferencing-zephyr-module
    west update

`zephyr` and the latest available `Edge Impulse SDK` will be pulled.

## Import the Edge Impulse SDK
There are different ways to import the `Edge Impulse SDK Zephyr module`_ to be used with this project:
1. Update the `west.yml` of your Zephyr repo adding the lines below for the SDK then call `west update` to downlaod the SDK into your Zepyr repo.

Here's the lines to add the EI-SDK:
.. code-block:: yml
    - name: edge-impulse-sdk-zephyr
      path: modules/edge-impulse-sdk-zephyr
      revision: ${SDK_VERSION}
      url: https://github.com/edgeimpulse/edge-impulse-sdk-zephyr


2. Use this project as a manifest repository, running from this project folder:
.. code-block:: console
    west init --local .
    cd ..
    west update

to pull or update the modules needed.

Check the `Zephyr module documentation`_ for best practice.

Update model
************

Go to the deployment page of your project and choose the Zephyr library option and extract the .zip in the parent folder of this project.

Then update the sample you want to test in [main.cpp](./src/main.cpp) :
.. code-block:: c
    static const float features[] = {
        // copy raw features here (for example from the 'Live classification' page)
        // see https://docs.edgeimpulse.com/docs/running-your-impulse-locally-zephyr
    };


The extracted model should be placed here [model](./model/) see [CMakeLists.txt](./CMakeLists.txt)
.. code-block:: cmake
    list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/model)    

Build and flash
***************

Build the project running:

.. code-block:: console
    west build -p


Then flash it:
.. code-block:: console
    west flash

You can specify the board you want to test by modifying the `.west/config` or by calling `west build -b <your board> -p`

References
==========

.. target-notes::

.. _example-standalone-inferencing-zephyr: https://github.com/edgeimpulse/example-standalone-inferencing-zephyr)
.. _Edge Impulse SDK Zephyr module :https://github.com/edgeimpulse/edge-impulse-sdk-zephyr
.. _Zephyr module documentation: https://docs.zephyrproject.org/latest/develop/modules.html
