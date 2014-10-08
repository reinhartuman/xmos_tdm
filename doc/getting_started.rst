Getting Started 
================

<<<<<<< HEAD
The following instructions explain how to build the demo for the
XR-AVB-LC-BRD endpoint.

To install the software, open the xTIMEcomposer Studio and
=======
Obtaining the latest firmware
-----------------------------

#. Log into xmos.com and access `My XMOS` |submenu| `Reference Designs`
#. Request access to the `XMOS AVB-DC Software Release` by clicking the `Request Access` link under `AVB DAISY-CHAIN KIT`. An email will be sent to your registered email address when access is granted.
#. A `Download` link will appear where the `Request Access` link previously appeared. Click and download the firmware zip.


Installing xTIMEcomposer Tools Suite
------------------------------------

The AVB-DC software requires xTIMEcomposer version 13.0.2 or greater. It can be downloaded at the following URL
https://www.xmos.com/en/support/downloads/xtimecomposer


Importing and building the firmware
-----------------------------------

To import and build the firmware, open xTIMEcomposer Studio and
>>>>>>> d4b36020fde9d35f0f2c5a925cd218d1f561d6db
follow these steps:

#. Choose `File` |submenu| `Import`.

#. Choose `General` |submenu| `Existing Projects into Workspace` and
   click **Next**.

<<<<<<< HEAD
#. Click **Browse** next to `Select archive file` and select
   the file firmware ZIP file.
=======
#. Click **Browse** next to **`Select archive file`** and select
   the firmware .zip file downloaded in section 1.
>>>>>>> d4b36020fde9d35f0f2c5a925cd218d1f561d6db

#. Make sure that all projects are ticked in the
   `Projects` list.
 
#. Click **Finish**.

<<<<<<< HEAD
To build, select the ``app_simple_avb_demo`` project in the
Project Explorer and click the **Build** icon.

.. cssclass:: cmd-only

From the command line, you can follow these steps:

#. To install, unzip the pacakge zipfile

#. To build, change into the ``app_simple_avb_demo`` directory and
   execute the command::

        xmake all

Makefiles
~~~~~~~~~

The main Makefile for the project is in the
``app_simple_avb_demo`` directory. This file specifies build
options and used modules.

Running the application
~~~~~~~~~~~~~~~~~~~~~~~

To upgrade the firmware you must, firstly connect the XTAG-2 to the 
relevant development board and plug the XTAG-2 into your PC or Mac.

Using the XMOS xTIMEcomposer Studio
-----------------------------------

Using the 12.0.0 tools or later and AVB version 5.2.0 or
later, from within the xTIMEcomposer Studio:

 #. Right click on the binary within the bin folder of the project.
 #. Choose `Run As` |submenu| `Run Configurations`
 #. Double click `xCORE Application` in the left panel
 #. Choose `hardware` in `Device options` and select the relevant XTAG-2 adapter
 #. Select the `Run XScope output server` check box.
 #. Click on **Apply** if configuration has changed
 #. Click on **Run**
=======
#. Select the ``app_daisy_chain`` project in the Project Explorer and click the **Build** icon in the main toolbar.

Installing the application onto flash memory
--------------------------------------------

#. Connect the xTAG-2 debug adapter (XA-SK-XTAG2) to the first sliceKIT core board. 
#. Connect the xTAG-2 to the debug adapter.
#. Plug the xTAG-2 into your development system via USB.
#. Plug in the 12V power adapter and connect it to the sliceKIT core board.
#. In xTIMEcomposer, right-click on the binary within the *app_daisy_chain/bin* folder of the project.
#. Choose `Flash As` |submenu| `Flash Configurations`.
#. Double click `xCORE Application` in the left panel.
#. Choose `hardware` in `Device options` and select the relevant xTAG-2 adapter.
#. Click on **Apply** if configuration has changed.
#. Click on **Flash**. Once completed, disconnect the power from the sliceKIT core board.
#. Repeat steps 1 through 8 for the second sliceKIT.
>>>>>>> d4b36020fde9d35f0f2c5a925cd218d1f561d6db

Using the Command Line Tools
----------------------------

#. Open the XMOS command line tools (Command Prompt) and
   execute the following command:


   ::

       xrun --xscope <binary>.xe

<<<<<<< HEAD
#. If multiple XTAG2s are connected, obtain the adapter ID integer by executing:
=======
#. If multiple xTAG-2s are connected, obtain the adapter ID integer by executing:
>>>>>>> d4b36020fde9d35f0f2c5a925cd218d1f561d6db

   :: 

      xrun -l

#. Execute the `xrun` command with the adapter ID flag

   :: 

      xrun --id <id> --xscope <binary>.xe



<<<<<<< HEAD
Installing the application onto flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Connect the XTAG-2 debug adapter to the relevant development
   board, then plug the XTAG-2 into your PC or Mac.


Using the XMOS xTIMEcomposer Studio
-----------------------------------

To upgrade the flash from the xTIMEcomposer Studio, follow these steps:


#. Start the xTIMEcomposer Studio and open the workspace created in **Running the application**.
#. Right click on the binary within the bin folder of the project.
#. Choose `Flash As` |submenu| `Flash Configurations`
#. Double click `xCORE Application` in the left panel
#. Choose `hardware` in `Device options` and select the relevant XTAG-2 adapter
#. Click on **Apply** if configuration has changed
#. Click on **Flash**

=======
Installing the application onto flash via Command Line
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Connect the xTAG-2 debug adapter to the relevant development
   board, then plug the XTAG-2 into your PC or Mac.

>>>>>>> d4b36020fde9d35f0f2c5a925cd218d1f561d6db
Using Command Line Tools
------------------------


#. Open the XMOS command line tools (Command Prompt) and
   execute the following command:

   ::

       xflash <binary>.xe

<<<<<<< HEAD
#. If multiple XTAG2s are connected, obtain the adapter ID integer by executing:
=======
#. If multiple xTAG-2s are connected, obtain the adapter ID integer by executing:
>>>>>>> d4b36020fde9d35f0f2c5a925cd218d1f561d6db

   :: 

      xrun -l

#. Execute the `xflash` command with the adapter ID flag

   :: 

      xflash --id <id> <binary>.xe

