# EPM Viewer

## Introduction

EPM Viewer is part of the QEPM Application Suite. It provides visualization
of EPM acquisition results. Begin by opening an EPM results file. The captured
channels are listed under the **Current and Voltage** and **Power** tabs.
Toggle the channel attributes you wish to visualize.

You can additionally change the properties of the graph by changing the limits of the area that
is plotted by editing the values of T0 and T1 in the "Cursor" section.

The statistics of the various attributes of the channels can be found in the "Statistics" 
section in the bottom right.

## The preferences window

![EPM Viewer preferences window](../resources/viewer-preferences.png)

The preferences window for EPM viewer lets you customize logging and export preferences.

## Troubleshooting

**Q. The legends for the graphs are black. I cannot see the graphs properly.**

A. Update the debug board firmware to the latest and update the version of QEPM to latest. And, re-record the sample. 

**Q. Nothing is exported when I try to export power, voltage or current data. Why?**

A. Make sure channels to be exported are selected. Then, retry exporting. EPM Viewer should generate a CSV output for selected channels.
