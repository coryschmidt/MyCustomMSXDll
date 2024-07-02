## MSX Sample DLL for Visual Studio 2019

### Customizing the DLL for your own use

The relevant code lives in MyCustomMSXDll.cpp. It defines 3 functions visible to MetaStock: MyMov, which calculates a simple moving average; AddN, which adds the user-supplied value to the incoming data array; and SumArrays, which adds two input arrays together. The purpose of these functions is to illustrate how to define calculation functions so they are usable from within MetaStock. The manual explains things in more detail.

The solution should build without any modifications. You can start by building your DLL and loading it in MSXTest which was sent with the developers kit. (NOTE: You may need to download ss32d25.dll in this repo and place it in the same folder as MSXTest.exe if you get an error when trying to load MSXTest.) MSXTest should detect the three functions mentioned earlier and allow the calling of the calculation functions with the specified parameters. This is all explained in the manual.

Next, you can either add a new function following the pattern laid out by the three sample functions, or modify an existing sample function to perform your own custom calculations.
