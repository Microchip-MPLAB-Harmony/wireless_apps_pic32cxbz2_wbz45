# Install Harmony 3 Dependencies

**This section will describe in detail how to install the correct versions of the required Harmony 3component dependencies. The following table summarizes the components and versions that will be installed**

|Harmony components to be cloned with MCC Content Manager|Version|
|csp|3.13.0|
|core|3.11.0|
|mhc|3.8.3|
|dev\_packs|3.13.0|
|bsp|3.13.0|
|CMSIS-FreeRTOS|10.3.1|
|crypto|3.7.4|
|wolfssl|4.7.0|
|PIC32CX-BZ\_DFP|1.0.107|
|wireless\_ble|1.0.0|
|wireless\_system\_pic32cxbz\_wbz|1.0.0|
|wireless\_apps\_pic32cxbz2\_wbz45|1.0.0|
|wireless\_pic32cxbz\_wbz|1.0.0|

## Clone the Harmony repositories using MCC Content Manager Wizard {#GUID-5E0F1FBF-073A-4D6D-B601-5B86194844FE .section}

1.  Create a new “MCC Harmony” project \(In order to clone the Harmony repositories user needs to create an empty project and clone the required repositories\)

    1.  Select "New Project"

        ![](media/GUID-F70CE34F-FFDC-488C-A74F-BF2388A4553B-low.png "New Project Creation")

    2.  Select "32-bit MCC Harmony Project"

        ![](media/GUID-FAF17099-C3DC-42F0-AC60-F3DED0182408-low.png "MCC Harmony Project option")

    3.  Framework Selection

        ![](media/GUID-0DB4E104-291B-4FFB-A169-AD54B683E76C-low.png "Framework Selection")

    4.  Project Settings

        ![](media/GUID-A736B97B-EC04-471F-8FE5-F9E288D95EC0-low.png "Project Settings")

    5.  Configuration Settings

        ![](media/GUID-080BD142-732A-4E10-999E-A79EEE37E0DB-low.png "Configuration Settings")

2.  Open “MPLAB Code Configurator” after creation of project. MCC Content Manager window will appear in IDE

    ![](media/GUID-9552FC6F-18E5-4B15-8116-13074B0A541B-low.png "MCC in IDE")

3.  Select “MPLAB Harmony” from the content manager window

    ![](media/GUID-C6A309DF-0505-469B-AD46-13E8017C8515-low.png "MCC Content Manager window")

    Select "Optional Content" as shown below

    ![](media/GUID-A59D5B0C-B65F-4352-B61F-93A78FAB5C8B-low.png "Optional Content Window MCC")

4.  Ensure all components are selected as displayed in the image below. Ignore version selection for now it will be taken care of at a later stage

    ![](media/GUID-2EFDA6C6-EB94-4BED-A9BC-B99F1DB3E136-low.png "Required Content")

5.  Select “Finish” The downloading of selected components from harmony repositories will take some minutes.If all the selected components are cloned successfully, MCC logs in IDE will display this information.

    ![](media/GUID-2BAAE5F6-555E-4B86-9EAD-B6338216502F-low.png "MCC Logs Snapshot when all selected components are cloned successfully")

    **Attention:**

    -   **If in the process a failure to download a particular component appears, try redownloading again only the component that failed to download. For example, if user received a prompt from MCC saying “dev\_packs” was not download**
    -   close the MCC reopen MCC again and start from step 3 in this section, with the “dev\_packs” as the only missing component for downloading and select “Finish”
    ![](media/GUID-DA2BA56B-FD06-4EB8-84BC-9435E090A9EB-low.png ""dev_packs" being redownloaded")

6.  Select/Confirm the Harmony Framework Path

    ![](media/GUID-9FA37C1C-2C21-452C-B301-EA9288917E60-low.png "Harmony Framework Path Selection")

    **Note:** Framework Path selection window appears beneath the Initializing Project pop-up, users should Choose Path as “C:\\mchp”

    ![](media/GUID-BF9919FE-A8D3-4493-9CF3-E4979F4773AD-low.png "Framework Selection Path")

    Project Graph Window will appear after choosing the framework path and successful initialization of project

    ![](media/GUID-173CA14B-8179-4925-BB5D-5B8DAB25B724-low.png "Project Graph")

7.  Change to content[versions](#GUID-AF986E94-2ACB-4D83-B1AD-C07C778186AE) recommended for use during development in the table using the content manager from Device Resources and select “Apply”. This step is important and should be followed by user as every wireless stack and app example release is tested with certain version of dependant component.

    ![](media/GUID-25B49C80-0340-4C6B-88F8-C2CD474A01BE-low.png "Changing content versions")


After successfully checking out the right version of content/components, users can [start developing application examples](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-FAC65B28-B78E-48AF-BCFE-2D63DC274CDB).

**Parent topic:**[Getting Started with Software Development](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-2AD37FE2-1915-4E34-9A05-79E3810726D7)

