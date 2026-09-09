.. zephyr:code-sample:: imu-inferencing
   :name: Edge Impulse Example: IMU inferencing

   Runs inference on IMU sensor using the Edge Impulse SDK Zephyr module.

Overview
********

This example demonstrates how to run inference on IMU using the STM IKS02A1 sensor hat data using the Edge Impulse SDK Zephyr module.

Update model
************

Go to the deployment page of your project and choose the Zephyr library option, then extract the .zip in the model folder of this sample.

The extracted model should be placed in the ``model`` folder, see ``CMakeLists.txt``:

.. code-block:: cmake

    list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/model)

Build and flash
****************

Build the project running:

.. code-block:: console

    west build -p

Then flash it:

.. code-block:: console

    west flash

You can specify the board you want to test by modifying the ``.west/config`` or by calling ``west build -b <your board> -p``

References
==========

.. target-notes::

.. _Edge Impulse SDK Zephyr module: https://github.com/edgeimpulse/edge-impulse-sdk-zephyr
.. _Zephyr module documentation: https://docs.zephyrproject.org/latest/develop/modules.html
.. _STM IKS02A1 sensor hat: https://www.st.com/en/evaluation-tools/x-nucleo-iks02a1.html
